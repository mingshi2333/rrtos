#include <stddef.h>
#include <stdint.h>

#define EMITC_IMPLEMENTATION
#include "generated/be_u1000_hello_world_float.h"

const uint8_t *be_u1000_hello_world_float_direct_const_data(
    size_t *out_size) {
    if (out_size != NULL) {
        *out_size = sizeof(be_u1000_hello_world_float__const);
    }
    return be_u1000_hello_world_float__const;
}
