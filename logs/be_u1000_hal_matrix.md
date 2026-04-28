# BE-U1000 HAL App Matrix

- Generated: 2026-04-28T15:19:53.011654+00:00

| App | Build | Run | Check | UART log | Error |
| --- | --- | --- | --- | --- | --- |
| `demo` | PASS | PASS | PASS | `logs/be_u1000_hal/demo.uart.log` | none |
| `gpio_ledblink` | PASS | PASS | PASS | `logs/be_u1000_hal/gpio_ledblink.uart.log` | none |
| `gpio_inputpolling` | PASS | PASS | PASS | `logs/be_u1000_hal/gpio_inputpolling.uart.log` | none |
| `uart_printf` | PASS | PASS | PASS | `logs/be_u1000_hal/uart_printf.uart.log` | none |
| `canfd_polling` | PASS | PASS | PASS | `logs/be_u1000_hal/canfd_polling.uart.log` | none |
| `qspi_flash` | PASS | PASS | PASS | `logs/be_u1000_hal/qspi_flash.uart.log` | none |
| `i2c_polling` | PASS | PASS | PASS | `logs/be_u1000_hal/i2c_polling.uart.log` | none |
| `tim_timebase` | PASS | PASS | PASS | `logs/be_u1000_hal/tim_timebase.uart.log` | none |
| `adc_single` | PASS | PASS | PASS | `logs/be_u1000_hal/adc_single.uart.log` | none |
| `wdt_heartbeat` | PASS | PASS | PASS | `logs/be_u1000_hal/wdt_heartbeat.uart.log` | none |
| `pwmg_outputcompare` | PASS | PASS | PASS | `logs/be_u1000_hal/pwmg_outputcompare.uart.log` | none |
| `dma_mem2mem` | PASS | PASS | PASS | `logs/be_u1000_hal/dma_mem2mem.uart.log` | none |
| `i2s_tx` | PASS | PASS | PASS | `logs/be_u1000_hal/i2s_tx.uart.log` | none |
| `pwma_timebase` | PASS | PASS | PASS | `logs/be_u1000_hal/pwma_timebase.uart.log` | none |
| `usb_runtime` | PASS | PASS | PASS | `logs/be_u1000_hal/usb_runtime.uart.log` | none |

The matrix builds each BE-U1000 HAL example as a separate `BE_U1000_APP` configuration, runs it under Renode unless disabled, and validates the captured UART log with the app-specific checker.