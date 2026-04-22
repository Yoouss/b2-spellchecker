import argparse
import struct

def read_ints(path):
    """Read a binary file of big-endian 32-bit ints into a list."""
    with open(path, "rb") as f:
        data = f.read()
    n = len(data) // 4
    data = data[:n*4] 
    return struct.unpack(f">{n}I", data)

def parse_students_offsets(data): 
    students_offsets = []
    
    it = iter(data)
    try:
        while True:
            num_line = next(it)
            dict_idx = next(it)
            offsets_amount = next(it)
            offsets = [next(it) for _ in range(offsets_amount)]
            if None in (dict_idx, *offsets) or len(offsets) != offsets_amount:
                raise Exception("Malformed line")
            students_offsets.append((num_line, dict_idx, offsets))
    except StopIteration:
        # Reached EOF
        pass
    except Exception as e:
        # Reached malformed line
        return None
    return students_offsets
    
def read_reference_offsets(path):
    with open(path, 'r') as f:
        return [list(map(int, line.strip().split(","))) for line in f]
    
def parse_students_fix(data):  
    students_fix = []        
    i = 0
    try: 
        while i < len(data):
            length = int.from_bytes(data[i:i+4], "big")
            i += 4
            word = data[i:i+length].decode('iso-8859-1')[:-1]
            students_fix.append(word)
            i += length
    except Exception:
        # Reached malformed line
        if not students_fix:
            raise Exception(f"Error: File is empty or malformed.")
        
    return students_fix

def read_reference_fix(path):
    with open(path, 'r', encoding='iso-8859-1') as f:
        return [line.strip().split() for line in f]
    
def validate_offsets(students_offsets, reference_offsets, verbose):
    if not students_offsets:
        return 0.0, {}, [], 0

    ref_dict = {r[0]: r[1:] for r in reference_offsets}
    stud_dict = {num_line: offsets for (num_line, _, offsets) in students_offsets}
    
    common_lines = sorted(set(ref_dict.keys()) & set(stud_dict.keys()))
    ref_sorted = [ref_dict[line] for line in common_lines]
    stud_sorted = [stud_dict[line] for line in common_lines]
    
    ref_sets = [set(r) for r in ref_sorted]
    stud_sets = [set(s) for s in stud_sorted]

    tp = sum(len(s & r) for s, r in zip(stud_sets, ref_sets))
    fp = sum(len(s - r) for s, r in zip(stud_sets, ref_sets))
    fn = sum(len(r - s) for s, r in zip(stud_sets, ref_sets))

    score = tp / (tp + fp + fn) if (tp + fp + fn) > 0 else 1.0

    if verbose:
        print(f"Detection score: {score:.2%} (TP: {tp}, FP: {fp}, FN: {fn})")

    intersections = {num_line: s & r for num_line, (s, r) in zip(common_lines, zip(stud_sets, ref_sets))}

    return score, intersections, students_offsets, len(students_offsets)

def validate_corrections(intersections, students_offsets, students_fix, reference_fix, verbose):
    if not students_offsets:
        return 0.0
    
    students_offsets_len = sum(len(offsets) for _, _, offsets in students_offsets)
    students_fix = students_fix[:min(students_offsets_len, len(students_fix))]

    students_offsets_new = []
    
    size = 0
    for line_idx, dict_idx, offsets in students_offsets:
        if size + len(offsets) <= len(students_fix):
            students_offsets_new.append((line_idx, dict_idx, offsets))
            size += len(offsets)
        else:
            break
    students_offsets = students_offsets_new
    
    wrong, total, s_fix_idx = 0, 0, 0

    for (line_idx, _, offsets) in students_offsets:
        inter = intersections[line_idx]
        r_fix = reference_fix[line_idx]
        offset_to_idx = {off: idx for idx, off in enumerate(offsets)}
        for idx in inter:
            s_idx = s_fix_idx + offset_to_idx.get(idx, -1)
            if students_fix[s_idx] != r_fix[idx]:
                wrong += 1
            total += 1
        s_fix_idx += len(offsets)
        
    score = (total - wrong) / total if total > 0 else 1.0
    if verbose:
        print(f"Correction accuracy: {score:.2%} ({total - wrong}/{total})")
        
    return score

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser(description="Validate the output of the spellchecker.")
    parser.add_argument("--students_offsets", type=str, required=True, help="Path to the students' offsets file.")
    parser.add_argument("--reference_offsets", type=str, required=True, help="Path to the reference offsets file.")
    parser.add_argument("--students_fix", type=str, help="Path to the students' fix file.")
    parser.add_argument("--reference_fix", type=str, help="Path to the reference fix file.")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output.")
    args = parser.parse_args()

    _, intersection, students_offsets, _ = validate_offsets(
        parse_students_offsets(read_ints(args.students_offsets)),
        read_reference_offsets(args.reference_offsets),
        args.verbose
    )

    if args.students_fix is not None and args.reference_fix is not None:
        with open(args.students_fix, 'rb') as f:
            stud_data = f.read()
        validate_corrections(
            intersection, students_offsets,
            parse_students_fix(stud_data), read_reference_fix(args.reference_fix),
            args.verbose
        )