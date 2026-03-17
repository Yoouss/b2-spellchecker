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

Dictionary_t* load_dictionary_for_test() {
    char* dictionaryPath = "fr.dict.test";
    // Load dictionnary
    Dictionary_t* dict = NULL;
    size_t dicts_count = 0;

    load_dictionaries(dictionaryPath, &dict, &dicts_count);

    return dict;
}

void test_get_candidate_words(void){
    char* wrong = "bomjour";
    int result_count=0;
    Dictionary_t* dict=load_dictionary_for_test();

    char** results=get_candidate_words(wrong,dict,&result_count);
    CU_ASSERT_PTR_NOT_NULL(results);
    CU_ASSERT(result_count >= 0);

    if (result_count > 0) {
        CU_ASSERT_PTR_NOT_NULL(results[0]);
    }
}

void test_calculate_distance(void){
    char* word1="chien";
    char* word2="chien";

    int distance1 =calculate_distance(word1,word2);
    CU_ASSERT_FALSE(distance1);
    char* word3="chien";
    char* word4="chat";
    int distance2 =calculate_distance(word3,word4);
    CU_ASSERT(distance2 > 0);

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
