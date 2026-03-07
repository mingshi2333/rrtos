# BE-U1000 DFU Flashing Guide

This guide documents how to flash `rrtos` to BE-U1000 eFlash using `dfu-util`.

## 1. Prerequisites

- Host has `dfu-util` installed (`dfu-util --version`)
- Board is connected over USB and placed in DFU boot mode
- Firmware is built in flash memory model (linked at `0xA0000000`)

Notes:
- If your board revision uses jumpers/boot straps, use the EVU-BA manual for your exact revision.
- On Linux, you may need udev permissions to access USB DFU devices.

## 2. Build Flash Image

Configure and build BE-U1000 in flash model:

```bash
cmake -B build-be_u1000_flash -DCONFIG_BOARD=be_u1000 -DBE_U1000_MEMORY_MODEL=flash
cmake --build build-be_u1000_flash
```

Expected output artifact:
- `build-be_u1000_flash/rrtos_be_u1000.bin`

## 3. Enter USB DFU Mode

For EVU-BA-2.3, the official quickstart requires this board setup before DFU flashing:

- Remove power from the board first
- Set `XP11 (PWR SEL)` to `USB`
- Set `XP1` jumpers to USB boot mode:
  - installed: `M0`, `M1`
  - removed: `M2`, `DBG`, `CLK`
- Connect USB Type-C to `XS1 (USB OTG)`
- Press `SB2 (RESET)` after the cable is connected

## 4. Confirm DFU Device

List DFU-capable interfaces:

```bash
dfu-util -l
```

For EVU-BA-2.3 DFU boot mode, the official quickstart shows output similar to:

- `alt=3, name="eFlash"`
- `alt=2, name="eFlash and run"`

It also uses `alt=0` for TCM writes.

If your board shows a different set of alt values, use the values printed by your own `dfu-util -l` output.

## 5. Flash to eFlash

Recommended command for immediate execution after flashing:

```bash
dfu-util --alt 2 --download build-be_u1000_flash/rrtos_be_u1000.bin
```

Recommended command for programming eFlash without immediate execution:

```bash
dfu-util --alt 3 --download build-be_u1000_flash/rrtos_be_u1000.bin
```

Where:
- `--alt 2` means `eFlash and run`
- `--alt 3` means `eFlash`
- `--download` specifies the binary to download

If multiple DFU devices are present, select the exact device path reported by `dfu-util -l`:

```bash
dfu-util --path <device-path> --alt 2 --download build-be_u1000_flash/rrtos_be_u1000.bin
```

If you use `alt 3`, remove jumpers `M0`, `M1`, `M2`, `DBG`, `CLK` from `XP1` and press `SB2 (RESET)` to boot from eFlash.

## 6. Post-Flash Verification

After boot, capture UART output and validate expected boot markers.

If the serial log is saved to `logs/be_u1000_boot.log`, run:

```bash
python scripts/be_u1000/check_boot_log.py \
  --log logs/be_u1000_boot.log \
  --expect-irq-model CLIC \
  --board-config boards/be_u1000/board_config.h
```

Expected result:
- `BOOT_LOG_CHECK_OK`

## 7. Troubleshooting

### No device in `dfu-util -l`

- Re-check USB cable/power and DFU boot mode straps
- Try a direct USB port (avoid passive hubs)
- Replug board and run `dfu-util -l` again
- On Linux, check permissions (`sudo` as a temporary test only)

### Download fails or writes wrong location

- Verify selected alt target matches your intent:
  - `alt 2` -> `eFlash and run`
  - `alt 3` -> `eFlash`
- Re-run `dfu-util -l` and use the exact displayed alt name/index/path

### Board boots old firmware

- Ensure you flashed `build-be_u1000_flash/rrtos_be_u1000.bin`
- Confirm build was done with `BE_U1000_MEMORY_MODEL=flash`
- Power-cycle board and capture a fresh UART log
