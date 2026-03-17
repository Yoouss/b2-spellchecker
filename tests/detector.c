#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <io.h>
#include "common.h"
#include "detector.h"

/**
 * @brief loads the dictionaries using load_dictionaries()'s implementation
 * 
 * @return a pointer to the Dictionary_t* dictionaries for the tests
 */
Dictionary_t* load_dictionaries_for_test() {
    char* dictionaryPath = "dummy"; // il sert à rien pour l'instant

    // Load dictionnary
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries(dictionaryPath, &dicts, &dicts_count);

    return dicts;
}

/**
 * @brief loads the dictionaries using load_dictionaries()'s implementation
 * 
 * @return a pointer to the Dictionary_t* dictionaries for the tests
 */
Dictionary_t load_dictionary_for_test() {
    return load_dictionaries_for_test()[1];
}

/**
 * @brief Helper function of test_words_in_file and inner function of allocate_memory_for_matrix() that frees the matrix's memory
 *
 * @param matrix The excepted matrix
 * @param matrixLength The number of rows the matrix has
 */
void free_matrix(int** matrix, int matrixLength) {
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
short allocate_memory_for_matrix(int** matrix, int index, int numberOfElements) {
    matrix[index] = malloc(numberOfElements * sizeof(int));

    if (matrix[index] == NULL) {
        free_matrix(matrix, index);
        return -1;
    }

    return 0;
}

/**
 * @brief Helper function of test_words_in_file() that gives the expected matrix for the file input_10l.txt
 *
 * You can find the expected positions on the file input_10l.pos
 * 
 * @return a int** matrix on succeed, NULL on failure
 */
int** give_expected_output_with_input_10l() {
    int** matrix = malloc(10 * sizeof(int *));
    if (matrix == NULL) return NULL;
    short status;

    status = allocate_memory_for_matrix(matrix, 0, 2);
    if (status == -1) return NULL;
    matrix[0][0] = 0;
    matrix[0][1] = 3;

    status = allocate_memory_for_matrix(matrix, 1, 2);
    if (status == -1) return NULL;
    matrix[1][0] = 1;
    matrix[1][1] = 10;

    status = allocate_memory_for_matrix(matrix, 2, 2);
    if (status == -1) return NULL;
    matrix[2][0] = 2;
    matrix[2][1] = 7;

    status = allocate_memory_for_matrix(matrix, 3, 2);
    if (status == -1) return NULL;
    matrix[3][0] = 3;
    matrix[3][1] = 8;

    status = allocate_memory_for_matrix(matrix, 4, 2);
    if (status == -1) return NULL;
    matrix[4][0] = 4;
    matrix[4][1] = 7;

    status = allocate_memory_for_matrix(matrix, 5, 2);
    if (status == -1) return NULL;
    matrix[5][0] = 5;
    matrix[5][1] = 3;

    status = allocate_memory_for_matrix(matrix, 6, 1);
    if (status == -1) return NULL;
    matrix[6][0] = 6;

    status = allocate_memory_for_matrix(matrix, 7, 1);
    if (status == -1) return NULL;
    matrix[7][0] = 7;

    status = allocate_memory_for_matrix(matrix, 8, 2);
    if (status == -1) return NULL;
    matrix[8][0] = 8;
    matrix[8][1] = 0;

    status = allocate_memory_for_matrix(matrix, 9, 2);
    if (status == -1) return NULL;
    matrix[9][0] = 9;
    matrix[9][1] = 4;

    return matrix;
}

void test_word_in_dictionary(void) {
    Dictionary_t dict = load_dictionary_for_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(&dict);

    CU_ASSERT_EQUAL(1, word_in_dictionary("bonjour", &dict));
    CU_ASSERT_EQUAL(0, word_in_dictionary("mmzjd", &dict));

    CU_ASSERT_EQUAL(-1, word_in_dictionary(NULL, &dict)); 
    CU_ASSERT_EQUAL(-1, word_in_dictionary("bonjour", NULL));
}


void test_words_in_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    char* line_test = "manger une,pommeee"; // "pommeee" n'est pas dans le dictionnaire test

    int* result = words_in_line(line_test, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
   
    CU_ASSERT_EQUAL(result[0], 2); 
    free(result);

    CU_ASSERT_PTR_NULL(words_in_line(line_test, NULL, dicts_count));
    CU_ASSERT_PTR_NULL(words_in_line(NULL, dicts, dicts_count));
    CU_ASSERT_PTR_NULL(words_in_line(line_test, dicts, 0));
}

void test_words_in_file(void) {
    char* inputPath = "input_10l.txt";

    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    int** expectedOutput = give_expected_output_with_input_10l();
    CU_ASSERT_PTR_NOT_NULL_FATAL(expectedOutput);

    int** noFile = words_in_file(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL_FATAL(noFile);
    int** noDicts = words_in_file(inputPath, NULL, dicts_count);
    CU_ASSERT_PTR_NULL_FATAL(noDicts);
    int** invalidDictsCount = words_in_file(inputPath, dicts, 0);
    CU_ASSERT_PTR_NULL_FATAL(invalidDictsCount);

    int** matrixOfBadWordsIndexes = words_in_file(inputPath, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(matrixOfBadWordsIndexes);
    

    printf("%i\n", matrixOfBadWordsIndexes[0][0]);
printf("%i\n", matrixOfBadWordsIndexes[0][1]);
printf("%i\n", matrixOfBadWordsIndexes[1][0]);
printf("%i\n", matrixOfBadWordsIndexes[1][1]);
printf("%i\n", matrixOfBadWordsIndexes[2][0]);
printf("%i\n", matrixOfBadWordsIndexes[2][1]);
printf("%i\n", matrixOfBadWordsIndexes[3][0]);
printf("%i\n", matrixOfBadWordsIndexes[3][1]);
printf("%i\n", matrixOfBadWordsIndexes[4][0]);
printf("%i\n", matrixOfBadWordsIndexes[4][1]);
printf("%i\n", matrixOfBadWordsIndexes[5][0]);
printf("%i\n", matrixOfBadWordsIndexes[5][1]);
printf("%i\n", matrixOfBadWordsIndexes[6][0]);
printf("%i\n", matrixOfBadWordsIndexes[7][0]);
printf("%i\n", matrixOfBadWordsIndexes[8][0]);
printf("%i\n", matrixOfBadWordsIndexes[8][1]);
printf("%i\n", matrixOfBadWordsIndexes[9][0]);
printf("%i\n", matrixOfBadWordsIndexes[9][1]);

    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[0][0], 0);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[0][1], 3);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[1][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[1][1], 10);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[2][0], 2);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[2][1], 7);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[3][0], 3);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[3][1], 8);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[4][0], 4);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[4][1], 7);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[5][0], 5);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[5][1], 3);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[6][0], 6);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[7][0], 7);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[8][0], 8);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[8][1], 0);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[9][0], 9);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[9][1], 4);

    free_matrix(expectedOutput, 10);
    free_matrix(matrixOfBadWordsIndexes, 10);
}