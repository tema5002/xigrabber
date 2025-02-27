#ifndef HELP_MESSAGE
#define HELP_MESSAGE XIGRABBER_VERSION "\n" \
    "Usage:\n" \
    "    ./xigrabber [options] <input_file>\n" \
    "\n" \
    "Options:\n" \
    "    -o, --output <output_directory>   Specify the output directory (default is the current directory).\n" \
    "    -v, --verbose                     Enable verbose output.\n" \
    "    -e, --skip-empty                  Skip empty instruments (no samples)\n"\
    "    -h, --help                        Show this help message and exit.\n" \
    "\n" \
    "Examples:\n" \
    "    1. Extract XI instruments from a single XM file:\n" \
    "        ./xigrabber song.xm\n" \
    "\n" \
    "    2. Extract XI instruments from an XM file and specify the output directory:\n" \
    "        ./xigrabber -o /path/to/output song.xm\n" \
    "\n" \
    "    3. Enable verbose output:\n" \
    "        ./xigrabber -v song.xm\n" \
    "    -e works the same way.\n"
#endif