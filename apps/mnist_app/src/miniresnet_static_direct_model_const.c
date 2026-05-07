#include <stddef.h>
#include <stdint.h>

#define EMITC_IMPLEMENTATION
#include "generated_miniresnet/miniresnetv1_s1_64x50_tl_int8.h"

const uint8_t *miniresnet_static_direct_const_data(size_t *out_size) {
    if (out_size != NULL) {
        *out_size = sizeof(miniresnetv1_s1_64x50_tl_int8__const);
    }
    return miniresnetv1_s1_64x50_tl_int8__const;
}
