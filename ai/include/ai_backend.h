#ifndef AI_BACKEND_H
#define AI_BACKEND_H

#include "ai_runtime.h"

#if OS_CFG_AI_EN

typedef struct ai_backend_ops {
    const char *name;

    ai_err_t (*init)(void *arena, uint32_t arena_size);
    void (*deinit)(void);

    ai_err_t (*load)(ai_model_t **model, const void *model_data, uint32_t size);
    void (*unload)(ai_model_t *model);

    ai_err_t (*invoke)(ai_model_t *model);
} ai_backend_ops_t;

const ai_backend_ops_t *ai_backend_iree_ops(void);
const ai_backend_ops_t *ai_backend_native_ops(void);

#endif

#endif
