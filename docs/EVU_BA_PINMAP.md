# EVU-BA-2.3 Pin Map

This note captures the board-level connector mapping for the BE-EVB-U1000 / EVU-BA-2.3 board family used by the current `be_u1000` lane.

## Sources

- BE-EVB-U1000 / EVU-BA-2.3 instruction, v2.3.1, dated 2025-12-30
- EVU-BA connector, power-source, boot-mode, and DFU tables from the official PDF
- `pdftotext -layout` extraction from the PDF
- OCR cross-check on noisy connector regions
- BE-U1000 SDK examples for CANFD behavior cross-checks

## Confidence

- `high`: the official document table is explicit, or vector-text extraction and OCR agree
- `medium`: connector role is clear, but some pin order or pairing is noisy
- `low`: only the exposed net set is reliable, not the exact connector pin order

## Connector Summary

| Connector | Role | Confidence | Notes |
| --- | --- | --- | --- |
| `XP7` | User LED link/header | medium | 2-pin link around `USER_LED/PC0` and `GND` |
| `XP8` | Arduino/power-side companion header | low | large expansion block exposing `PCx`, reset, power rails, and `VIN<n>` |
| `XS3` | Arduino companion header block | low | exact pin order is too noisy in extracted text |
| `XS4` | Arduino analog header | high | `VIN<0>`..`VIN<5>` |
| `XS5` | Arduino digital header, PA-side | high | `PA4`..`PA13`, `AREF`, `GND`, `USER_LED/PC0` nearby |
| `XS6` | Arduino/QSPI/PB-side header | high | `PB6`..`PB15` subset plus `QSPI1_*` |
| `XP9` | PA/PC breakout companion header | low | net set is clear, exact numbering is not |
| `XP10` | PB/QSPI breakout companion header | medium | net set is clear, some positions are noisy |
| `XP11` | Power-source select header | high | 2x3 jumper block with explicit source-selection legend |

## High-Confidence Tables

### XP11 - Power Source Select

The PDF text explicitly gives the source-selection legend.

| Pins | Function |
| --- | --- |
| `1-2` | power from USB Type-C (`USB_5V_IN`) |
| `3-4` | power from external `5V` (`XP8-18`) |
| `5-6` | power from external `VIN` (`XP8-24`, `10V-48V`) |

Visible nets on the header:

| Pin | Net |
| --- | --- |
| `1` | `USB_5V_IN` |
| `2` | `USB` |
| `3` | `5V_EXT` |
| `4` | `EXT5V` |
| `5` | `5V_INT` |
| `6` | `VIN` |

### XS4 - Arduino Analog Header

| XS4 pin | Net |
| --- | --- |
| `1` | `VIN<0>` |
| `2` | `VIN<1>` |
| `3` | `VIN<2>` |
| `4` | `VIN<3>` |
| `5` | `VIN<4>` |
| `6` | `VIN<5>` |

Related nets visible nearby on the companion connector region: `VIN<6>`, `VIN<7>`, `VIN`, and `GND`.

### XS5 - Arduino Digital Header, PA-Side

This is the cleanest PA-side expansion block in the official connector tables.

| XS5 pin | Net |
| --- | --- |
| `1` | `PA13` |
| `2` | `TIM0_PWM0/PA12` |
| `3` | `CS_1/PA8` |
| `4` | `MOSI_1/PA10` |
| `5` | `MISO_1/PA11` |
| `6` | `SCK_1/PA9` |
| `7` | `GND` |
| `8` | `AREF` |
| `9` | `SDA_0/PA5` |
| `10` | `SCL_0/PA4` |

Nearby companion/breakout nets in the same region also expose `UART0_TX/PA6`, `UART0_RX/PA7`, `TCK/PA0`, `TMS/PA1`, `TDO/PA2`, `TDI/PA3`, `PA14`, and `PA15`.

### XS6 - Arduino/QSPI/PB-Side Header

