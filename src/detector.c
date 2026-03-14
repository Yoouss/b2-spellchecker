#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include "detector.h"
#include <string.h>

int word_in_dictionary(char* word, Dictionary_t* dict){
    if (word==NULL||dict==NULL){
        return -1;
    }

    char **p = dict->words;
    while(*p!=NULL){
        if (strcmp(*p, word) == 0){
            return 1;
        }p++;
    }
    return 0;
}

int* words_in_line(char* line, int length, Dictionary_t* dict){

    if (line==NULL||dict==NULL|| length<=0){
        return NULL;
    }
    
    int count = 0;
    char *copy = strdup(line);
    if (copy == NULL) return NULL;

    char* mot = strtok(copy," %\t.,;:!?\"'()[]{}-_/\\|@#$^&*+=~`");
    while (mot != NULL){
        if(word_in_dictionary(mot,dict)==0){
            count++;
        }   
        mot = strtok(NULL, " %\t.,;:!?\"'()[]{}-_/\\|@#$^&*+=~`");
    }

    free(copy);

    if (count == 0) return NULL;


    int* index = malloc(count*sizeof(int));
    if (index == NULL) {
        free(copy); 
        return NULL;
    }
    
    int recount = 0;
    int pos=0;

    copy = strdup(line);
    if (copy == NULL) {
        free(index); 
        return NULL;
    }

    char* mot2 = strtok(copy," %\t.,;:!?\"'()[]{}-_/\\|@#$^&*+=~`");
        while (mot2 != NULL){
        if(word_in_dictionary(mot2,dict)==0){
            index[pos] = recount;
            pos++;
        }   
        recount++;

        mot2 = strtok(NULL, " %\t.,;:!?\"'()[]{}-_/\\|@#$^&*+=~`");
    }

    free(copy);

    return index; 
}


int** words_in_file(char* filename, Dictionary_t* dict){

    if (filename == NULL || dict == NULL) return NULL;

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(filename, &lines, &lines_sizes, &line_count);

    int** matrice = malloc(line_count * sizeof(int*));

    for (int i = 0; i < line_count; i++){
        char* line = lines[i];
        int* lst_pos_badwords = words_in_line(line, lines_sizes[i], dict);
        matrice[i]= lst_pos_badwords;
    }

    return matrice;
}