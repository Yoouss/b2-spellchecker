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


int32_t numbers_of_bad_words_in_line(char* line, Dictionary_t* dict) {
    if (line==NULL || dict==NULL) return -1;

    int32_t numberOfBadWords = 0;
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

int* get_indexes_of_bad_words_in_line(char* line, int32_t numberOfBadWords, Dictionary_t* dict) {
    if (line==NULL || dict==NULL) return NULL;

    int* indexesOfBadWords = malloc(numberOfBadWords * sizeof(int32_t));
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

Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, uint16_t number_of_dictionaries) {
    if (line == NULL || dicts == NULL || number_of_dictionaries == 0)
        return NULL;

    Dictionary_t* final_candidate = &dicts[0];
    int32_t number_of_errors = numbers_of_bad_words_in_line(line, final_candidate);

    for (int i = 1; i < number_of_dictionaries; i++) {
        Dictionary_t* current_dictionary = &dicts[i];
        int32_t current_number_of_errors = numbers_of_bad_words_in_line(line, current_dictionary);

        if (current_number_of_errors < number_of_errors) {
            number_of_errors = current_number_of_errors;
            final_candidate = current_dictionary;
        }
    }

    return final_candidate;
}

int* words_in_line(char* line, Dictionary_t* dicts, uint16_t number_of_dictionaries){
    if (line==NULL || dicts==NULL || number_of_dictionaries == 0) return NULL;

    Dictionary_t* candidate_dictionary = find_candidate_dict_for_line(line, dicts, number_of_dictionaries);
    if (candidate_dictionary == NULL) return NULL;
    
    int32_t numberOfBadWords = numbers_of_bad_words_in_line(line, candidate_dictionary);
    if (numberOfBadWords < 0) return NULL;

    return get_indexes_of_bad_words_in_line(line, numberOfBadWords, candidate_dictionary);
}


int** words_in_file(char* filename, Dictionary_t* dicts, uint16_t number_of_dictionaries){
    if (filename == NULL || dicts == NULL || number_of_dictionaries <= 0) return NULL;

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(filename, &lines, &lines_sizes, &line_count);
    if (lines == NULL || lines_sizes == NULL || line_count <= 0) return NULL;

    int** matrix = malloc(line_count * sizeof(int*));

    for (int i = 0; i < line_count; i++){
        char* line = lines[i];
        int* indexesOfBadWords = words_in_line(line, dicts, number_of_dictionaries);
        matrix[i]= indexesOfBadWords;
    }

    return matrix;
}