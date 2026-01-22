# ESP32-C3 Platform Configuration

# Set global definitions
add_compile_definitions(CONFIG_PLATFORM_ESP32C3=1)
set(CONFIG_PLATFORM_ESP32C3 1 CACHE INTERNAL "ESP32 Platform Config")

# Platform specific compile options
add_compile_options(
    -march=rv32imc_zicsr
    -mabi=ilp32
    -mcmodel=medany
)

# Set linker script (Relative to Source Dir)
set(LINKER_SCRIPT "hal/src/esp32/esp32c3.ld" CACHE STRING "Linker script path" FORCE)
