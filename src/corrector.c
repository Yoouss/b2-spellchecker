#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include "algo.h"
#include <string.h>


char** get_candidate_words(char* wrong_word, Dictionary_t* dict, int* result_count) {
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

        if (current_word[0] == wrong_word[0] && (wrong_word_size >= current_word_size - 3 && wrong_word_size <= current_word_size + 3)) { // on va calculer le soundex que quand la différence de taille entre wrong_word et current_word est dans un intervalle et que les mots commencent par la même lettre 
            char* current_word_soundex = soundex(current_word);

            if (strcmp(current_word_soundex, wrong_word_soundex) == 0) {
                candidate_words[candidate_words_index] = current_word;
                candidate_words_index += 1;
                candidates_counter += 1;
            }
        }
    }

    *(result_count) = candidates_counter;
    return candidate_words;
}


int** get_candidates_distances(char* wrong_word, char** candidates, int nb_candidates) {
    int** candidates_distances = malloc((nb_candidates) * sizeof(int*));

    if (candidates_distances == NULL) return NULL;

    for (int i = 0; i < nb_candidates; i++) {
        char* current_word = candidates[i];
        int distance = calculate_distance(wrong_word, current_word);

        int* index_distance_array = malloc(2 * sizeof(int));
        // nettoyage en cas d'erreur d'allocation de la mémoire
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