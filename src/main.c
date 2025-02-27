#include "all.h"

#include "process_arguments.h"
#include "process_xm.h"

int MAIN(const int argc, STRING argv[]) {
    STRING input = NULL;
    STRING output = CURRENT_DIR;
    bool verbose = false;
    bool skip_empty_instruments = false;

    process_arguments(&argc, argv, &input, &output, &verbose, &skip_empty_instruments);

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
    process_xm(fb, input, output, verbose, skip_empty_instruments);
    DEBUG(SUCCESS_TEXT);
    free_file_buffer(fb);
    return 0;
}
