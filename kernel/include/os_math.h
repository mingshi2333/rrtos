#ifndef OS_MATH_H
#define OS_MATH_H

#include "os_types.h"

OS_INLINE float os_fabsf(float x) {
    return x < 0.0f ? -x : x;
}

OS_INLINE float os_sqrtf(float x) {
    if (x <= 0.0f) return 0.0f;
    float guess = x / 2.0f;
    for (int i = 0; i < 10; i++) {
        guess = (guess + x / guess) / 2.0f;
    }
    return guess;
}

OS_INLINE float os_logf(float x) {
    if (x <= 0.0f) return -1e30f;
    float result = 0.0f;
    while (x > 2.0f) { x /= 2.718281828f; result += 1.0f; }
    while (x < 0.5f) { x *= 2.718281828f; result -= 1.0f; }
    x -= 1.0f;
    float term = x;
    for (int i = 1; i <= 8; i++) {
        result += (i & 1 ? 1.0f : -1.0f) * term / (float)i;
        term *= x;
    }
    return result;
}

OS_INLINE float os_expf(float x) {
    float result = 1.0f;
    float term = 1.0f;
    for (int i = 1; i <= 12; i++) {
        term *= x / (float)i;
        result += term;
    }
    return result;
}

OS_INLINE float os_sinf(float x) {
    while (x > 3.14159265f) x -= 6.28318530f;
    while (x < -3.14159265f) x += 6.28318530f;
    float x2 = x * x;
    return x * (1.0f - x2/6.0f * (1.0f - x2/20.0f * (1.0f - x2/42.0f)));
}

OS_INLINE float os_cosf(float x) {
    return os_sinf(x + 1.57079632f);
}

#define sqrtf os_sqrtf
#define logf  os_logf
#define expf  os_expf
#define fabsf os_fabsf
#define sinf  os_sinf
#define cosf  os_cosf

#endif
