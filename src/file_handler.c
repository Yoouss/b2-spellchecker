#include <stdio.h>
#include "common.h"
#include <io.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

static size_t count_lines(const char* map, size_t size) {
    size_t n = 0;
    for (size_t i = 0; i < size; i++) {
        if (map[i] == '\n') n++;
    }
    
    if (size > 0 && map[size - 1] != '\n') {
        n++;
    }
    return n;
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

int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count) {

    size_t file_size = 0;
    char *file_map = map_file(input_path, &file_size);
    if (!file_map) return -1;

    size_t total_lines = count_lines(file_map, file_size);

    *lines = malloc(total_lines * sizeof(char *));
    *line_sizes = malloc(total_lines * sizeof(uint32_t));

    if (!*lines || !*line_sizes) {
        clean_lines(*lines, *line_sizes, 0);
        munmap(file_map, file_size);
        return -1;
    }

    size_t current_line = 0;
    size_t start = 0;

    for (size_t i = 0; i <= file_size; i++) {
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
    
    *line_count = current_line;
    munmap(file_map, file_size);
    return 0;
}


int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count)  {
    DIR *d;
    struct dirent *entry;
    size_t count = 0;
    Dictionary_t *temp_dicts = NULL;

    d = opendir(path);
    if(d == NULL){
        return -1; //si le dossier n'existe pas/ pas accés
    }
    entry=readdir(d);
    while (entry != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..")==0){
            continue;
        }

        char full_path[1024]; //chemin du fichier
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        
        struct stat st;
        if (stat(full_path, &st) == -1 || !S_ISREG(st.st_mode)) {
            continue; //si pas un fichier ou infos pas lisibles --> on passe
        }

        Dictionary_t *new_d = realloc(temp_dicts, (count + 1) * sizeof(Dictionary_t));
        if (new_d == NULL){ //erreur -> donc on free l'espace
            free(temp_dicts);
            closedir(d);
            return -1;
        }
        temp_dicts = new_d;

        if (load_single_dictionary(&temp_dicts[count], full_path) ==0){
            count++;
        }
        else{ continue;}//fichier ilisible

        entry = readdir(d);//fichier suivant;


    }


    *dicts = temp_dicts;
    *dict_count = count;
    closedir(d);

    return 0;
}

int load_single_dictionary(Dictionary_t *dict, const char *filepath) {

    uint32_t *temp_sizes = NULL;
    size_t temp_count = 0;

    if (read_input_file((char *)filepath, &dict->words, &temp_sizes, &temp_count) != 0) {
        return -1; //échec de la lectuers
    }

    dict->word_count = (uint32_t)temp_count;
    dict->lang = NULL;
    dict ->id=0;
    //id et langque initialisé pour avoir quelque chose en mémoire


    free(temp_sizes);
    return 0;
}