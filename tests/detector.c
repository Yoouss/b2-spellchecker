#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "common.h"
#include <io.h>
#include <stdlib.h>
#include "detector.h"

/**
 * @brief loads the dictionary fr.dict.test using load_dictionaries()'s implementation
 */
Dictionary_t* load_dictionary_for_test() {
    char* dictionaryPath = "fr.dict.test";
   
    // Load dictionnary
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries(dictionaryPath, &dicts, &dicts_count);

    return dicts;
}

/**
 * @brief Helper function of test_words_in_file and inner function of allocate_memory_for_matrix() that frees the matrix's memory
 *
 * @param matrix The excepted matrix representing input_5l_test.pos
 * @param matrixLength The number of rows the matrix has
 */
void free_matrix_input_5l_test(int** matrix, int matrixLength) {
    for (int i = 0; i < matrixLength; i++) {
        if (matrix[i] != NULL) free(matrix[i]);
    }

    free(matrix);
}

/**
 * @brief Inner function of give_expected_output_with_input_5l_test() that allocates memory depending on :
 *
 * @param matrix The future output of give_expected_output_with_input_5l_test()
 * @param index The target row of the allocation
 * @param numberOfElements The numbers of bad words on this row
 * 
 * @return 0 on succeed, -1 on failure after freeing the whole matrix
 */
int allocate_memory_for_matrix(int** matrix, int index, int numberOfElements) {
    matrix[index] = malloc(numberOfElements * sizeof(int));

    if (matrix[index] == NULL) {
        free_matrix_input_5l_test(matrix, index);
        return -1;
    }

    return 0;
}

/**
 * @brief Helper function of test_words_in_file() that gives the expected matrix for the file input_5l_test.txt
 *
 * You can find the expected positions on the file input_5l_test.pos
 * 
 * @return a int** matrix on succeed, NULL on failure
 */
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

void test_word_in_dictionary(void) {
    Dictionary_t* dict = load_dictionary_for_test();

    CU_ASSERT_PTR_NOT_NULL(dict);
    CU_ASSERT_EQUAL(1, word_in_dictionary("bonjour", dict));
    CU_ASSERT_EQUAL(0, word_in_dictionary("mmzjd", dict));
    CU_ASSERT_EQUAL(-1, word_in_dictionary(NULL, dict)); // cas extrême 1 : on ne donne pas de mot à chercher
    CU_ASSERT_EQUAL(-1, word_in_dictionary("bonjour", NULL)); // cas extrême 2 : pas de dictionnaire trouvé 
}


void test_words_in_line(void) {
    Dictionary_t* dict = load_dictionary_for_test();

    char* line_test = "manger une,pomme";//(Younes est dans le dico, "pomme" n'y est pas)
    int line_length = 16;

    int* result = words_in_line(line_test, line_length, dict);
    //"pomme" est le seul mot faux à la 2ème position (on commence à 0)
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
   
    CU_ASSERT_EQUAL(result[0], 2); 
    free(result);

    CU_ASSERT_PTR_NULL(words_in_line(line_test, line_length, NULL));// Test cas extrême : dictionnaire NULL
    
    CU_ASSERT_PTR_NULL(words_in_line(NULL, 0, dict));// Si la ligne est vide
}

void test_words_in_file(void) {
    char* inputPath = "input_5l_test.txt";

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(inputPath, &lines, &lines_sizes, &line_count);

    Dictionary_t* dict = load_dictionary_for_test();

    int** expectedOutput = give_expected_output_with_input_5l_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(expectedOutput);

    int** noFile = words_in_file(NULL, dict);
    CU_ASSERT_PTR_NULL_FATAL(noFile);
    int** noDicts = words_in_file(inputPath, NULL);
    CU_ASSERT_PTR_NULL_FATAL(noDicts);

    int** result = words_in_file(inputPath, dict);

    CU_ASSERT_EQUAL(result[0][0], 3);
    CU_ASSERT_EQUAL(result[0][1], 7);
    CU_ASSERT_EQUAL(result[1][0], 5);
    CU_ASSERT_EQUAL(result[2][0], 4);
    CU_ASSERT_PTR_NULL(result[3]);
    CU_ASSERT_EQUAL(result[4][0], 4);

    free_matrix_input_5l_test(expectedOutput, 5);
}