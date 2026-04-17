#include <stdio.h>
#include "common.h"
#include <file_handler.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

char* map_file(const char* path, size_t* file_size) {
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

size_t count_lines(const char* map, size_t size) {
    size_t n = 0;
    for (size_t i = 0; i < size; i++) {
        if (map[i] == '\n') n++;
    }
    
    if (size > 0 && map[size - 1] != '\n') {
        n++;
    }
    return n;
}

void clean_lines(char** lines, uint32_t* sizes, size_t count) {
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

int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count) {

    size_t file_size = 0;
    char *file_map = map_file(input_path, &file_size);
    if (!file_map) return -1;

    size_t total_lines = count_lines(file_map, file_size);

    *lines = malloc((total_lines + 1) * sizeof(char *));
    *line_sizes = malloc(total_lines * sizeof(uint32_t));

    if (!*lines || !*line_sizes) {
        clean_lines(*lines, *line_sizes, 0);
        munmap(file_map, file_size);
        return -1;
    }

    size_t current_line = 0;
    size_t start = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (i == file_size || file_map[i] == '\n') {
            size_t length = i - start;

            char *line = malloc(length + 1);
            if (!line) {
                clean_lines(*lines, *line_sizes, current_line);
                munmap(file_map, file_size);
                return -1;
            }

            for (size_t j = 0; j < length; j++) {
                line[j] = file_map[start + j];
            }
            line[length] = '\0';

            (*lines)[current_line] = line;
            (*line_sizes)[current_line] = (uint32_t)length;
            
            current_line++;
            start = i + 1;
        }
    }
    (*lines)[current_line] = NULL;
    *line_count = current_line;
    munmap(file_map, file_size);
    return 0;
}

char* get_language(const char* filepath){
    char *filename = strrchr(filepath, '/');
    
    if (filename == NULL) {
        filename = (char *)filepath;
    } else {
        filename++;
    }

    char *lang = strdup(filename);

    char *extension = strstr(lang, ".dict");
    if (extension != NULL) {
        *extension = '\0';
    }
    return lang;
}

int load_single_dictionary(Dictionary_t *dict, const char *filepath, uint32_t id) {

    uint32_t *temp_sizes = NULL;
    size_t temp_count = 0;

    if (read_input_file((char *)filepath, &dict->words, &temp_sizes, &temp_count) != 0) {
        return -1;
    }

    dict->word_count = (uint32_t)temp_count;
    dict->lang = get_language(filepath);
    dict ->id=id;

    free(temp_sizes);
    return 0;
}

int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count) {
    const char *actual_path = path;
    if (strcmp(path, "all") == 0) {
        actual_path = "./dicts";
    }
    if (strcmp(path, "all_small") == 0) {
        actual_path = "./small_dicts";
    }

    struct stat st;
    if (stat(actual_path, &st) == -1) {
        perror("Erreur stat");
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        DIR *d=opendir(actual_path);
        if (d == NULL) {
            perror("Erreur opendir");
            return -1;
        }

        struct dirent *entry;
        size_t count = 0;
        Dictionary_t *temp_dicts = NULL;

        while ((entry = readdir(d))!= NULL) {

            if (entry->d_name[0] == '.') {
                continue;
            }

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", actual_path, entry->d_name);
            
            struct stat st;
            if (stat(full_path, &st) == -1 || !S_ISREG(st.st_mode)) {
                continue;
            }

            Dictionary_t *new_d = realloc(temp_dicts, (count + 1) * sizeof(Dictionary_t));
            if (new_d == NULL) {
                free(temp_dicts);
                closedir(d);
                return -1;
            }
            temp_dicts = new_d;

            if (load_single_dictionary(&temp_dicts[count], full_path, (uint32_t)count) == 0) {
                count++;
            }
        }

        *dicts = temp_dicts;
        *dict_count = count;
        closedir(d);
    }
    else if (S_ISREG(st.st_mode)) {
        *dicts = malloc(sizeof(Dictionary_t));
        if (*dicts == NULL) return -1;

        if (load_single_dictionary(&(*dicts)[0], actual_path, 0) == 0) {
            *dict_count = 1;
        } else {
            free(*dicts);
            return -1;
        }
    }
    return 0;
}