# IREE Model Zoo

This directory is kept for optional model-conversion experiments. The supported
AI runtime path for this repository is the generated MNIST model under
`apps/mnist_app/generated/` and is validated by:

```bash
pixi run -e rv32 validate-mnist-runtime
```

Do not treat artifacts under `zoo/iree_static/` as supported runtime inputs
unless they have an explicit validation target and are listed in the supported
matrix.

## Conversion Helper

`scripts/tflite_to_iree_c.sh` converts a TFLite file into VM-C and static
library artifacts for local experimentation:

```bash
cd zoo/scripts
./tflite_to_iree_c.sh -a rv32 <path/to/model.tflite> <output_name>
```

Generated files are written under `zoo/iree_static/`.
