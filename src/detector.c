#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include <input.pos>


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




//load_dictionaries &  read_input_files



















