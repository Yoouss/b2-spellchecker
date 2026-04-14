#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <io.h>
#include "common.h"
#include "detector.h"

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
    Dictionary_t* dict = NULL;
    size_t dicts_count = 0;

    load_dictionaries("dicts/fr.dict", &dict, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(&dict);

    CU_ASSERT_EQUAL(1, word_in_dictionary("bonjour", dict));
    CU_ASSERT_EQUAL(0, word_in_dictionary("mmzjd", dict));

    CU_ASSERT_EQUAL(-1, word_in_dictionary(NULL, dict)); 
    CU_ASSERT_EQUAL(-1, word_in_dictionary("bonjour", NULL));
}

void test_find_candidate_dict_for_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("all", &dicts, &dicts_count);
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
    printf("\n hello %s \n",candidateDictForLineFr->lang);
    CU_ASSERT_STRING_EQUAL(candidateDictForLineEn->lang, "en");
}

void test_wrong_words_count_and_indexes_in_line(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("all", &dicts, &dicts_count);
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

void test_get_wrong_words_in_line(void) {

    // Test 1 : Aucun mauvais mot
    char* ligne1 = "Bonjour, je suis un ninja";
    uint32_t* indexes_of_bad_word1 = NULL;
    char** res1 = get_wrong_words_in_line(ligne1, indexes_of_bad_word1,0);
    CU_ASSERT_PTR_NULL(res1);
    if (res1 != NULL){
        free(res1);
    }

    // Test 2 : Cas classique
    char* ligne2 = "Bonjour, he çuis un ninja";
    uint32_t indexes_of_bad_word2[] = {1, 2}; // indices 1 et 2
    char** res2 = get_wrong_words_in_line(ligne2, indexes_of_bad_word2,2);
    CU_ASSERT_PTR_NOT_NULL(res2);
    if (res2 != NULL) {
        CU_ASSERT_STRING_EQUAL(res2[0], "he");
        CU_ASSERT_STRING_EQUAL(res2[1], "çuis");
        free(res2[0]); free(res2[1]); free(res2);
    }

    // Test 3 : Nombres 
    char* ligne3 = "1234";
    uint32_t indexes_of_bad_word3[] = {0}; // indice 0
    char** res3 = get_wrong_words_in_line(ligne3, indexes_of_bad_word3,1);
    CU_ASSERT_PTR_NOT_NULL(res3);
    CU_ASSERT_STRING_EQUAL(res3[0],"1234");
    if (res3 != NULL) {
        free(res3[0]); free(res3);
    }

    // Test 4 : Que des ponctuations
    char* ligne4 = ".,.;^?+=";
    uint32_t* indexes_of_bad_word4 = NULL;
    char** res4 = get_wrong_words_in_line(ligne4, indexes_of_bad_word4,0);
    CU_ASSERT_PTR_NULL(res4);
    if (res4 != NULL) {
        free(res4[0]); free(res4);
    }

    // Test 5 : Ligne NULL
    char* ligne5 = NULL;
    uint32_t indexes_of_bad_word5[] = {0,1};
    char** res5 = get_wrong_words_in_line(ligne5, indexes_of_bad_word5,2);
    CU_ASSERT_PTR_NULL(res5);

    // Test 6 : tab index NULL
    char* ligne6 = "Bonjour, he çuis la féequedemanger";
    uint32_t* indexes_of_bad_word6 = NULL;
    char** res6 = get_wrong_words_in_line(ligne6, indexes_of_bad_word6,0);
    CU_ASSERT_PTR_NULL(res6);

    // Test 7 : tout est NULL
    char* ligne7 = NULL;
    uint32_t* indexes_of_bad_word7 = NULL;
    char** res7 = get_wrong_words_in_line(ligne7, indexes_of_bad_word7,0);
    CU_ASSERT_PTR_NULL(res7);

    // Test 8 : tab vide
    char* ligne8 = "";
    char* ligne8_bis = "Je suis une justicière masquée";

    uint32_t* indexes_of_bad_word8 = NULL;
    char** res8 = get_wrong_words_in_line(ligne8, indexes_of_bad_word8, 0);
    char** res8_bis = get_wrong_words_in_line(ligne8_bis, indexes_of_bad_word8,0);

    CU_ASSERT_PTR_NULL(res8);
    CU_ASSERT_PTR_NULL(res8_bis);

    if(res8 != NULL) free(res8);
    if(res8_bis != NULL) free(res8_bis);

    // Test 9 : erreur outOfIndex
    char* ligne9 = "Hurlement du drakon de feu ";
    uint32_t indexes_of_bad_word9[] = {2,5};
    char** res9 = get_wrong_words_in_line(ligne9, indexes_of_bad_word9, 2);
    
    CU_ASSERT_PTR_NOT_NULL(res9);
    CU_ASSERT_STRING_EQUAL(res9[0],"drakon");
    if (res9 != NULL) {
        if (res9[0]) free(res9[0]);
        if (res9[1]) free(res9[1]);
        free(res9);
    }
}

void test_scan_line_for_errors(void) {
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("all", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    char* line_test = "manger une,pommeee"; // "pommeee" n'est pas dans le dictionnaire test

    line_t* line_detection = scan_line_for_errors(line_test, dicts, dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(line_detection);

    uint32_t* wrong_words_indexes = line_detection->wrong_words_indexes;
    uint32_t wrong_words_count = line_detection->wrong_words_count;

    CU_ASSERT_EQUAL(wrong_words_count, 1)
    CU_ASSERT_EQUAL(wrong_words_indexes[0], 2); 
    free(line_detection);

    line_t* no_line = scan_line_for_errors(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL(no_line);
    free_line_detection(no_line);

    line_t* no_dicts = scan_line_for_errors(line_test, NULL, dicts_count);
    CU_ASSERT_PTR_NULL(no_dicts);
    free_line_detection(no_dicts);

    line_t* zero_dicts = scan_line_for_errors(line_test, dicts, 0);
    CU_ASSERT_PTR_NULL(zero_dicts);
    free_line_detection(zero_dicts);
}

void test_scan_file_for_errors(void) {
    char* inputPath = "inputs/input_10l.txt";

    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    load_dictionaries("all", &dicts, &dicts_count);
    CU_ASSERT_PTR_NOT_NULL_FATAL(dicts);

    file_t* no_file = scan_file_for_errors(NULL, dicts, dicts_count);
    CU_ASSERT_PTR_NULL(no_file);
    free_file_detection(no_file);

    file_t* no_dicts = scan_file_for_errors(inputPath, NULL, dicts_count);
    CU_ASSERT_PTR_NULL(no_dicts);
    free_file_detection(no_dicts);

    file_t* zero_dicts = scan_file_for_errors(inputPath, dicts, 0);
    CU_ASSERT_PTR_NULL(zero_dicts);
    free_file_detection(zero_dicts);

    file_t* file_detection = scan_file_for_errors(inputPath, dicts, dicts_count);
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