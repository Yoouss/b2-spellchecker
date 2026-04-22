#include <CUnit/CUnit.h> 
#include <CUnit/Basic.h> 
#include <ctype.h>
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

#include <corrector.h>

void test_get_candidate_words_manual(void) {
    Dictionary_t dict;
    char* words[] = {"bonjour", "chat", "chien"};
    dict.words = words;
    dict.word_count = 3;

    int count = 0;
    char** results = get_candidate_words("bomjour", &dict, &count);
    
    CU_ASSERT_PTR_NOT_NULL(results);
    // On ne free pas results[0] car c'est un pointeur vers notre tableau statique
    free(results); 
}

void test_compute_levenshtein_distance(void) {
    CU_ASSERT_EQUAL(compute_levenshtein_distance("chien", "chien"), 0);
    CU_ASSERT(compute_levenshtein_distance("chien", "chat") > 0);
}