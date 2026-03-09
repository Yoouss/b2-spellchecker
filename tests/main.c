#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>


void test_word_in_dictionary();

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Détecteur d'erreurs S5", 0, 0);
    CU_add_test(suite, "test_word_in_dictionary", test_word_in_dictionary);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}