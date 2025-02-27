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
