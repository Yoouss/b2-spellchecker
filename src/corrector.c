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


int min3(int a, int b, int c){
    int ref = a;
    if (b<ref) ref=b;
    if (c<ref) ref = c;
    return ref;
}
int calculate_distance(char* word1, char* word2){
    int n = strlen(word1);
    int m = strlen(word2);
    //cas de base
    if (n==0) return NULL;
    if (m==0) return NULL;
    //construire la matrice
    int matrix[n+1][m+1];

    //initialiser la 1ére col et la 1ére ligne à 0->n
    for (int ligne=0;ligne<=n;ligne++){
        matrix[ligne][0] = ligne;
    }
    for (int col = 0;col<=m;col++){
        matrix[0][col] = col;
    }

    for (int i = 0;i<=n;i++){
        for (int j = 0;j<=m;j++){
            int cost;
            //si les lettres sont égales ou non
            if(word1[i]==word2[j]){
                cost = 0;
            }else{cost =1;}

            // [i,j] est egal au min de :
            int deletion = matrix[i - 1][j] + 1;       
            int insertion = matrix[i][j - 1] + 1;      
            int substitution = matrix[i - 1][j - 1] + cost; 

            int min = deletion;
            if (insertion < min) min = insertion;
            if (substitution < min) min = substitution;

            matrix[i][j] = min;


        }
    }
    return matrix[n][m];





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