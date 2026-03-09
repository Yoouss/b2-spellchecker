#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "common.h"
#include <io.h>
#include <stdlib.h>
// Ajouter le include de detector.c quand il sera dans header

// À enlever après implémentation
int word_in_dictionary(char* word, Dictionary_t* dict) {
    return 0;
}

void test_word_in_dictionary(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;
    char* dictionaryPath = "fr.dict.test";
    load_dictionaries(dictionaryPath, &dicts, &dicts_count);

    CU_ASSERT_PTR_NOT_NULL(dicts);
    CU_ASSERT_EQUAL(1, word_in_dictionary("bonjour", dicts));
    CU_ASSERT_EQUAL(0, word_in_dictionary("mmzjd", dicts));
    CU_ASSERT_EQUAL(-1, word_in_dictionary(NULL, dicts)); // cas extrême 1 : on ne donne pas de mot à chercher
    CU_ASSERT_EQUAL(-1, word_in_dictionary("bonjour", NULL)); // cas extrême 2 : pas de dictionnaire trouvé 
}

void free_matrix_input_5l_test(int** matrix, int matrixLength) {
    for (int i = 0; i < matrixLength; i++) {
        if (matrix[i] != NULL) free(matrix[i]);
    }

    free(matrix);
}

int allocate_memory_for_matrix(int** matrix, int index, int numberOfElements) {
    matrix[index] = malloc(numberOfElements * sizeof(int));

    if (matrix[index] == NULL) {
        free_matrix_input_5l_test(matrix, index);
        return -1;
    }

    return 0;
}

int** give_expected_output_with_input_5l_test() {
    int** matrix = malloc(5 * sizeof(int *));
    if (matrix == NULL) return NULL;
    int status;

    status = allocate_memory_for_matrix(matrix, 0, 2);
    if (status == -1) return NULL;
    matrix[0][0] = 3;
    matrix[0][1] = 7;

    status = allocate_memory_for_matrix(matrix, 1, 1);
    if (status == -1) return NULL;
    matrix[1][0] = 6;

    status = allocate_memory_for_matrix(matrix, 2, 1);
    if (status == -1) return NULL;
    matrix[2][0] = 4;

    matrix[3] = NULL;

    status = allocate_memory_for_matrix(matrix, 4, 1);
    if (status == -1) return NULL;
    matrix[4][0] = 4;

    return matrix;
}

// A enlever après implémentation !!! 
int** words_in_file(char** file, int length, Dictionary_t* dict ) {
    return give_expected_output_with_input_5l_test();
}


void test_words_in_file(void) {
    char* dictionaryPath = "fr.dict.test";
    char* inputPath = "input_5l_test.txt";

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(inputPath, &lines, &lines_sizes, &line_count);

    // Load dictionnary
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries(dictionaryPath, &dicts, &dicts_count);

    int** expectedOutput = give_expected_output_with_input_5l_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(expectedOutput);

    int** result = words_in_file(lines, line_count, dicts);

    CU_ASSERT_EQUAL(result[0][0], 3);
    CU_ASSERT_EQUAL(result[0][1], 7);
    CU_ASSERT_EQUAL(result[1][0], 6);
    CU_ASSERT_EQUAL(result[2][0], 4);
    CU_ASSERT_PTR_NULL(result[3]);
    CU_ASSERT_EQUAL(result[4][0], 4);

    free_matrix_input_5l_test(expectedOutput, 5);
}
