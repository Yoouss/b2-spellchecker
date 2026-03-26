#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_word_in_dictionary();
void test_find_candidate_dict_for_line();
void test_number_and_indexes_of_bad_words_in_line();
void test_words_in_line();
void test_words_in_file();

void test_get_soundex_code(); 
void test_soundex(); 
void test_get_candidate_words_manual(); 
void test_calculate_distance(); 
void test_sort_candidate_distances(); 

void test_read_input_file();
void test_load_dictionaries();

int main(void) {
    CU_initialize_registry();
    CU_pSuite suiteDetector = CU_add_suite("Détécteur d'erreurs S5", 0, 0);
    CU_add_test(suiteDetector, "test_word_in_dictionary", test_word_in_dictionary);
    CU_add_test(suiteDetector, "test_find_candidate_dict_for_line", test_find_candidate_dict_for_line);
    CU_add_test(suiteDetector, "test_number_and_indexes_of_bad_words_in_line", test_number_and_indexes_of_bad_words_in_line);
    CU_add_test(suiteDetector, "test_words_in_line", test_words_in_line);
    CU_add_test(suiteDetector, "test_words_in_file", test_words_in_file);

    CU_pSuite suiteCorrector = CU_add_suite("Correcteur d'erreurs S6", 0, 0);
    CU_add_test(suiteCorrector, "test_get_soundex_code", test_get_soundex_code);
    CU_add_test(suiteCorrector, "test_soundex", test_soundex);
    CU_add_test(suiteCorrector, "test_get_candidate_words_manual", test_get_candidate_words_manual);
    CU_add_test(suiteCorrector, "test_calculate_distance", test_calculate_distance);
    CU_add_test(suiteCorrector, "test_sort_candidate_distances", test_sort_candidate_distances);

    CU_pSuite suiteFileHandler = CU_add_suite("Manipuleur de fichiers S8", 0, 0);
    CU_add_test(suiteFileHandler, "test_read_input_file", test_read_input_file);
    CU_add_test(suiteFileHandler, "test_load_dictionaries", test_load_dictionaries);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}