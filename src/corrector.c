#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include "algo.h"
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
char* soundex(char* word) {
    if (word == NULL || strlen(word) == 0) return NULL;

    char* code = malloc(5 * sizeof(char));
    if (code == NULL) return NULL;

    code[0] = toupper(word[0]);
    
    int code_index = 1;
    char last_digit = get_soundex_code(word[0]);

    for (int i = 1; word[i] != '\0' && code_index < 4; i++) {
        char current_digit = get_soundex_code(word[i]);

        if (current_digit == '-') {
            last_digit = '-';
            continue;
        }

        if (current_digit != '0' && current_digit != last_digit) {
            code[code_index] = current_digit;
            code_index++;
            last_digit = current_digit;
        }
    }

    while (code_index < 4) {
        code[code_index] = '0';
        code_index++;
    }

    code[4] = '\0';
    return code;
}


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
    if (n==0) return -1;
    if (m==0) return -1;
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

            matrix[i][j] = min3(deletion,insertion,substitution);


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


void sort_candidate_distances(int** distance_matrix, int nb_candidates){
    int j;
    int* temp;

    for(int i = 0;i<=nb_candidates;i++){
        temp = distance_matrix[i];
        j= i;

        //on compare la distance situé à temp[1]
        //on déplace les éléments qui ont une dist > temp[1]

        while (0<=j&&distance_matrix[j][1]>temp[1]){
            distance_matrix[j+1] = distance_matrix[j];
            j = j-1;
        }

        distance_matrix[j+1] = temp;
    }

}
