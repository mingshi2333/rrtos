#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t jedec_id;
    uint32_t capacity_bytes;
    uint32_t sector_size;
    uint32_t page_size;
} hal_flash_info_t;

typedef struct {
    uint32_t pclk1en;
    uint32_t syscr0;
    uint32_t qspienr;
    uint32_t ser;
    uint32_t ctrlr0;
    uint32_t ctrlr1;
    uint32_t spi_ctrlr0;
    uint32_t baudr;
    uint32_t sr;
    uint32_t isr;
    uint32_t risr;
    uint32_t txflr;
    uint32_t rxflr;
    uint32_t jedec_raw;
    uint32_t received;
    uint32_t status;
    uint32_t xip_was_enabled;
} hal_flash_diag_t;

int hal_flash_init(uintptr_t base, uint32_t size);
int hal_flash_read(uint32_t offset, void *buf, size_t len);
int hal_flash_read_u32(uint32_t offset, uint32_t *value);
int hal_flash_get_info(hal_flash_info_t *info);
int hal_flash_get_diag(hal_flash_diag_t *diag);

#endif
