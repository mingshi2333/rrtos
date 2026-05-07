import tempfile
import unittest
from pathlib import Path

from scripts import generate_static_direct_descriptor as generator


SCHEDULE = {
    "schema": "rrtos.iree_hal_schedule.v1",
    "source": "model.11.hal.mlir",
    "fixed_shape": True,
    "is_linear": True,
    "allocations": [
        {"id": "input", "size": 3136, "lifetime": "input"},
        {"id": "output", "size": 40, "lifetime": "output"},
        {"id": "weights", "size": 128, "lifetime": "constant"},
        {"id": "arena", "size": 576, "lifetime": "transient"},
    ],
    "dispatches": [
        {
            "ordinal": 1,
            "workgroups": [1, 1, 1],
            "constants": [7, 11],
            "bindings": [
                {
                    "ordinal": 0,
                    "base": "arena",
                    "offset": 0,
                    "length": 576,
                    "access": "rw",
                },
                {
                    "ordinal": 1,
                    "base": "weights",
                    "offset": 16,
                    "length": 32,
                    "access": "ro",
                },
            ],
        }
    ],
    "peak_transient_bytes": 576,
}


class StaticDirectDescriptorGeneratorTest(unittest.TestCase):
    def test_generates_descriptor_c_and_header_from_linear_fixed_schedule(self):
        with tempfile.TemporaryDirectory() as tmp:
            output_c = Path(tmp) / "test_model_static_direct_desc.c"
            output_h = Path(tmp) / "test_model_static_direct_desc.h"

            generator.generate_descriptor(
                SCHEDULE,
                model_name="test_model",
                entry="predict",
                output_c=output_c,
                output_h=output_h,
            )

            c_text = output_c.read_text(encoding="ascii")
            h_text = output_h.read_text(encoding="ascii")

        self.assertIn("const ai_static_direct_dispatch_t test_model_dispatches[] = {", c_text)
        self.assertIn(".base = AI_STATIC_DIRECT_BUFFER_ARENA,", c_text)
        self.assertIn(".offset = 0u,", c_text)
        self.assertIn(".length = 576u,", c_text)
        self.assertIn(".base = AI_STATIC_DIRECT_BUFFER_CONST,", c_text)
        self.assertIn("AI_STATIC_DIRECT_BUFFER_INPUT", c_text)
        self.assertIn("AI_STATIC_DIRECT_BUFFER_OUTPUT", c_text)
        self.assertIn("AI_STATIC_DIRECT_BUFFER_CONST", c_text)
        self.assertIn("AI_STATIC_DIRECT_BUFFER_ARENA", c_text)
        self.assertIn(".model_name = \"test_model\"", c_text)
        self.assertIn(".entry_name = \"predict\"", c_text)
        self.assertIn(".input_bytes = 3136u", c_text)
        self.assertIn(".output_bytes = 40u", c_text)
        self.assertIn(".const_bytes = 128u", c_text)
        self.assertIn(".arena_bytes = 576u", c_text)
        self.assertIn(".commands = test_model_commands", c_text)
        self.assertIn(".command_count = 1u", c_text)
        self.assertIn(".unsupported_feature_flags = 0u", c_text)
        self.assertIn("extern const ai_static_direct_descriptor_t test_model_descriptor;", h_text)

    def test_rejects_non_fixed_or_non_linear_schedule(self):
        for field in ("fixed_shape", "is_linear"):
            schedule = dict(SCHEDULE)
            schedule[field] = False

            with self.subTest(field=field):
                with self.assertRaisesRegex(ValueError, field):
                    generator.render_descriptor(
                        schedule,
                        model_name="test_model",
                        entry="predict",
                        header_name="test_model_static_direct_desc.h",
                    )

    def test_disjoint_transient_lifetimes_reuse_arena_offsets(self):
        schedule = dict(SCHEDULE)
        schedule["allocations"] = [
            {"id": "arena_a", "size": 128, "lifetime": "transient"},
            {"id": "arena_b", "size": 256, "lifetime": "transient"},
        ]
        schedule["dispatches"] = [
            {
                "ordinal": 0,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {
                        "ordinal": 0,
                        "base": "arena_a",
                        "offset": 0,
                        "length": 128,
                        "access": "rw",
                    }
                ],
            },
            {
                "ordinal": 1,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {
                        "ordinal": 0,
                        "base": "arena_b",
                        "offset": 4,
                        "length": 256,
                        "access": "rw",
                    }
                ],
            }
        ]
        schedule["commands"] = [
            {"type": "dispatch", "dispatch_index": 0},
            {"type": "dispatch", "dispatch_index": 1},
        ]
        schedule["peak_transient_bytes"] = 256

        c_text, _ = generator.render_descriptor(
            schedule,
            model_name="test_model",
            entry="predict",
            header_name="test_model_static_direct_desc.h",
        )

        self.assertIn(".constants = 0,", c_text)
        self.assertNotIn("static const uint32_t *const", c_text)
        self.assertIn(".offset = 4u,", c_text)
        self.assertIn(".arena_bytes = 256u", c_text)

    def test_overlapping_transient_lifetimes_do_not_reuse_arena_offsets(self):
        schedule = dict(SCHEDULE)
        schedule["allocations"] = [
            {"id": "arena_a", "size": 128, "lifetime": "transient"},
            {"id": "arena_b", "size": 256, "lifetime": "transient"},
        ]
        schedule["dispatches"] = [
            {
                "ordinal": 0,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {
                        "ordinal": 0,
                        "base": "arena_a",
                        "offset": 0,
                        "length": 128,
                        "access": "rw",
                    },
                    {
                        "ordinal": 1,
                        "base": "arena_b",
                        "offset": 4,
                        "length": 256,
                        "access": "rw",
                    },
                ],
            }
        ]
        schedule["commands"] = [{"type": "dispatch", "dispatch_index": 0}]
        schedule["peak_transient_bytes"] = 384

        c_text, _ = generator.render_descriptor(
            schedule,
            model_name="test_model",
            entry="predict",
            header_name="test_model_static_direct_desc.h",
        )

        self.assertIn(".offset = 132u,", c_text)
        self.assertIn(".arena_bytes = 384u", c_text)

    def test_generates_interleaved_fill_and_dispatch_commands(self):
        schedule = dict(SCHEDULE)
        schedule["commands"] = [
            {
                "type": "fill",
                "target": {
                    "base": "arena",
                    "offset": 4,
                    "length": 8,
                    "access": "write",
                },
                "pattern": 255,
            },
            {"type": "dispatch", "dispatch_index": 0},
        ]

        c_text, h_text = generator.render_descriptor(
            schedule,
            model_name="test_model",
            entry="predict",
            header_name="test_model_static_direct_desc.h",
        )

        self.assertIn("const ai_static_direct_command_t test_model_commands[] = {", c_text)
        self.assertIn(".type = AI_STATIC_DIRECT_COMMAND_FILL", c_text)
        self.assertIn(".pattern = 255u", c_text)
        self.assertIn(".type = AI_STATIC_DIRECT_COMMAND_DISPATCH", c_text)
        self.assertIn(".dispatch = &test_model_dispatches[0]", c_text)
        self.assertIn(".command_count = 2u", c_text)
        self.assertIn("#define TEST_MODEL_COMMAND_COUNT 2u", h_text)

    def test_descriptor_can_consume_verified_memory_plan_placements(self):
        schedule = dict(SCHEDULE)
        schedule["allocations"] = [
            {"id": "arena_a", "size": 128, "lifetime": "transient"},
            {"id": "arena_b", "size": 256, "lifetime": "transient"},
        ]
        schedule["dispatches"] = [
            {
                "ordinal": 0,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {
                        "ordinal": 0,
                        "base": "arena_a",
                        "offset": 0,
                        "length": 128,
                        "access": "rw",
                    },
                    {
                        "ordinal": 1,
                        "base": "arena_b",
                        "offset": 4,
                        "length": 128,
                        "access": "rw",
                    },
                ],
            }
        ]
        schedule["commands"] = [{"type": "dispatch", "dispatch_index": 0}]
        schedule["peak_transient_bytes"] = 384
        memory_plan = {
            "arena_bytes": 1024,
            "input_bytes": 0,
            "output_bytes": 0,
            "const_bytes": 0,
            "placements": {
                "arena_a": {"offset": 512, "size": 128, "interval": [0, 1]},
                "arena_b": {"offset": 768, "size": 256, "interval": [0, 1]},
            },
        }

        c_text, h_text = generator.render_descriptor(
            schedule,
            model_name="test_model",
            entry="predict",
            header_name="test_model_static_direct_desc.h",
            memory_plan=memory_plan,
        )

        self.assertIn(".offset = 512u,", c_text)
        self.assertIn(".offset = 772u,", c_text)
        self.assertIn(".arena_bytes = 1024u", c_text)
        self.assertIn("#define TEST_MODEL_ARENA_BYTES 1024u", h_text)
        self.assertNotIn(".offset = 132u,", c_text)

    def test_rejects_memory_plan_with_missing_arena_placement(self):
        memory_plan = {
            "arena_bytes": 576,
            "input_bytes": 3136,
            "output_bytes": 40,
            "const_bytes": 128,
            "placements": {},
        }

        with self.assertRaisesRegex(ValueError, "missing memory-plan placement"):
            generator.render_descriptor(
                SCHEDULE,
                model_name="test_model",
                entry="predict",
                header_name="test_model_static_direct_desc.h",
                memory_plan=memory_plan,
            )

    def test_rejects_memory_plan_with_mismatched_buffer_totals(self):
        memory_plan = {
            "arena_bytes": 576,
            "input_bytes": 3137,
            "output_bytes": 40,
            "const_bytes": 128,
            "placements": {
                "arena": {"offset": 0, "size": 576, "interval": [0, 1]},
            },
        }

        with self.assertRaisesRegex(ValueError, "input_bytes"):
            generator.render_descriptor(
                SCHEDULE,
                model_name="test_model",
                entry="predict",
                header_name="test_model_static_direct_desc.h",
                memory_plan=memory_plan,
            )


if __name__ == "__main__":
    unittest.main()
