#include <stdio.h>
#include <io.h>
#include "common.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <portable_endian.h>
#include <detector.h>

OutputStreams_t *open_outputs(const char *pathname) {
    if (pathname == NULL || strlen(pathname) == 0) return NULL;

    // TODO : mettre dans report.md qu'on peut pas mettre ./<output_path>
    if (strrchr(pathname, '.') != NULL) { 
        return NULL;
    }

    OutputStreams_t* output_streams = malloc(sizeof(OutputStreams_t));
    if (output_streams == NULL) return NULL;

    int pathname_length = strlen(pathname);
    
    char detection_file_pathname[pathname_length + 5];
    strcpy(detection_file_pathname, pathname);
    strcat(detection_file_pathname, ".err");

    char correction_file_pathname[pathname_length + 5];
    strcpy(correction_file_pathname, pathname);
    strcat(correction_file_pathname, ".fix");
    
    int detection_file = open(detection_file_pathname, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP);
    if (detection_file == -1) {
        perror("Erreur lors de l'ouverture du fichier pour la detection : "); 
        free(output_streams);
        return NULL;
    }

    int correction_file = open(correction_file_pathname, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP);
    if (correction_file == -1) {
        perror("Erreur lors de l'ouverture du fichier pour la correction : ");
        free(output_streams);
        close(detection_file);
        return NULL;
    }

    output_streams->detection = detection_file;
    output_streams->correction = correction_file;

    return output_streams;
}

void close_outputs(OutputStreams_t *streams) {
    if (streams == NULL) return;
    
    if (streams->detection >= 0) {
        close(streams->detection);
    }
    if (streams->correction >= 0) {
        close(streams->correction);
    }
    free(streams);
}

int write_detection(OutputStreams_t *output_stream, uint32_t line_number,
                    uint32_t dict_index, uint32_t word_count, uint32_t *word_indices) {

    int detection_file_descriptor;
    if (output_stream == NULL) {
        detection_file_descriptor = 1;
    }
    else {
        detection_file_descriptor = output_stream->detection;
    }

    uint32_t line_number_endian = htobe32(line_number);
    if (write(detection_file_descriptor, &line_number_endian, sizeof(uint32_t)) == -1) return -1;

    uint32_t dict_index_endian = htobe32(dict_index);
    if (write(detection_file_descriptor, &dict_index_endian, sizeof(uint32_t)) == -1) return -1;

    uint32_t word_count_endian = htobe32(word_count);
    if (write(detection_file_descriptor, &word_count_endian, sizeof(uint32_t)) == -1) return -1;

    if (word_count == 0 || word_indices == NULL) return 0;
    
    for (int i = 0; i < word_count; i++) {
        uint32_t word_indice_edian = htobe32(word_indices[i]);
        if (write(detection_file_descriptor, &word_indice_edian, sizeof(uint32_t)) == -1) return -1;
    }

    return 0;
}

int write_all_detection(OutputStreams_t *output_stream, Dictionary_t* dicts, size_t dicts_count, char** lines, size_t line_count) {
    if (dicts == NULL || dicts_count == 0 || lines == NULL || line_count == 0) return -1;

    for (size_t i = 0; i < line_count; i++) {
        char* current_line = lines[i];
        line_t* line_detection = scan_line_for_errors(current_line, dicts, dicts_count);
        if (line_detection == NULL) return -1;

        uint32_t wrong_words_count = line_detection->wrong_words_count;
        uint32_t used_dict_id = line_detection->used_dict_id;
        uint32_t* wrong_words_indexes = line_detection->wrong_words_indexes;

        write_detection(output_stream, i, used_dict_id, wrong_words_count, wrong_words_indexes);
    }

    return 0;
}

int write_correction(OutputStreams_t *output_streams, uint32_t word_count, char **corrections) {
    if (word_count == 0 || corrections == NULL) {
        return 0; 
    }

    int fd;
    if (output_streams == NULL) {
        fd = STDOUT_FILENO;
    } else {
        fd = output_streams->correction;
    }

    for (uint32_t i = 0; i < word_count; i++) {
        uint32_t n = (uint32_t)strlen(corrections[i]);
        
        uint32_t n_be = htobe32(n);

        if (write(fd, &n_be, sizeof(uint32_t)) == -1) {
            return -1;
        }

        if (write(fd, corrections[i], n) == -1) {
            return -1;
        }
        
    }

    return 0;
}