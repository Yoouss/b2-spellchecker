#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_word_in_dictionary();
void test_words_in_line();
void test_words_in_file();
void test_get_soundex_code();
void test_soundex();
void test_calculate_distance();
void test_get_candidate_words();

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Détécteur d'erreurs S5", 0, 0);
    CU_add_test(suite, "test_word_in_dictionary", test_word_in_dictionary);
    CU_add_test(suite, "test_words_in_line", test_words_in_line);
    CU_add_test(suite, "test_words_in_file", test_words_in_file);
    CU_pSuite suite = CU_add_suite("Correcteur d'erreurs S6", 0, 0);
    CU_add_test(suite, "test de get_soundex_code", test_get_soundex_code);
    CU_add_test(suite, "test de soundex complet", test_soundex);
    CU_add_test(suite, "test_get_candidate_words", test_get_candidate_words);
    CU_add_test(suite, "test_calculate_distance", test_calculate_distance);

    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}