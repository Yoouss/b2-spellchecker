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
    char* dictionaryPath = "dicts/"; // il sert seulement après implémentation du fichier file_handler.c
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

static void free_line_detection(line_t* line_detection) {
    if (line_detection == NULL) return;

    free(line_detection->wrong_words_indexes);
    free(line_detection);
}

static void free_file_detection(file_t* file_detection) {
    if (file_detection == NULL) return;

    size_t incorrect_lines_count = file_detection->incorrect_lines_count;
    for (size_t i = 0; i < incorrect_lines_count; i++) {
        free(file_detection->incorrect_lines[i].wrong_words_indexes);
    }

    free(file_detection->incorrect_lines_indexes);
    free(file_detection);
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

void test_wrong_words_count_and_indexes_in_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    // ─────────────── TEST 1 ───────────────
    // Au moins une faute détectée

    char* line_fr = "prestant d\351billardez n'insuffla d'xaltante p\351nicilliums t'accablassiez sublim\342tes tois\342tes accouch\351 n'octaviant n'\351palasse qu'agresserai l'enivreraient santonnerai am\351liorai t'habill\350rent t'all\351guerait qu'enrayasses magasini\350re";

    Dictionary_t* candidate_dict_for_line_fr = find_candidate_dict_for_line(line_fr, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(candidate_dict_for_line_fr);
    CU_ASSERT_STRING_EQUAL(candidate_dict_for_line_fr->lang, "fr");
    
    uint32_t wrong_words_count = 0;
    int status = set_wrong_words_count_in_line(line_fr, &wrong_words_count, candidate_dict_for_line_fr);
    CU_ASSERT_NOT_EQUAL_FATAL(status, -1);

    CU_ASSERT_EQUAL(wrong_words_count, 1); 

    uint32_t* wrong_words_indexes = get_wrong_words_indexes_in_line(line_fr, wrong_words_count, candidate_dict_for_line_fr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(wrong_words_indexes);

    CU_ASSERT_EQUAL(wrong_words_indexes[0], 3);

    free(wrong_words_indexes);

    // ─────────────── TEST 2 ───────────────
    // Aucune faute détectée

    char* line_en = "whether you win or lose, looking back and learning from your experiences is a part of life";

    Dictionary_t* candidate_dict_for_line_en = find_candidate_dict_for_line(line_en, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(candidate_dict_for_line_en);
    CU_ASSERT_STRING_EQUAL(candidate_dict_for_line_en->lang, "en");

    status = set_wrong_words_count_in_line(line_en, &wrong_words_count, candidate_dict_for_line_en);
    CU_ASSERT_NOT_EQUAL_FATAL(status, -1);

    // Ce test permet également de vérifier si le compteur est bien réinitialisé à chaque appel de set_wrong_words_count_in_line
    CU_ASSERT_EQUAL(wrong_words_count, 0);

    wrong_words_indexes = get_wrong_words_indexes_in_line(line_en, wrong_words_count, candidate_dict_for_line_en);
    CU_ASSERT_PTR_NULL(wrong_words_indexes);

    // Au cas où wrong_words_indexes != NULL -> Pas sensé être le cas
    free(wrong_words_indexes);
}

void test_scan_line_for_wrong_words(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    char* line_test = "manger une,pommeee"; // "pommeee" n'est pas dans le dictionnaire test

    line_t* line_detection = scan_line_for_wrong_words(line_test, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(line_detection);

    uint32_t* wrong_words_indexes = line_detection->wrong_words_indexes;
    uint32_t wrong_words_count = line_detection->wrong_words_count;

    CU_ASSERT_EQUAL(wrong_words_count, 1)
    CU_ASSERT_EQUAL(wrong_words_indexes[0], 2); 
    free(line_detection);

    CU_ASSERT_PTR_NULL(scan_line_for_wrong_words(line_test, NULL, dicts_count));
    CU_ASSERT_PTR_NULL(scan_line_for_wrong_words(NULL, dicts, dicts_count));
    CU_ASSERT_PTR_NULL(scan_line_for_wrong_words(line_test, dicts, 0));

    line_t* no_line = scan_line_for_wrong_words(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL(no_line);
    free_line_detection(no_line);

    line_t* no_dicts = scan_line_for_wrong_words(line_test, NULL, dicts_count);
    CU_ASSERT_PTR_NULL(no_dicts);
    free_line_detection(no_dicts);

    line_t* zero_dicts = scan_line_for_wrong_words(line_test, dicts, 0);
    CU_ASSERT_PTR_NULL(zero_dicts);
    free_line_detection(zero_dicts);
}

void test_scan_file_for_wrong_words(void) {
    char* inputPath = "input_10l.txt";

    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dummy", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    file_t* no_file = scan_file_for_wrong_words(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL(no_file);
    free_file_detection(no_file);

    file_t* no_dicts = scan_file_for_wrong_words(inputPath, NULL, dicts_count);
    CU_ASSERT_PTR_NULL(no_dicts);
    free_file_detection(no_dicts);

    file_t* zero_dicts = scan_file_for_wrong_words(inputPath, dicts, 0);
    CU_ASSERT_PTR_NULL(zero_dicts);
    free_file_detection(zero_dicts);

    file_t* file_detection = scan_file_for_wrong_words(inputPath, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(file_detection);

    line_t* incorrect_lines = file_detection->incorrect_lines;
    size_t* incorrect_lines_indexes = file_detection->incorrect_lines_indexes;
    size_t incorrect_lines_count = file_detection->incorrect_lines_count;

    CU_ASSERT_EQUAL(incorrect_lines_count, 8);

    for (size_t i = 0; i < incorrect_lines_count; i++) {
        line_t* line = &incorrect_lines[i];
        size_t line_index = incorrect_lines_indexes[i];

        switch (line_index) {
            case 0:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 3);
                break;
            case 1:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 10);
                break;
            case 2:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 7);
                break;
            case 3:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 8);
                break;
            case 4:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 7);
                break;
            case 5:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 3);
                break;
            case 8:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 0);
                break;
            case 9:
                CU_ASSERT_EQUAL(line->wrong_words_count, 1);
                CU_ASSERT_EQUAL(line->wrong_words_indexes[0], 4);
                break;
            default:
                CU_FAIL("Une ligne du fichier %s a été détéctée avec au moins une erreur alors qu'elle n'est pas sensée en avoir...");
                break;
        }
    }

    free_file_detection(file_detection);
}