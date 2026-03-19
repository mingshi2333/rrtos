#ifndef HAL_USB_H
#define HAL_USB_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t function_address;
    bool soft_connect;
    bool high_speed_enable;
    uint8_t intrusb_enable;
} hal_usb_config_t;

typedef enum {
    HAL_USB_EP_VIEW_EP0 = 0,
    HAL_USB_EP_VIEW_TXRX = 1,
} hal_usb_endpoint_view_t;

typedef enum {
    HAL_USB_EP_ROLE_CONTROL = 0,
    HAL_USB_EP_ROLE_TX = 1,
    HAL_USB_EP_ROLE_RX = 2,
    HAL_USB_EP_ROLE_BIDIR = 3,
} hal_usb_endpoint_role_t;

typedef enum {
    HAL_USB_CSR_KIND_CSR0 = 0,
    HAL_USB_CSR_KIND_TXCSR = 1,
    HAL_USB_CSR_KIND_RXCSR = 2,
} hal_usb_endpoint_csr_kind_t;

typedef enum {
    HAL_USB_COUNT_KIND_COUNT0 = 0,
    HAL_USB_COUNT_KIND_RXCOUNT = 1,
} hal_usb_endpoint_count_kind_t;

typedef struct {
    uint8_t function_address;
    uint8_t power;
    uint8_t intrusb;
    uint8_t intrusbe;
    uint8_t configdata;
    uint16_t intrtx;
    uint16_t frame;
    uint8_t index;
} hal_usb_controller_state_t;

typedef struct {
    uint8_t index;
    hal_usb_endpoint_view_t register_view;
    hal_usb_endpoint_role_t role;
    hal_usb_endpoint_csr_kind_t csr_kind;
    hal_usb_endpoint_count_kind_t count_kind;
    bool csr_valid;
    bool count_valid;
    bool ready;
    uint16_t max_packet;
    uint16_t csr_value;
    uint16_t count_value;
    uint16_t txmaxp;
    uint16_t rxmaxp;
    uint16_t csr0;
    uint16_t txcsr;
    uint16_t rxcsr;
    uint16_t count0;
    uint16_t rxcount;
} hal_usb_endpoint_state_t;

typedef struct {
    hal_usb_controller_state_t controller;
    hal_usb_endpoint_state_t endpoint;
} hal_usb_runtime_state_t;

int hal_usb_init(uintptr_t base, const hal_usb_config_t *config);
int hal_usb_set_function_address(uint8_t addr);
int hal_usb_set_soft_connect(bool enable);
int hal_usb_set_index(uint8_t index);
int hal_usb_get_index(uint8_t *value);
int hal_usb_get_controller_state(hal_usb_controller_state_t *state);
int hal_usb_get_intrtx(uint16_t *value);
int hal_usb_get_intrusb(uint8_t *value);
int hal_usb_get_intrusbe(uint8_t *value);
int hal_usb_get_configdata(uint8_t *value);
int hal_usb_get_function_address(uint8_t *value);
int hal_usb_get_frame(uint16_t *value);
int hal_usb_get_power(uint8_t *value);
int hal_usb_get_txmaxp(uint16_t *value);
int hal_usb_get_rxmaxp(uint16_t *value);
int hal_usb_get_csr0(uint16_t *value);
int hal_usb_get_txcsr(uint16_t *value);
int hal_usb_get_rxcsr(uint16_t *value);
int hal_usb_get_count0(uint16_t *value);
int hal_usb_get_rxcount(uint16_t *value);
int hal_usb_get_endpoint_state(hal_usb_endpoint_state_t *state);
int hal_usb_get_runtime_state(hal_usb_runtime_state_t *state);

#endif
