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
    char **p;
    p=dict;
    while(*p!=NULL){
        if (strcmp(*p, word) == 0){
            return 1;
        }p++;
    }
    return 0;
}





//load_dictionaries &  read_input_files


