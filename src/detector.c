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


uint32_t number_of_bad_words_in_line(char* line, Dictionary_t* dict) {
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

uint32_t* get_indexes_of_bad_words_in_line(char* line, uint32_t numberOfBadWords, Dictionary_t* dict) {
    if (line==NULL || dict==NULL) return NULL;

    uint32_t* indexesOfBadWords = malloc((numberOfBadWords + 1) * sizeof(int32_t));
    if (indexesOfBadWords == NULL) return NULL;

    indexesOfBadWords[0] = numberOfBadWords;
    if (numberOfBadWords == 0) return indexesOfBadWords;

    uint32_t indexInLine = 0;
    uint32_t indexInArray = 1;

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

char** get_bad_words_in_line(char* line, uint32_t* indexesOfBadWords) {
    if (line == NULL || indexesOfBadWords == NULL) return NULL;

    uint32_t numberOfBadWords = indexesOfBadWords[0];
    if (numberOfBadWords == 0) return NULL;

    char** badWords = malloc(numberOfBadWords * sizeof(char*));
    if (badWords == NULL) return NULL;
        
    char *lineCopy = strdup(line);
    if (lineCopy == NULL) {
        free(badWords);
        return NULL;
    }

    char* word = strtok(lineCopy, SEPARATORS);
    uint32_t indexInbadWords = 0;
    uint32_t indexInLine = 0;

    while (word != NULL && indexInbadWords  < numberOfBadWords) {
        if (indexInLine ==  indexesOfBadWords[indexInbadWords + 1]) {
            badWords[indexInbadWords] = strdup(word);

            indexInbadWords++;
        }   

        indexInLine++;
        word = strtok(NULL, SEPARATORS);
    }

    free(lineCopy);

    return badWords;
}

Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, uint16_t number_of_dictionaries) {
    if (line == NULL || dicts == NULL || number_of_dictionaries == 0) return NULL;

    Dictionary_t* final_candidate = &dicts[0];
    uint32_t number_of_errors = number_of_bad_words_in_line(line, final_candidate);

    for (int i = 1; i < number_of_dictionaries; i++) {
        Dictionary_t* current_dictionary = &dicts[i];
        uint32_t current_number_of_errors = number_of_bad_words_in_line(line, current_dictionary);

        if (current_number_of_errors < number_of_errors) {
            number_of_errors = current_number_of_errors;
            final_candidate = current_dictionary;
        }
    }

    return final_candidate;
}

uint32_t* words_in_line(char* line, Dictionary_t* dicts, uint16_t number_of_dictionaries){
    if (line==NULL || dicts==NULL || number_of_dictionaries == 0) return NULL;

    Dictionary_t* candidate_dictionary = find_candidate_dict_for_line(line, dicts, number_of_dictionaries);
    if (candidate_dictionary == NULL) return NULL;
    
    uint32_t numberOfBadWords = number_of_bad_words_in_line(line, candidate_dictionary);
    if (numberOfBadWords < 0) return NULL;

    return get_indexes_of_bad_words_in_line(line, numberOfBadWords, candidate_dictionary);
}


uint32_t** words_in_file(char* filename, Dictionary_t* dicts, uint16_t number_of_dictionaries){
    if (filename == NULL || dicts == NULL || number_of_dictionaries == 0) return NULL;

    // Load input
    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file(filename, &lines, &lines_sizes, &line_count);
    if (lines == NULL || lines_sizes == NULL || line_count <= 0) return NULL;

    uint32_t** matrix = malloc(line_count * sizeof(int32_t*));

    for (int i = 0; i < line_count; i++) {
        char* line = lines[i];
        uint32_t* indexesOfBadWords = words_in_line(line, dicts, number_of_dictionaries);
        matrix[i]= indexesOfBadWords;
    }

    return matrix;
}