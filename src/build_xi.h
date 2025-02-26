#pragma once

#include "string_buffer.h"

void build_xi(const xi_instrument* inst, string_buffer* buf) {
    append_to_string_buffer(buf, "Extended Instrument: ", 21);
    append_to_string_buffer(buf, inst->name, 22);
    append_byte_to_string_buffer(buf, 0x1A);
    append_to_string_buffer(buf, XIGRABBER_VERSION, 20);
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
