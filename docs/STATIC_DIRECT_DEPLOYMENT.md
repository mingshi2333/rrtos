# Static-Direct Deployment Workflow

This workflow turns a verified HAL schedule into a static-direct package and
then runs deployment gates against the emitted artifacts.

## Generate A Package

```bash
python3 scripts/generate_static_direct_model.py \
  --schedule logs/miniresnetv2_s1_64x50_tl_int8_fresh_hal_schedule.json \
  --model-name miniresnetv2_s1_64x50_tl_int8_fresh \
  --entry main \
  --output-dir logs/static_direct_packages/miniresnetv2_s1_64x50_tl_int8_fresh \
  --native-header apps/mnist_app/generated_miniresnet/miniresnetv1_s1_64x50_tl_int8.h \
  --native-object apps/mnist_app/generated_miniresnet/miniresnetv1_s1_64x50_tl_int8.o
```

The output directory contains:

- `static_direct.schedule.json`
- `static_direct.verify.json`
- `<model>_static_direct_desc.c/.h`
- `static_direct.package.json`

`static_direct.verify.json` is the MemoryPlanIR. The descriptor renderer
consumes its arena placements instead of independently choosing offsets.

## Validate Package Artifacts

```bash
python3 scripts/validate_static_direct_package.py \
  --package logs/static_direct_packages/miniresnetv2_s1_64x50_tl_int8_fresh/static_direct.package.json
```

This verifies SHA-256 hashes recorded in `static_direct.package.json`.

## Bind Firmware Gates

When the package is linked into a firmware validation target, regenerate the
package with map and output-oracle gates:

```bash
python3 scripts/generate_static_direct_model.py \
  --schedule <schedule.json> \
  --model-name <model_name> \
  --entry <entry> \
  --output-dir <package_dir> \
  --native-header <generated_model.h> \
  --native-object <generated_model.o> \
  --map-file <firmware.map> \
  --oracle-runner scripts/run_miniresnet_patch_validation.py \
  --oracle-kernel <firmware.elf> \
  --oracle-log <qemu.log> \
  --oracle-memory 64M
```

Then run the manifest's deploy command:

```bash
python3 scripts/validate_static_direct_package.py \
  --package <package_dir>/static_direct.package.json \
  --run-oracle
```

Current MiniResNet fresh package intentionally has `deploy_ready=false` because
its generated descriptor package is not yet the descriptor linked by the
validated firmware canary. The current runnable canary gates are:

```bash
python3 scripts/check_no_iree_vm_symbols.py \
  --map build-qemu-test/apps/mnist_app/miniresnet_patch_validation.map

python3 scripts/run_miniresnet_patch_validation.py \
  --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation \
  --log logs/miniresnet_patch_validation_qemu_test.log \
  --memory 64M
```
