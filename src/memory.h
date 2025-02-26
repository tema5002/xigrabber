#pragma once

#include "exit_error.h"

void* safe_malloc(const size_t bytes) {
    void* result = malloc(bytes);
    if (result == NULL) exit_error(MEMORY_FAIL_TEXT);
    return result;
}

void* safe_realloc(void* ptr, const size_t size) {
    void* result = realloc(ptr, size);
    if (result == NULL) exit_error(MEMORY_FAIL_TEXT);
    return result;
}

void safe_fwrite(const void* ptr, const size_t n, FILE* s)  {
    if (fwrite(ptr, 1, n, s) != n) {
        exit_error(FILE_WRITE_FAIL_TEXT);
    }
}

FILE* safe_open(STRING s, STRING mode) {
    FILE* f;
    OPEN_FILE(f, s, mode);
    if (f == NULL)
        exit_error(FAILED_TO_OPEN_TEXT, s, mode);
    return f;
}

FILE* safe_open_r(STRING s) {
    return safe_open(s, READ_FMODE);
}

FILE* safe_open_w(STRING s) {
    return safe_open(s, WRITE_FMODE);
}

void safe_fread(void* ptr, const size_t n, FILE* stream) {
    if (fread(ptr, 1, n, stream) != n) {
        exit_error(END_OF_FILE_TEXT);
    }
}
