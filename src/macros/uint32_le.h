#ifndef UINT32_LE
#define UINT32_LE(a) ((uint32_t)(a[0]) | ((uint32_t)(a[1]) << 8) | ((uint32_t)(a[2]) << 16) | ((uint32_t)(a[3]) << 24))
#endif
