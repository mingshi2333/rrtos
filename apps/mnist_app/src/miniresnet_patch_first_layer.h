#ifndef MINIRESNET_PATCH_FIRST_LAYER_H
#define MINIRESNET_PATCH_FIRST_LAYER_H

#include <stdint.h>

#include "rrtos_ai_ukernel.h"

rrtos_ai_ukernel_status_t miniresnet_patch_run_first_layer(
    const int8_t *input,
    uint8_t *arena,
    uint32_t output_offset);

#endif /* MINIRESNET_PATCH_FIRST_LAYER_H */
