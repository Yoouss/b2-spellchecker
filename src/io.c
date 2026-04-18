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
    // TODO
    return NULL;
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