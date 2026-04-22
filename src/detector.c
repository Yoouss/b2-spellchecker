#include <stdio.h>
#include <string.h>

#include <detector.h>
#include <file_handler.h>

void free_line_detection(line_t* line_detection) {
    if (line_detection == NULL) return;

    free(line_detection->wrong_words_indexes);
    free(line_detection);
}

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

    uint32_t* wrong_words_indexes = NULL;
    if (wrong_words_count > 0) {
        wrong_words_indexes = get_wrong_words_indexes_in_line(line, wrong_words_count, candidate_dictionary);
        if (wrong_words_indexes == NULL) return NULL;  
    }
    
    line_t* line_detection = malloc(sizeof(line_t));
    if (line_detection == NULL) {
        free(wrong_words_indexes);
        return NULL;
    }

    line_detection->wrong_words_indexes = wrong_words_indexes;
    line_detection->wrong_words_count = wrong_words_count;
    line_detection->used_dictionary = candidate_dictionary;
    line_detection->used_dict_id = candidate_dictionary->id;

    return line_detection;
}