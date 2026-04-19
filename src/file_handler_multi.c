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

static int set_lines_sizes(const char* map, size_t file_size, uint32_t** lines_sizes, size_t line_count) {
    *lines_sizes = malloc(line_count * sizeof(uint32_t));
    if (*lines_sizes == NULL) return -1;

    size_t line_index = 0;
    size_t current_chars_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (map[i] == '\n') {
            (*lines_sizes)[line_index] = current_chars_count;

            line_index++;
            current_chars_count = 0;
        } 
        else {
            current_chars_count++;
        }
    }
    
    // Cas spécial où la dernière ligne n'a pas de retour à la ligne à sa fin
    if (file_size > 0 && map[file_size - 1] != '\n') {
        (*lines_sizes)[line_index] = current_chars_count;
    }

    return 0;
}

static void free_lines(char** lines, size_t line_count) {
    for (size_t i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
}

static int set_lines(char*** lines, uint32_t* lines_sizes, size_t line_count) {
    *lines = malloc(line_count * sizeof(char *));
    if (*lines == NULL) return -1;

    for (size_t i = 0; i < line_count; i++) {
        size_t line_size = lines_sizes[i];

        char* line = malloc((line_size + 1) * sizeof(char));
        if (line == NULL) {
            free_lines(*lines, i);
            return -1;
        }

        line[line_size] = '\0';
        (*lines)[i] = line;
    }

    return 0;
}

void set_start_line_index_and_file_offset(size_t* start_line_index, size_t* file_offset, read_input_data_t* data) {
    uint32_t* lines_sizes = data->lines_sizes;
    size_t line_count = data->line_count;
    int start_index = data->start_index;

    // Précaution
    *start_line_index = 0;
    *file_offset = 0;

    while (*start_line_index < line_count) {
        int line_end_index = *file_offset + lines_sizes[*start_line_index];
        if (line_end_index >= start_index) break;

        *file_offset += lines_sizes[*start_line_index];

        // Ca nous aide à éviter le cas où la dernière ligne n'a pas de \n à sa fin
        if (*start_line_index < line_count - 1) {
            (*file_offset)++; 
        }

        (*start_line_index)++;
    }
}

void read_chunk_of_input_file(read_input_data_t* data) {
    size_t start_line_index = 0;
    size_t file_offset = 0;
    set_start_line_index_and_file_offset(&start_line_index, &file_offset, data);

    size_t start_index = data->start_index;
    size_t write_offset = start_index - file_offset;

    size_t current_line = start_line_index;
    size_t current_index_in_line = write_offset;

    char* file_map = data->file_map;
    char** lines = data->lines;
    size_t end_index = data->end_index;

    for (size_t i = start_index; i < end_index; i++) {
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

int read_input_file_multi(char* input_path, char*** lines, uint32_t** lines_sizes, size_t* line_count) {
    size_t file_size = 0;
    char *file_map = map_file(input_path, &file_size);
    if (!file_map) return -1;

    set_line_count(file_map, file_size, line_count);

    if (set_lines_sizes(file_map, file_size, lines_sizes, *line_count) == -1) {
        munmap(file_map, file_size);
        return -1;
    }

    if (set_lines(lines, *lines_sizes, *line_count) == -1) {
        free(*lines_sizes);
        munmap(file_map, file_size);
        return -1;
    }

    read_input_data_t* read_input_data = malloc(num_threads * sizeof(read_input_data_t));
    if (read_input_data == NULL) {
        free(*lines_sizes);
        free_lines(*lines, *line_count);
        munmap(file_map, file_size);
        return -1;
    }

    if (num_threads > 1) {
        pthread_t threads[num_threads];
        int chunk = file_size / num_threads;

        for (int i = 0; i < num_threads; i++) {
            read_input_data[i].lines = *lines;
            read_input_data[i].lines_sizes = *lines_sizes;
            read_input_data[i].line_count = *line_count;

            read_input_data[i].file_map = file_map;
            read_input_data[i].start_index = i * chunk;
            read_input_data[i].end_index = (i == num_threads - 1) ? file_size : (i + 1) * chunk;

            pthread_create(&threads[i], NULL, read_input_file_thread, &read_input_data[i]);
        }
        
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    else {
        read_input_data[0].lines = *lines;
        read_input_data[0].lines_sizes = *lines_sizes;
        read_input_data[0].line_count = *line_count;

        read_input_data[0].file_map = file_map;
        read_input_data[0].start_index = 0;
        read_input_data[0].end_index = file_size;
        read_chunk_of_input_file(read_input_data);
    }

    munmap(file_map, file_size);
    
    return 0;
}

void* read_input_file_thread(void* args) {
    read_input_data_t* data = (read_input_data_t *) args;
    read_chunk_of_input_file(data);

    pthread_exit(NULL);
}