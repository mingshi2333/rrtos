# Model Sources

This directory stores source models that are needed to regenerate checked-in
IREE artifacts, plus explicitly downloaded measurement candidates.

## `st_mnistv1_28_tfs_int8.tflite`

- Source: STMicroelectronics STM32 AI Model Zoo
- Upstream path: `image_classification/st_mnistv1/ST_pretrainedmodel_public_dataset/emnist_byclass/st_mnistv1_28_tfs/st_mnistv1_28_tfs_int8.tflite`
- Download URL: `https://github.com/STMicroelectronics/stm32ai-modelzoo/raw/main/image_classification/st_mnistv1/ST_pretrainedmodel_public_dataset/emnist_byclass/st_mnistv1_28_tfs/st_mnistv1_28_tfs_int8.tflite`
- SHA-256: `ab692fae141d25b0decb97713e0cc413e26697739e34cefebd273d00105c0aca`
- Local generation config: `ai_models_mnist_quant.yaml`

## `stm32ai_zoo/`

This subdirectory is a local cache for the STM32AI Model Zoo sweep tool:

```bash
python3 scripts/sweep_stm32ai_zoo_candidates.py
```

The sweep downloads the selected ST candidates, runs every supported `.tflite`
model through `scripts/ai_codegen.py`, and writes reports to:

- `build/stm32ai_zoo_sweep/stm32ai_zoo_sweep.csv`
- `build/stm32ai_zoo_sweep/stm32ai_zoo_sweep.md`

The cache is intentionally separate from the canonical MNIST validation model so
experiments do not rewrite the supported validation artifacts.

`miniresnetv1_s1_64x50_tl_int8.tflite` is also used by the experimental
MiniResNet runtime measurement target declared in
`ai_models_stm32ai_miniresnet.yaml`.
