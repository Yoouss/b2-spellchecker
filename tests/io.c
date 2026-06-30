#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <common.h>
#include <io.h>
#include <portable_endian.h>

void test_open_outputs(void) {
    OutputStreams_t* no_pathname = open_outputs(NULL);
    CU_ASSERT_PTR_NULL(no_pathname);

    OutputStreams_t* empty_pathname = open_outputs("");
    CU_ASSERT_PTR_NULL(empty_pathname);

    OutputStreams_t* wrong_pathname = open_outputs("fichier.txt");
    CU_ASSERT_PTR_NULL(wrong_pathname);

    OutputStreams_t* res1 = open_outputs("res1");
    CU_ASSERT_PTR_NOT_NULL_FATAL(res1);

    int detection_file_descriptor1 = res1->detection;
    int corrector_file_descriptor1 = res1->correction;

    CU_ASSERT_EQUAL(detection_file_descriptor1, 3);
    CU_ASSERT_EQUAL(corrector_file_descriptor1, 4);

    close(res1->detection);
    close(res1->correction);
    remove("res1.err");
    remove("res1.fix");
    free(res1);

    OutputStreams_t* res2 = open_outputs("res2");
    CU_ASSERT_PTR_NOT_NULL_FATAL(res2);

    int detection_file_descriptor2 = res2->detection;
    int corrector_file_descriptor2 = res2->correction;

    CU_ASSERT_EQUAL(detection_file_descriptor2, 3);
    CU_ASSERT_EQUAL(corrector_file_descriptor2, 4);

    close(res2->detection);
    close(res2->correction);
    remove("res2.err");
    remove("res2.fix");
    free(res2);
}

void test_close_outputs(void) {
    int fd1 = open("test_err.tmp", O_CREAT | O_WRONLY, 0644);
    int fd2 = open("test_fix.tmp", O_CREAT | O_WRONLY, 0644);
    
    OutputStreams_t *streams = malloc(sizeof(OutputStreams_t));
    streams->detection = fd1;
    streams->correction = fd2;

    close_outputs(streams);

    CU_ASSERT_EQUAL(fcntl(fd1, F_GETFD), -1);
    CU_ASSERT_EQUAL(fcntl(fd2, F_GETFD), -1);

    close_outputs(NULL);
    CU_PASS("Succès");

    OutputStreams_t *streams_fail = malloc(sizeof(OutputStreams_t));
    streams_fail->detection = -1;
    streams_fail->correction = -1;

    close_outputs(streams_fail);
    CU_PASS("Les descripteurs invalides ont été ignorés");

    int fd_valide = open("test_valide_nonvalide.tmp", O_CREAT | O_WRONLY, 0644);
    OutputStreams_t *streams_valide_nonvalide = malloc(sizeof(OutputStreams_t));
    streams_valide_nonvalide->detection = fd_valide;
    streams_valide_nonvalide->correction = -1;

    close_outputs(streams_valide_nonvalide);
    CU_ASSERT_EQUAL(fcntl(fd_valide, F_GETFD), -1);
    CU_PASS("Le mélange fd valide / fd -1 est géré");

    remove("test_err.tmp");
    remove("test_fix.tmp");
    remove("test_valide_nonvalide.tmp");
}

void test_write_detection(void) {
    uint32_t word_indices[] = {0, 2, 3};

    int no_output_stream = write_detection(NULL, 2, 0, 3, word_indices);
    CU_ASSERT_EQUAL(no_output_stream, 0);

    OutputStreams_t* output_stream = open_outputs("chat");

    int no_wrong_words = write_detection(output_stream, 5, 2, 0, word_indices);
    CU_ASSERT_NOT_EQUAL(no_wrong_words, -1);
    
    int no_word_indices = write_detection(output_stream, 2, 3, 3, NULL);
    CU_ASSERT_NOT_EQUAL(no_word_indices, -1);

    int normal_case = write_detection(output_stream, 4, 0, 3, word_indices);
    CU_ASSERT_EQUAL(normal_case, 0);

    close_outputs(output_stream);
    remove("chat.err");
    remove("chat.fix");
}

void test_write_correction(void) {
    char* words[] = {"c'est", "frite"};
    uint32_t words_count = 2;
    const char* filename = "test_write.fix";

    CU_ASSERT_EQUAL(write_correction(NULL, words_count, words), 0);

    int file_descriptor = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
    CU_ASSERT_TRUE_FATAL(file_descriptor >= 0);

    OutputStreams_t streams;
    streams.detection = -1;
    streams.correction = file_descriptor;

    int status = write_correction(&streams, words_count, words);
    CU_ASSERT_EQUAL(status, 0);

    lseek(file_descriptor, 0, SEEK_SET);

    uint32_t word_length1;
    read(file_descriptor, &word_length1, sizeof(uint32_t));
    CU_ASSERT_EQUAL(be32toh(word_length1), 6); 

    char word1[6] = {0};
    read(file_descriptor, word1, 6);
    CU_ASSERT_STRING_EQUAL(word1, "c'est");

    uint32_t word_length2;
    read(file_descriptor, &word_length2, sizeof(uint32_t));
    CU_ASSERT_EQUAL(be32toh(word_length2), 6);

    char word2[6] = {0};
    read(file_descriptor, word2, 6);
    CU_ASSERT_STRING_EQUAL(word2, "frite");

    CU_ASSERT_EQUAL(write_correction(&streams, 0, NULL), 0);
    
    close(file_descriptor);
    remove(filename);
}
