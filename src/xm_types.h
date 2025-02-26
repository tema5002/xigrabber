#pragma once

typedef struct xi_sample {
    uint8_t header[XI_SAMPLE_HEADER_SIZE];
    uint8_t* pcm;
} xi_sample;

typedef struct xi_instrument {
    uint8_t name[22];
    uint8_t header[XI_HEADER_SIZE];

    struct {
        uint16_t size;
        xi_sample* data;
    } samples;
} xi_instrument;
