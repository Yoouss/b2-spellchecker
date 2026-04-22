#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

// On désactive l'utilisation de threads pour les tests (ne pas supprimer cette variable !)
int num_threads = 1;

void test_word_in_dictionary();
void test_find_candidate_dict_for_line();
void test_wrong_words_count_and_indexes_in_line();
void test_get_wrong_words_in_line();
void test_scan_line_for_errors();

void test_get_candidate_words_manual(); 
void test_compute_levenshtein_distance(); 

void test_read_input_file();
void test_load_dictionaries();

void test_open_outputs();
void test_close_outputs();
void test_write_detection();
void test_write_correction();

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite_detector = CU_add_suite("Détécteur d'erreurs S5", 0, 0);
    CU_add_test(suite_detector, "test_word_in_dictionary", test_word_in_dictionary);
    CU_add_test(suite_detector, "test_find_candidate_dict_for_line", test_find_candidate_dict_for_line);
    CU_add_test(suite_detector, "test_wrong_words_count_and_indexes_in_line", test_wrong_words_count_and_indexes_in_line);
    CU_add_test(suite_detector, "test_get_wrong_words_in_file", test_get_wrong_words_in_line);
    CU_add_test(suite_detector, "test_scan_line_for_errors", test_scan_line_for_errors);

    CU_pSuite suite_corrector = CU_add_suite("Correcteur d'erreurs S6", 0, 0);
    CU_add_test(suite_corrector, "test_get_candidate_words_manual", test_get_candidate_words_manual);
    CU_add_test(suite_corrector, "test_compute_levenshtein_distance", test_compute_levenshtein_distance);

    CU_pSuite suite_file_handler = CU_add_suite("Manipuleur de fichiers S8", 0, 0);
    CU_add_test(suite_file_handler, "test_read_input_file", test_read_input_file);
    CU_add_test(suite_file_handler, "test_load_dictionaries", test_load_dictionaries);

    CU_pSuite suite_io = CU_add_suite("Détéction et Correction I/O S12", 0, 0);
    CU_add_test(suite_io, "test_open_outputs", test_open_outputs);
    CU_add_test(suite_io, "test_close_outputs", test_close_outputs);
    CU_add_test(suite_io, "test_write_detection", test_write_detection);
    CU_add_test(suite_io, "test_write_correction", test_write_correction);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}