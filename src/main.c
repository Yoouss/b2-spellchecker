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

// /!\ : viens des tests (copier-coller)
// TODO : eviter ce duplicage de code (Younes s'en occupera)
static void free_file_detection(file_t* file_detection) {
    if (file_detection == NULL) return;

    size_t incorrect_lines_count = file_detection->incorrect_lines_count;
    for (size_t i = 0; i < incorrect_lines_count; i++) {
        free(file_detection->incorrect_lines[i].wrong_words_indexes);
    }

    free(file_detection->incorrect_lines_indexes);
    free(file_detection);
}

// BEAUCOUP DE FUITES DE MEMOIRE ICI
// TODO : créer des fonctions pour free les grosses structures tel que dicts, ect...
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

    file_t* file_detection = scan_file_for_errors(args->input_path, dicts, dicts_count);
    if (file_detection == NULL) {
        free(lines_sizes);
        free_args(args);
        return -1;
    }

    line_t* incorrect_lines = file_detection->incorrect_lines;
    size_t* incorrect_lines_indexes = file_detection->incorrect_lines_indexes;
    size_t incorrect_lines_count = file_detection->incorrect_lines_count;

    printf("Correction des erreurs du fichier %s : \n\n\n", args->input_path);

    for (size_t i = 0; i < incorrect_lines_count; i++) {
        size_t line_index = incorrect_lines_indexes[i]; 
        char* line = lines[line_index];

        line_t* current_line_detection = &incorrect_lines[i];
        uint32_t* wrong_words_indexes = current_line_detection->wrong_words_indexes;
        uint32_t wrong_words_count = current_line_detection->wrong_words_count;

        Dictionary_t* dict = find_candidate_dict_for_line(line, dicts, dicts_count);

        char** corrections = malloc(wrong_words_count * sizeof(char*));
        if (corrections == NULL) {
            free_file_detection(file_detection);
            free(lines_sizes);
            free_args(args);
            return -1;
        }

        char** wrong_words = get_wrong_words_in_line(line, wrong_words_indexes, wrong_words_count);

        for (uint32_t j = 0; j < wrong_words_count; j++) {
            corrections[j] = get_word_correction(wrong_words[j], dict);
        }

        pretty_print_correction(line, line_index+1, wrong_words_count, wrong_words_indexes, corrections);

        free(corrections);
        free(wrong_words);
    }

    free_file_detection(file_detection);
    free(lines_sizes);
    free_args(args);

    return 0;
}