OBJ_DIR=build
TEST_OBJ_DIR=build/tests
SRC_DIR=src
TEST_DIR=tests
HEADERS_DIR=headers

CC=clang

CPPFLAGS=-Iinclude -Iheaders -DDISABLE_IO # /!\ Enlever -DDISABLE_IO pour utiliser votre implementation io.c
CFLAGS=-Wall -Werror # Ajoutez -g pour Valgrind :)
LDLIBS=-lm 

TARGET=spellchecker
TEST=test

# List path to sources
SOURCES   = $(wildcard $(SRC_DIR)/*.c)
# List path to object files
OBJECTS   = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
# List path to headers files
HEADERS 	= $(wildcard $(HEADERS_DIR)/*.h)

# Filter object files to exclude program's main
OBJECTS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS))

# List path to test sources
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
# List path to test object files
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c, $(TEST_OBJ_DIR)/%.o, $(TEST_SOURCES))


all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(CC)  $^ $(LDLIBS) -o $@ 

$(TEST): $(OBJECTS_NO_MAIN) $(TEST_OBJECTS)
	@$(CC)  $^ $(LDLIBS) -lcunit  -o $@

clean:
	@rm -f $(TARGET) $(TEST) $(OBJECTS) $(TEST_OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(HEADERS)
	@mkdir -p $(TEST_OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all clean test
