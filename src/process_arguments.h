#pragma once

#include "exit_error.h"

void process_arguments(
        const int* argc,
        STRING argv[],
        STRING* input,
        STRING* output,
        bool* verbose
) {
    for (int i = 1; i < *argc; i++) {
        if (ARE_EQ(argv[i], HELP1, HELP2)) {
            printf(HELP_MESSAGE);
            exit(0);
        }

        if (ARE_EQ(argv[i], OUTPUT1, OUTPUT2) && i + 1 < *argc) {
            *output = argv[++i];
        }
        else if (ARE_EQ(argv[i], VERBOSE1, VERBOSE2)) {
            *verbose = true;
        }
        else if (argv[i][0] != DASH) {
            *input = argv[i];
        }
        else {
            exit_error(UNKNOWN_OPTION_TEXT, argv[i], argv[0]);
        }
    }
    if (*input == NULL) {
        exit_error(NO_INPUT_FILE_TEXT, argv[0]);
    }
}