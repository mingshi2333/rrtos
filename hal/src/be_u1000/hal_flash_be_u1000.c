#include <stddef.h>
#include <stdint.h>

#include "hal_flash.h"

static volatile const uint8_t *g_flash_base;
static uint32_t g_flash_size;

#define FLASH_INFO_JEDEC_OFFSET  0x10u
#define FLASH_INFO_PAGE_OFFSET   0x14u
#define FLASH_INFO_SECTOR_OFFSET 0x18u
#define FLASH_INFO_SIZE_OFFSET   0x1Cu

static int hal_flash_range_valid(uint32_t offset, size_t len)
{
    if (g_flash_base == 0) {
        return -1;
    }

    if (offset > g_flash_size) {
        return -1;
    }

    if (len > (size_t)(g_flash_size - offset)) {
        return -1;
    }

    return 0;
}

int hal_flash_init(uintptr_t base, uint32_t size)
{
    if (base == 0 || size == 0) {
        return -1;
    }

    g_flash_base = (volatile const uint8_t *)base;
    g_flash_size = size;
    return 0;
}

int hal_flash_read(uint32_t offset, void *buf, size_t len)
{
    volatile const uint8_t *src;
    uint8_t *dst;
    size_t i;

    if (len == 0u) {
        return 0;
    }

    if (buf == 0) {
        return -1;
    }

    if (hal_flash_range_valid(offset, len) != 0) {
        return -1;
    }

    src = g_flash_base + offset;
    dst = (uint8_t *)buf;

    for (i = 0; i < len; ++i) {
        dst[i] = src[i];
    }

    return 0;
}

int hal_flash_read_u32(uint32_t offset, uint32_t *value)
{
    if (value == 0) {
        return -1;
    }

    return hal_flash_read(offset, value, sizeof(*value));
}

int hal_flash_get_info(hal_flash_info_t *info)
{
    if (info == 0) {
        return -1;
    }

    if (hal_flash_read_u32(FLASH_INFO_JEDEC_OFFSET, &info->jedec_id) != 0 ||
        hal_flash_read_u32(FLASH_INFO_PAGE_OFFSET, &info->page_size) != 0 ||
        hal_flash_read_u32(FLASH_INFO_SECTOR_OFFSET, &info->sector_size) != 0 ||
        hal_flash_read_u32(FLASH_INFO_SIZE_OFFSET, &info->capacity_bytes) != 0) {
        return -1;
    }

    if (info->jedec_id == 0u ||
        info->page_size == 0u ||
        info->sector_size == 0u ||
        info->capacity_bytes == 0u) {
        return -1;
    }

    return 0;
}
