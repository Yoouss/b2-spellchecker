#include <stdio.h>
#include "common.h"
#include "pretty_print.h"
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <getopt.h>
#include "detector.h"
#include "corrector.h"

void free_args(CommandLineArgs_t* args) {
    if (args) {
        if (args->output_path != NULL)
            free(args->output_path);
        free(args->input_path);
        free(args->mode);
        free(args);
    }
}

void print_help(const char* program_name) {
    printf("Usage: %s --dicts <dictionary_path> --input <input_path> [--output <output_path>] [--mode <mode>] [--verbose]\n", program_name);
    printf("Modes: detection\n");
    printf("Options:\n");
    printf("  -d|--dicts <path>          Path to the dictionaries directory.\n");
    printf("  -i|--input <path>          Path to input .bin file to read words from.\n");
    printf("  -o|--output <path>         Path to output file to write results to (default is stdout).\n");
    printf("  -m|--mode <mode>           Mode of operation (e.g., detection).\n");
    printf("  -v|--verbose               Enable verbose output.\n");
    printf("  -t|--threads <num>         Number of threads to use (default is 1).\n");
    printf("  -h|--help                  Show this help message.\n");
    printf("Examples:\n");
    printf("  %s --dicts ./dictionaries --input input.bin \n", program_name);
    printf("  %s --dicts ./dictionaries --mode detection --input input.bin --dicts dicts/ --output output\n", program_name);
    printf("  %s --help\n", program_name);
    printf("Note: If no mode is specified, the default is 'championship'.\n");
    printf("Note: If no output file is specified, the program will write to stdout.\n");
}

CommandLineArgs_t* parse_args(int argc, char const *argv[]) {
    CommandLineArgs_t* args = (CommandLineArgs_t*) malloc(sizeof(CommandLineArgs_t)); 
    args->output_path = NULL;
    args->verbose = false;
    args->input_path = NULL;
    args->mode = NULL;
    args->dictionnaries_path[0] = '\0';
    args->threads = 1;
    
    int opt;
    struct option long_options[] = {
        {"dicts", required_argument, NULL, 'd'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"mode", required_argument, NULL, 'm'},
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"threads", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    while ((opt = getopt_long(argc, (char* const*)argv, "d:i:o:m:vht:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
                strncpy(args->dictionnaries_path, optarg, MAX_PATH_LENGTH - 1);
                args->dictionnaries_path[MAX_PATH_LENGTH - 1] = '\0';
                break;
            case 'i':
                args->input_path = strdup(optarg);
                break;
            case 'o':
                args->output_path = strdup(optarg);
                break;
            case 'm':
                args->mode = strdup(optarg);
                if (args->mode == NULL) {
                    perror("Failed to allocate memory for mode");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                args->threads = atoi(optarg);
                break;
            case 'v':
                args->verbose = true;
                break;
            case 'h':
                print_help(argv[0]);
                free_args(args);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Unknown argument: %s\n", argv[optind - 1]);
                exit(EXIT_FAILURE);
        }
    }

    if (args->mode == NULL) {
        args->mode = strdup("detection");
        if (args->mode == NULL) {
            perror("Failed to allocate memory for mode");
            exit(EXIT_FAILURE);
        }
    }

    return args;
}

int main(int argc, char const *argv[]) {
    CommandLineArgs_t* args = parse_args(argc, argv);

    printf("Dictionary Path: %s\n", strlen(args->dictionnaries_path) > 0 ? args->dictionnaries_path : "Not Provided");
    printf("Input File: %s\n", args->input_path ? args->input_path : "Not Provided");
    printf("Output File: %s\n", args->output_path ? args->output_path : "Not Provided");
    printf("Mode: %s\n", args->mode ? args->mode : "Not Provided");
    printf("Verbose: %s\n", args->verbose ? "Enabled" : "Disabled");
    printf("Threads: %u\n\n", args->threads);

    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    if (strlen(args->dictionnaries_path) > 0) {
        load_dictionaries(args->dictionnaries_path, &dicts, &dicts_count);
    }

    if (args->input_path) {
        read_input_file(args->input_path, &lines, &lines_sizes, &line_count);
    }

    uint32_t** matrixOfBadWordsIndexes = words_in_file(args->input_path, dicts, dicts_count);

    printf("Detection des erreurs du fichier %s : \n\n", args->input_path);

    for (int i = 0; i < line_count; i++) {
        char* line = lines[i];
        uint32_t numberOfBadWords = matrixOfBadWordsIndexes[i][0];
        uint32_t* IndexesOfBadWords = matrixOfBadWordsIndexes[i] + 1;

        pretty_print_detection(line, i, numberOfBadWords, IndexesOfBadWords);
    }

    printf("\n\nCorrection des erreurs du fichier %s : \n\n", args->input_path);

    for (int i = 0; i < line_count; i++) {
        char* line = lines[i];

        uint32_t numberOfBadWords = matrixOfBadWordsIndexes[i][0];
        uint32_t* IndexesOfBadWords = matrixOfBadWordsIndexes[i] + 1;

        Dictionary_t* dict = find_candidate_dict_for_line(line, dicts, dicts_count);

        char** corrections = malloc(numberOfBadWords * sizeof(char*));
        if (corrections == NULL) return -1;

        // copie de la ligne pour récupérer les mots
        char* lineCopy = strdup(line);
        char* words[lines_sizes[i]]; 
        int word_count = 0;

        char* word = strtok(lineCopy, SEPARATORS);
        while (word != NULL) {
            words[word_count++] = word;
            word = strtok(NULL, SEPARATORS);
        }

        for (int j = 0; j < numberOfBadWords; j++) {
            char* badWord = words[IndexesOfBadWords[j]];
            corrections[j] = get_word_correction(badWord, dict);
        }

        pretty_print_correction(line, i, numberOfBadWords, IndexesOfBadWords, corrections);

        free(corrections);
        free(lineCopy);
    }
    
    free_matrix(matrixOfBadWordsIndexes, line_count);
    free_args(args);
    return 0;
}