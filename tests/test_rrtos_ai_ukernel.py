import pathlib
import subprocess
import tempfile
import unittest


PROJECT_ROOT = pathlib.Path(__file__).resolve().parents[1]


class RrtosAiUkernelTest(unittest.TestCase):
    def test_header_exports_v0_kernel_catalog(self):
        header = (PROJECT_ROOT / "ai" / "ukernel" / "rrtos_ai_ukernel.h").read_text()
        self.assertIn("RRTOS_AI_UKERNEL_ABI_VERSION 1u", header)
        self.assertIn("rrtos_ai_ukernel_status_t", header)
        self.assertIn("RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_HWIO", header)
        self.assertIn("RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_OHWI", header)
        self.assertIn("rrtos_ai_ukernel_conv2d_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_requant_i32_to_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_maxpool_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_add_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_clamp_i8", header)
        self.assertIn(
            "rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t",
            header,
        )
        self.assertIn("uint32_t struct_size", header)
        self.assertIn("uint32_t abi_version", header)
        self.assertIn("uint32_t flags", header)
        self.assertIn("uint32_t reserved", header)
        self.assertIn(
            "rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile",
            header,
        )
        self.assertIn(
            "rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31",
            header,
        )
        self.assertNotIn("requant_row", header)
        self.assertNotIn("ch1_ohwi_7x7_s2_pad3", header)

    def test_source_is_freestanding_and_exports_kernel_functions(self):
        source = (PROJECT_ROOT / "ai" / "ukernel" / "rrtos_ai_ukernel.c").read_text()
        self.assertIn('#include "rrtos_ai_ukernel.h"', source)
        self.assertNotIn("#include <stdio.h>", source)
        self.assertNotIn("#include <stdlib.h>", source)
        self.assertIn("static int8_t saturate_i8", source)
        self.assertIn("static int32_t multiply_by_quantized_multiplier", source)
        self.assertIn("rrtos_ai_ukernel_conv2d_i8_tile(", source)
        self.assertIn("rrtos_ai_ukernel_requant_i32_to_i8_tile(", source)
        self.assertIn("rrtos_ai_ukernel_maxpool_i8_tile(", source)
        self.assertIn("static int32_t round_float_to_i32", source)
        self.assertIn(
            "rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(",
            source,
        )
        self.assertIn(
            "rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31(",
            source,
        )
        self.assertIn("static int range_exceeds_i32", source)
        self.assertIn("static int padded_dim_excludes_halo", source)
        self.assertIn("static int is_invalid_requant_tile_params", source)
        self.assertIn("params->struct_size != sizeof(*params)", source)
        self.assertIn("params->abi_version != RRTOS_AI_UKERNEL_ABI_VERSION", source)
        self.assertIn("params->weight_zero_point == 0", source)
        self.assertNotIn("/ (1LL << 31)", source)

    def test_build_script_emits_rv32_bitcode_and_object(self):
        script = (PROJECT_ROOT / "scripts" / "build_rrtos_ai_ukernel.sh").read_text()
        self.assertIn("--target=riscv32-unknown-elf", script)
        self.assertIn("-march=${RRTOS_AI_UKERNEL_RISCV_MARCH}", script)
        self.assertIn("-mabi=${RRTOS_AI_UKERNEL_RISCV_ABI}", script)
        self.assertIn("-emit-llvm", script)
        self.assertIn("rrtos_ai_ukernel_riscv32.bc", script)
        self.assertIn("rrtos_ai_ukernel_riscv32.o", script)

    def test_build_script_creates_bitcode_and_object(self):
        script = PROJECT_ROOT / "scripts" / "build_rrtos_ai_ukernel.sh"
        with tempfile.TemporaryDirectory() as temp_dir:
            result = subprocess.run(
                ["bash", str(script), temp_dir],
                cwd=PROJECT_ROOT,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )
            self.assertEqual(
                result.returncode,
                0,
                msg=f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}",
            )
            self.assertTrue(
                (pathlib.Path(temp_dir) / "rrtos_ai_ukernel_riscv32.bc").is_file()
            )
            self.assertTrue(
                (pathlib.Path(temp_dir) / "rrtos_ai_ukernel_riscv32.o").is_file()
            )

    def test_conv2d_ohwi_requant_tile_matches_host_oracle(self):
        program = r"""
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "rrtos_ai_ukernel.h"

static int8_t ref_sat_i8(int32_t value, int32_t lo, int32_t hi) {
    if (lo < -128) {
        lo = -128;
    }
    if (hi > 127) {
        hi = 127;
    }
    if (value < lo) {
        value = lo;
    }
    if (value > hi) {
        value = hi;
    }
    return (int8_t)value;
}

static int32_t ref_round(float value) {
    if (value >= 2147483647.0f) {
        return 2147483647;
    }
    if (value <= -2147483648.0f) {
        return (-2147483647 - 1);
    }
    return (int32_t)(value >= 0.0f ? value + 0.5f : value - 0.5f);
}

static int32_t ref_rounding_high_mul(int32_t lhs, int32_t rhs) {
    int64_t product = (int64_t)lhs * (int64_t)rhs;
    int64_t nudge = product >= 0 ? (1LL << 30) : (1LL - (1LL << 30));
    return (int32_t)((product + nudge) / (1LL << 31));
}

static int32_t ref_rounding_divide_by_power_of_two(int32_t value,
                                                   int32_t exponent) {
    if (exponent <= 0) {
        return value;
    }
    if (exponent >= 31) {
        return value >= 0 ? 0 : -1;
    }
    const int32_t mask = (1 << exponent) - 1;
    const int32_t remainder = value & mask;
    const int32_t threshold = (mask >> 1) + (value < 0 ? 1 : 0);
    return (value >> exponent) + (remainder > threshold ? 1 : 0);
}

static int32_t ref_multiply_by_quantized_multiplier(int32_t value,
                                                    int32_t multiplier,
                                                    int32_t shift) {
    const int32_t left_shift = shift > 0 ? shift : 0;
    const int32_t right_shift = shift > 0 ? 0 : -shift;
    int64_t shifted = (int64_t)value * (1LL << left_shift);
    if (shifted > 2147483647LL) {
        shifted = 2147483647LL;
    }
    if (shifted < (-2147483647LL - 1LL)) {
        shifted = (-2147483647LL - 1LL);
    }
    return ref_rounding_divide_by_power_of_two(
        ref_rounding_high_mul((int32_t)shifted, multiplier), right_shift);
}

static void ref_tile(const int8_t *input,
                     const int8_t *weights,
                     const int32_t *bias,
                     const float *multipliers,
                     int8_t *padded_output,
                     const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *p) {
    for (int32_t tr = 0; tr < p->tile_row_count; ++tr) {
        const int32_t output_row = p->tile_row_begin + tr;
        for (int32_t ow = 0; ow < p->output_w; ++ow) {
            for (int32_t oc = 0; oc < p->output_c; ++oc) {
                int32_t acc = bias[oc];
                for (int32_t kh = 0; kh < p->kernel_h; ++kh) {
                    const int32_t ih = output_row * p->stride_h + kh - p->pad_top;
                    if (ih < 0 || ih >= p->input_h) {
                        continue;
                    }
                    for (int32_t kw = 0; kw < p->kernel_w; ++kw) {
                        const int32_t iw = ow * p->stride_w + kw - p->pad_left;
                        if (iw < 0 || iw >= p->input_w) {
                            continue;
                        }
                        for (int32_t ic = 0; ic < p->input_c; ++ic) {
                            const int32_t input_index =
                                ((ih * p->input_w + iw) * p->input_c) + ic;
                            const int32_t weight_index =
                                (((oc * p->kernel_h + kh) * p->kernel_w + kw) *
                                    p->input_c) +
                                ic;
                            acc += ((int32_t)input[input_index] - p->input_zero_point) *
                                   ((int32_t)weights[weight_index] - p->weight_zero_point);
                        }
                    }
                }
                const int32_t output_q =
                    ref_round((float)acc * multipliers[oc]) + p->output_zero_point;
                const int32_t output_index =
                    (((output_row + 1) * p->padded_output_w + (ow + 1)) *
                        p->output_c) +
                    oc;
                padded_output[output_index] =
                    ref_sat_i8(output_q, p->activation_min, p->activation_max);
            }
        }
    }
}

static void ref_tile_q31(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    const int32_t *multipliers,
    const int32_t *shifts,
    int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *p) {
    for (int32_t tr = 0; tr < p->tile_row_count; ++tr) {
        const int32_t output_row = p->tile_row_begin + tr;
        for (int32_t ow = 0; ow < p->output_w; ++ow) {
            for (int32_t oc = 0; oc < p->output_c; ++oc) {
                int32_t acc = bias[oc];
                for (int32_t kh = 0; kh < p->kernel_h; ++kh) {
                    const int32_t ih = output_row * p->stride_h + kh - p->pad_top;
                    if (ih < 0 || ih >= p->input_h) {
                        continue;
                    }
                    for (int32_t kw = 0; kw < p->kernel_w; ++kw) {
                        const int32_t iw = ow * p->stride_w + kw - p->pad_left;
                        if (iw < 0 || iw >= p->input_w) {
                            continue;
                        }
                        for (int32_t ic = 0; ic < p->input_c; ++ic) {
                            const int32_t input_index =
                                ((ih * p->input_w + iw) * p->input_c) + ic;
                            const int32_t weight_index =
                                (((oc * p->kernel_h + kh) * p->kernel_w + kw) *
                                    p->input_c) +
                                ic;
                            acc += ((int32_t)input[input_index] - p->input_zero_point) *
                                   ((int32_t)weights[weight_index] - p->weight_zero_point);
                        }
                    }
                }
                const int32_t scaled = ref_multiply_by_quantized_multiplier(
                    acc, multipliers[oc], shifts[oc]);
                const int32_t output_q = scaled + p->output_zero_point;
                const int32_t output_index =
                    (((output_row + 1) * p->padded_output_w + (ow + 1)) *
                        p->output_c) +
                    oc;
                padded_output[output_index] =
                    ref_sat_i8(output_q, p->activation_min, p->activation_max);
            }
        }
    }
}

static int run_case(const char *name,
                    const int8_t *input,
                    const int8_t *weights,
                    const int32_t *bias,
                    const float *multipliers,
                    rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    int8_t got[8 * 8 * 3];
    int8_t expected[8 * 8 * 3];

    memset(got, 0x55, sizeof(got));
    memset(expected, 0x55, sizeof(expected));

    rrtos_ai_ukernel_status_t rc =
        rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
            input, weights, bias, multipliers, got, params);
    if (rc != RRTOS_AI_UKERNEL_OK) {
        printf("%s unexpected rc=%d\n", name, (int)rc);
        return 1;
    }

    ref_tile(input, weights, bias, multipliers, expected, params);
    if (memcmp(got, expected, sizeof(got)) != 0) {
        for (int32_t i = 0; i < (int32_t)sizeof(got); ++i) {
            if (got[i] != expected[i]) {
                printf("%s mismatch index=%d got=%d expected=%d\n",
                       name,
                       (int)i,
                       (int)got[i],
                       (int)expected[i]);
                break;
            }
        }
        return 2;
    }
    return 0;
}

static int run_case_q31(
    const char *name,
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    const int32_t *multipliers,
    const int32_t *shifts,
    rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    int8_t got[8 * 8 * 3];
    int8_t expected[8 * 8 * 3];

    memset(got, 0x55, sizeof(got));
    memset(expected, 0x55, sizeof(expected));

    rrtos_ai_ukernel_status_t rc =
        rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31(
            input, weights, bias, multipliers, shifts, got, params);
    if (rc != RRTOS_AI_UKERNEL_OK) {
        printf("%s unexpected q31 rc=%d\n", name, (int)rc);
        return 1;
    }

    ref_tile_q31(input, weights, bias, multipliers, shifts, expected, params);
    if (memcmp(got, expected, sizeof(got)) != 0) {
        for (int32_t i = 0; i < (int32_t)sizeof(got); ++i) {
            if (got[i] != expected[i]) {
                printf("%s q31 mismatch index=%d got=%d expected=%d\n",
                       name,
                       (int)i,
                       (int)got[i],
                       (int)expected[i]);
                break;
            }
        }
        return 2;
    }
    return 0;
}

int main(void) {
    int8_t input[8 * 8 * 2];
    int8_t weights[3 * 7 * 7 * 2];
    int32_t bias[3] = {13, -17, 23};
    float multipliers[3] = {0.125f, 0.0625f, 0.25f};
    int32_t multipliers_q31[3] = {1073741824, 1073741824, 1073741824};
    int32_t shifts[3] = {-2, -3, -1};

    for (int32_t i = 0; i < (int32_t)(sizeof(input) / sizeof(input[0])); ++i) {
        input[i] = (int8_t)((i * 5 + 7) % 31 - 15);
    }
    for (int32_t i = 0; i < (int32_t)(sizeof(weights) / sizeof(weights[0])); ++i) {
        weights[i] = (int8_t)((i * 3 + 11) % 17 - 8);
    }

    rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t fast_params = {
        .struct_size = sizeof(fast_params),
        .abi_version = RRTOS_AI_UKERNEL_ABI_VERSION,
        .flags = 0,
        .reserved = 0,
        .input_h = 8,
        .input_w = 8,
        .input_c = 1,
        .output_h = 4,
        .output_w = 4,
        .output_c = 2,
        .kernel_h = 7,
        .kernel_w = 7,
        .stride_h = 2,
        .stride_w = 2,
        .pad_top = 3,
        .pad_left = 3,
        .tile_row_begin = 2,
        .tile_row_count = 1,
        .padded_output_h = 6,
        .padded_output_w = 6,
        .input_zero_point = -3,
        .weight_zero_point = 0,
        .output_zero_point = -7,
        .activation_min = -128,
        .activation_max = 127,
    };

    if (run_case("fast", input, weights, bias, multipliers, &fast_params) != 0) {
        return 1;
    }

    rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t generic_params = {
        .struct_size = sizeof(generic_params),
        .abi_version = RRTOS_AI_UKERNEL_ABI_VERSION,
        .flags = 0,
        .reserved = 0,
        .input_h = 4,
        .input_w = 4,
        .input_c = 2,
        .output_h = 4,
        .output_w = 4,
        .output_c = 3,
        .kernel_h = 3,
        .kernel_w = 3,
        .stride_h = 1,
        .stride_w = 1,
        .pad_top = 1,
        .pad_left = 1,
        .tile_row_begin = 1,
        .tile_row_count = 2,
        .padded_output_h = 6,
        .padded_output_w = 6,
        .input_zero_point = -2,
        .weight_zero_point = 3,
        .output_zero_point = 5,
        .activation_min = -20,
        .activation_max = 25,
    };

    if (run_case("generic", input, weights, bias, multipliers, &generic_params) != 0) {
        return 2;
    }
    if (run_case_q31("fast", input, weights, bias,
                     multipliers_q31, shifts, &fast_params) != 0) {
        return 7;
    }
    if (run_case_q31("generic", input, weights, bias,
                     multipliers_q31, shifts, &generic_params) != 0) {
        return 8;
    }

    rrtos_ai_ukernel_status_t rc;
    fast_params.tile_row_begin = fast_params.output_h;
    rc = rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
        input, weights, bias, multipliers, input, &fast_params);
    if (rc != RRTOS_AI_UKERNEL_INVALID_ARGUMENT) {
        printf("expected invalid row rc, got %d\n", (int)rc);
        return 3;
    }

    fast_params.tile_row_begin = INT32_MAX;
    fast_params.tile_row_count = 2;
    fast_params.output_h = INT32_MAX;
    rc = rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
        input, weights, bias, multipliers, input, &fast_params);
    if (rc != RRTOS_AI_UKERNEL_INVALID_ARGUMENT) {
        printf("expected overflow-safe range rc, got %d\n", (int)rc);
        return 4;
    }

    fast_params = (rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t){
        .struct_size = sizeof(fast_params),
        .abi_version = RRTOS_AI_UKERNEL_ABI_VERSION,
        .flags = 0,
        .reserved = 0,
        .input_h = 8,
        .input_w = 8,
        .input_c = 1,
        .output_h = INT32_MAX - 1,
        .output_w = 4,
        .output_c = 2,
        .kernel_h = 7,
        .kernel_w = 7,
        .stride_h = 2,
        .stride_w = 2,
        .pad_top = 3,
        .pad_left = 3,
        .tile_row_begin = 0,
        .tile_row_count = 1,
        .padded_output_h = INT32_MAX,
        .padded_output_w = 6,
        .input_zero_point = -3,
        .weight_zero_point = 0,
        .output_zero_point = -7,
        .activation_min = -128,
        .activation_max = 127,
    };
    rc = rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
        input, weights, bias, multipliers, input, &fast_params);
    if (rc != RRTOS_AI_UKERNEL_INVALID_ARGUMENT) {
        printf("expected overflow-safe padded bound rc, got %d\n", (int)rc);
        return 5;
    }

    fast_params.output_h = 4;
    fast_params.padded_output_h = 6;
    fast_params.struct_size = 0;
    rc = rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
        input, weights, bias, multipliers, input, &fast_params);
    if (rc != RRTOS_AI_UKERNEL_INVALID_ARGUMENT) {
        printf("expected invalid struct size rc, got %d\n", (int)rc);
        return 6;
    }
    return 0;
}
"""
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = pathlib.Path(temp_dir)
            source_path = temp_path / "ukernel_oracle.c"
            binary_path = temp_path / "ukernel_oracle"
            source_path.write_text(program)

            build = subprocess.run(
                [
                    "cc",
                    "-std=c99",
                    "-Wall",
                    "-Wextra",
                    "-I",
                    str(PROJECT_ROOT / "ai" / "ukernel"),
                    str(PROJECT_ROOT / "ai" / "ukernel" / "rrtos_ai_ukernel.c"),
                    str(source_path),
                    "-o",
                    str(binary_path),
                ],
                cwd=PROJECT_ROOT,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )
            self.assertEqual(
                build.returncode,
                0,
                msg=f"stdout:\n{build.stdout}\nstderr:\n{build.stderr}",
            )

            run = subprocess.run(
                [str(binary_path)],
                cwd=PROJECT_ROOT,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )
            self.assertEqual(
                run.returncode,
                0,
                msg=f"stdout:\n{run.stdout}\nstderr:\n{run.stderr}",
            )


if __name__ == "__main__":
    unittest.main()
