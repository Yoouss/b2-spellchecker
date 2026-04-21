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

OutputStreams_t *open_outputs(const char *pathname) {
    if (pathname == NULL) return NULL;

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
    // TODO
    return;
}

int write_detection(OutputStreams_t *output_stream, uint32_t line_number,
                    uint32_t dict_index, uint32_t word_count, uint32_t *word_indices) {

    if (word_count == 0 || word_indices == NULL) return -1;

    int detection_file_descriptor;

    if (output_stream == NULL) detection_file_descriptor = 1;

    else detection_file_descriptor = output_stream->detection;
    
    char* buffer = malloc(2048);
    if (buffer == NULL) return -1;

    int current_position = sprintf(buffer, "n=%d|d=%d|nb=%d|[", line_number, dict_index, word_count);

    for (int i = 0; i < word_count; i++) {
        if (i == word_count - 1) current_position += sprintf(buffer + current_position, "%d]", word_indices[i]);
        
        else current_position += sprintf(buffer + current_position, "%d,", word_indices[i]);
    }

    current_position += sprintf(buffer + current_position, "\n");

    if (write(detection_file_descriptor, buffer, current_position) == -1) {
        perror("Erreur lors de l'écriture de la détection : ");
        return -1;
    }
   
    free(buffer);

    return 0;        
}

int write_correction(OutputStreams_t *output_streams, uint32_t word_count, char **corrections) {
    // TODO
    return 0;
}