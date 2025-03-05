#pragma once

#include "exit_error.h"

static struct {
    STRING input;
    STRING output;
    bool verbose;
    bool skip_empty_instruments;
    char name[23];
    bool name_used;
} arguments;

void init_arguments() {
    arguments.input = NULL;
    arguments.output = CURRENT_DIR;
    arguments.verbose = false;
    arguments.skip_empty_instruments = false;
    arguments.name[0] = '\0';
    arguments.name_used = false;
}

#define DEBUG(...) if (arguments.verbose) { PRINT(__VA_ARGS__); }
#define DEBUGC(...) if (arguments.verbose) { printf(__VA_ARGS__); }

void process_arguments(
        const int* argc,
        STRING argv[]
) {
    init_arguments();

    for (int i = 1; i < *argc; i++) {
        if (ARE_EQ(argv[i], HELP_ARG1, HELP_ARG2)) {
            puts(HELP_MESSAGE);
            exit(0);
        }

        if (IS_EQ(argv[i], VERSION_ARG1)) {
            puts(XIGRABBER_VERSION);
            exit(0);
        }

        if (ARE_EQ(argv[i], OUTPUT_ARG1, OUTPUT_ARG2) && i + 1 < *argc) {
            arguments.output = argv[++i];
        }
        else if (ARE_EQ(argv[i], NAME_ARG1, NAME_ARG2) && i + 1 < *argc) {
            arguments.name_used = true;
            i++;
            size_t j;
            for (j = 0; j < 22 && argv[i][j] != '\0'; j++) {
                arguments.name[j] = (char)argv[i][j];
            }
            arguments.name[j] = '\0';
        }
        else if (ARE_EQ(argv[i], VERBOSE_ARG1, VERBOSE_ARG2)) {
            arguments.verbose = true;
        }
        else if (ARE_EQ(argv[i], SKIPEMPTY_ARG1, SKIPEMPTY_ARG2)) {
            arguments.skip_empty_instruments = true;
        }
        else if (argv[i][0] != DASH) {
            arguments.input = argv[i];
        }
        else {
            exit_error(UNKNOWN_OPTION_TEXT, argv[i], argv[0]);
        }
    }
    if (arguments.input == NULL) {
        exit_error(NO_INPUT_FILE_TEXT, argv[0]);
    }
}
