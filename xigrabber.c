// fucking windows
#define XIGRABBER_VERSION "2.3"
#define XIGRABBER_VERSION_FULL "XIGrabber v" XIGRABBER_VERSION
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

#ifdef WINDOWS
#define MAIN wmain
#else
#define MAIN main
#endif

#ifdef WINDOWS
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <shlwapi.h>
#include <wchar.h>
#else
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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
    "        ./xigrabber -n \"square\" -e song.xm\n"

#define UINT32_LE(a) ((uint32_t)(a[0]) | ((uint32_t)(a[1]) << 8) | ((uint32_t)(a[2]) << 16) | ((uint32_t)(a[3]) << 24))

#define XI_HEADER_SIZE (\
    96 + /* sample number for each note    */\
    48 + /* volume envelopes               */\
    48 + /* volume envelopes               */\
    1 +  /* volume envelope points amount  */\
    1 +  /* panning envelope points amount */\
    1 +  /* volume sustain point           */\
    1 +  /* volume loop start point        */\
    1 +  /* volume loop end point          */\
    1 +  /* panning sustain point          */\
    1 +  /* panning loop start point       */\
    1 +  /* panning loop end point         */\
    1 +  /* volume type                    */\
    1 +  /* panning type                   */\
    1 +  /* vibtype                        */\
    1 +  /* vibsweep                       */\
    1 +  /* vibdepth                       */\
    1 +  /* vibspeed                       */\
    2 +  /* fadeout                        */\
    22   /* reserved                       */)

#define XI_SAMPLE_HEADER_SIZE (\
    4 +  /* sample size        */\
    4 +  /* sample loop start  */\
    4 +  /* sample loop length */\
    1 +  /* volume             */\
    1 +  /* finetune           */\
    1 +  /* flags              */\
    1 +  /* panning            */\
    1 +  /* relative note      */\
    1 +  /* reserved           */\
    22   /* name               */)

#define INSTRUMENT_NAME_SIZE 22

#ifdef WINDOWS
#define _(str) (L##str)
#define FMT_STR "%S"
#else
#define _(str) (str)
#define FMT_STR "%s"
#endif

#ifdef WINDOWS
#define PRINT wprintf
#define FPRINT fwprintf
#define STRING_CHAR wchar_t
#define STRING STRING_CHAR*
#define STRCMP wcscmp
#define SPRINT swprintf
#define STRLEN wcslen
#else
#define PRINT printf
#define FPRINT fprintf
#define STRING_CHAR char
#define STRING STRING_CHAR*
#define STRCMP strcmp
#define SPRINT snprintf
#define STRLEN strlen
#endif

#define ARE_EQ(arg, arg1, arg2) (STRCMP(arg, arg1) == 0 || STRCMP(arg, arg2) == 0)

#define exit_error(format, ...) do { \
    FPRINT(stderr, (format), ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while(0)

#ifndef WINDOWS
#define INVALID_HANDLE_VALUE NULL
#endif

static struct {
    STRING input;
    STRING output;
    bool verbose;
    bool skip_empty_instruments;
    char name[23]; // last byte == 0 means it is used
} arguments;

#define DEBUG(...) if (arguments.verbose) { PRINT(__VA_ARGS__); }

typedef struct xi_sample {
    uint8_t header[XI_SAMPLE_HEADER_SIZE];
    uint8_t* pcm;
} xi_sample;

typedef struct xi_instrument {
    char name[INSTRUMENT_NAME_SIZE + 1];
    uint8_t header[XI_HEADER_SIZE];

    struct {
        uint16_t size;
        xi_sample* data;
    } samples;
} xi_instrument;

typedef struct file_buffer {
    uint8_t* begin;
    uint8_t* pos;
    uint8_t* end;
} file_buffer;

static file_buffer create_file_buffer(STRING filename) {
#ifdef WINDOWS
    HANDLE file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) exit_error(_("Failed to open '" FMT_STR "'."), filename);

    LARGE_INTEGER filesize_li;
    GetFileSizeEx(file, &filesize_li);
    const DWORD filesize = (DWORD)filesize_li.QuadPart;

    uint8_t* buffer = (uint8_t*)HeapAlloc(GetProcessHeap(), 0, filesize);

    DWORD bytesRead;
    const BOOL result = ReadFile(
        file,
        buffer,
        filesize,
        &bytesRead,
        NULL
    );

    if (!result || bytesRead != filesize)
        exit_error(_("Error: reached end of the file"));

    CloseHandle(file);
#else
    FILE* file = fopen(filename, "r");
    if (file == INVALID_HANDLE_VALUE) exit_error(_("Failed to open '" FMT_STR "'."), filename);

    fseek(file, 0, SEEK_END);
    const unsigned long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = malloc(filesize);

    if (fread(buffer, 1, filesize, file) != filesize) exit_error(_("Error: reached end of the file"));

    fclose(file);
#endif
    file_buffer fb;
    fb.begin = buffer;
    fb.pos = buffer;
    fb.end = buffer + filesize;
    return fb;
}

