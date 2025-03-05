#ifndef HELP_MESSAGE
#define HELP_MESSAGE XIGRABBER_VERSION_FULL "\n" \
    "Usage:\n" \
    "    ./xigrabber [options] <input_file>\n" \
    "\n" \
    "Options:\n" \
    "    -o, --output <output_directory>   Specify the output directory (default is the current directory).\n" \
    "    -v, --verbose                     Enable verbose output.\n" \
    "    -e, --skip-empty                  Skip empty instruments (no samples).\n" \
    "    -n, --name <instrument_name>      Extract only instruments with the specified name.\n" \
    "    --version                         Show version information.\n" \
    "    -h, --help                        Show this help message and exit.\n" \
    "\n" \
    "Examples:\n" \
    "    1. Extract XI instruments from a single XM file:\n" \
    "        ./xigrabber song.xm\n" \
    "\n" \
    "    2. Extract XI instruments from an XM file, specify the output directory\n" \
    "       and enable verbose output:\n" \
    "        ./xigrabber -o /path/to/output song.xm -v\n" \
    "\n" \
    "    3. Extract instruments with specific name and skip empty instruments:\n" \
    "        ./xigrabber -n \"square\" -e song.xm"
#endif
