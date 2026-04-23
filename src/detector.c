#include <stdio.h>
#include <string.h>

#include <detector.h>
#include <file_handler.h>

#include <pthread.h>


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

Dictionary_t* find_candidate_dict_for_line_simple(char* line, Dictionary_t* dicts, size_t dictionaries_count) {
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

typedef struct {
    char* line;
    Dictionary_t* dicts;
    size_t start;
    size_t end;
    Dictionary_t* best_dict_found; 
    uint32_t min_errors_found;
    int success; // Pour la gestion d'erreur
} ThreadArgs_t;

void* thread_find_best_in_chunk(void* arg) {
    ThreadArgs_t* data = (ThreadArgs_t*)arg;

    data->min_errors_found = UINT32_MAX;
    data->best_dict_found = NULL;
    data->success = 0;

    for(size_t i = data->start;i<data->end;i++){
        uint32_t current_errors = 0;

        if (set_wrong_words_count_in_line(data->line, &current_errors, &data->dicts[i]) < 0) {
            data->success = -1; 
            return NULL;
        }
        if (current_errors < data->min_errors_found) {
        data->min_errors_found = current_errors;
        data->best_dict_found = &data->dicts[i];
        }
    }
    return NULL;
}


Dictionary_t* find_candidate_dict_for_line_thread(char* line, Dictionary_t* dicts, size_t dictionaries_count) {
    if (line == NULL || dicts == NULL || dictionaries_count == 0) return NULL;

    int nbr_thread = (num_threads > (int)dictionaries_count) ? (int)dictionaries_count : num_threads;

    pthread_t threads[nbr_thread];
    ThreadArgs_t* args = malloc( nbr_thread * sizeof(ThreadArgs_t));
    if (args == NULL) return NULL; // Sécu malloc

    size_t current_start = 0;
    size_t chunk = dictionaries_count / nbr_thread;
    size_t rest_dico = dictionaries_count % nbr_thread;

    for (int i = 0; i < nbr_thread; i++) {
        size_t current_chunk = chunk + (i < rest_dico ? 1: 0);

        args[i].line = line;
        args[i].dicts = dicts; // On donne tout le tableau
        args[i].start = current_start;
        args[i].end = current_start + current_chunk;
        args[i].success = 0;

        pthread_create(&threads[i], NULL, thread_find_best_in_chunk, &args[i]);

        current_start = args[i].end;
    }

        

    Dictionary_t* final_candidate = NULL;
    uint32_t min_errors = UINT32_MAX;

    for (size_t i = 0; i < nbr_thread; i++) {
        pthread_join(threads[i], NULL);

        // Si le thread a réussi et qu'il a trouvé mieux que le record actuel
        if (args[i].success == 0 && args[i].best_dict_found != NULL) { 
            if (args[i].min_errors_found < min_errors) {               
                min_errors = args[i].min_errors_found;
                final_candidate = args[i].best_dict_found;
        }
    }
    }


    free(args);
    return final_candidate;
}


Dictionary_t* find_candidate_dict_for_line(char* line, Dictionary_t* dicts, size_t dictionaries_count) {
    if (line == NULL || dicts == NULL || dictionaries_count == 0) return NULL;
    if(num_threads == 1){
       return find_candidate_dict_for_line_simple(line,dicts,dictionaries_count);
    }
    if(num_threads > 1){
       return find_candidate_dict_for_line_thread(line,dicts,dictionaries_count);
    }
    return NULL;
}