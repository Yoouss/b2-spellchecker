import argparse
from pathlib import Path
import asyncio
import time
import uuid
import os
from math import log2
import subprocess
import resource
import signal
import psutil
import numpy as np
import threading

import gen_input
import input2bin
import validate_output

tasks = []

WORD_COUNT_RANGE = (20, 25)

NON_EXECUTABLE = "Non-executable or missing spellchecker"
EMPTY_OUTPUT = "Empty output files"
MAKE_FAILED = "Make failed"
TOO_MUCH_MEMORY = "Exceeded memory limit"

N_ITER = 3
TIMEOUTS = (2, 20) # for detection and correction modes respectively, in seconds
LINE_AMOUNTS = [250, 10] # for detection and correction modes respectively
gen_input.MAX_ALTERATIONS = 2
gen_input.ERRORS_PERCENTAGE = 0.1

# Number of timing values 
# ! CHANGE IN groups.html AS WELL !
TIMESTAMPED_LINE_AMOUNT = 20

# Maximal virtual memory for subprocesses (in bytes).
MAX_VIRTUAL_MEMORY = 300 * 1024 * 1024 # 300 MB

today = time.strftime("%Y-%m-%d")

class AttrDict(dict):
    def __getattr__(self, key):
        return self[key]

    def __setattr__(self, key, value):
        self[key] = value
        
def limit_virtual_memory():
    # The tuple below is of the form (soft limit, hard limit). Limit only
    # the soft part so that the limit can be increased later (setting also
    # the hard limit would prevent that).
    # When the limit cannot be changed, setrlimit() raises ValueError.
    resource.setrlimit(resource.RLIMIT_AS, (MAX_VIRTUAL_MEMORY, resource.RLIM_INFINITY))
        
def run_timeout(command, project_path, timeout):
    """
    Run the spellchecker with a timeout.
    If any error occurs, return the error message.
    
    Results are returned as a tuple: (success, error_message)
    """
            
    proc = subprocess.Popen(
        command,
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        cwd=project_path,
        text=True,
        bufsize=0,
        preexec_fn=limit_virtual_memory
    )

    process = psutil.Process(proc.pid)
    
    try:
        proc.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        mem = process.memory_info().rss // 1024
        proc.send_signal(signal.SIGTERM)
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()
        return False, None, mem
    except OSError:
        return False, TOO_MUCH_MEMORY, 0
    
    if proc.returncode in (126, 127):
        return False, NON_EXECUTABLE, 0

    return True, None, 0
                
def read_file(path, total_time, read_fn, out):
    i = 0
    time.sleep(0.1)
    while i < TIMESTAMPED_LINE_AMOUNT:
        i += 1
        time.sleep(total_time / TIMESTAMPED_LINE_AMOUNT)
        try:
            out.append(read_fn(path))
        except Exception:
            # we've been killed
            break
            
