OBJ_DIR=build
TEST_OBJ_DIR=build/tests
SRC_DIR=src
TEST_DIR=tests
HEADERS_DIR=headers

CC=clang

CPPFLAGS=-Iinclude -Iheaders # /!\ Enlever -DDISABLE_IO pour utiliser votre implementation io.c
CFLAGS=-Wall -Werror -O3 # Ajoutez -g pour Valgrind :)
LDLIBS=-lm

TARGET=spellchecker
TEST=test

VALGRIND = valgrind --leak-check=full
SPELLCHECKER_ARGS = --dicts dicts --input inputs/input_10l.txt --mode correction

# List path to sources
SOURCES   = $(wildcard $(SRC_DIR)/*.c)
# List path to object files
OBJECTS   = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
# List path to headers files
HEADERS        = $(wildcard $(HEADERS_DIR)/*.h)

# Filter object files to exclude program's main
OBJECTS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS))

# List path to test sources
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
# List path to test object files
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c, $(TEST_OBJ_DIR)/%.o, $(TEST_SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CPPFLAGS) $(CFLAGS)  $^ $(LDLIBS) -o $@

$(TEST): $(OBJECTS_NO_MAIN) $(TEST_OBJECTS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ $(LDLIBS) -lcunit  -o $@

tests: $(TEST)
	./$(TEST)

valgrind-all : $(TARGET) $(TEST)
	$(VALGRIND) ./$(TARGET) $(SPELLCHECKER_ARGS) > fichier_sortie
	rm -f fichier_sortie
	$(VALGRIND) ./$(TEST)

valgrind : $(TARGET)
	$(VALGRIND) ./$(TARGET) $(SPELLCHECKER_ARGS) > fichier_sortie
	rm -f fichier_sortie

# Valgrind va détécter des erreurs de free car dans tests/detector.c, test_get_wrong_words_in_line()
# Il y a des tests qui vont avoir des indexes externe au tableau des mauvais mots,
# Et donc on va essayer de free une adresse aléatoire dans la mémoire "à cause de" free_array_of_words()
# La solution serait d'utiliser calloc au lieu de malloc dans get_wrong_words_in_line
# Mais ça impacterait les performances comme la dit le professeur Bonaventure une fois
# Niveau code il y a aucun bug si toutes les fonctions font bien leur travail
valgrind-test : $(TEST)
	$(VALGRIND) ./$(TEST)

clean:
	rm -f $(TARGET) $(TEST) $(OBJECTS) $(TEST_OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(HEADERS)
	mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all clean tests
