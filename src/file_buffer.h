#pragma once

#include "memory.h"

typedef struct file_buffer {
    uint8_t* begin;
    uint8_t* pos;
    uint8_t* end;
}* file_buffer;

file_buffer create_file_buffer(STRING filename) {
    FILE* file = safe_open_r(filename);

    fseek(file, 0, SEEK_END);
    const size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = safe_malloc(filesize);
    safe_fread(buffer, filesize, file);
    fclose(file);

    const file_buffer fb = safe_malloc(sizeof(struct file_buffer));
    fb->begin = buffer;
    fb->pos = buffer;
    fb->end = buffer + filesize;

    return fb;
}

void free_file_buffer(const file_buffer fb) {
    free(fb->begin);
    free(fb);
}

uint8_t read_uint8(const file_buffer fb) {
    if (fb->pos >= fb->end) {
        exit_error(END_OF_FILE_TEXT);
    }
    return *fb->pos++;
}

uint16_t read_uint16(const file_buffer fb) {
    return (uint16_t)read_uint8(fb) +
           (uint16_t)(read_uint8(fb) << 8);
}

uint32_t read_uint32(const file_buffer fb) {
    return (uint32_t)read_uint8(fb) +
           (uint32_t)(read_uint8(fb) << 8) +
           (uint32_t)(read_uint8(fb) << 16) +
           (uint32_t)(read_uint8(fb) << 24);
}

void skip_bytes(const file_buffer fb, const int b) {
    if (fb->pos + b > fb->end) {
        exit_error(END_OF_FILE_TEXT);
    }
    fb->pos += b;
}

void read_bytes(const file_buffer fb, void* s, const size_t size) {
    if (fb->pos + size > fb->end) {
        exit_error(END_OF_FILE_TEXT);
    }
    memcpy(s, fb->pos, size);
    fb->pos += size;
}

