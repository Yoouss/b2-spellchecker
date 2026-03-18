#include <CUnit/CUnit.h> 
#include <CUnit/Basic.h> 
#include <ctype.h>
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

#include "common.h" 
#include "algo.h" 
#include "dict.h"

void test_get_soundex_code(void) {
    CU_ASSERT_EQUAL(get_soundex_code('B'), '1');
    CU_ASSERT_EQUAL(get_soundex_code('R'), '6');
    CU_ASSERT_EQUAL(get_soundex_code('A'), '-');
}

void test_soundex(void) {
    char* result = soundex("Robert");
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_STRING_EQUAL(result, "R163");
    free(result);
}

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

void test_calculate_distance(void) {
    CU_ASSERT_EQUAL(calculate_distance("chien", "chien"), 0);
    CU_ASSERT(calculate_distance("chien", "chat") > 0);
}

void test_sort_candidate_distances(void){
    int nb_candidates = 3;
    
    // 1. Allocation de la matrice (tableau de pointeurs)
    int** matrix = malloc(nb_candidates * sizeof(int*));
    for (int i = 0; i < nb_candidates; i++) {
        matrix[i] = malloc(2 * sizeof(int));
    }

    // données non triées
    matrix[0][0] = 10; matrix[0][1] = 5;  // Candidat A, dist 5
    matrix[1][0] = 20; matrix[1][1] = 2;  // Candidat B, dist 2
    matrix[2][0] = 30; matrix[2][1] = 8;  // Candidat C, dist 8

    sort_candidate_distances(matrix, nb_candidates);

    // Le plus petit (dist 2) doit être en premier
    CU_ASSERT_EQUAL(matrix[0][1], 2);
    CU_ASSERT_EQUAL(matrix[0][0], 20);
    CU_ASSERT_EQUAL(matrix[1][1], 5);
    CU_ASSERT_EQUAL(matrix[1][0], 10);
    CU_ASSERT_EQUAL(matrix[2][1], 8);
    CU_ASSERT_EQUAL(matrix[2][0], 30);

    for (int i = 0; i < nb_candidates; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
