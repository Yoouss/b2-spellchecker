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
bool verbose;
char* mode;

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

    num_threads = args->threads > 1 ? args->threads : 1;
    verbose = args->verbose;
    mode = args->mode ? args->mode : "detection";

    fprintf(stderr, "Dictionary Path: %s\n", strlen(args->dictionnaries_path) > 0 ? args->dictionnaries_path : "Not Provided");
    fprintf(stderr, "Input File: %s\n", args->input_path ? args->input_path : "Not Provided");
    fprintf(stderr, "Output File: %s\n", args->output_path ? args->output_path : "Not Provided");
    fprintf(stderr, "Mode: %s\n", args->mode ? args->mode : "Not Provided");
    fprintf(stderr, "Verbose: %s\n", args->verbose ? "Enabled" : "Disabled");
    fprintf(stderr, "Threads: %u\n\n", num_threads);

    Dictionary_t* dicts = NULL;
    size_t dicts_count = 0;

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
        free_dictionaries(dicts, dicts_count);
        return -1;
    }

    OutputStreams_t* file_streams = open_outputs(args->output_path);

    if (strcmp(mode, "detection") == 0) {
        if (write_all_detection(file_streams, dicts, dicts_count, lines, line_count) == -1) {
            perror("Échec du programme");
            free_dictionaries(dicts, dicts_count);
            free_lines(lines, line_count);
            free(lines_sizes);
            free_args(args);
            return -1;
        }
    }
    else if (strcmp(mode, "correction") == 0) {
        if (write_all_detection_and_correction(file_streams, dicts, dicts_count, lines, line_count) == -1) {
            perror("Échec du programme");
            free_dictionaries(dicts, dicts_count);
            free_lines(lines, line_count);
            free(lines_sizes);
            free_args(args);
            return -1;
        }
    }
    else {
        perror("Mode invalide : veuillez choisir soit detection ou correction");
        free_dictionaries(dicts, dicts_count);
        free_lines(lines, line_count);
        free(lines_sizes);
        free_args(args);
        return -1;
    }

    close_outputs(file_streams);

    free_dictionaries(dicts, dicts_count);
    free_lines(lines, line_count);
    free(lines_sizes);
    free_args(args);

    fprintf(stderr, "\nFin du programme : succès \n");

    return 0;
}