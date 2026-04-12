#include <stdio.h>
#include "common.h"
#include <io.h>
#include <io_multi.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct {
    char*** lines;
    uint32_t** lines_sizes;
    size_t* line_count;
    char* file_map;

    int thread_id;
} thread_data_t;

pthread_mutex_t found_mutex = PTHREAD_MUTEX_INITIALIZER;
int status = 0;

static char* map_file(const char* path, size_t* file_size) {
    int file_descriptor = open(path, O_RDONLY);
    if (file_descriptor == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    struct stat file_stat;
    if (fstat(file_descriptor, &file_stat) == -1) {
        perror("Erreur fstat");
        close(file_descriptor);
        return NULL;
    }

    *file_size = file_stat.st_size;

    if (*file_size == 0) {
        close(file_descriptor);
        return NULL; 
    }
    
    char *file_map = mmap(NULL, *file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);

    close(file_descriptor);

    if (file_map == MAP_FAILED) {
        perror("Erreur mmap");
        return NULL;
    }

    return file_map;
}

static void set_line_count_and_lines_sizes(const char* map, size_t file_size, uint32_t** lines_sizes, size_t* line_count) {
    size_t line_index = 0;
    size_t current_words_count = 0;

    // Précaution
    *line_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (map[i] == '\n') {
            (*line_count)++;
            (*lines_sizes)[line_index] = current_words_count;

            line_index++;
            current_words_count = 0;
        } 
        else {
            current_words_count++;
        }
    }
    
    if (file_size > 0 && map[file_size - 1] != '\n') {
        (*line_count)++;
        (*lines_sizes)[line_index] = current_words_count;
    }
}

static void clean_lines(char** lines, uint32_t* sizes, size_t count) {
    if (lines) {
        for (size_t i = 0; i < count; i++) {
            free(lines[i]);
        }
        free(lines);
    }
    if (sizes) {
        free(sizes);
    }
}

int read_input_file_multi(char* input_path, char*** lines, uint32_t** lines_sizes, size_t* line_count) {
    status = 0;

    size_t file_size = 0;
    char *file_map = map_file(input_path, &file_size);
    if (!file_map) return -1;

    set_line_count_and_lines_sizes(file_map, file_size, lines_sizes, line_count);

    *lines = malloc(*line_count * sizeof(char *));
    *lines_sizes = malloc(*line_count * sizeof(uint32_t));

    if (!*lines || !*lines_sizes) {
        clean_lines(*lines, *lines_sizes, 0);
        munmap(file_map, file_size);
        return -1;
    }

    pthread_t threads[NUM_THREADS];
    thread_data_t * thread_data = malloc(NUM_THREADS * sizeof(thread_data_t));
    if (thread_data == NULL) return -1;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].lines = lines;
        thread_data[i].lines_sizes = lines_sizes;
        thread_data[i].line_count = line_count;
        thread_data[i].file_map = file_map;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], NULL, read_input_file_thread, &thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    munmap(file_map, file_size);
    
    return status;
}

void* read_input_file_thread(void* args) {
    if (status == -1) pthread_exit(NULL);

    thread_data_t *data = (thread_data_t *) args;
    char*** lines = data->lines;
    uint32_t* lines_sizes = *(data->lines_sizes);
    size_t line_count = *(data->line_count);
    char* file_map = data->file_map;
    int thread_id = data->thread_id;

    int start = 0;

    for (int i = 0; i < thread_id; i++) {
        start += lines_sizes[i];
    }

    // THREADS
    for (size_t i = thread_id; i < line_count; i += NUM_THREADS) {

        char* line = malloc((lines_sizes[i] + 1) * sizeof(char));
        if (line == NULL) {
            status = -1;
            pthread_exit(NULL);
        }

        for (size_t j = 0; j < lines_sizes[i]; j++) {
            line[j] = file_map[start];
            start++;
        }

        line[lines_sizes[i]] = '\0';

        (*lines)[i] = line;
    }
    
    pthread_exit(NULL);
}