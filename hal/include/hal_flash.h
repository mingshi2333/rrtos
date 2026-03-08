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

int hal_flash_init(uintptr_t base, uint32_t size);
int hal_flash_read(uint32_t offset, void *buf, size_t len);
int hal_flash_read_u32(uint32_t offset, uint32_t *value);
int hal_flash_get_info(hal_flash_info_t *info);

#endif
