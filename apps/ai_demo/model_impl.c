#ifndef EMITC_IMPLEMENTATION
#define EMITC_IMPLEMENTATION
#endif
#include <stddef.h>
#include "ai_model.h"
#include "yoloface_int8.h"

size_t ai_model_rodata_size(void) {
    return sizeof(module__const);
}