static void destroy_file_buffer(file_buffer fb) {
#ifdef WINDOWS
    HeapFree(GetProcessHeap(), 0, fb.begin);
#else
    free(fb.begin);
#endif
}

static uint8_t read_uint8(file_buffer* fb) {
    if (fb->pos >= fb->end) {
        exit_error(_("Error: reached end of the file"));
    }
    return *fb->pos++;
}

static uint16_t read_uint16(file_buffer* fb) {
    return (uint16_t)read_uint8(fb) +
           (uint16_t)(read_uint8(fb) << 8);
}

static uint32_t read_uint32(file_buffer* fb) {
    return (uint32_t)read_uint8(fb) +
           (uint32_t)(read_uint8(fb) << 8) +
           (uint32_t)(read_uint8(fb) << 16) +
           (uint32_t)(read_uint8(fb) << 24);
}

static void skip_bytes(file_buffer* fb, const unsigned int b) {
    if (fb->pos + b > fb->end) {
        exit_error(_("Error: reached end of the file"));
    }
    fb->pos += b;
}

static void read_bytes(file_buffer* fb, void* s, const size_t size) {
    if (fb->pos + size > fb->end) {
        exit_error(_("Error: reached end of the file"));
    }
    memcpy(s, fb->pos, size);
    fb->pos += size;
}

static size_t calculate_xi_size(const xi_instrument* inst) {
    size_t size = 0;
    
    size += 21; // "Extended Instrument: "
    size += INSTRUMENT_NAME_SIZE;
    size += 1; // 0x1A
    size += 20; // program
    size += 2; // version
    size += XI_HEADER_SIZE; // XI header
    size += 2; // sample count (2 bytes)
    
    size += inst->samples.size * XI_SAMPLE_HEADER_SIZE;
    
    for (int i = 0; i < inst->samples.size; i++) {
        size += UINT32_LE(inst->samples.data[i].header);
    }
    
    return size;
}

