#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>

#include <detector_multi.h>
#include <detector.h>

typedef struct {
    char* line;
    Dictionary_t* dict;
    uint32_t errors_count;
    int success; // Pour la gestion d'erreur
} ThreadArgs_t;

void* thread_count_errors(void* arg) {
    ThreadArgs_t* data = (ThreadArgs_t*)arg;
    
    if (set_wrong_words_count_in_line(data->line, &data->errors_count, data->dict) < 0) {
        data->success = -1;
    } else {
        data->success = 0;
    }
    
    return NULL;
}


Dictionary_t* find_candidate_dict_for_line_thread(char* line, Dictionary_t* dicts, size_t dictionaries_count) {
    if (line == NULL || dicts == NULL || dictionaries_count == 0) return NULL;

    pthread_t threads[dictionaries_count]; // liste contenant les dico
    ThreadArgs_t thread_args[dictionaries_count]; // liste contenant les arguments de toutes les threads

    for (size_t i = 0; i < dictionaries_count; i++) { // remplir les arguments des thread avec celle entrée de la fonction
        thread_args[i].line = line;
        thread_args[i].dict = &dicts[i];
        thread_args[i].errors_count = 0;
        thread_args[i].success = 0;

        pthread_create(&threads[i], NULL, thread_count_errors, &thread_args[i]);
        }

    Dictionary_t* final_candidate = NULL;
    uint32_t min_errors = UINT32_MAX;

    for (size_t i = 0; i < dictionaries_count; i++) {
        pthread_join(threads[i], NULL);

        // Si un thread a échoué, on décide de l'ignorer
        if (thread_args[i].success < 0) continue; 

        if (thread_args[i].errors_count < min_errors) {
            min_errors = thread_args[i].errors_count;
            final_candidate = thread_args[i].dict;
        }
    }

    return final_candidate;
}



