# Spellchecker - our 2nd year C project

## Context behind the project
In the LEPL1503 UCLouvain class, we had to code a C program called **spellchecker** which given an input file and dictionaries, can detect and can optionaly correct mistakes generating binary output files <br>
In order to achieve that, we had to program an efficient, fast and clean program capable of using multiple threads to speed up the process

Our group members (Group S1) :

- Younes
- Sofia
- Maryam
- Adjovi
- Maurice

## Documentation
- [The architecture & modules](docs/architecture.md)
- [How to run tests & Valgrind](docs/testing.md)
- [Benchmarks & how to use them](docs/benchmarks.md)


## Installation

### requirements 

...

(coming soon)


## Compiling and executing the spellchecker program

### 1) Compilation

```bash
make
```
Compiles the program's file (spellchecker)

---

### 2) Execution


```bash
./spellchecker --dicts <dictionary_path> --input <input_path> [--output <output_path>] [--mode <mode>] [--threads <number>] [--verbose]
```
Here's the specification of each arguments, the ones in brackets being optionals :

---

1. `--dicts <dictionary_path>` : Specifies the dictornaries' path 

The user can also choose a single dictionary or use `all` as an argument to specify all the dictionaries from the `dicts` directory

-> examples : `dicts`, `all`, `dicts/fr.dict`

---

2. `--input <input_path>` : Specifies the input file's path 

-> example : `inputs/input_10l.txt`

---

3. `--output_path <output_path>` : Specifies the output files' name, **without extension** 

-> exemple : `output`

If this argument isn't specified, the terminal will be used by default, the user then has 2 options :

```bash
./spellchecker ... > <output_path>
```
Use a chevron `>` to get the binary output in the file `<output_path>` specified

```bash
./spellchecker ... | hexdump
```
Or use `| hexdump` to see the binary output on the terminal

---

4. `--mode <mode>` : Specifies the program mode between :

    - `detection` : detects errors and writes them in `stdout` or in a new file `<output_path>.err` if `<output_path>` is specified

    - `correction` : detects errors, corrects them and writes them in `stdout` or in new files `<output_path>.err` for the detection and `<output_path>.fix` for the correction if `<output_path>` is specified

If the mode isn't specified, the `detection` mode will be used by default

---

5. `--threads <number>` : Specifies the number of threads used

If `<number>` is smaller or equal to 1, the program will be executed in a mono-threaded way (sequentialy)

If `<number>` is greater than 1, the program will be executed in a multi-threaded way using `<number>` threads maximum per functions with configurable parallelisme

---

6. `--verbose` : Displays more details in real time

---

### Exemple

```bash
make
```

```bash
./spellchecker --dicts dicts --input inputs/input_50l.txt --output my_output --mode correction --threads 2
```

## Conclusion

This project as been a good opportunity to work together in groups despite the lack of time and motivation from some classmates

Overall it forced use to follows similar pratices, to review others codes via forge (UCLouvain's Gitlab), to manage merge requests, to use tickets in order to stay focused on our own tasks and develop our programming skills