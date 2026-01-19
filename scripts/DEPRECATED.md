# Deprecated Scripts

These scripts are deprecated and replaced by Pixi Source Build.

## Migration Guide

### Old: `get_picolibc.sh`

```bash
pixi run get-picolibc  # New method
```

### Old: `build_picolibc.sh`

```bash
cd packages/picolibc-rv64
pixi build             # New method (RV64)

cd packages/picolibc-rv32
pixi build             # New method (RV32)
```

## Why Deprecated?

- Manual scripts are error-prone
- No caching or incremental builds
- Hard to maintain multi-architecture support
- Pixi Source Build provides:
  - Automatic dependency management
  - Reproducible builds via `pixi.lock`
  - Incremental compilation
  - Easy architecture switching

## Legacy Support

If you still need the old scripts, rename them back:

```bash
mv scripts/build_picolibc.sh.deprecated scripts/build_picolibc.sh
mv scripts/get_picolibc.sh.deprecated scripts/get_picolibc.sh
pixi run build-picolibc-legacy
```