def run_benchmark(project_path, dicts_path, thread_count, mode):
    """
    Run a benchmark for a program on multiple input sizes and return the results.
    Loop over increasing input sizes until timeout occurs. 
    On timeout, return the average results over all completed runs.
    
    Results are returned as a tuple:
    (avg_line_amount, avg_mem_kb, avg_det_score, avg_cor_score, timings, error_messages)
    
    - If no errors occurred, error_messages is "OK".
    - If any error occurs, return the error message.
    """

    input_uuid = uuid.uuid4()
    output_uuid = uuid.uuid4()
    
    timeout = TIMEOUTS[mode == "correction"]
    line_amount = LINE_AMOUNTS[mode == "correction"]

    input2bin_args = AttrDict({
        "input": str(project_path / f"{input_uuid}.txt"),
        "output": str(project_path / f"{input_uuid}.bin")
    })
    
    gen_args = AttrDict({
        "dicts": dicts_path,
        "output_file": str(project_path / f"{input_uuid}"),
        "word_count_range": WORD_COUNT_RANGE,
        "verbose": False
    })
    
    command = [
        "stdbuf", "-o0", "-e0",
        "./spellchecker",
        "-d", str(dicts_path),
        "-t", str(thread_count),
        "-i", f"{input_uuid}.bin",
        "-o", str(output_uuid),
        "-m", mode,
    ]
        
    det_reader_out = []
    cor_reader_out = []

    def clear_files():
        det_reader_out.clear()
        cor_reader_out.clear()
        for ext in ("", ".bin", ".pos", ".fix", ".err", ".txt", ".err.time", ".fix.time"):
            (project_path / f"{input_uuid}{ext}").unlink(missing_ok=True)
            (project_path / f"{output_uuid}{ext}").unlink(missing_ok=True)  
                    
    def gen_files():     
        gen_input.generate_input_file(gen_args)
        input2bin.convert(input2bin_args)
        
    def start_readers(path_base):
        t1 = threading.Thread(target=read_file, args=(project_path / f"{path_base}.err", timeout, validate_output.read_ints, det_reader_out), daemon=True)
        t1.start()
        t2 = None
        if mode == "correction":
            def reader(path):
                with open(path, 'rb') as f:
                    return f.read()
            t2 = threading.Thread(target=read_file, args=(project_path / f"{path_base}.fix", timeout, reader, cor_reader_out), daemon=True)
            t2.start()
        return t1, t2

    total_line_amount = 0
    total_det_score = 0
    total_cor_score = 0
    total_mem = 0
    total_line_amounts = []

    error_messages = set()

    for _ in range(N_ITER):
        
        # --- Run until timeout ---
        gen_args.line_amount = line_amount
        gen_files()
        tr, tc = start_readers(output_uuid)
        success, message, mem_kb = run_timeout(command, project_path, timeout)
        
        # --- Run until timeout (success is False) ---
        while success: 
            # Kill the threads as we don't need their results
            tr.join(0) 
            if tc: tr.join(0)

            clear_files()

            # --- Double line amount ---
            line_amount *= 2
            gen_args.line_amount = line_amount
            gen_files()
            tr, tc = start_readers(output_uuid)
            success, message, mem_kb = run_timeout(command, project_path, timeout)
            
        tr.join()
        if tc: tc.join()
        
        if message:
            error_messages.add(message)
            clear_files()
            return (0, 0, 0, 0, "|".join(error_messages))
                
        # --- Validate output files ---
        detection_score = 0
        correction_score = 0
        current_line_amount = 0
        try:
            detection_score, intersection, students_offsets, current_line_amount = validate_output.validate_offsets(
                validate_output.parse_students_offsets(det_reader_out[-1]),
                validate_output.read_reference_offsets(project_path / f"{input_uuid}.pos"),
                False
            )

            if mode == "correction" and detection_score > 0:
                with open(project_path / f"{output_uuid}.fix", 'rb') as f:
                    stud_data = f.read()
                correction_score = validate_output.validate_corrections(
                    intersection,
                    students_offsets,
                    validate_output.parse_students_fix(stud_data),
                    validate_output.read_reference_fix(project_path / f"{input_uuid}.fix"),
                    False
                )
        except Exception:
            # Program timed out too early and produced an empty output
            clear_files()
            error_messages.add(EMPTY_OUTPUT)
            return (0, 0, 0, 0, EMPTY_OUTPUT)
        
        arr = det_reader_out if mode == "detection" else cor_reader_out
        line_amounts = np.array([len(validate_output.parse_students_offsets(t)) for t in arr])
        
        if len(total_line_amounts) == 0:        
            total_line_amounts = line_amounts
        else:
            s = min(len(total_line_amounts), len(line_amounts))
            total_line_amounts = total_line_amounts[:s] + line_amounts[:s]

        clear_files()

        # --- Accumulate results ---
        total_line_amount += current_line_amount
        total_det_score += detection_score
        total_cor_score += correction_score
        total_mem += mem_kb
        
    # --- Compute averages ---
    avg_line_amount = total_line_amount / N_ITER
    avg_det_score = total_det_score / N_ITER 
    avg_cor_score = total_cor_score / N_ITER 
    avg_mem = total_mem / N_ITER
    avg_line_amounts = (total_line_amounts / N_ITER).tolist()
    
    if not error_messages:
        error_messages.add("OK")
        
    return (avg_line_amount, avg_mem, avg_det_score, avg_cor_score, avg_line_amounts, "|".join(error_messages))


