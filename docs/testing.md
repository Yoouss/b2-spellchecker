## Compiling and executing the tests

```bash
make test
```

```bash
./test
```

or compile and execute in a single command :

```bash
make tests
```

## How to use Valgrind

Valgrind is a usefull tool to check if there are memory leaks when we forgets to free allocated ressources

- **To detect memory leaks in the main program `spellchecker`**

```bash
make valgrind
```

- **To detect memory leaks in the test's files used in the binary file `test`**

```bash
make valgrind-test
```

- **To detect all the memory leaks (`spellchecker` and `test`)**

```bash
make valgrind-all
```