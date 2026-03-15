#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include "algo.h"
#include <string.h>


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