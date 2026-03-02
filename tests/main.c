#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_function(void) {
    CU_ASSERT(1 == 1);
}

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("MySuite", 0, 0);
    CU_add_test(suite, "test_function", test_function);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}