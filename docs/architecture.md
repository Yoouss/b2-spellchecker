## Architecture of the project

This repo is organised in multiple modules, each of them having a header file and a test file

```tree
.
├── benchmark
├── build
├── dicts
├── docs
├── inputs
│
├── headers
│   ├── ...
│   ├── corrector.h
│   ├── detector.h
│   ├── file_handler.h
│   └── io.h
├── src
│   ├── ...
│   ├── corrector.c
│   ├── detector.c
│   ├── file_handler.c
│   ├── io.c
│   └── main.c
├── tests
│   ├── corrector.c
│   ├── detector.c
│   ├── file_handler.c
│   ├── io.c
│   └── main.c
│
├── .gitignore
├── Makefile
└── README.md
```

### The main modules 

- `src/detector.c`: Detects and localise errors using binary search to find the dictionary that minimises the errors count for each lines

- `src/corrector.c`: Sugests corrections for founded errors using Levenshtein distance and finding the closest candidats

- `src/file_handler.c`: Loads the input file and the dictionaries

- `src/io.c`: Writes the binary outputs

- `src/main.c`: Parses the arguments and uses the modules when needed