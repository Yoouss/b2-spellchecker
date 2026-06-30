#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#include <corrector.h>
#include <file_handler.h>

void free_dictionaries(Dictionary_t* dictionaries, size_t dictionaries_count) {
    if (dictionaries == NULL) return;

    for (size_t i = 0; i < dictionaries_count; i++) {
        free_array_of_words(dictionaries[i].words, dictionaries[i].word_count);
        free(dictionaries[i].lang);
    }

    free(dictionaries);
}

char* map_file(const char* file_path, size_t* file_size) {
    int file_descriptor = open(file_path, O_RDONLY);
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
    
    char *mapped_file = mmap(NULL, *file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    close(file_descriptor);

    if (mapped_file == MAP_FAILED) {
        perror("Erreur mmap");
        return NULL;
    }

    return mapped_file;
}

void set_line_count(const char* mapped_file, size_t file_size, size_t* line_count) {
    // Précaution
    *line_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (mapped_file[i] == '\n') (*line_count)++;
    }
    
    if (file_size > 0 && mapped_file[file_size - 1] != '\n') {
        (*line_count)++;
    }
}

int set_lines_sizes(const char* mapped_file, size_t file_size, uint32_t** lines_sizes, size_t line_count) {
    *lines_sizes = malloc(line_count * sizeof(uint32_t));
    if (*lines_sizes == NULL) return -1;

    size_t line_index = 0;
    size_t current_chars_count = 0;

    for (size_t i = 0; i < file_size; i++) {
        if (mapped_file[i] == '\n') {
            (*lines_sizes)[line_index] = current_chars_count;

            line_index++;
            current_chars_count = 0;
        } 
        else {
            current_chars_count++;
        }
    }
    
    // Cas spécial où la dernière ligne n'a pas de retour à la ligne à sa fin
    if (file_size > 0 && mapped_file[file_size - 1] != '\n') {
        (*lines_sizes)[line_index] = current_chars_count;
    }

    return 0;
}

int set_lines(char*** lines, uint32_t* lines_sizes, size_t line_count) {
    *lines = malloc(line_count * sizeof(char *));
    if (*lines == NULL) return -1;

    for (size_t i = 0; i < line_count; i++) {
        size_t line_size = lines_sizes[i];

        char* line = malloc((line_size + 1) * sizeof(char));
        if (line == NULL) {
            free_array_of_words(*lines, i);
            return -1;
        }

        line[line_size] = '\0';
        (*lines)[i] = line;
    }

    return 0;
}

void set_read_input_data(read_input_data_t* read_input_data, char** lines, uint32_t* lines_sizes, 
                         size_t line_count, char* mapped_file, size_t file_size, size_t chunk) {      

    for (size_t i = 0; i < num_threads; i++) {
        read_input_data[i].lines = lines;
        read_input_data[i].lines_sizes = lines_sizes;
        read_input_data[i].line_count = line_count;

        read_input_data[i].mapped_file = mapped_file;
        read_input_data[i].start_index = i * chunk;
        read_input_data[i].end_index = (i == num_threads - 1) ? file_size : (i + 1) * chunk;
    }
}

void set_start_line_index_and_file_offset(size_t* start_line_index, size_t* file_offset, read_input_data_t* read_input_data) {
    uint32_t* lines_sizes = read_input_data->lines_sizes;
    size_t line_count = read_input_data->line_count;
    size_t start_index = read_input_data->start_index;

    // Précaution
    *start_line_index = 0;
    *file_offset = 0;

    while (*start_line_index < line_count) {
        size_t line_end_index = *file_offset + lines_sizes[*start_line_index];
        if (line_end_index >= start_index) break;

        *file_offset += lines_sizes[*start_line_index];

        // Ca nous aide à éviter le cas où la dernière ligne n'a pas de \n à sa fin
        if (*start_line_index < line_count - 1) {
            (*file_offset)++; 
        }

        (*start_line_index)++;
    }
}

