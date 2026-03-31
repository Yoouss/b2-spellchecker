#include <stdio.h>
#include "common.h"
#include <io.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>

// Il pourrait y avoir d'autres fonctions mais je ne pense pas que ça soit le cas

int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count) {
    int file_descriptor = open(input_path, O_RDONLY);
    if (file_descriptor == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    struct stat st;
    if (fstat(file_descriptor, &st) == -1) {
        perror("Erreur fstat");
        close(file_descriptor);
        return -1;
    }

    if (st.st_size == 0) {
        *lines = NULL;
        *line_sizes = NULL;
        *line_count = 0;
        close(file_descriptor);
        return 0;
    }

    char *buffer = malloc(st.st_size + 1);
    if (!buffer) {
        close(file_descriptor);
        return -1;
    }

    ssize_t bytes_read = read(file_descriptor, buffer, st.st_size);
    if (bytes_read == -1) {
        perror("Erreur read");
        free(buffer);
        close(file_descriptor);
        return -1;
    }

    buffer[bytes_read] = '\0';
    close(file_descriptor);

    size_t count = 0;
    char **temp_lines = NULL;
    uint32_t *temp_sizes = NULL;
    int start = 0;
    for (int i = 0; i <= bytes_read; i++) {
        if (buffer[i] == '\n' || (buffer[i] == '\0' && i > start)) {
            int length = i - start;

            temp_lines = realloc(temp_lines, (count + 1) * sizeof(char *));
            temp_sizes = realloc(temp_sizes, (count + 1) * sizeof(uint32_t));

            if (temp_lines == NULL || temp_sizes == NULL) {
                free(buffer);
                return -1;
            }

            char *line = malloc((length + 1) * sizeof(char));
            if (line) {
                for (int j = 0; j < length; j++) {
                    line[j] = buffer[start + j];
                }
                line[length] = '\0';
                temp_lines[count] = line;
                temp_sizes[count] = (uint32_t)length;
                count++;
            }
            start = i + 1;
        }
    }

    *lines = temp_lines;
    *line_sizes = temp_sizes;
    *line_count = count;

    free(buffer);
    return 0;
}

int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count)  {
    // TODO
    return 0;
}