static size_t build_xi_and_write_to_file(const xi_instrument* inst, STRING dest) {
    const size_t size = calculate_xi_size(inst);
#ifdef WINDOWS
    uint8_t* buffer = (uint8_t*)HeapAlloc(GetProcessHeap(), 0, size);
#else
    uint8_t* buffer = malloc(size);
#endif

    uint8_t* pos = buffer;
    memcpy(pos, "Extended Instrument: ", 21);
    pos += 21;

    memcpy(pos, inst->name, INSTRUMENT_NAME_SIZE);
    pos += INSTRUMENT_NAME_SIZE;
    
    *pos++ = 0x1A;

    memcpy(pos, XIGRABBER_VERSION_FULL, sizeof(XIGRABBER_VERSION_FULL));
    memset(pos + sizeof(XIGRABBER_VERSION_FULL), 0, 20 - sizeof(XIGRABBER_VERSION_FULL));
    pos += 20;
    
    *pos++ = 0x02;
    *pos++ = 0x01;

    memcpy(pos, inst->header, XI_HEADER_SIZE);
    pos += XI_HEADER_SIZE;

    *pos++ = inst->samples.size & 0xFF;
    *pos++ = (inst->samples.size >> 8) & 0xFF;

    for (int i = 0; i < inst->samples.size; i++) {
        memcpy(pos, inst->samples.data[i].header, XI_SAMPLE_HEADER_SIZE);
        pos += XI_SAMPLE_HEADER_SIZE;
    }
    
    for (int i = 0; i < inst->samples.size; i++) {
        const uint32_t sample_size = UINT32_LE(inst->samples.data[i].header);
        memcpy(pos, inst->samples.data[i].pcm, sample_size);
        pos += sample_size;
    }

#ifdef WINDOWS
    HANDLE file = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    FILE* file = fopen(dest, "w");
#endif
    if (file == INVALID_HANDLE_VALUE)
        exit_error(_("Failed to open '" FMT_STR "'."), dest);

#ifdef WINDOWS
    DWORD bytesWritten;
    const BOOL result = WriteFile(file, buffer, (DWORD)size, &bytesWritten, NULL);
    if (!result || bytesWritten != size) {
#else
    if (fwrite(buffer, 1, size, file) != size) {
#endif
        exit_error(_("Error writing to file"));
    }

#ifdef WINDOWS
    HeapFree(GetProcessHeap(), 0, buffer);
    CloseHandle(file);
#else
    free(buffer);
    fclose(file);
#endif
    return size;
}

static void read_xi_instrument(file_buffer* fb, xi_instrument* inst) {
    const uint32_t header_size = read_uint32(fb);
    read_bytes(fb, inst->name, INSTRUMENT_NAME_SIZE);
    inst->name[INSTRUMENT_NAME_SIZE] = '\0';
    skip_bytes(fb, 1); // Instrument type
    inst->samples.size = read_uint16(fb);
    if (inst->samples.size > 0) {
        skip_bytes(fb,
            4 // sample header size
        );
        if (header_size >= XI_HEADER_SIZE + 33) {
            read_bytes(fb, inst->header, XI_HEADER_SIZE);
            skip_bytes(fb, header_size - XI_HEADER_SIZE - 33);
        }
        else {
            memset(inst->header, 0, XI_HEADER_SIZE);
            read_bytes(fb, inst->header, header_size - 33);
        }
#ifdef WINDOWS  
        inst->samples.data = HeapAlloc(GetProcessHeap(), 0, sizeof(xi_sample) * inst->samples.size);
#else
        inst->samples.data = malloc(sizeof(xi_sample) * inst->samples.size);
#endif
        for (uint16_t i = 0; i < inst->samples.size; i++) {
            read_bytes(fb, inst->samples.data[i].header, XI_SAMPLE_HEADER_SIZE);
        }
        for (uint16_t i = 0; i < inst->samples.size; i++) {
            const uint32_t size = UINT32_LE(inst->samples.data[i].header);
            if (size > 0) {
#ifdef WINDOWS
                inst->samples.data[i].pcm = HeapAlloc(GetProcessHeap(), 0, size);
#else
                inst->samples.data[i].pcm = malloc(size);
#endif
                read_bytes(fb, inst->samples.data[i].pcm, size);
            }
        }
    }
    else {
        skip_bytes(fb, header_size - 29);
    }
}

static void free_xi_instrument(const xi_instrument* inst) {
    if (inst->samples.size == 0) return;
    for (uint16_t i = 0; i < inst->samples.size; i++) {
        const uint32_t size = UINT32_LE(inst->samples.data[i].header);
        if (size > 0) {
#ifdef WINDOWS
            HeapFree(GetProcessHeap(), 0, inst->samples.data[i].pcm);
#else
            free(inst->samples.data[i].pcm);
#endif
        }
    }
#ifdef WINDOWS
    HeapFree(GetProcessHeap(), 0, inst->samples.data);
#else
    free(inst->samples.data);
#endif
}

static void skip_xm_pattern(file_buffer* fb) {
    skip_bytes(fb,
        4 + // Pattern header length
        1 + // Packing type
        2   // Rows
    );
    const uint16_t packed_size = read_uint16(fb);
    skip_bytes(fb, packed_size);
}

static void process_xm(file_buffer* fb) {
    skip_bytes(fb,
        17 // "Extended Module: "
    );
    if (arguments.verbose) {
        char name[21];
        read_bytes(fb, name, 20);
        name[20] = '\0';
        DEBUG(_("Module loaded: %s\n"), name);
    }
    else {
        skip_bytes(fb, 20);
    }
    skip_bytes(fb,
        1 +  // 0x1A
        20 + // Tracker name
        2 +  // Tracker version
        4 +  // Header size
        2 +  // Song length
        2 +  // Song restart position
        2    // Channels
    );
    const uint16_t patterns = read_uint16(fb);
    const uint16_t instruments = read_uint16(fb);
    DEBUG(_("Patterns in module: %u\nInstruments in module: %u\n"), patterns, instruments);
    skip_bytes(fb,
        2 + // flags
        2 + // tempo
        2 + // bpm
        256 // pattern order table
    );

    for (int i = 0; i < patterns; i++) {
        skip_xm_pattern(fb);
    }
    DEBUG(_("Skipped %u patterns."), patterns)

#ifdef WINDOWS
    arguments.input = PathFindFileName(arguments.input);
#else
    arguments.input = basename(arguments.input);
#endif

    const size_t dest_size =
        STRLEN(arguments.output) + // %s
        1 +                        // /
        STRLEN(arguments.input) +  // %s
        1 +                        // -
        2 +                        // %02X
        3 +                        // .xi
        1;                         // \0

#ifdef WINDOWS
    STRING dest = HeapAlloc(GetProcessHeap(), 0, sizeof(STRING_CHAR) * dest_size);
#else
    STRING dest = malloc(sizeof(STRING_CHAR) * dest_size);
#endif
    for (uint16_t i = 0; i < instruments; i++) {
        DEBUG(_("Reading instrument %u...\n"), i + 1);
        xi_instrument inst;
        read_xi_instrument(fb, &inst);
        if (arguments.name[22] == '\0' && strcmp(inst.name, arguments.name) != 0) {
            DEBUG(_("Instrument name is '%s', not '%s'. Skipping.\n"), inst.name, arguments.name);
            free_xi_instrument(&inst);
            continue;
        }
        if (inst.samples.size == 0 && arguments.skip_empty_instruments) {
            DEBUG(_("No samples, skipping.\n"));
            free_xi_instrument(&inst);
            continue;
        }

        char* s = "s";
        if (inst.samples.size == 1) s++;
        DEBUG(_("Read instrument '%s' with %u sample%s.\n"), inst.name, inst.samples.size, s);

        SPRINT(dest, dest_size, _("" FMT_STR "/" FMT_STR "-%02X.xi\n"), arguments.output, arguments.input, i + 1);
        DEBUG(_("Saving instrument to '" FMT_STR "'...\n"), dest);

        const size_t bytes = build_xi_and_write_to_file(&inst, dest);
        DEBUG(_("Saved %zu total bytes.\n"), bytes);

        free_xi_instrument(&inst);
    }
#ifdef WINDOWS
    HeapFree(GetProcessHeap(), 0, dest);
#else
    free(dest);
#endif
}

int MAIN(const int argc, STRING argv[]) {
    arguments.input = NULL;
    arguments.output = _(".");
    arguments.verbose = false;
    arguments.skip_empty_instruments = false;
    arguments.name[22] = '\xFF';

    for (int i = 1; i < argc; i++) {
        if (ARE_EQ(argv[i], _("-h"), _("--help"))) {
            PRINT(_("" FMT_STR ""), _("" HELP_MESSAGE ""));
            exit(EXIT_SUCCESS);
        }

        if (STRCMP(argv[i], _("--version")) == 0) {
            PRINT(_("" FMT_STR ""), _("" XIGRABBER_VERSION "\n"));
            exit(EXIT_SUCCESS);
        }

        if (ARE_EQ(argv[i], _("-o"), _("--output")) && i + 1 < argc) {
            arguments.output = argv[++i];
        }
        else if (ARE_EQ(argv[i], _("-n"), _("--name")) && i + 1 < argc) {
            i++;
            arguments.name[22] = '\0';
#ifdef WINDOWS
            size_t j;
            for (j = 0; j < 22 && argv[i][j] != '\0'; j++) {
                arguments.name[j] = (char)argv[i][j];
            }
            arguments.name[j] = '\0';
#else
            strncpy(arguments.name, argv[i], 22);
#endif
        }
        else if (ARE_EQ(argv[i], _("-v"), _("--verbose"))) {
            arguments.verbose = true;
        }
        else if (ARE_EQ(argv[i], _("-e"), _("--skip-empty"))) {
            arguments.skip_empty_instruments = true;
        }
        else if (argv[i][0] != _('-')) {
            arguments.input = argv[i];
        }
        else {
            exit_error(_("Unknown option: '" FMT_STR "'\nTry '" FMT_STR " --help'.\n"), argv[i], argv[0]);
        }
    }
    if (arguments.input == NULL) {
        exit_error(_("Error: No input file specified.\nTry '" FMT_STR " --help'.\n"), argv[0]);
    }

    file_buffer fb = create_file_buffer(arguments.input);

    DEBUG(_("" XIGRABBER_VERSION_FULL "\n"));
    DEBUG(_("Verbose mode enabled.\n"));

#ifdef WINDOWS
    if (_waccess(arguments.output, 0) != 0) {
#else
    if (access(arguments.output, F_OK) != 0) {
#endif
        DEBUG(_("Directory '" FMT_STR "' does not exist. Creating it...\n"), arguments.output);
#ifdef WINDOWS
        if (_wmkdir(arguments.output)) {
#else
        if (mkdir(arguments.output, 0755)) { // rwxr-xr-x
#endif
            exit_error(_("Failed to create directory '" FMT_STR "'.\n"), arguments.output);
        }
        DEBUG(_("Directory '" FMT_STR "' created successfully.\n"), arguments.output);
    }
    DEBUG(_("Extracting instruments from '" FMT_STR "' to '" FMT_STR "'.\n"), arguments.input, arguments.output);
    process_xm(&fb);
    DEBUG(_("Success!\n"));
    destroy_file_buffer(fb);
    return 0;
}
