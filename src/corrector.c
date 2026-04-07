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

        char* word1_current_trigramme = malloc(4 * sizeof(char));
        if (word1_current_trigramme == NULL) return -1;

        word1_current_trigramme[0] = word1[i];
        word1_current_trigramme[1] = word1[i + 1];
        word1_current_trigramme[2] = word1[i + 2];
        word1_current_trigramme[3] = '\0';

        for (int j = 0; j <= word2_size - 3; j++) {

            char* word2_current_trigramme = malloc(4 * sizeof(char));
            if (word2_current_trigramme == NULL) {
                free(word1_current_trigramme);
                return -1;
            }

            word2_current_trigramme[0] = word2[j];
            word2_current_trigramme[1] = word2[j + 1];
            word2_current_trigramme[2] = word2[j + 2];
            word2_current_trigramme[3] = '\0';

            if (strcmp(word1_current_trigramme, word2_current_trigramme) == 0) {
                number_of_shared_trigrammes++;
                free(word2_current_trigramme);
                break;
            }

            free(word2_current_trigramme);

        }

        free(word1_current_trigramme);
    }

    return number_of_shared_trigrammes;
}

char** get_candidate_words(char* wrong_word, Dictionary_t* dict, int* result_count) {
    if (wrong_word == NULL || strlen(wrong_word) == 0 || dict == NULL) return NULL;

    uint32_t dict_size = dict->word_count;

    char** candidate_words = malloc(dict_size * sizeof(char*));
    if (candidate_words == NULL) return NULL;
    
    
    char** words = dict->words;
    int candidate_words_index = 0;
    int number_of_candidates = 0;
    int min_shared_trigrams;

    if (strlen(wrong_word) < 6) {
        min_shared_trigrams = 1;
    }
    else {
        min_shared_trigrams = 2;
    }
    
    for (int i = 0; i < dict_size; i++) {
        char* current_word = words[i];
        if (count_number_of_shared_trigrammes(wrong_word, current_word) >= min_shared_trigrams) {
            candidate_words[candidate_words_index] = current_word;
            candidate_words_index++;
            number_of_candidates++;
        }
    }

    *(result_count) = number_of_candidates;
    return candidate_words;
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
            if (word1[i - 1] == word2[j - 1]) {
                cost = 0;
            }
            else {
                cost = 1;
            }

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

int** get_candidates_distances(char* wrong_word, char** candidates, int nb_candidates) {
    if (wrong_word == NULL || strlen(wrong_word) == 0 || candidates == NULL || nb_candidates <= 0) return NULL;

    int** candidates_distances = malloc((nb_candidates) * sizeof(int*));

    if (candidates_distances == NULL) return NULL;

    for (int i = 0; i < nb_candidates; i++) {
        char* current_word = candidates[i];
        int distance = compute_levenshtein_distance(wrong_word, current_word);

        int* index_distance_array = malloc(2 * sizeof(int));
        
        if (index_distance_array == NULL) {
            for (int j = 0; j < i; j++) {
                free(candidates_distances[j]);
            }
            free(candidates_distances);
            return NULL;
        }

        index_distance_array[0] = i;
        index_distance_array[1] = distance;
        candidates_distances[i] = index_distance_array;
    }

    return candidates_distances;
}

void sort_candidate_distances(int** distance_matrix, int nb_candidates){
    int* actual_word_candidate;
    int j;

    for(int i = 1; i < nb_candidates; i++) {
        actual_word_candidate = distance_matrix[i];
        j = i - 1;

        while (0 <= j && distance_matrix[j][1] > actual_word_candidate[1]) {
            distance_matrix[j + 1] = distance_matrix[j];
            j = j - 1;
        }

        distance_matrix[j + 1] = actual_word_candidate;
    }
}

char* get_word_correction(char* wrong_word, Dictionary_t* dict) {  
    if (wrong_word == NULL || strlen(wrong_word) == 0 || dict == NULL) return NULL;

    int number_of_candidates = 0;
    char** candidate_words = get_candidate_words(wrong_word, dict, &number_of_candidates); 
    if (candidate_words == NULL) return NULL;

    // juste au cas où mais un mot détecté comme mal orthographié a d'office une correction 
    if (number_of_candidates == 0) { 
        free(candidate_words);
        return wrong_word;
    }

    int** candidate_distances = get_candidates_distances(wrong_word, candidate_words, number_of_candidates);

    if (candidate_distances == NULL) {
        free(candidate_words);
        return NULL;
    }

    sort_candidate_distances(candidate_distances, number_of_candidates);

    int word_correction_index = candidate_distances[0][0];
    char* word_correction = strdup(candidate_words[word_correction_index]);

    for (int i = 0; i < number_of_candidates; i++) {
        free(candidate_distances[i]);
    }

    free(candidate_distances);
    free(candidate_words);

    return word_correction;
}
