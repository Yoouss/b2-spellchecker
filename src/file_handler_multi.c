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
    char** lines;
    uint32_t* lines_sizes;
    size_t line_count;
    char* file_map;

    int start_index;
    int end_index;
} thread_data_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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

static void set_line_count(const char* map, size_t file_size, size_t* line_count) {
    // Précaution
    *line_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (map[i] == '\n') (*line_count)++;
    }
    
    if (file_size > 0 && map[file_size - 1] != '\n') {
        (*line_count)++;
    }
}

static void set_lines_sizes(const char* map, size_t file_size, uint32_t** lines_sizes) {
    size_t line_index = 0;
    size_t current_words_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (map[i] == '\n') {
            (*lines_sizes)[line_index] = current_words_count;

            line_index++;
            current_words_count = 0;
        } 
        else {
            current_words_count++;
        }
    }
    
    if (file_size > 0 && map[file_size - 1] != '\n') {
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

    set_line_count(file_map, file_size, line_count);

    *lines = malloc(*line_count * sizeof(char *));
    *lines_sizes = malloc(*line_count * sizeof(uint32_t));

    if (!*lines || !*lines_sizes) {
        clean_lines(*lines, *lines_sizes, 0);
        munmap(file_map, file_size);
        return -1;
    }

    set_lines_sizes(file_map, file_size, lines_sizes);

    for (size_t i = 0; i < *line_count; i++) {
        (*lines)[i] = malloc((*lines_sizes)[i] + 1);
        (*lines)[i][(*lines_sizes)[i]] = '\0';
    }

    pthread_t threads[NUM_THREADS];
    thread_data_t * thread_data = malloc(NUM_THREADS * sizeof(thread_data_t));
    if (thread_data == NULL) return -1;

    int chunk = file_size / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].lines = *lines;
        thread_data[i].lines_sizes = *lines_sizes;
        thread_data[i].line_count = *line_count;
        thread_data[i].file_map = file_map;

        thread_data[i].start_index = i * chunk;
        thread_data[i].end_index = (i == NUM_THREADS - 1) ? file_size : (i + 1) * chunk;

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

    char** lines = data->lines;
    uint32_t* lines_sizes = data->lines_sizes;
    char* file_map = data->file_map;
    
    int start_index = data->start_index;
    int end_index = data->end_index;

    int line_index = 0;
    int total_char = 0;

    while (total_char + lines_sizes[line_index] < start_index) {
        total_char += lines_sizes[line_index] + 1;
        line_index++;
    }

    int write_offset = start_index - total_char;

    int current_line = line_index;
    int current_index_in_line = write_offset;
    
    for (int i = start_index; i < end_index; i++) {
        if (lines[current_line] != NULL) {
            char c = file_map[i];

            if (c == '\n') {
                current_line++;
                current_index_in_line = 0;
            } 
            else {
                lines[current_line][current_index_in_line] = c;
                current_index_in_line++;
            }
        }
    }
    
    pthread_exit(NULL);
}