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
    OutputStreams_t* output_streams = malloc(sizeof(OutputStreams_t));
    if (output_streams == NULL) return NULL;

    int pathname_length = strlen(pathname);
    
    char detection_file_pathname[pathname_length + 5];
    strcpy(detection_file_pathname, pathname);
    strcat(detection_file_pathname, ".err");

    char correction_file_pathname[pathname_length + 5];
    strcpy(correction_file_pathname, pathname);
    strcat(correction_file_pathname, ".fix");
    
    int detection_file = open(detection_file_pathname, O_WRONLY|O_CREAT);
    if (detection_file == -1) {
        perror("Erreur lors de l'ouverture du fichier pour la detection : "); 
        exit(EXIT_FAILURE);
    }

    int correction_file = open(correction_file_pathname, O_WRONLY|O_CREAT);
    if (correction_file == -1) {
        perror("Erreur lors de l'ouverture du fichier pour la correction : "); 
        close(detection_file);
        exit(EXIT_FAILURE);
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
    // TODO
    return 0;        
}

int write_correction(OutputStreams_t *output_streams, uint32_t word_count, char **corrections) {
    // TODO
    return 0;
}