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

void test_word_in_dictionary(void) {
    Dictionary_t dict = load_dictionary_for_test();
    CU_ASSERT_PTR_NOT_NULL_FATAL(&dict);

    CU_ASSERT_EQUAL(1, word_in_dictionary("bonjour", &dict));
    CU_ASSERT_EQUAL(0, word_in_dictionary("mmzjd", &dict));

    CU_ASSERT_EQUAL(-1, word_in_dictionary(NULL, &dict)); 
    CU_ASSERT_EQUAL(-1, word_in_dictionary("bonjour", NULL));
}

void test_find_candidate_dict_for_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    Dictionary_t* noFile = find_candidate_dict_for_line(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL_FATAL(noFile);
    Dictionary_t* noDicts = find_candidate_dict_for_line("I am a line", NULL, dicts_count);
    CU_ASSERT_PTR_NULL_FATAL(noDicts);
    Dictionary_t* InvalidNumberOfDicts = find_candidate_dict_for_line("I am a line", dicts, 0);
    CU_ASSERT_PTR_NULL_FATAL(InvalidNumberOfDicts);
    
    char* lineFr = "je suis supper occupé et j'aime les boulettes";
    Dictionary_t* candidateDictForLineFr = find_candidate_dict_for_line(lineFr, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(candidateDictForLineFr);

    char* lineEn = "I am veryy tired and sleepy";
    Dictionary_t* candidateDictForLineEn = find_candidate_dict_for_line(lineEn, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(candidateDictForLineEn);

    CU_ASSERT_STRING_EQUAL(candidateDictForLineFr->lang, "fr");
    CU_ASSERT_STRING_EQUAL(candidateDictForLineEn->lang, "en");
}

void test_number_and_indexes_of_bad_words_in_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    // ─────────────── TEST 1 ───────────────
    // Au moins une faute détectée

    char* lineFr = "prestant d\351billardez n'insuffla d'xaltante p\351nicilliums t'accablassiez sublim\342tes tois\342tes accouch\351 n'octaviant n'\351palasse qu'agresserai l'enivreraient santonnerai am\351liorai t'habill\350rent t'all\351guerait qu'enrayasses magasini\350re";

    Dictionary_t* candidateDictForLineFr = find_candidate_dict_for_line(lineFr, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(candidateDictForLineFr);
    CU_ASSERT_STRING_EQUAL(candidateDictForLineFr->lang, "fr");

    uint32_t numberOfBadWords = number_of_bad_words_in_line(lineFr, candidateDictForLineFr);
    CU_ASSERT_NOT_EQUAL_FATAL(numberOfBadWords, -1);

    CU_ASSERT_EQUAL(numberOfBadWords, 1); 

    uint32_t* indexesOfBadWords = get_indexes_of_bad_words_in_line(lineFr, numberOfBadWords, candidateDictForLineFr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(indexesOfBadWords);

    CU_ASSERT_EQUAL(indexesOfBadWords[0], 1);
    CU_ASSERT_EQUAL(indexesOfBadWords[1], 3);

    free(indexesOfBadWords);

    // ─────────────── TEST 2 ───────────────
    // Aucune faute détectée

    char* lineEn = "whether you win or lose, looking back and learning from your experiences is a part of life";

    Dictionary_t* candidateDictForLineEn = find_candidate_dict_for_line(lineEn, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(candidateDictForLineEn);
    CU_ASSERT_STRING_EQUAL(candidateDictForLineEn->lang, "en");

    numberOfBadWords = number_of_bad_words_in_line(lineEn, candidateDictForLineEn);
    CU_ASSERT_NOT_EQUAL_FATAL(numberOfBadWords, -1);

    CU_ASSERT_EQUAL(numberOfBadWords, 0); 

    indexesOfBadWords = get_indexes_of_bad_words_in_line(lineEn, numberOfBadWords, candidateDictForLineEn);
    CU_ASSERT_PTR_NOT_NULL_FATAL(indexesOfBadWords);

    CU_ASSERT_EQUAL(indexesOfBadWords[0], 0);

    free(indexesOfBadWords);
}

void test_words_in_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    char* line_test = "manger une,pommeee"; // "pommeee" n'est pas dans le dictionnaire test

    uint32_t* result = words_in_line(line_test, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
   
    CU_ASSERT_EQUAL(result[1], 2); 
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

    uint32_t** noFile = words_in_file(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL_FATAL(noFile);
    uint32_t** noDicts = words_in_file(inputPath, NULL, dicts_count);
    CU_ASSERT_PTR_NULL_FATAL(noDicts);
    uint32_t** invalidDictsCount = words_in_file(inputPath, dicts, 0);
    CU_ASSERT_PTR_NULL_FATAL(invalidDictsCount);

    uint32_t** matrixOfBadWordsIndexes = words_in_file(inputPath, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(matrixOfBadWordsIndexes);

    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[0][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[0][1], 3);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[1][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[1][1], 10);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[2][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[2][1], 7);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[3][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[3][1], 8);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[4][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[4][1], 7);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[5][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[5][1], 3);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[6][0], 0);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[7][0], 0);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[8][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[8][1], 0);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[9][0], 1);
    CU_ASSERT_EQUAL(matrixOfBadWordsIndexes[9][1], 4);

    free_matrix(matrixOfBadWordsIndexes, 10);
}