class CoreScheduler:
    """
    Scheduler that manages the total number of logical cores for concurrency.
    `acquire(n)` is atomic: it waits until n cores are available and then
    consumes them all at once, preventing the "partial-acquire" deadlock.
    """
    def __init__(self, total_cores):
        self.total_cores = int(total_cores)
        self._available = int(total_cores)
        self._cond = asyncio.Condition()

    async def acquire(self, n_threads):
        """Atomically acquire n_threads. Wait if not enough are available."""
        if n_threads <= 0:
            return

        if n_threads > self.total_cores:
            raise ValueError(f"Requested {n_threads} threads > total cores {self.total_cores}")

        async with self._cond:
            await self._cond.wait_for(lambda: self._available >= n_threads)
            self._available -= n_threads
            
    async def release(self, n_threads):
        """Release n_threads back to the pool and notify waiters."""
        if n_threads <= 0:
            return

        async with self._cond:
            self._available += n_threads
            if self._available > self.total_cores:
                self._available = self.total_cores
            self._cond.notify_all()
            
class ProjectBuilder:
    """ Build projects using `make`, ensuring each project is built only once."""
    def __init__(self):
        self.built_projects = {}
        self.locks = {}

    async def build(self, project_path):
        """Build the project at project_path using `make` if not already built. Return True if the build was successful."""
        if project_path not in self.locks:
            self.locks[project_path] = asyncio.Lock()
        
        async with self.locks[project_path]:
            if project_path in self.built_projects:
                return self.built_projects[project_path]
            
            result = subprocess.run(["make", "clean"], cwd=project_path)
            if result.returncode != 0:
                self.built_projects[project_path] = False
                return False

            result = subprocess.run(["make"], cwd=project_path)
            if result.returncode != 0:
                self.built_projects[project_path] = False
                return False

            self.built_projects[project_path] = True
        return True

tasks_completed = 0
tasks_completed_lock = asyncio.Lock()

async def run_task_scheduled(scheduler, project_builder, task, idx, total, verbose):
    global tasks_completed, tasks_completed_lock
    """Run a single benchmark task while respecting total core limits"""        
    label = f"[{idx}/{total}] {task['path'].name}"

    duration = 0
    stream = task["stream"]

    # --- Acquire cores ---
    await scheduler.acquire(task["thread_count"])

    # --- Build project ---
    if not await project_builder.build(task["path"]):
        await scheduler.release(task["thread_count"])
        stream.write(f"{task['path'].name},-,{task['mode']},-,-,-,-,{MAKE_FAILED}\n")
        stream.flush()
        if verbose: 
            print(f"✘ {label} (mode={task['mode']}, threads={task['threads']}) - Make failed")
        return
    
    # --- Run benchmark ---
    try:
        start = time.perf_counter()
        result = await asyncio.to_thread(run_benchmark, task["path"], task["dicts_path"], task["thread_count"], task["mode"])
        duration = time.perf_counter() - start
    finally:
        await scheduler.release(task["thread_count"])

    # --- Log results ---
    avg_line_amount, avg_mem, avg_det_score, avg_cor_score, avg_timings_detect, error_messages = result
    stream.write(
        f"{task['path'].name},{task['thread_count']},{task['mode']},{avg_line_amount:.2f},{avg_mem:.2f},"
        f"{avg_det_score*100:.2f},{avg_cor_score*100:.2f},{today},\"{avg_timings_detect}\",{error_messages}\n"
    )
    stream.flush()

    # --- Verbose output ---
    if verbose:
        async with tasks_completed_lock:
            tasks_completed += 1
            print(f"✔ Finished {label} (mode={task['mode']}, threads={task['thread_count']}) in {duration:.2f}s | Progress: {1.0*tasks_completed/total:.2%}")


