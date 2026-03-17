#include <CUnit/CUnit.h> 
#include <CUnit/Basic.h> 
#include <ctype.h>
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include "common.h" 
#include "algo.h" 
#include "dict.h" 
#include "input.h" 
#include "io.h" 

char get_soundex_code(char c);

void test_get_soundex_code(void) {
    CU_ASSERT_EQUAL(get_soundex_code('B'), '1');
    CU_ASSERT_EQUAL(get_soundex_code('R'), '6');
    CU_ASSERT_EQUAL(get_soundex_code('A'), '-');
}
char* soundex(char* word);

void test_soundex(void) {
    char* result = soundex("Robert");
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_STRING_EQUAL(result, "R163");
    free(result);
}

int main() {
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    CU_pSuite pSuite = CU_add_suite("Suite_Soundex", NULL, NULL);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(pSuite, "test de get_soundex_code", test_get_soundex_code);
    CU_add_test(pSuite, "test de soundex complet", test_soundex);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    return CU_get_error();
}
