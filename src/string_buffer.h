#pragma once

#include "memory.h"

size_t roundup(const size_t n) {
    size_t result = 1;
    while (result < n) {
        result <<= 1;
    }
    return result;
}

typedef struct string_buffer {
    char* str;
    size_t len;
    size_t capacity;
} string_buffer;

void make_string_buffer(string_buffer* buf, const size_t capacity) {
    buf->str = safe_malloc(capacity);
    buf->len = 0;
    buf->capacity = 1;
}

void resize_string_buffer(string_buffer* buf, const size_t new_capacity) {
    buf->str = safe_realloc(buf->str, new_capacity);
    buf->capacity = new_capacity;
}

void append_to_string_buffer(string_buffer* buf, const void* const str, const size_t size) {
    const size_t new_capacity = roundup(buf->len + size);
    if (new_capacity > buf->capacity) resize_string_buffer(buf, new_capacity);
    memcpy(buf->str + buf->len, str, size);
    buf->len += size;
}

void append_byte_to_string_buffer(string_buffer* buf, const char c) {
    const size_t new_capacity = roundup(buf->len + 1);
    if (new_capacity > buf->capacity) resize_string_buffer(buf, new_capacity);
    buf->str[buf->len] = c;
    buf->len++;
}

void write_string_buffer_to_file(const string_buffer* buf, STRING fp) {
    FILE* file = safe_open_w(fp);
    safe_fwrite(buf->str, buf->len, file);
    fclose(file);
    free(buf->str);
}

