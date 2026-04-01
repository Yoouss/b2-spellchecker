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

// Il pourrait y avoir d'autres fonctions mais je ne pense pas que ça soit le cas

int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count) {
    int file_descriptor = open(input_path, O_RDONLY); //test de lecture 
    if (file_descriptor == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        return  -1;
    }

    struct stat fileStat; 
    if (fstat(file_descriptor, &fileStat) == -1) {
        perror("Erreur fstat");
        close(file_descriptor);
        return -1;
    }
    //taille du fichier
    size_t fileSize = fileStat.st_size;

    //si fichier vide
    if (fileSize == 0) {
        *lines = NULL;
        *line_sizes = NULL;
        *line_count = 0;
        close(file_descriptor);
        return 0;
    }
    //mapping mémoire -> racourci pour visualiser le fichier
    char *file_map = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    if (file_map == MAP_FAILED) {
        perror("Erreur mmap");
        close(file_descriptor);
        return -1;
    }
    close(file_descriptor);

    
    size_t count = 0;
    char **temp_lines = NULL;
    uint32_t *temp_sizes = NULL;
    int start = 0;

    for (int i = 0; i <= (int)fileSize; i++) {
        if (i == (int)fileSize || file_map[i] == '\n') {
            int length = i - start; //longueur de la ligne
            char **new_lines = realloc(temp_lines, (count + 1) * sizeof(char *));
            uint32_t *new_sizes = realloc(temp_sizes, (count + 1) * sizeof(uint32_t));

            if (!new_lines || !new_sizes) {
                for (size_t j = 0; j < count; j++) free(temp_lines[j]);
                free(temp_lines);
                free(temp_sizes);
                munmap(file_map, fileSize);
                return -1;
            }
            temp_lines = new_lines;
            temp_sizes = new_sizes;

            char *line = malloc(length + 1);
            if (line) {//copie des caractéres ->nouvelle chaîné
                for (int j = 0; j < length; j++) {
                    line[j] = file_map[start + j];
                }
                line[length] = '\0'; 
                temp_lines[count] = line;
                temp_sizes[count] = (uint32_t)length;
                count++;
            }
            start = i + 1;
        }
    }
    //résulats et libération du mapping
    *lines = temp_lines;
    *line_sizes = temp_sizes;
    *line_count = count;
    if (munmap(file_map, fileSize) == -1) {
        perror("Erreur munmap");
    }
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