#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include "common.h"
#include "detector.h"
#include <string.h>

short word_in_dictionary(char* word, Dictionary_t* dict) {
    return binary_search_word_in_dictonary(word, dict);
}


uint32_t numbers_of_bad_words_in_line(char* line, Dictionary_t* dict) {
    if (line==NULL || dict==NULL) return -1;

    uint32_t numberOfBadWords = 0;
    char *lineCopy = strdup(line);
    if (lineCopy == NULL) return -1;

    char* word = strtok(lineCopy, SEPARATORS);
    while (word != NULL){
        if (!word_in_dictionary(word, dict)) numberOfBadWords++;

        word = strtok(NULL, SEPARATORS);
    }

    free(lineCopy);

    return numberOfBadWords;
}

int* get_indexes_of_bad_words_in_line(char* line, uint32_t numberOfBadWords, Dictionary_t* dict) {
    if (line==NULL || dict==NULL) return NULL;

    int* indexesOfBadWords = malloc(numberOfBadWords * sizeof(uint32_t));
    if (indexesOfBadWords == NULL) return NULL;

    uint32_t indexInLine = 0;
    uint32_t indexInArray = 0;

    char *lineCopy = strdup(line);
    if (lineCopy == NULL) {
        free(indexesOfBadWords);
        return NULL;
    }

    char* word = strtok(lineCopy, SEPARATORS);
    while (word != NULL) {
        if (!word_in_dictionary(word, dict)) {
            indexesOfBadWords[indexInArray] = indexInLine;
            indexInArray++;
        }   

        indexInLine++;
        word = strtok(NULL, SEPARATORS);
    }

    free(lineCopy);

    return indexesOfBadWords; 
}

int* words_in_line(char* line, Dictionary_t* dict){
    if (line==NULL || dict==NULL) return NULL;
    
    uint32_t numberOfBadWords = numbers_of_bad_words_in_line(line, dict);
    if (numberOfBadWords <= 0) return NULL;

    return get_indexes_of_bad_words_in_line(line, numberOfBadWords, dict);
}


int** words_in_file(char* filename, Dictionary_t* dict){
    if (filename == NULL || dict == NULL) return NULL;

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(filename, &lines, &lines_sizes, &line_count);
    if (lines == NULL || lines_sizes == NULL || line_count <= 0) return NULL;

    int** matrix = malloc(line_count * sizeof(int*));

    for (int i = 0; i < line_count; i++){
        char* line = lines[i];
        int* indexesOfBadWords = words_in_line(line, dict);
        matrix[i]= indexesOfBadWords;
    }

    return matrix;
}