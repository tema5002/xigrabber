#include "all.h"

#include "process_xm.h"

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

int MAIN(const int argc, STRING argv[]) {
    STRING input = NULL;
    STRING output = CURRENT_DIR;
    bool verbose = false;

    process_arguments(&argc, argv, &input, &output, &verbose);

    file_buffer fb = create_file_buffer(input);

    DEBUGC(XIGRABBER_VERSION "\n");
    DEBUG(VERBOSE_MODE_ENABLED_TEXT);

    if (!DIRECTORY_EXISTS(output)) {
        DEBUG(DIR_NOT_EXISTS_TEXT, output);
        if (CREATE_DIRECTORY(output)) {
            exit_error(DIR_CREATE_FAILED_TEXT, output);
        }
        DEBUG(DIR_CREATE_SUCCESS_TEXT, output);
    }
    DEBUG(EXTRACTING_INSTRUMENTS_TEXT, input, output);
    process_xm(fb, input, output, verbose);
    DEBUG(SUCCESS_TEXT);
    free_file_buffer(fb);
    return 0;
}
