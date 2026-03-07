# BE-U1000 Interrupt Consistency Validation

This checklist validates consistency across board config, simulator model, and runtime boot logs.

## 1) Static map consistency (board vs Renode)

Run:

```bash
pixi run beu1000-irq-consistency
```

Expected:
- `INTERRUPT_CONSISTENCY_OK`
- matching values for CLINT base, UART0 base, timer frequency, and CPU count

## 2) Boot self-check path

Build and run demo:

```bash
pixi run -e be-u1000 configure
pixi run -e be-u1000 build
pixi run -e be-u1000 sim-be_u1000
```

Runtime should include:
- `[BOOT] IRQ model: CLIC`
- `[CHK] CLINT mtime monotonic: OK (...)`
- `[CHK] GPIO init: OK (...)`
- `[CHK] SPI init: OK (...)`
- `[CHK] I2C init: OK (...)`

## 3) Board log verification

After collecting serial log to `logs/be_u1000_boot.log`:

```bash
pixi run beu1000-bootlog-check
```

Expected:
- `BOOT_LOG_CHECK_OK`

## 4) Common mismatch causes

- simulator map and board config diverged (base address, cpu count, timer freq)
- boot image not updated after board config changes
- serial log from old firmware run
- simulator model still using outdated `.repl`
