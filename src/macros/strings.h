#pragma once

#ifdef WINDOWS
    #define FAILED_TO_OPEN_TEXT L"Failed to open '%S'."
    #define UNKNOWN_OPTION_TEXT L"Unknown option: '%S'\nTry '%S --help'.\n"
    #define NO_INPUT_FILE_TEXT L"Error: No input file specified.\nTry '%S --help'.\n"
    #define VERBOSE_MODE_ENABLED_TEXT L"Verbose mode enabled.\n"
    #define DIR_NOT_EXISTS_TEXT L"Directory '%S' does not exist. Creating it...\n"
    #define DIR_CREATE_FAILED_TEXT L"Failed to create directory '%S'.\n"
    #define DIR_CREATE_SUCCESS_TEXT L"Directory '%S' created successfully.\n"
    #define EXTRACTING_INSTRUMENTS_TEXT L"Extracting instruments from '%S' to '%S'.\n"
    #define SUCCESS_TEXT L"Success!\n"
    #define MEMORY_FAIL_TEXT L"Failed to allocate memory.\n"
    #define FILE_WRITE_FAIL_TEXT L"Error writing to file\n"
    #define END_OF_FILE_TEXT L"Error: reached end of the file\n"
    #define SAVING_INSTRUMENT_TEXT L"Saving instrument to '%S'...\n"
#else
    #define FAILED_TO_OPEN_TEXT "Failed to open '%s'."
    #define UNKNOWN_OPTION_TEXT "Unknown option: '%s'\nTry '%s --help'.\n"
    #define NO_INPUT_FILE_TEXT "Error: No input file specified.\nTry '%s --help'.\n"
    #define VERBOSE_MODE_ENABLED_TEXT "Verbose mode enabled.\n"
    #define DIR_NOT_EXISTS_TEXT "Directory '%s' does not exist. Creating it...\n"
    #define DIR_CREATE_FAILED_TEXT "Failed to create directory '%s'.\n"
    #define DIR_CREATE_SUCCESS_TEXT "Directory '%s' created successfully.\n"
    #define EXTRACTING_INSTRUMENTS_TEXT "Extracting instruments from '%s' to '%s'.\n"
    #define SUCCESS_TEXT "Success!\n"
    #define MEMORY_FAIL_TEXT "Failed to allocate memory.\n"
    #define FILE_WRITE_FAIL_TEXT "Error writing to file\n"
    #define END_OF_FILE_TEXT "Error: reached end of the file\n"
    #define SAVING_INSTRUMENT_TEXT "Saving instrument to '%s'...\n"
#endif

#ifdef WINDOWS
    #define XI_PATH_FORMAT L"%S\\%S-%02X.xi"

    #define CURRENT_DIR L"."
    #define DASH L'-'

    #define HELP1 L"-h"
    #define HELP2 L"--help"
    #define OUTPUT1 L"-o"
    #define OUTPUT2 L"--output"
    #define VERBOSE1 L"-v"
    #define VERBOSE2 L"--verbose"
#else
    #define XI_PATH_FORMAT "%s/%s-%02X.xi"

    #define CURRENT_DIR "."
    #define DASH '-'

    #define HELP1 "-h"
    #define HELP2 "--help"
    #define OUTPUT1 "-o"
    #define OUTPUT2 "--output"
    #define VERBOSE1 "-v"
    #define VERBOSE2 "--verbose"
#endif

#ifdef WINDOWS
    #define PRINT wprintf
    #define STRING_CHAR wchar_t
    #define STRING STRING_CHAR*
    #define STRCMP wcscmp
    #define VFPRINT vfwprintf
    #define SPRINT swprintf
    #define STRLEN wcslen
#else
    #define PRINT printf
    #define STRING_CHAR char
    #define STRING STRING_CHAR*
    #define STRCMP strcmp
    #define VFPRINT vfprintf
    #define SPRINT snprintf
    #define STRLEN strlen
#endif

#define ARE_EQ(arg, arg1, arg2) ((arg) && ((STRCMP(arg, arg1) == 0) || (STRCMP(arg, arg2) == 0)))
#define DEBUG(...) if (verbose) { PRINT(__VA_ARGS__); }
#define DEBUGC(...) if (verbose) { printf(__VA_ARGS__); }