#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../include/hal_usb.h"

static uintptr_t g_usb_base;

#define USB_POWER_HSEN          (1u << 5)
#define USB_POWER_SOFTCONN      (1u << 6)

#define USB_REG_FADDR           0x00u
#define USB_REG_POWER           0x01u
#define USB_REG_INTRTX          0x02u
#define USB_REG_INTRUSB         0x0Au
#define USB_REG_INTRUSBE        0x0Bu
#define USB_REG_FRAME           0x0Cu
#define USB_REG_INDEX           0x0Eu
#define USB_REG_TXMAXP          0x10u
#define USB_REG_RXMAXP          0x14u
#define USB_REG_CSR0            0x12u
#define USB_REG_TXCSR           0x12u
#define USB_REG_RXCSR           0x16u
#define USB_REG_COUNT0          0x18u
#define USB_REG_RXCOUNT         0x18u
#define USB_REG_CONFIGDATA      0x1Fu

static volatile uint16_t *usb_reg16(uint32_t offset)
{
    return (volatile uint16_t *)(g_usb_base + offset);
}

static volatile uint8_t *usb_reg8(uint32_t offset)
{
    return (volatile uint8_t *)(g_usb_base + offset);
}

static hal_usb_endpoint_view_t usb_endpoint_view_for_index(uint8_t index)
{
    return (index == 0u) ? HAL_USB_EP_VIEW_EP0 : HAL_USB_EP_VIEW_TXRX;
}

static hal_usb_endpoint_role_t usb_endpoint_role_for_index(uint8_t index)
{
    switch (index) {
    case 0u:
        return HAL_USB_EP_ROLE_CONTROL;
    case 2u:
        return HAL_USB_EP_ROLE_RX;
    case 1u:
    case 3u:
        return HAL_USB_EP_ROLE_TX;
    default:
        return HAL_USB_EP_ROLE_BIDIR;
    }
}

static void usb_fill_endpoint_semantics(hal_usb_endpoint_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->register_view = usb_endpoint_view_for_index(state->index);
    state->role = usb_endpoint_role_for_index(state->index);
    state->csr_kind = HAL_USB_CSR_KIND_CSR0;
    state->count_kind = HAL_USB_COUNT_KIND_COUNT0;
    state->csr_valid = false;
    state->count_valid = false;
    state->ready = false;
    state->max_packet = state->txmaxp;
    state->csr_value = 0u;
    state->count_value = 0u;

    if (state->index == 0u) {
        state->csr_valid = true;
        state->count_valid = true;
        state->csr_kind = HAL_USB_CSR_KIND_CSR0;
        state->count_kind = HAL_USB_COUNT_KIND_COUNT0;
        state->csr_value = state->csr0;
        state->count_value = state->count0;
        state->ready = ((state->csr0 & 0x0001u) != 0u);
        if (state->rxmaxp != 0u) {
            state->max_packet = state->rxmaxp;
        }
        return;
    }

    if (state->role == HAL_USB_EP_ROLE_RX) {
        state->csr_valid = true;
        state->count_valid = true;
        state->csr_kind = HAL_USB_CSR_KIND_RXCSR;
        state->count_kind = HAL_USB_COUNT_KIND_RXCOUNT;
        state->csr_value = state->rxcsr;
        state->count_value = state->rxcount;
        state->ready = ((state->rxcsr & 0x0001u) != 0u);
        if (state->rxmaxp != 0u) {
            state->max_packet = state->rxmaxp;
        }
        return;
    }

    state->csr_valid = true;
    state->csr_kind = HAL_USB_CSR_KIND_TXCSR;
    state->csr_value = state->txcsr;
    state->ready = ((state->txcsr & 0x0001u) != 0u);
}

int hal_usb_init(uintptr_t base, const hal_usb_config_t *config)
{
    uint8_t power = 0;

    if (base == 0 || config == NULL) {
        return -1;
    }

    g_usb_base = base;
    if (g_usb_base == 0u) {
        return -1;
    }

    if (config->high_speed_enable) {
        power |= USB_POWER_HSEN;
    }
    if (config->soft_connect) {
        power |= USB_POWER_SOFTCONN;
    }

    *usb_reg8(USB_REG_FADDR) = config->function_address & 0x7Fu;
    *usb_reg8(USB_REG_POWER) = power;
    *usb_reg8(USB_REG_INTRUSBE) = config->intrusb_enable;
    return 0;
}

int hal_usb_set_function_address(uint8_t addr)
{
    if (g_usb_base == 0u) {
        return -1;
    }

    *usb_reg8(USB_REG_FADDR) = addr & 0x7Fu;
    return 0;
}

int hal_usb_set_soft_connect(bool enable)
{
    if (g_usb_base == 0u) {
        return -1;
    }

    if (enable) {
        *usb_reg8(USB_REG_POWER) |= USB_POWER_SOFTCONN;
    } else {
        *usb_reg8(USB_REG_POWER) &= (uint8_t)~USB_POWER_SOFTCONN;
    }
    return 0;
}

