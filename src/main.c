#include "all.h"

#include "process_arguments.h"
#include "process_xm.h"

int MAIN(const int argc, STRING argv[]) {
    process_arguments(&argc, argv);

    file_buffer fb = create_file_buffer(arguments.input);

    DEBUGC(XIGRABBER_VERSION_FULL "\n");
    DEBUG(VERBOSE_MODE_ENABLED_TEXT);

    if (!DIRECTORY_EXISTS(arguments.output)) {
        DEBUG(DIR_NOT_EXISTS_TEXT, arguments.output);
        if (CREATE_DIRECTORY(arguments.output)) {
            exit_error(DIR_CREATE_FAILED_TEXT, arguments.output);
        }
        DEBUG(DIR_CREATE_SUCCESS_TEXT, arguments.output);
    }
    DEBUG(EXTRACTING_INSTRUMENTS_TEXT, arguments.input, arguments.output);
    process_xm(fb);
    DEBUG(SUCCESS_TEXT);
    free_file_buffer(fb);
    return 0;
}
