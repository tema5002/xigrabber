#pragma once

#include "xm_types.h"
#include "file_buffer.h"
#include "build_xi.h"

void read_xi_instrument(file_buffer fb, xi_instrument* inst) {
    const uint32_t header_size = read_uint32(fb);
    read_bytes(fb, inst->name, 22);
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

        inst->samples.data = safe_malloc(sizeof(xi_sample) * inst->samples.size);
        for (uint16_t i = 0; i < inst->samples.size; i++) {
            read_bytes(fb, inst->samples.data[i].header, XI_SAMPLE_HEADER_SIZE);
        }
        for (uint16_t i = 0; i < inst->samples.size; i++) {
            const uint32_t size = UINT32_LE(inst->samples.data[i].header);
            if (size > 0) {
                inst->samples.data[i].pcm = safe_malloc(size);
                read_bytes(fb, inst->samples.data[i].pcm, size);
            }
        }
    }
    else {
        skip_bytes(fb, header_size - 29);
    }
}

void free_xi_instrument(xi_instrument* inst) {
    for (uint16_t i = 0; i < inst->samples.size; i++) {
        const uint32_t size = UINT32_LE(inst->samples.data[i].header);
        if (size > 0) {
            free(inst->samples.data[i].pcm);
        }
    }
    free(inst->samples.data);
}

void skip_xm_pattern(file_buffer fb) {
    skip_bytes(fb,
        4 + // Pattern header length
        1 + // Packing type
        2   // Rows
    );
    const uint16_t packed_size = read_uint16(fb);
    skip_bytes(fb, packed_size);
}

void process_xm(file_buffer fb, STRING input, STRING output, const bool verbose) {
    skip_bytes(fb,
        17 // "Extended Module: "
    );
    {
        char name[21];
        read_bytes(fb, name, 20);
        name[20] = '\0';
        DEBUGC("Module loaded: %s\n", name);
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
    DEBUGC("Patterns in module: %u\nInstruments in module: %u\n", patterns, instruments);
    skip_bytes(fb,
        2 + // flags
        2 + // tempo
        2 + // bpm
        256 // pattern order table
    );

    for (int i = 0; i < patterns; i++) {
        skip_xm_pattern(fb);
    }
    DEBUGC("Skipped %u patterns.\n", patterns)

    const size_t dest_size =
        STRLEN(output) + // %s
        1 +              // /
        STRLEN(input) +  // %s
        1 +              // -
        2 +              // %02X
        3 +              // .xi
        1;               // \0

    STRING dest = safe_malloc(
#ifdef WINDOWS
        2 *
#endif
        dest_size
    );
    for (uint16_t i = 0; i < instruments; i++) {
        DEBUGC("Reading instrument %u...\n", i);
        xi_instrument inst;
        read_xi_instrument(fb, &inst);
        SPRINT(dest, dest_size, XI_PATH_FORMAT, output, input, i);
        DEBUG(SAVING_INSTRUMENT_TEXT, dest);
        string_buffer buf;
        make_string_buffer(&buf, 512);
        build_xi(&inst, &buf);
        const size_t bytes = buf.len;
        write_string_buffer_to_file(&buf, dest);
        DEBUGC("Saved %zu total bytes.\n", bytes);
        free_xi_instrument(&inst);
    }
    free(dest);
}
