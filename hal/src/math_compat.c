/**
 * @file math_compat.c
 * @brief C23 Math Compatibility Functions for picolibc
 * 
 * picolibc (1.8.x) 不包含 C23 的 fmaximum/fminimum 系列函数。
 * 这些函数与 fmax/fmin 的区别在于 NaN 处理：
 *   - fmax(NaN, x) → x (忽略 NaN)
 *   - fmaximum(NaN, x) → NaN (传播 NaN)
 * 
 * IREE 编译器生成的代码使用 fmaximumf/fminimumf，需要此兼容层。
 */

#include <math.h>

/**
 * @brief C23 fmaximumf - 返回两个浮点数中的最大值，传播 NaN
 */
float fmaximumf(float x, float y) {
    /* 如果任一参数是 NaN，返回 NaN */
    if (isnan(x)) return x;
    if (isnan(y)) return y;
    /* 否则返回较大值 */
    return fmaxf(x, y);
}

/**
 * @brief C23 fminimumf - 返回两个浮点数中的最小值，传播 NaN
 */
float fminimumf(float x, float y) {
    if (isnan(x)) return x;
    if (isnan(y)) return y;
    return fminf(x, y);
}

/**
 * @brief C23 fmaximum - double 版本
 */
double fmaximum(double x, double y) {
    if (isnan(x)) return x;
    if (isnan(y)) return y;
    return fmax(x, y);
}

/**
 * @brief C23 fminimum - double 版本
 */
double fminimum(double x, double y) {
    if (isnan(x)) return x;
    if (isnan(y)) return y;
    return fmin(x, y);
}
