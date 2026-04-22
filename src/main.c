#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <common.h>
#include <corrector.h>
#include <detector.h>
#include <file_handler.h>
#include <io.h>
#include <pretty_print.h>

int num_threads;

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

// BEAUCOUP DE FUITES DE MEMOIRE ICI
// TODO : créer des fonctions pour free les grosses structures tel que dicts, ect...
int main(int argc, char const *argv[]) {
    CommandLineArgs_t* args = parse_args(argc, argv);

    num_threads = args->threads > 1 ? args->threads : 1;

    printf("Dictionary Path: %s\n", strlen(args->dictionnaries_path) > 0 ? args->dictionnaries_path : "Not Provided");
    printf("Input File: %s\n", args->input_path ? args->input_path : "Not Provided");
    printf("Output File: %s\n", args->output_path ? args->output_path : "Not Provided");
    printf("Mode: %s\n", args->mode ? args->mode : "Not Provided");
    printf("Verbose: %s\n", args->verbose ? "Enabled" : "Disabled");
    printf("Threads: %u\n\n", num_threads);

    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

    // utilisation de THREADS
    if (strlen(args->dictionnaries_path) > 0) {
        load_dictionaries(args->dictionnaries_path, &dicts, &dicts_count);
    }
    else {
        perror("Veillez spécifier le/les dictionnaires à utiliser");
        return -1;
    }

    char** lines = NULL;
    uint32_t* lines_sizes = NULL;
    size_t line_count = 0;

    if (args->input_path) {
        read_input_file(args->input_path, &lines, &lines_sizes, &line_count);
    }
    else {
        perror("Veillez spécifier le fichier d'entrée");
        return -1;
    }

    printf("Correction des erreurs du fichier %s : \n\n\n", args->input_path);

    for (size_t i = 0; i < line_count; i++) {
        char* current_line = lines[i];
        line_t* line_detection = scan_line_for_errors(current_line, dicts, dicts_count);
        if (line_detection == NULL) continue;

        uint32_t* wrong_words_indexes = line_detection->wrong_words_indexes;
        uint32_t wrong_words_count = line_detection->wrong_words_count;

        char** corrections = malloc(wrong_words_count * sizeof(char*));
        if (corrections == NULL) {
            free_line_detection(line_detection);
            free_lines(lines, line_count);
            free(lines_sizes);
            free_args(args);
            return -1;
        }

        char** wrong_words = get_wrong_words_in_line(current_line, wrong_words_indexes, wrong_words_count);
        Dictionary_t* used_dictionary = line_detection->used_dictionary;

        // utilisation de THREADS (seulement si le nombre de mauvais mots >= 2 * num_threads)
        for (size_t j = 0; j < wrong_words_count; j++) {
            corrections[j] = get_word_correction(wrong_words[j], used_dictionary);
        }

        size_t current_line_number = i + 1;
        pretty_print_correction(current_line, current_line_number, wrong_words_count, wrong_words_indexes, corrections);

        free_line_detection(line_detection);
        free(corrections);
        free(wrong_words);
    }

    free_lines(lines, line_count);
    free(lines_sizes);
    free_args(args);

    return 0;
}