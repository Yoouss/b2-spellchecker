#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include <input.pos>
// Il faut ajouter le inclde de detector.h

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

words_in_line(char* line, int length, Dictionary_t* dict){
    if (line==NULL||dict==NULL|| length<=0){
        return -1;
    }
    int count=0;
    for(int i=0;i<length;i++){
        if (word_in_dictionary(*line,dict)!=1){
            count++;
        }
    }
    int index[count];
    int recount=0;
    for(int i=0;i<length;i++){
        if (word_in_dictionary(*line,dict)!=1){
            index[recount]=i;
        }
        recount++;
    }

   return index; 
}


int** words_in_file(char* filename, Dictionary_t* dicts ){

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(filename, &lines, &lines_sizes, &line_count);

    int** matrice = malloc(line_count*sizeof(int*));

    for(int i=0;i<line_count;i++){
        int* ligne = lines[i];
        int* lst_pos_badwords = words_in_line(ligne,lines_sizes[i],dicts);
        matrice[i]= lst_pos_badwords;

    }
    return matrice;

}