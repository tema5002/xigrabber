#pragma once

#include "string_buffer.h"

void build_xi(const xi_instrument* inst, string_buffer* buf) {
    append_to_string_buffer(buf, "Extended Instrument: ", 21);
    append_to_string_buffer(buf, inst->name, INSTRUMENT_NAME_SIZE);
    append_byte_to_string_buffer(buf, 0x1A);
    append_to_string_buffer(buf, XIGRABBER_VERSION_FULL, sizeof(XIGRABBER_VERSION_FULL));
    for (unsigned long i = 0; i < 20 - sizeof(XIGRABBER_VERSION_FULL); i++) {
        append_byte_to_string_buffer(buf, 0x00);
    }
    append_byte_to_string_buffer(buf, 0x02);
    append_byte_to_string_buffer(buf, 0x01);
    append_to_string_buffer(buf, inst->header, XI_HEADER_SIZE);
    append_byte_to_string_buffer(buf, inst->samples.size & 0xFF);
    append_byte_to_string_buffer(buf, (inst->samples.size >> 8) & 0xFF);
    for (int i = 0; i < inst->samples.size; i++) {
        append_to_string_buffer(buf, inst->samples.data[i].header, sizeof(inst->samples.data[i].header));
    }
    for (int i = 0; i < inst->samples.size; i++) {
        append_to_string_buffer(buf, inst->samples.data[i].pcm, UINT32_LE(inst->samples.data[i].header));
    }
}

size_t build_xi_and_write_to_file(const xi_instrument* inst, STRING dest) {
    string_buffer buf;
    make_string_buffer(&buf, 512);
    build_xi(inst, &buf);
    const size_t bytes = buf.len;
    write_string_buffer_to_file(&buf, dest);
    return bytes;
}
