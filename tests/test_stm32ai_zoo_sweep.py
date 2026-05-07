import tempfile
import unittest
from pathlib import Path
from unittest import mock

import yaml

from scripts import sweep_stm32ai_zoo_candidates as sweep


class Stm32AiZooSweepTest(unittest.TestCase):
    def test_candidate_manifest_has_stable_official_urls(self):
        self.assertGreaterEqual(len(sweep.CANDIDATES), 17)
        names = [candidate.name for candidate in sweep.CANDIDATES]
        self.assertEqual(len(names), len(set(names)))
        for expected in (
            "st_conv_freq_1channel_512_int8",
            "miniresnetv2_s1_64x50_tl_int8",
            "mobilenetv2_a035_128_int8",
            "resnet8_32_tfs_int8",
            "squeezenetv11_128_tfs_int8",
            "fastdepth_224_int8",
            "st_yololcv1_192_int8",
            "handlandmarks_full_224_int8",
            "st_movenet_lightning_a100_heatmaps_192_int8",
        ):
            self.assertIn(expected, names)

        for candidate in sweep.CANDIDATES:
            with self.subTest(candidate=candidate.name):
                self.assertRegex(candidate.name, r"^[a-z0-9_]+$")
                self.assertTrue(candidate.upstream_path)
                self.assertTrue(candidate.filename)
                self.assertTrue(candidate.suffix)
                self.assertTrue(candidate.url.startswith(sweep.ST_ZOO_BASE_URL + "/"))
                self.assertNotIn(" ", candidate.url)

    def test_generated_config_uses_rv32_size_defaults(self):
        candidate = sweep.candidate_by_name("miniresnetv1_s1_64x50_tl_int8")
        config = sweep.generated_config(
            candidate,
            sweep.PROJECT_ROOT / "models" / "stm32ai_zoo" / candidate.filename,
            sweep.PROJECT_ROOT / "build" / "stm32ai_zoo_sweep" / candidate.name / "generated",
        )

        self.assertEqual(config["defaults"]["target_arch"], "rv32")
        self.assertEqual(config["defaults"]["target_abi"], "ilp32f")
        self.assertEqual(config["defaults"]["cpu_features"], "+m,+a,+f,+zicsr")
        self.assertFalse(config["defaults"]["enable_llvmcpu_microkernels"])
        self.assertFalse(config["defaults"]["enable_data_tiling"])
        self.assertFalse(config["defaults"]["enable_stream_memory_flags"])
        self.assertEqual(config["toolchain"]["tflite_toolchain_env"], "iree-toolchain310")
        self.assertEqual(config["models"][0]["name"], candidate.name)
        self.assertEqual(
            config["models"][0]["file"],
            str(sweep.PROJECT_ROOT / "models" / "stm32ai_zoo" / candidate.filename),
        )
        self.assertNotIn("iree_backend", config["defaults"])

    def test_generated_config_can_override_iree_backend(self):
        candidate = sweep.candidate_by_name("miniresnetv2_s1_64x50_tl_int8")
        config = sweep.generated_config(
            candidate,
            sweep.PROJECT_ROOT / "models" / "stm32ai_zoo" / candidate.filename,
            sweep.PROJECT_ROOT / "build" / "stm32ai_zoo_vmvx_sweep" / candidate.name / "generated",
            iree_backend="vmvx_inline",
        )

        self.assertEqual(config["defaults"]["iree_backend"], "vmvx_inline")
        self.assertEqual(config["defaults"]["target_arch"], "rv32")

    def test_write_config_persists_yaml(self):
        candidate = sweep.candidate_by_name("st_dense_freq_1channel_512_int8")
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            model_path = root / "models" / candidate.filename
            model_path.parent.mkdir(parents=True)
            model_path.write_bytes(b"model")

            config_path = sweep.write_config(candidate, model_path, root / "candidate")
            config = yaml.safe_load(config_path.read_text(encoding="utf-8"))

        self.assertEqual(config["project_name"], f"rrtos_stm32ai_zoo_{candidate.name}")
        self.assertEqual(config["models"][0]["name"], candidate.name)
        self.assertEqual(config["models"][0]["file"], str(model_path))

    def test_onnx_candidate_is_reported_unsupported_without_codegen(self):
        candidate = sweep.candidate_by_name("mobilenetv2_a035_128_qdq_w4w8a8")
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            models_dir = root / "models"
            output_root = root / "out"
            model_path = models_dir / candidate.filename
            model_path.parent.mkdir(parents=True)
            model_path.write_bytes(b"onnx")

            with mock.patch.object(sweep, "run_command") as run_command:
                result = sweep.run_candidate(
                    candidate,
                    output_root=output_root,
                    models_dir=models_dir,
                    skip_compile=False,
                )

        self.assertEqual(result["status"], "unsupported_source_type")
        self.assertEqual(result["source_bytes"], 4)
        run_command.assert_not_called()


if __name__ == "__main__":
    unittest.main()
