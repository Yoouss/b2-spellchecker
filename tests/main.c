#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_word_in_dictionary();
void test_find_candidate_dict_for_line();
void test_number_and_indexes_of_bad_words_in_line();
void test_words_in_line();
void test_words_in_file();

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Détécteur d'erreurs S5", 0, 0);
    CU_add_test(suite, "test_word_in_dictionary", test_word_in_dictionary);
    CU_add_test(suite, "test_find_candidate_dict_for_line", test_find_candidate_dict_for_line);
    CU_add_test(suite, "test_number_and_indexes_of_bad_words_in_line", test_number_and_indexes_of_bad_words_in_line);
    CU_add_test(suite, "test_words_in_line", test_words_in_line);
    CU_add_test(suite, "test_words_in_file", test_words_in_file);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}