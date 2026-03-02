/**
 * @brief This header file contains common definitions and structures used in the spellchecker.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_PATH_LENGTH 256

#define DEFAULT_DICT_PATH "./wallon.dict"

#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define RESET   "\033[0m"

typedef struct CommandLineArgs {
    char dictionnaries_path[MAX_PATH_LENGTH];
    char* output_path;
    bool verbose;
    char* input_path;
    char* mode;
    uint32_t threads;
} CommandLineArgs_t;

typedef struct Dictionary {
    char** words;
    uint32_t word_count;
    char* lang;
    uint32_t id;
} Dictionary_t;

#endif // COMMON_H