| XS6 pin | Net |
| --- | --- |
| `1` | `RX_3/PB7` |
| `2` | `TX_3/PB6` |
| `3` | `I2C2_SCL/PB8` |
| `4` | `TIM1_PWM3/PB15` |
| `5` | `VBUS_FLG_USB/PB14` |
| `6` | `TIM1_PWM1/PB13` |
| `7` | `TIM1_PWM0/PB12` |
| `8` | `QSPI1_MOSI/PB2` |

Companion nets in the same PB-side breakout region also expose:

- `QSPI1_CS/PB0`
- `QSPI1_SCK/PB1`
- `QSPI1_MISO/PB3`
- `QSPI1_IO2/PB4`
- `QSPI1_IO3/PB5`
- `I2C2_SDA/PB9`
- `PB10`
- `TUSB_INT/PB11`
- `3V3`
- `GND`

## Partial Connector Recoveries

### XP7 - User LED Link

- Dedicated 2-pin header around `USER_LED/PC0`
- Other side is `GND`
- Exact left/right pin numbering is not clean enough in the extracted text to make authoritative

### XP8 - Power / PC / VIN Companion Header

Visible net set:

- `PC0`..`PC15`
- `3V3`, `5V`, `5V_EXT`, `GND`, `VIN`
- `nRST_U`, `nRST_ALL`
- `VIN<0>`..`VIN<7>`

The connector role is clear, but the exact pin order is not stable enough in the text extraction to encode as a strict pin table yet.

### XS3 - Companion Header Block

Visible nets near the block include:

- `nRST_U`
- `3V3`
- `5V_EXT`
- `GND`
- `VIN`
- nearby PA-side signals from the same connector cluster

The block is clearly part of the Arduino/power-control area, but its exact ordered table is not reliable enough from the extracted text alone.

### XP9 - PA/PC Breakout Companion

Visible net set:

- `TCK/PA0`, `TMS/PA1`, `TDO/PA2`, `TDI/PA3`
- `SCL_0/PA4`, `SDA_0/PA5`
- `UART0_TX/PA6`, `UART0_RX/PA7`
- `CS_1/PA8`, `SCK_1/PA9`, `MOSI_1/PA10`, `MISO_1/PA11`
- `TIM0_PWM0/PA12`, `PA13`, `PA14`, `PA15`
- `USER_LED/PC0`, `PC1`..`PC15`
- `AREF`, `3V3`, `GND`

### XP10 - PB/QSPI Breakout Companion

Visible net set:

- `QSPI1_CS/PB0`, `QSPI1_SCK/PB1`, `QSPI1_MOSI/PB2`, `QSPI1_MISO/PB3`
- `QSPI1_IO2/PB4`, `QSPI1_IO3/PB5`
- `TX_3/PB6`, `RX_3/PB7`
- `I2C2_SCL/PB8`, `I2C2_SDA/PB9`
- `PB10`, `TUSB_INT/PB11`
- `TIM1_PWM0/PB12`, `TIM1_PWM1/PB13`, `VBUS_FLG_USB/PB14`, `TIM1_PWM3/PB15`
- `3V3`, `GND`

## Board-Support Pinmux Groups

The repo board support now encodes these high-confidence EVU-BA groups directly:

- `UART0`: `PA6` / `PA7`
- `I2C0`: `PA4` / `PA5`
- `SPI1`: `PA8` / `PA9` / `PA10` / `PA11`
- `QSPI1`: `PB0`..`PB5`
- user LED / button: `PC0` / `PC13`

CANFD mux groups are encoded from the official connector tables and cross-checked against SDK examples:

- `CANFD0`: `PA14` / `PA15`, AF3
- `CANFD1`: `PB6` / `PB7`, AF2

## Important Limits

- CAN is exposed through multiplexed header pins; the docs do not show a separate dedicated CAN connector in the board-support path used here.
- The PB-side connector region exposes signals that can also be used by `UART3`, `I2C2`, `QSPI1`, and `CANFD1`, so not every possible mux should be forced on by default.
- If we later need a strict production-ready connector table for `XP8`, `XS3`, or `XP9`, the next step should be a visual/manual pass on the original PDF pages rather than relying only on text extraction.