void read_chunk_of_input_file(read_input_data_t* read_input_data) {
    size_t start_line_index = 0;
    size_t file_offset = 0;
    set_start_line_index_and_file_offset(&start_line_index, &file_offset, read_input_data);

    size_t start_index = read_input_data->start_index;
    size_t write_offset = start_index - file_offset;

    size_t current_line = start_line_index;
    size_t current_index_in_line = write_offset;

    char* mapped_file = read_input_data->mapped_file;
    char** lines = read_input_data->lines;
    size_t end_index = read_input_data->end_index;

    for (size_t i = start_index; i < end_index; i++) {
        char c = mapped_file[i];

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

void* read_input_file_thread(void* args) {
    read_input_data_t* read_input_data = (read_input_data_t *) args;
    read_chunk_of_input_file(read_input_data);

    pthread_exit(NULL);
}

int read_input_file(char* input_path, char*** lines, uint32_t** lines_sizes, size_t* line_count) {
    size_t file_size = 0;
    char* mapped_file = map_file(input_path, &file_size);
    if (mapped_file == NULL) return -1;

    set_line_count(mapped_file, file_size, line_count);

    if (set_lines_sizes(mapped_file, file_size, lines_sizes, *line_count) == -1) {
        munmap(mapped_file, file_size);
        return -1;
    }

    if (set_lines(lines, *lines_sizes, *line_count) == -1) {
        free(*lines_sizes);
        munmap(mapped_file, file_size);
        return -1;
    }

    read_input_data_t* read_input_data = malloc(num_threads * sizeof(read_input_data_t));
    if (read_input_data == NULL) {
        free(*lines_sizes);
        free_array_of_words(*lines, *line_count);
        munmap(mapped_file, file_size);
        return -1;
    }

    size_t chunk = file_size / num_threads;
    set_read_input_data(read_input_data, *lines, *lines_sizes, *line_count, mapped_file, file_size, chunk);

    if (num_threads == 1) {
        read_chunk_of_input_file(read_input_data);
    }

    else if (num_threads > 1) {
        pthread_t threads[num_threads];
        for (size_t i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, read_input_file_thread, &read_input_data[i]);
        }

        for (size_t i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    // Précaution (num_threads est normalement bien configuré à être plus grand ou égal à 1 dans src/main.c)
    else {
        perror("Le nombre de threads ne peut pas être inférieur à 1 ! Si c'est le cas, il doit être forcé à 1 dans src/main.c !");
        free(read_input_data);
        free(*lines_sizes);
        free_array_of_words(*lines, *line_count);
        munmap(mapped_file, file_size);
        return -1;
    }

    free(read_input_data);
    munmap(mapped_file, file_size);
    
    return 0;
}

char* get_language(const char* filepath) {
    char* filename = strrchr(filepath, '/');
    
    if (filename == NULL) {
        filename = (char *) filepath;
    } else {
        // We jump one char after the '/'
        filename++;
    }

    char* language = strdup(filename);

    char* extension = strstr(language, ".dict");
    if (extension != NULL) {
        *extension = '\0';
    }
    return language;
}

int load_single_dictionary(Dictionary_t *dict, const char *filepath, uint32_t id) {

    uint32_t* tmp_sizes = NULL;
    size_t tmp_count = 0;

    if (read_input_file((char *) filepath, &dict->words, &tmp_sizes, &tmp_count) != 0) {
        return -1;
    }

    dict->word_count = (uint32_t) tmp_count;
    dict->lang = get_language(filepath);
    dict->id = id;

    free(tmp_sizes);
    return 0;
}

int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count) {
    const char *actual_path = path;
    if (strcmp(path, "all") == 0) {
        actual_path = "./dicts";
    }

    struct stat file_stat;
    if (stat(actual_path, &file_stat) == -1) {
        perror("Erreur stat");
        return -1;
    }
    if (S_ISDIR(file_stat.st_mode)) {
        DIR* directory = opendir(actual_path);
        if (directory == NULL) {
            perror("Erreur opendir");
            free_dictionaries(*dicts, *dict_count);
            return -1;
        }

        struct dirent *directory_entry;
        uint32_t tmp_dict_count = 0;
        Dictionary_t *tmp_dicts = NULL;

        while ((directory_entry = readdir(directory)) != NULL) {

            if (directory_entry->d_name[0] == '.') {
                continue;
            }

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", actual_path, directory_entry->d_name);
            
            struct stat file_stat;
            if (stat(full_path, &file_stat) == -1 || !S_ISREG(file_stat.st_mode)) {
                continue;
            }

            Dictionary_t* new_dictionary = realloc(tmp_dicts, (tmp_dict_count + 1) * sizeof(Dictionary_t));
            if (new_dictionary == NULL) {
                free_dictionaries(tmp_dicts, tmp_dict_count);
                closedir(directory);
                return -1;
            }
            tmp_dicts = new_dictionary;

            if (load_single_dictionary(&tmp_dicts[tmp_dict_count], full_path, tmp_dict_count) == 0) {
                tmp_dict_count++;
            }
        }

        *dicts = tmp_dicts;
        *dict_count = tmp_dict_count;
        closedir(directory);
    }

    else if (S_ISREG(file_stat.st_mode)) {
        *dicts = malloc(sizeof(Dictionary_t));
        if (*dicts == NULL) return -1;

        if (load_single_dictionary(&(*dicts)[0], actual_path, 0) == 0) {
            *dict_count = 1;
        } else {
            free_dictionaries(*dicts, *dict_count);
            return -1;
        }
    }
    return 0;
}