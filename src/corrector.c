#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
#include "corrector.h"
#include <ctype.h>

int count_number_of_shared_trigrammes(char* word1, char* word2) {
    int number_of_shared_trigrammes = 0;
    int word1_size = strlen(word1);
    int word2_size = strlen(word2);

    if (word1_size < 3 || word2_size < 3) return 0;

    for (int i = 0; i <= word1_size - 3; i++) {
        char word1_current_trigramme[4];
        word1_current_trigramme[0] = word1[i];
        word1_current_trigramme[1] = word1[i + 1];
        word1_current_trigramme[2] = word1[i + 2];
        word1_current_trigramme[3] = '\0';

        for (int j = 0; j <= word2_size - 3; j++) {
            char word2_current_trigramme[4];
            word2_current_trigramme[0] = word2[j];
            word2_current_trigramme[1] = word2[j + 1];
            word2_current_trigramme[2] = word2[j + 2];
            word2_current_trigramme[3] = '\0';

            if (strcmp(word1_current_trigramme, word2_current_trigramme) == 0) {
                number_of_shared_trigrammes++;
                break;
            }
        }
    }

    return number_of_shared_trigrammes;
}

char** get_final_candidates_list(char** candidate_words, int number_of_candidates) {
    if (candidate_words == NULL || number_of_candidates <= 0) return NULL;

    char** final_candidate_words = malloc(number_of_candidates * sizeof(char*));
    if (final_candidate_words == NULL) return NULL;

    for (int i = 0; i < number_of_candidates; i++) {
        final_candidate_words[i] = candidate_words[i];
    }

    free(candidate_words);

    return final_candidate_words;
}

char** get_candidate_words(char* wrong_word, Dictionary_t* dict, int* result_count) {
    int wrong_word_length = strlen(wrong_word);

    if (wrong_word == NULL || wrong_word_length == 0 || dict == NULL) return NULL;

    uint32_t dict_size = dict->word_count;
    char** words = dict->words;

    char** candidate_words = malloc(dict_size * sizeof(char*));
    if (candidate_words == NULL) return NULL;
    
    int candidate_words_index = 0;
    int number_of_candidates = 0;
    
    int min_shared_trigrams;

    if (wrong_word_length < 6) min_shared_trigrams = 1;

    else min_shared_trigrams = 2;
    
    for (int i = 0; i < dict_size; i++) {
        char* current_word = words[i];
        int current_word_length = strlen(current_word);

        if (current_word_length - 2 <= wrong_word_length && current_word_length + 2 >= wrong_word_length && count_number_of_shared_trigrammes(wrong_word, current_word) >= min_shared_trigrams) {
            candidate_words[candidate_words_index] = current_word;
            candidate_words_index++;
            number_of_candidates++;
        }
    }

    *(result_count) = number_of_candidates;

    char** final_candidate_words = get_final_candidates_list(candidate_words, number_of_candidates);
    if (final_candidate_words == NULL) return NULL;

    return final_candidate_words;
}

int get_min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

int** initialize_matrix(int n, int m) {
    int** matrix = malloc((n + 1) * sizeof(int*));
    if (matrix == NULL) return NULL;

    for (int i = 0; i <= n; i++) {
        matrix[i] = malloc((m + 1) * sizeof(int));
        if (matrix[i] == NULL) {
            for (int j = i - 1; j >= 0; j--) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

int compute_levenshtein_distance(char* word1, char* word2) {
    int n = strlen(word1);
    int m = strlen(word2);

    if (n == 0) return m;
    if (m == 0) return n; 
    
    int** distances_matrix = initialize_matrix(n, m);
    if (distances_matrix == NULL) return -1; 

    for (int line = 0; line <= n; line++) {
        distances_matrix[line][0] = line;
    }

    for (int col = 0; col <= m; col++) {
        distances_matrix[0][col] = col;
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m ; j++) {
            int cost;

            if (word1[i - 1] == word2[j - 1]) cost = 0;

            else cost = 1;

            int deletion = distances_matrix[i - 1][j] + 1;       
            int insertion = distances_matrix[i][j - 1] + 1;      
            int substitution = distances_matrix[i - 1][j - 1] + cost; 

            distances_matrix[i][j] = get_min3(deletion, insertion, substitution);
        }
    }

    int distance = distances_matrix[n][m];

    for (int i = 0; i <= n; i++) {
        free(distances_matrix[i]);
    }
    free(distances_matrix);

    return distance;
}

int get_index_of_smallest_distance_word(char* wrong_word, char** candidates, int nb_candidates) {
    if (wrong_word == NULL || strlen(wrong_word) == 0 || candidates == NULL || nb_candidates <= 0) return -1;

    Distance_t* smallest_distance = malloc(sizeof(Distance_t));
    if (smallest_distance == NULL) return -1;

    smallest_distance->index = 0;
    smallest_distance->value = compute_levenshtein_distance(wrong_word, candidates[0]);

    for (int i = 1; i < nb_candidates; i++) {
        char* current_word = candidates[i];
        int current_distance = compute_levenshtein_distance(wrong_word, current_word);

        if (smallest_distance->value > current_distance) {
            smallest_distance->value = current_distance;
            smallest_distance->index = i;
        }
    }

    int smallest_distance_word_index = smallest_distance->index;
    free(smallest_distance);

    return smallest_distance_word_index;
}

char* get_word_correction(char* wrong_word, Dictionary_t* dict) {  
    if (wrong_word == NULL || strlen(wrong_word) == 0 || dict == NULL) return NULL;

    int number_of_candidates = 0;
    char** candidate_words = get_candidate_words(wrong_word, dict, &number_of_candidates); 
    if (candidate_words == NULL) return NULL;

    if (number_of_candidates == 0) { 
        free(candidate_words);
        return wrong_word;
    }

    int smallest_distance_word_index = get_index_of_smallest_distance_word(wrong_word, candidate_words, number_of_candidates);
    char* word_correction = strdup(candidate_words[smallest_distance_word_index]);

    free(candidate_words);

    return word_correction;
}
