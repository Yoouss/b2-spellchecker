#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <detector.h>
#include <file_handler.h>

void free_line_detection(line_t* line_detection) {
    if (line_detection == NULL) return;

    free(line_detection->wrong_words_indexes);
    free(line_detection);
}

int word_in_dictionary(char* target_word, Dictionary_t* dict) {
    if (target_word == NULL || dict == NULL) return -1;
    // Empêche un bug avec left = 1 et right = 0
    if (dict->word_count == 0) return 1;

    char** words = dict->words;
    int left = 1;
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

void set_candidate_dict_data(candidate_dict_data_t* candidate_dict_data, int active_threads,
                             char* line, Dictionary_t* dicts, size_t dicts_count) {
    
    int chunk = dicts_count / active_threads;

    for (size_t i = 0; i < active_threads; i++) {
        candidate_dict_data[i].line = line;
        candidate_dict_data[i].dicts = dicts;
        candidate_dict_data[i].dict_found = NULL;
        candidate_dict_data[i].errors_count = 0;
        candidate_dict_data[i].status = 0;
        candidate_dict_data[i].start_index = i * chunk;
        candidate_dict_data[i].end_index = (i == active_threads - 1) ? dicts_count : (i + 1) * chunk;
    }
}

void find_potential_candidate_dict_for_line(candidate_dict_data_t* candidate_dict_data) {
    char* line = candidate_dict_data->line;
    Dictionary_t* dicts = candidate_dict_data->dicts;
    size_t start_index = candidate_dict_data->start_index;
    
    Dictionary_t* potential_candidate = &dicts[start_index];
    uint32_t errors_count = 0;
    if (set_wrong_words_count_in_line(line, &errors_count, potential_candidate) < 0) {
        candidate_dict_data->status = -1;
        return;
    }

    size_t end_index = candidate_dict_data->end_index;
    for (int i = start_index + 1; i < end_index; i++) {
        Dictionary_t* current_dictionary = &dicts[i];
        uint32_t current_errors_count = 0;
        if (set_wrong_words_count_in_line(line, &current_errors_count, current_dictionary) < 0) {
            candidate_dict_data->status = -1;
            return;
        }

        if (current_errors_count < errors_count) {
            errors_count = current_errors_count;
            potential_candidate = current_dictionary;
        }
    }

    candidate_dict_data->dict_found = potential_candidate;
    candidate_dict_data->errors_count = errors_count;
}

void* find_candidate_dict_for_line_thread(void* args) {
    candidate_dict_data_t* candidate_dict_data = (candidate_dict_data_t*) args;
    find_potential_candidate_dict_for_line(candidate_dict_data);
    pthread_exit(NULL);
}

Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, size_t dicts_count) {
    if (line == NULL || dicts == NULL || dicts_count == 0) return NULL;

    int active_threads = (dicts_count >= 2 * num_threads) ? num_threads : 1;
    candidate_dict_data_t* candidate_dict_data = malloc(active_threads * sizeof(candidate_dict_data_t));
    if (candidate_dict_data == NULL) return NULL;
    set_candidate_dict_data(candidate_dict_data, active_threads, line, dicts, dicts_count);

    if (active_threads == 1) {
        find_potential_candidate_dict_for_line(candidate_dict_data);
    }
    else if (active_threads > 1) {
        pthread_t threads[num_threads];
        for (size_t i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, find_candidate_dict_for_line_thread, &candidate_dict_data[i]);
        }

        for (size_t i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    Dictionary_t* final_candidate = NULL;
    uint32_t errors_count = 0;
    for (size_t i = 0; i < active_threads; i++) {
        candidate_dict_data_t current_data = candidate_dict_data[i];
        if (current_data.status == -1) {
            free(candidate_dict_data);
            return NULL;
        }

        Dictionary_t* potential_candidate = current_data.dict_found;
        uint32_t current_errors_count = current_data.errors_count;
        if (final_candidate == NULL || current_errors_count < errors_count) {
            final_candidate = potential_candidate;
            errors_count = current_errors_count;
        }
    }

    free(candidate_dict_data);

    return final_candidate;
}

int set_wrong_words_count_in_line(char* line, uint32_t* wrong_words_count, Dictionary_t* dict) {
    if (line == NULL || dict == NULL) return -1;

    // Précaution
    *wrong_words_count = 0;

    char *line_copy = strdup(line);
    if (line_copy == NULL) return -1;

    char *rest_of_line_copy;

    char* word = strtok_r(line_copy, SEPARATORS, &rest_of_line_copy);
    while (word != NULL){
        if (word_in_dictionary(word, dict) == 0) (*wrong_words_count)++;

        word = strtok_r(NULL, SEPARATORS, &rest_of_line_copy);
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

    char* rest_of_line_copy;

    char* word = strtok_r(line_copy, SEPARATORS, &rest_of_line_copy);
    while (word != NULL) {
        if (word_in_dictionary(word, dict) == 0) {
            wrong_words_indexes[index_in_array] = index_in_line;
            index_in_array++;
        }   

        index_in_line++;
        word = strtok_r(NULL, SEPARATORS, &rest_of_line_copy);
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

    char* rest_of_line_copy;

    char* word = strtok_r(line_copy, SEPARATORS, &rest_of_line_copy);
    uint32_t index_in_bad_words = 0;
    uint32_t index_in_line = 0;

    while (word != NULL && index_in_bad_words < wrong_words_count) {
        if (index_in_line ==  wrong_words_indexes[index_in_bad_words]) {
            wrong_words[index_in_bad_words] = strdup(word);

            index_in_bad_words++;
        }   

        index_in_line++;
        word = strtok_r(NULL, SEPARATORS, &rest_of_line_copy);
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