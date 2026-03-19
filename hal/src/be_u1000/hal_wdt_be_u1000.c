#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../include/hal_wdt.h"

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t TORR;
    volatile uint32_t CCVR;
    volatile uint32_t CRR;
    volatile uint32_t STAT;
    volatile uint32_t EOI;
} be_u1000_wdt_t;

static volatile be_u1000_wdt_t *g_wdt;

#define WDT_CR_WDT_EN           (1u << 0)
#define WDT_CR_RMOD             (1u << 1)
#define WDT_CR_RPL_Pos          2u
#define WDT_CR_RPL_Msk          (0x7u << WDT_CR_RPL_Pos)
#define WDT_TORR_TOP_Msk        0xFu
#define WDT_STAT_ACTIVE         (1u << 0)
#define WDT_KEY_RELOAD          0x76u

int hal_wdt_init(uintptr_t base, const hal_wdt_config_t *config)
{
    uint32_t cr;

    if (base == 0 || config == NULL) {
        return -1;
    }

    g_wdt = (volatile be_u1000_wdt_t *)base;
    if (g_wdt == NULL) {
        return -1;
    }

    cr = WDT_CR_WDT_EN | (((uint32_t)config->reset_len << WDT_CR_RPL_Pos) & WDT_CR_RPL_Msk);
    if (config->mode == HAL_WDT_MODE_INTERRUPT) {
        cr |= WDT_CR_RMOD;
    }

    g_wdt->TORR = ((uint32_t)config->timeout & WDT_TORR_TOP_Msk);
    g_wdt->CR = cr;
    g_wdt->CRR = WDT_KEY_RELOAD;
    return 0;
}

int hal_wdt_reload(void)
{
    if (g_wdt == NULL) {
        return -1;
    }

    g_wdt->CRR = WDT_KEY_RELOAD;
    return 0;
}

int hal_wdt_get_counter(uint32_t *value)
{
    if (g_wdt == NULL || value == NULL) {
        return -1;
    }

    *value = g_wdt->CCVR;
    return 0;
}

int hal_wdt_irq_active(bool *active)
{
    if (g_wdt == NULL || active == NULL) {
        return -1;
    }

    *active = (g_wdt->STAT & WDT_STAT_ACTIVE) != 0u;
    return 0;
}
