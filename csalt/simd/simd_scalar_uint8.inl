
#ifndef CSALT_SIMD_SCALAR_UINT8_INL
#define CSALT_SIMD_SCALAR_UINT8_INL

#include <stdint.h>
#include <stddef.h>
#include <string.h>
// ================================================================================ 
// ================================================================================ 

static void simd_reverse_uint8(uint8_t* data, size_t len, size_t data_size) {
    if (data == NULL || len < 2u || data_size == 0u) return;
 
    size_t  lo = 0u;
    size_t  hi = len - 1u;
    uint8_t tmp[256];
 
    if (data_size <= sizeof(tmp)) {
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
            memcpy(tmp,    lo_ptr, data_size);
            memcpy(lo_ptr, hi_ptr, data_size);
            memcpy(hi_ptr, tmp,    data_size);
            lo++;
            hi--;
        }
    } else {
        /* Element larger than the stack buffer — swap byte-by-byte */
        while (lo < hi) {
            uint8_t* lo_ptr = data + lo * data_size;
            uint8_t* hi_ptr = data + hi * data_size;
            for (size_t b = 0u; b < data_size; b++) {
                uint8_t byte = lo_ptr[b];
                lo_ptr[b]    = hi_ptr[b];
                hi_ptr[b]    = byte;
            }
            lo++;
            hi--;
        }
    }
}
// ================================================================================ 
// ================================================================================ 
#endif /* CSALT_SIMD_SCALAR_UINT8_INL */

