#include "list.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdlib.h>

/* Test incomplet : ne couvre qu'un seul cas */
void test_list_contains_incomplet(void)
{
    struct node *list = list_insert(NULL, 42);
    CU_ASSERT_EQUAL(list_contains(list, 42), 1);
    list_free(list);
}

int main(void)
{
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("list", NULL, NULL);
    CU_add_test(suite, "contains_incomplet", test_list_contains_incomplet);
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
