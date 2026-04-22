#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <unistd.h>
#include <io.h>
#include "common.h"

void test_open_outputs(void) {
    OutputStreams_t* no_pathname = open_outputs(NULL);
    CU_ASSERT_PTR_NULL(no_pathname);

    OutputStreams_t* empty_pathname = open_outputs("");
    CU_ASSERT_PTR_NULL(empty_pathname);

    OutputStreams_t* wrong_pathname = open_outputs("fichier.txt");
    CU_ASSERT_PTR_NULL(wrong_pathname);

    OutputStreams_t* res1 = open_outputs("test");
    CU_ASSERT_PTR_NOT_NULL_FATAL(res1);

    int detection_file_descriptor1 = res1->detection;
    int corrector_file_descriptor1 = res1->correction;

    CU_ASSERT_EQUAL(detection_file_descriptor1, 3);
    CU_ASSERT_EQUAL(corrector_file_descriptor1, 4);

    close(res1->detection);
    close(res1->correction);
    free(res1);

    OutputStreams_t* res2 = open_outputs("chats");
    CU_ASSERT_PTR_NOT_NULL_FATAL(res2);

    int detection_file_descriptor2 = res2->detection;
    int corrector_file_descriptor2 = res2->correction;

    CU_ASSERT_EQUAL(detection_file_descriptor2, 3);
    CU_ASSERT_EQUAL(corrector_file_descriptor2, 4);

    close(res2->detection);
    close(res2->correction);
    free(res2);
}

void test_close_outputs(void) {
    // TODO
    CU_ASSERT(1 == 1);
}

void test_write_detection(void) {
    uint32_t word_indices[] = {0, 2, 3};

    int no_output_stream = write_detection(NULL, 2, 0, 3, word_indices);
    CU_ASSERT_EQUAL(no_output_stream, 0);

    OutputStreams_t* output_stream = open_outputs("test");

    int no_wrong_words = write_detection(output_stream, 5, 2, 0, word_indices);
    CU_ASSERT_EQUAL(no_wrong_words, -1);
    
    int no_word_indices = write_detection(output_stream, 2, 3, 3, NULL);
    CU_ASSERT_EQUAL(no_word_indices, -1);
    
    int no_word_indices_and_no_wrong_words = write_detection(output_stream, 2, 3, 0, NULL);
    CU_ASSERT_EQUAL(no_word_indices_and_no_wrong_words, -1);

    int normal_case = write_detection(output_stream, 4, 0, 3, word_indices);
    CU_ASSERT_EQUAL(normal_case, 0);

    close(output_stream->detection);
    close(output_stream->correction);
    free(output_stream);
}

void test_write_correction(void) {
    // TODO
    CU_ASSERT(1 == 1);
}