int hal_usb_set_index(uint8_t index)
{
    if (g_usb_base == 0u) {
        return -1;
    }

    *usb_reg8(USB_REG_INDEX) = index;
    return 0;
}

int hal_usb_get_index(uint8_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg8(USB_REG_INDEX);
    return 0;
}

int hal_usb_get_controller_state(hal_usb_controller_state_t *state)
{
    if (g_usb_base == 0u || state == NULL) {
        return -1;
    }

    state->function_address = *usb_reg8(USB_REG_FADDR) & 0x7Fu;
    state->power = *usb_reg8(USB_REG_POWER);
    state->intrusb = *usb_reg8(USB_REG_INTRUSB);
    state->intrusbe = *usb_reg8(USB_REG_INTRUSBE);
    state->configdata = *usb_reg8(USB_REG_CONFIGDATA);
    state->intrtx = *usb_reg16(USB_REG_INTRTX);
    state->frame = *usb_reg16(USB_REG_FRAME);
    state->index = *usb_reg8(USB_REG_INDEX);
    return 0;
}

int hal_usb_get_intrtx(uint16_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg16(USB_REG_INTRTX);
    return 0;
}

int hal_usb_get_intrusb(uint8_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg8(USB_REG_INTRUSB);
    return 0;
}

int hal_usb_get_intrusbe(uint8_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg8(USB_REG_INTRUSBE);
    return 0;
}

int hal_usb_get_configdata(uint8_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg8(USB_REG_CONFIGDATA);
    return 0;
}

int hal_usb_get_function_address(uint8_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg8(USB_REG_FADDR) & 0x7Fu;
    return 0;
}

int hal_usb_get_frame(uint16_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg16(USB_REG_FRAME);
    return 0;
}

int hal_usb_get_power(uint8_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg8(USB_REG_POWER);
    return 0;
}

int hal_usb_get_txmaxp(uint16_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg16(USB_REG_TXMAXP);
    return 0;
}

int hal_usb_get_rxmaxp(uint16_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg16(USB_REG_RXMAXP);
    return 0;
}

int hal_usb_get_csr0(uint16_t *value)
{
    uint8_t index = 0;

    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    index = *usb_reg8(USB_REG_INDEX);
    if (index == 0u) {
        *value = *usb_reg16(USB_REG_CSR0);
    } else {
        *value = 0u;
    }
    return 0;
}

int hal_usb_get_txcsr(uint16_t *value)
{
    uint8_t index = 0;

    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    index = *usb_reg8(USB_REG_INDEX);
    if (index == 0u) {
        *value = 0u;
    } else {
        *value = *usb_reg16(USB_REG_TXCSR);
    }
    return 0;
}

int hal_usb_get_rxcsr(uint16_t *value)
{
    uint8_t index = 0;

    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    index = *usb_reg8(USB_REG_INDEX);
    if (index == 0u) {
        *value = 0u;
    } else {
        *value = *usb_reg16(USB_REG_RXCSR);
    }
    return 0;
}

int hal_usb_get_count0(uint16_t *value)
{
    uint8_t index = 0;

    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    index = *usb_reg8(USB_REG_INDEX);
    if (index == 0u) {
        *value = *usb_reg16(USB_REG_COUNT0);
    } else {
        *value = 0u;
    }
    return 0;
}

int hal_usb_get_rxcount(uint16_t *value)
{
    if (g_usb_base == 0u || value == NULL) {
        return -1;
    }

    *value = *usb_reg16(USB_REG_RXCOUNT);
    return 0;
}

int hal_usb_get_endpoint_state(hal_usb_endpoint_state_t *state)
{
    uint8_t index = 0;

    if (g_usb_base == 0u || state == NULL) {
        return -1;
    }

    index = *usb_reg8(USB_REG_INDEX);
    state->index = index;
    state->txmaxp = *usb_reg16(USB_REG_TXMAXP);
    state->rxmaxp = *usb_reg16(USB_REG_RXMAXP);
    state->csr0 = 0u;
    state->txcsr = 0u;
    state->rxcsr = 0u;
    state->count0 = 0u;
    state->rxcount = 0u;

    if (index == 0u) {
        state->csr0 = *usb_reg16(USB_REG_CSR0);
        state->count0 = *usb_reg16(USB_REG_COUNT0);
    } else {
        state->txcsr = *usb_reg16(USB_REG_TXCSR);
        state->rxcsr = *usb_reg16(USB_REG_RXCSR);
        state->rxcount = *usb_reg16(USB_REG_RXCOUNT);
    }

    usb_fill_endpoint_semantics(state);

    return 0;
}

int hal_usb_get_runtime_state(hal_usb_runtime_state_t *state)
{
    if (g_usb_base == 0u || state == NULL) {
        return -1;
    }

    if (hal_usb_get_controller_state(&state->controller) != 0) {
        return -1;
    }

    if (hal_usb_get_endpoint_state(&state->endpoint) != 0) {
        return -1;
    }

    return 0;
}
