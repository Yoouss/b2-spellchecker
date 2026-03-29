CC      = clang
CFLAGS  = -std=gnu99 -Wall -Wextra -g

# Détection de llvm-cov compatible avec la version de clang installée
CLANG_VERSION := $(shell clang --version 2>/dev/null | grep -oE '[0-9]+' | head -1)
LLVM_COV      := $(shell command -v llvm-cov-$(CLANG_VERSION) 2>/dev/null \
                   || command -v llvm-cov 2>/dev/null \
                   || echo llvm-cov)

.PHONY: all coverage helgrind-race helgrind-deadlock clean

all: coverage helgrind-race helgrind-deadlock

## -----------------------------------------------------------------------
## Section 8 : couverture de code (gcov)
##
## Illustre une suite de tests incomplète sur list.c / test_list.c.
## Compile avec instrumentation, lance les tests, génère le rapport gcov.
## -----------------------------------------------------------------------
coverage: test_list
	@echo ""
	@echo "=== Exécution des tests ==="
	./test_list
	@echo ""
	@echo "=== Rapport de couverture (list.c) ==="
	$(LLVM_COV) gcov list.c \
	    -gcno=test_list-list.gcno \
	    -gcda=test_list-list.gcda
	@echo ""
	@echo "Rapport détaillé : list.c.gcov"

test_list: test_list.c list.c list.h
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage \
	    -o test_list test_list.c list.c -lcunit

## -----------------------------------------------------------------------
## Section 9 (optionnelle) : threads — data race
##
## Illustre une variable partagée sans mutex détectée par Helgrind.
## -----------------------------------------------------------------------
helgrind-race: data_race
	@echo ""
	@echo "=== Helgrind : data race ==="
	valgrind --tool=helgrind ./data_race

data_race: data_race.c
	$(CC) $(CFLAGS) -o data_race data_race.c -lpthread

## -----------------------------------------------------------------------
## Section 9 (optionnelle) : threads — deadlock
##
## Illustre une violation d'ordre d'acquisition de mutex détectée par Helgrind.
## -----------------------------------------------------------------------
helgrind-deadlock: deadlock
	@echo ""
	@echo "=== Helgrind : deadlock ==="
	valgrind --tool=helgrind ./deadlock

deadlock: deadlock.c
	$(CC) $(CFLAGS) -o deadlock deadlock.c -lpthread

## -----------------------------------------------------------------------
clean:
	rm -f test_list data_race deadlock
	rm -f *.gcno *.gcda *.gcov
