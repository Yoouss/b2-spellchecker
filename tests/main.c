#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_word_in_dictionary();
void test_words_in_line();
void test_words_in_file();
void test_get_soundex_code(); 
void test_soundex(); 
void test_get_candidate_words_manual(); 
void test_calculate_distance(); 
void test_sort_candidate_distances(); 

int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    // Suite S5
    CU_pSuite suiteS5 = CU_add_suite("Detecteur S5", 0, 0);
    CU_add_test(suiteS5, "word_in_dictionary", test_word_in_dictionary);
    CU_add_test(suiteS5, "words_in_line", test_words_in_line);
    CU_add_test(suiteS5, "words_in_file", test_words_in_file);
    // Suite S6
    CU_pSuite suiteCorrector = CU_add_suite("Corrector S6", 0, 0);
    CU_add_test(suiteCorrector, "get_soundex_code", test_get_soundex_code);
    CU_add_test(suiteCorrector, "soundex", test_soundex);
    CU_add_test(suiteCorrector, "get_candidate_words_manual", test_get_candidate_words_manual);
    CU_add_test(suiteCorrector, "calculate_distance", test_calculate_distance);
    CU_add_test(suiteCorrector, "sort_candidate_distances", test_sort_candidate_distances);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}