async def run_all_tasks(tasks, verbose):
    """Run all benchmark tasks concurrently, respecting total core limits."""
    total_cores = os.cpu_count() or 8
    scheduler = CoreScheduler(total_cores)
    project_builder = ProjectBuilder()
    
    if verbose:
        print(f"\n=== Running {len(tasks)} benchmarks using up to {total_cores} logical cores ===\n")
    
    # --- Run all tasks concurrently ---
    await asyncio.gather(*[
        run_task_scheduled(scheduler, project_builder, task, idx=i + 1, total=len(tasks), verbose=verbose)
        for i, task in enumerate(tasks)
    ], return_exceptions=True)


def collect_tasks(mode, paths, stream, dicts_path, thread_counts):
    """ Recursively collect benchmark tasks from project directories, looking for Makefiles. """
    for path in paths:
        if not path.is_dir():
            continue
        if not (path / "Makefile").exists():
            collect_tasks(mode, path.iterdir(), stream, dicts_path, thread_counts)
            continue
        for thread_count in thread_counts:
            # For correction mode, also add a detection mode task
            if mode == "correction":
                tasks.append(dict(
                    mode="detection",
                    path=path,
                    stream=stream,
                    thread_count=thread_count,
                    dicts_path=dicts_path or (path / "dicts").absolute()
                ))
            tasks.append(dict(
                mode=mode,
                path=path,
                stream=stream,
                thread_count=thread_count,
                dicts_path=dicts_path or (path / "dicts").absolute()
            ))
    # Sort tasks by thread count to improve scheduling
    tasks.sort(key=lambda t: t["thread_count"])


def main():
    parser = argparse.ArgumentParser(description="Test the students' spellcheckers")
    parser.add_argument("--project", help="Path to a single project directory")
    parser.add_argument("--projects", help="Path to a directory containing multiple projects")
    parser.add_argument("--mode", choices=["detection", "correction"], default="detection", help="Benchmark mode")
    parser.add_argument("--max_num_threads", type=int, default=1, help="Maximum number of threads to use")
    parser.add_argument("--num_threads", type=int, default=0, help="Number of threads to use")
    parser.add_argument("--mem_limit_mb", type=int, default=300, help="Maximum memory limit for subprocesses in MB")
    parser.add_argument("--output", required=True, help="Path to the output file (CSV)")
    parser.add_argument("--dicts", help="Path to the dictionaries directory (defaults to project's dicts)", default=None)
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    args = parser.parse_args()

    if args.project and args.projects or not (args.project or args.projects):
        print("Please specify either --project or --projects (but not both).")
        return
    if args.max_num_threads < 1:
        print("Invalid number of threads. Must be >= 1.")
        return
    
    global MAX_VIRTUAL_MEMORY
    MAX_VIRTUAL_MEMORY = args.mem_limit_mb * 1024 * 1024

    if args.num_threads > 0:
        thread_counts = [args.num_threads]
    else:
        thread_counts = [2 ** i for i in range(int(log2(args.max_num_threads)) + 1)]
        if thread_counts[-1] != args.max_num_threads:
            thread_counts.append(args.max_num_threads)

    with open(f"{args.output}", "w") as f:
        f.write("group,threads,mode,lines,memory(KB),detection_accuracy(%),correction_accuracy(%),date,timings,comments\n")
        f.flush()

        if args.project:
            # Single project mode, run for both modes if correction is specified
            project_path = Path(args.project)
            run_tasks = [
                dict(
                    mode=args.mode,
                    path=project_path,
                    stream=f,
                    thread_count=thread_count,
                    dicts_path=Path(args.dicts).absolute() if args.dicts else (project_path / "dicts").absolute()
                )
                for thread_count in thread_counts
                for args.mode in ([ "detection", "correction"] if args.mode == "correction" else [args.mode])
            ]
            asyncio.run(run_all_tasks(run_tasks, verbose=args.verbose))
        else:
            # Multiple projects mode, recursively find all projects with Makefiles
            collect_tasks(args.mode, Path(args.projects).iterdir(), f, Path(args.dicts).absolute() if args.dicts else None, thread_counts)
            asyncio.run(run_all_tasks(tasks, verbose=args.verbose))


if __name__ == "__main__":
    main()
