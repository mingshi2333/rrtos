#include <stddef.h>
#include <stdint.h>

#define EMITC_IMPLEMENTATION
#include "generated/st_mnist_28.h"

const uint8_t *st_mnist_28_direct_const_data(size_t *out_size) {
    if (out_size != NULL) {
        *out_size = sizeof(st_mnist_28__const);
    }
    return st_mnist_28__const;
}
