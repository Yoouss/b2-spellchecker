#include <stdio.h>
#include <dict.h>
#include <input.h>
#include <io.h>
#include <file_handler.h>
#include "common.h"
#include "detector.h"
#include <string.h>

int word_in_dictionary(char* target_word, Dictionary_t* dict) {
    if (target_word == NULL || dict == NULL) return -1;

    char** words = dict->words;
    int left = 0;
    int right = dict->word_count - 1;

    while (left <= right) {
        int middle = (left + right) / 2;
        char* word = words[middle];
        int comparison = strcmp(word, target_word); // Is word smaller or greater than target_word ?

        if (comparison < 0) left = middle + 1;

        else if (comparison > 0) right = middle - 1;

        else return 1;
    }

    return 0;
}


Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, size_t dictionaries_count) {
    if (line == NULL || dicts == NULL || dictionaries_count == 0) return NULL;

    Dictionary_t* final_candidate = &dicts[0];
    uint32_t errors_count = 0;
    if (set_wrong_words_count_in_line(line, &errors_count, final_candidate) < 0) return NULL;

    for (int i = 1; i < dictionaries_count; i++) {
        Dictionary_t* current_dictionary = &dicts[i];
        uint32_t current_errors_count = 0;
        if (set_wrong_words_count_in_line(line, &current_errors_count, current_dictionary) < 0) return NULL;

        if (current_errors_count < errors_count) {
            errors_count = current_errors_count;
            final_candidate = current_dictionary;
        }
    }

    return final_candidate;
}

int set_wrong_words_count_in_line(char* line, uint32_t* wrong_words_count, Dictionary_t* dict) {
    if (line == NULL || dict == NULL) return -1;

    // Précaution
    *wrong_words_count = 0;

    char *line_copy = strdup(line);
    if (line_copy == NULL) return -1;

    char* word = strtok(line_copy, SEPARATORS);
    while (word != NULL){
        if (word_in_dictionary(word, dict) == 0) (*wrong_words_count)++;

        word = strtok(NULL, SEPARATORS);
    }

    free(line_copy);

    return 0;
}

uint32_t* get_wrong_words_indexes_in_line(char* line, uint32_t wrong_words_count, Dictionary_t* dict) {
    if (line == NULL || dict == NULL || wrong_words_count == 0) return NULL;

    uint32_t* wrong_words_indexes = malloc(wrong_words_count * sizeof(uint32_t));
    if (wrong_words_indexes == NULL) return NULL;

    uint32_t index_in_line = 0;
    uint32_t index_in_array = 0;

    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        free(wrong_words_indexes);
        return NULL;
    }

    char* word = strtok(line_copy, SEPARATORS);
    while (word != NULL) {
        if (word_in_dictionary(word, dict) == 0) {
            wrong_words_indexes[index_in_array] = index_in_line;
            index_in_array++;
        }   

        index_in_line++;
        word = strtok(NULL, SEPARATORS);
    }

    free(line_copy);

    return wrong_words_indexes; 
}

char** get_wrong_words_in_line(char* line, uint32_t* wrong_words_indexes, uint32_t wrong_words_count) {
    if (line == NULL || wrong_words_indexes == NULL || wrong_words_count == 0) return NULL;

    char** wrong_words = malloc(wrong_words_count * sizeof(char*));
    if (wrong_words == NULL) return NULL;
        
    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        free(wrong_words);
        return NULL;
    }

    char* word = strtok(line_copy, SEPARATORS);
    uint32_t index_in_bad_words = 0;
    uint32_t index_in_line = 0;

    while (word != NULL && index_in_bad_words < wrong_words_count) {
        if (index_in_line ==  wrong_words_indexes[index_in_bad_words]) {
            wrong_words[index_in_bad_words] = strdup(word);

            index_in_bad_words++;
        }   

        index_in_line++;
        word = strtok(NULL, SEPARATORS);
    }

    free(line_copy);

    return wrong_words;
}

line_t* scan_line_for_errors(char* line, Dictionary_t* dicts, size_t dictionaries_count){
    if (line == NULL || dicts == NULL || dictionaries_count == 0) return NULL;

    Dictionary_t* candidate_dictionary = find_candidate_dict_for_line(line, dicts, dictionaries_count);
    if (candidate_dictionary == NULL) return NULL;
    
    uint32_t wrong_words_count = 0;
    if (set_wrong_words_count_in_line(line, &wrong_words_count, candidate_dictionary) < 0) return NULL;
    if (wrong_words_count == 0) return NULL;

    uint32_t* wrong_words_indexes = get_wrong_words_indexes_in_line(line, wrong_words_count, candidate_dictionary);
    if (wrong_words_indexes == NULL) return NULL;

    line_t* file_detection = malloc(sizeof(line_t));
    if (file_detection == NULL) {
        free(wrong_words_indexes);
        return NULL;
    }

    file_detection->wrong_words_indexes = wrong_words_indexes;
    file_detection->wrong_words_count = wrong_words_count;

    return file_detection;
}


file_t* scan_file_for_errors(char* filename, Dictionary_t* dicts, size_t dictionaries_count){
    if (filename == NULL || dicts == NULL || dictionaries_count == 0) return NULL;

    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    read_input_file_multi(filename, &lines, &lines_sizes, &line_count);
    if (lines == NULL || lines_sizes == NULL || line_count == 0) return NULL;
    free(lines_sizes);

    file_t* file_detection = malloc(sizeof(file_t));
    if (file_detection == NULL) return NULL;

    // ─────────────── Etape 1 ───────────────
    // Calculer le nombre de lignes incorrectes et allouées les variables de file_detection

    size_t incorrect_lines_count = 0;
    for (size_t i = 0; i < line_count; i++) {
        line_t* line_detection = scan_line_for_errors(lines[i], dicts, dictionaries_count);
        if (line_detection != NULL) incorrect_lines_count++;
        free(line_detection);
    }

    file_detection->incorrect_lines_count = incorrect_lines_count;


    file_detection->incorrect_lines = malloc(incorrect_lines_count * sizeof(line_t));
    if (file_detection->incorrect_lines == NULL) {
        free(file_detection);
        return NULL;
    }

    file_detection->incorrect_lines_indexes = malloc(incorrect_lines_count * sizeof(size_t));
    if (file_detection->incorrect_lines_indexes == NULL) {
        free(file_detection->incorrect_lines);
        free(file_detection);
        return NULL;
    }

    // ─────────────── Etape 2 ───────────────
    // Remplir file_detection

    size_t index_in_array = 0;
    for (size_t i = 0; i < line_count; i++) {
        line_t* line_detection = scan_line_for_errors(lines[i], dicts, dictionaries_count);
        if (line_detection != NULL) {
            file_detection->incorrect_lines[index_in_array] = *line_detection;
            file_detection->incorrect_lines_indexes[index_in_array] = i;
            index_in_array++;
        }
        free(line_detection);
    }

    return file_detection;
}