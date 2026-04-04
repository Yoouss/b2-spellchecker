#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
#include "corrector.h"
#include <string.h>
#include <ctype.h>

char get_soundex_code(char c) {
    c = toupper(c);

    switch (c) {
        case 'B': case 'F': case 'P': case 'V': return '1';
        case 'C': case 'G': case 'J': case 'K': case 'Q': case 'S': case 'X': case 'Z': return '2';
        case 'D': case 'T': return '3';
        case 'L': return '4';
        case 'M': case 'N': return '5';
        case 'R': return '6';
        case 'A': case 'E': case 'I': case 'O': case 'U': case 'Y': case 'H': case 'W': return '-';
        default: return '0';
    }
}

// voir si on garde ou non la fonction
char* soundex(char* word) { // problèmes : si erreurs sur première lettre : susywrk et busywork != code, ignore les voyelles : peler, piler, polaire, pull ont même code, etc.
    if (word == NULL || strlen(word) == 0) return NULL;

    char* soundex_code = malloc(5 * sizeof(char));
    if (soundex_code == NULL) return NULL;

    soundex_code[0] = toupper(word[0]);
    
    int code_index = 1;
    char last_digit = get_soundex_code(word[0]);

    for (int i = 1; word[i] != '\0' && code_index < 4; i++) {
        char current_digit = get_soundex_code(word[i]);

        if (current_digit == '-') {
            last_digit = '-';
            continue;
        }

        if (current_digit != '0' && current_digit != last_digit) {
            soundex_code[code_index] = current_digit;
            code_index++;
            last_digit = current_digit;
        }
    }

    while (code_index < 4) {
        soundex_code[code_index] = '0';
        code_index++;
    }
    
    soundex_code[4] = '\0';
    return soundex_code;
}

char** get_candidate_words(char* wrong_word, Dictionary_t* dict, int* result_count) {
    if (wrong_word == NULL || strlen(wrong_word) == 0 || dict == NULL) return NULL;

    uint32_t dict_size = dict->word_count;

    char** candidate_words = malloc(dict_size * sizeof(char*));
    if (candidate_words == NULL) return NULL;
    
    int wrong_word_size = strlen(wrong_word);
    char* wrong_word_soundex = soundex(wrong_word);
    
    char** words = dict->words;
    int candidate_words_index = 0;
    int candidates_counter = 0;
    
    for (int i = 0; i < dict_size; i++) {
        char* current_word = words[i];
        int current_word_size = strlen(current_word);

        if (wrong_word_size >= current_word_size - 5 && wrong_word_size <= current_word_size + 5) {
            char* current_word_soundex = soundex(current_word);

            if (strcmp(current_word_soundex, wrong_word_soundex) == 0) {
                candidate_words[candidate_words_index] = current_word;
                candidate_words_index += 1;
                candidates_counter += 1;
            }
            free(current_word_soundex);
        }
    }
    
    free(wrong_word_soundex);
    *(result_count) = candidates_counter;
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

int compute_levenshtein_distance(char* word1, char* word2) { // ne s'occupe pas du cas où les mots ont des accents (la distance est la même pour peche : pêche et poche)
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

    // C'est pas normal, les fonctions de corrections ont un problème, je retourne le mauvais mot pour ne pas avoir un seg fault
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
