#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <file_handler.h>

void test_read_input_file(void) {
    char* input_path = "inputs/input_10l.txt";
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    CU_ASSERT_EQUAL(read_input_file(input_path, &lines, &lines_sizes, &line_count), 0);
    CU_ASSERT_EQUAL(line_count, 10);
    CU_ASSERT_PTR_NOT_NULL(lines);
    CU_ASSERT_PTR_NOT_NULL(lines_sizes);
}

void test_load_dictionaries(void) {
    char* dictionaries_path = "dicts";
    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    CU_ASSERT_EQUAL(load_dictionaries(dictionaries_path, &dicts, &dicts_count), 0);
    CU_ASSERT_PTR_NOT_NULL(dicts);
    CU_ASSERT(dicts_count > 0);
}