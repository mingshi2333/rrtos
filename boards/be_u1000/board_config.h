/**
 * @file board_config.h
 * @brief Board configuration for Baikal Electronics BE-U1000 (EVU-BA boards)
 *
 * BE-U1000 is a 3-core RISC-V RV32IMAFC MCU from Baikal Electronics.
 * Core0/Core1: Main compute cores with 64KB TCMA + 96KB TCMB
 * Core2: Monitor core with 2KB TCMA + 8KB TCMB (limited resources)
 *
 * Interrupt model: CLIC (Core-Local Interrupt Controller) — NOT standard PLIC
 * Timer: Standard CLINT mtime/mtimecmp at 0x02000000
 */

#ifndef BOARD_CONFIG_BE_U1000_H
#define BOARD_CONFIG_BE_U1000_H

/* ============================================================================
 * CPU Configuration
 * ========================================================================= */
#define BE_U1000_NUM_CORES          3       /* Core0, Core1 (main), Core2 (monitor) */
#define BE_U1000_NUM_MAIN_CORES     2       /* Core0 + Core1 for RTOS scheduling */
#define BE_U1000_TIMER_FREQ         1000000 /* 1 MHz mtime clock (F_TIMER_PULSE) */
#define BE_U1000_CPU_FREQ           200000000 /* 200 MHz core clock (typical) */

/* ============================================================================
 * Interrupt Controller — CLIC (NOT PLIC)
 * ========================================================================= */
#define BE_U1000_IRQ_MODEL_CLIC     1
#define BE_U1000_CLIC_NUM_IRQS      77      /* Total CLIC interrupt sources */

/* CLIC CSR addresses (non-standard RISC-V extensions) */
#define CSR_MTVT                    0x307   /* Machine Trap Vector Table Base */
#define CSR_NMITVEC                 0x7C0   /* NMI Trap Vector */
#define MTVEC_MODE_CLIC             0x3     /* mtvec mode bits for CLIC */

/* ============================================================================
 * CLINT — Standard timer + IPI (compatible with RRTOS hal_clint)
 * ========================================================================= */
#define BE_U1000_CLINT_BASE         0x02000000UL

/* ============================================================================
 * Memory Layout — Core0/Core1
 * ========================================================================= */
#define BE_U1000_TCMA_BASE          0x40010000UL
#define BE_U1000_TCMA_SIZE          0x00010000UL  /* 64 KB */
#define BE_U1000_TCMB_BASE          0x40020000UL
#define BE_U1000_TCMB_SIZE          0x00018000UL  /* 96 KB */
#define BE_U1000_SRAM_BASE          0x70000000UL
#define BE_U1000_SRAM_SIZE          0x00008000UL  /* 32 KB */
#define BE_U1000_QSPI0_BASE        0x80000000UL
#define BE_U1000_QSPI0_SIZE        0x01000000UL  /* 16 MB */
#define BE_U1000_QSPI1_BASE        0x90000000UL
#define BE_U1000_QSPI1_SIZE        0x01000000UL  /* 16 MB */
#define BE_U1000_EFLASH_BASE        0xA0000000UL
#define BE_U1000_EFLASH_SIZE        0x00040000UL  /* 256 KB */

/* Memory Layout — Core2 (limited) */
#define BE_U1000_C2_TCMA_BASE       0x40007800UL
#define BE_U1000_C2_TCMA_SIZE       0x00000800UL  /* 2 KB */
#define BE_U1000_C2_TCMB_BASE       0x40008000UL
#define BE_U1000_C2_TCMB_SIZE       0x00002000UL  /* 8 KB */

/* Default RAM/Flash for RTOS (Core0 using TCMA + TCMB) */
#define BE_U1000_RAM_BASE           BE_U1000_TCMA_BASE
#define BE_U1000_RAM_SIZE           (BE_U1000_TCMA_SIZE + BE_U1000_TCMB_SIZE) /* 160 KB */
#define BE_U1000_FLASH_BASE         BE_U1000_EFLASH_BASE
#define BE_U1000_FLASH_SIZE         BE_U1000_EFLASH_SIZE

/* ============================================================================
 * Peripheral Base Addresses
 * ========================================================================= */
#define BE_U1000_PERIPH_BASE        0x11000000UL
#define BE_U1000_APB0_BASE          BE_U1000_PERIPH_BASE
#define BE_U1000_APB1_BASE          (BE_U1000_PERIPH_BASE + 0x02000000UL) /* 0x13000000 */
#define BE_U1000_APB2_BASE          (BE_U1000_PERIPH_BASE + 0x03000000UL) /* 0x14000000 */
#define BE_U1000_AHB_BASE           (BE_U1000_PERIPH_BASE + 0x04000000UL) /* 0x15000000 */

/* APB0 Peripherals */
#define BE_U1000_QSPI0_CTRL_BASE   (BE_U1000_APB0_BASE + 0x0000)
#define BE_U1000_UART0_BASE        (BE_U1000_APB0_BASE + 0x1000)
#define BE_U1000_UART1_BASE        (BE_U1000_APB0_BASE + 0x2000)
#define BE_U1000_UART2_BASE        (BE_U1000_APB0_BASE + 0x3000)
#define BE_U1000_I2C0_BASE         (BE_U1000_APB0_BASE + 0x4000)
#define BE_U1000_I2C1_BASE         (BE_U1000_APB0_BASE + 0x5000)
#define BE_U1000_I2S0_BASE         (BE_U1000_APB0_BASE + 0x6000)
#define BE_U1000_TIM0_BASE         (BE_U1000_APB0_BASE + 0x7000)
#define BE_U1000_WDT0_BASE         (BE_U1000_APB0_BASE + 0x8000)
#define BE_U1000_GPIO0_BASE        (BE_U1000_APB0_BASE + 0x9000)
#define BE_U1000_PWMG0_BASE        (BE_U1000_APB0_BASE + 0xA000)
#define BE_U1000_SPI0_BASE         (BE_U1000_APB0_BASE + 0xB000)
#define BE_U1000_SPI1_BASE         (BE_U1000_APB0_BASE + 0xC000)
#define BE_U1000_CANFD0_BASE       (BE_U1000_APB0_BASE + 0x10000)

/* APB1 Peripherals */
#define BE_U1000_QSPI1_CTRL_BASE   (BE_U1000_APB1_BASE + 0x0000)
#define BE_U1000_UART3_BASE        (BE_U1000_APB1_BASE + 0x1000)
#define BE_U1000_UART4_BASE        (BE_U1000_APB1_BASE + 0x2000)
#define BE_U1000_UART5_BASE        (BE_U1000_APB1_BASE + 0x3000)
#define BE_U1000_I2C2_BASE         (BE_U1000_APB1_BASE + 0x4000)
#define BE_U1000_I2C3_BASE         (BE_U1000_APB1_BASE + 0x5000)
#define BE_U1000_I2S1_BASE         (BE_U1000_APB1_BASE + 0x6000)
#define BE_U1000_TIM1_BASE         (BE_U1000_APB1_BASE + 0x7000)
#define BE_U1000_WDT1_BASE         (BE_U1000_APB1_BASE + 0x8000)
#define BE_U1000_GPIO1_BASE        (BE_U1000_APB1_BASE + 0x9000)
#define BE_U1000_PWMG1_BASE        (BE_U1000_APB1_BASE + 0xA000)
#define BE_U1000_SPI2_BASE         (BE_U1000_APB1_BASE + 0xB000)
#define BE_U1000_SPI3_BASE         (BE_U1000_APB1_BASE + 0xC000)
#define BE_U1000_CANFD1_BASE       (BE_U1000_APB1_BASE + 0x10000)

/* APB2 Peripherals */
#define BE_U1000_CRU_BASE          (BE_U1000_APB2_BASE + 0x0000)
#define BE_U1000_ADC0_BASE         (BE_U1000_APB2_BASE + 0x1000)
#define BE_U1000_ADC1_BASE         (BE_U1000_APB2_BASE + 0x2000)
#define BE_U1000_ADC2_BASE         (BE_U1000_APB2_BASE + 0x3000)
#define BE_U1000_PWMA0_BASE        (BE_U1000_APB2_BASE + 0x4000)
#define BE_U1000_PWMA1_BASE        (BE_U1000_APB2_BASE + 0x5000)
#define BE_U1000_PWMA2_BASE        (BE_U1000_APB2_BASE + 0x6000)
#define BE_U1000_PWMA3_BASE        (BE_U1000_APB2_BASE + 0x7000)
#define BE_U1000_GPIO2_BASE        (BE_U1000_APB2_BASE + 0x8000)
#define BE_U1000_UART6_BASE        (BE_U1000_APB2_BASE + 0x9000)
#define BE_U1000_UART7_BASE        (BE_U1000_APB2_BASE + 0xA000)

/* AHB Peripherals */
#define BE_U1000_USB_BASE          (BE_U1000_AHB_BASE + 0x000000)
#define BE_U1000_DMA0_BASE         (BE_U1000_AHB_BASE + 0x100000)
#define BE_U1000_DMA1_BASE         (BE_U1000_AHB_BASE + 0x101000)
#define BE_U1000_MB0_BASE          (BE_U1000_AHB_BASE + 0x103000)
#define BE_U1000_MB1_BASE          (BE_U1000_AHB_BASE + 0x103020)

/* ============================================================================
 * CLIC Interrupt Numbers (from SDK startup vector table)
 * ========================================================================= */
#define BE_U1000_IRQ_SW             3   /* Software interrupt */
#define BE_U1000_IRQ_TIMER          7   /* Timer interrupt (CLINT mtime) */
#define BE_U1000_IRQ_CLIC_SW        12  /* CLIC software interrupt */
#define BE_U1000_IRQ_QSPI0          13
#define BE_U1000_IRQ_QSPI1          14
#define BE_U1000_IRQ_UART0          17
#define BE_U1000_IRQ_UART1          18
#define BE_U1000_IRQ_UART2          19
#define BE_U1000_IRQ_I2C0           20
#define BE_U1000_IRQ_I2C1           21
#define BE_U1000_IRQ_I2S0           22
#define BE_U1000_IRQ_TIM0_CH0       23
#define BE_U1000_IRQ_TIM0_CH1       24
#define BE_U1000_IRQ_TIM0_CH2       25
#define BE_U1000_IRQ_TIM0_CH3       26
#define BE_U1000_IRQ_WDT0           27
#define BE_U1000_IRQ_GPIO0          28
#define BE_U1000_IRQ_GPIO1          29
#define BE_U1000_IRQ_GPIO2          30
#define BE_U1000_IRQ_PWMG0          31
#define BE_U1000_IRQ_SPI0           32
#define BE_U1000_IRQ_SPI1           33
#define BE_U1000_IRQ_UART3          34
#define BE_U1000_IRQ_UART4          35
#define BE_U1000_IRQ_UART5          36
#define BE_U1000_IRQ_I2C2           37
#define BE_U1000_IRQ_I2C3           38
#define BE_U1000_IRQ_I2S1           39
#define BE_U1000_IRQ_TIM1_CH0       40
#define BE_U1000_IRQ_TIM1_CH1       41
#define BE_U1000_IRQ_TIM1_CH2       42
#define BE_U1000_IRQ_TIM1_CH3       43
#define BE_U1000_IRQ_WDT1           44
#define BE_U1000_IRQ_PWMG1          45
#define BE_U1000_IRQ_SPI2           46
#define BE_U1000_IRQ_SPI3           47
#define BE_U1000_IRQ_CANFD0         48
#define BE_U1000_IRQ_CANFD1         49
#define BE_U1000_IRQ_ADC0           50
#define BE_U1000_IRQ_ADC1           51
#define BE_U1000_IRQ_ADC2           52
#define BE_U1000_IRQ_PWMA0          53
#define BE_U1000_IRQ_PWMA1          54
#define BE_U1000_IRQ_PWMA2          55
#define BE_U1000_IRQ_PWMA3          56
#define BE_U1000_IRQ_QE0            57
#define BE_U1000_IRQ_QE1            58
#define BE_U1000_IRQ_QE2            59
#define BE_U1000_IRQ_QE3            60
#define BE_U1000_IRQ_USB            61
#define BE_U1000_IRQ_UART6          62
#define BE_U1000_IRQ_UART7          63
#define BE_U1000_IRQ_DMA0           64
#define BE_U1000_IRQ_DMA1           65
#define BE_U1000_IRQ_USB_DMA        66
#define BE_U1000_IRQ_CORE2          67
#define BE_U1000_IRQ_MAILBOX        68
#define BE_U1000_IRQ_EXTI           69
#define BE_U1000_IRQ_BUS_ERROR      70
#define BE_U1000_IRQ_PLL_UNLOCK     71

/* ============================================================================
 * Default console UART for RTOS
 * ========================================================================= */
#define BE_U1000_CONSOLE_UART_BASE  BE_U1000_UART0_BASE
#define BE_U1000_CONSOLE_UART_IRQ   BE_U1000_IRQ_UART0
#define BE_U1000_CONSOLE_BAUD       115200

#define BE_U1000_DIAG_GPIO_BASE     BE_U1000_GPIO2_BASE
#define BE_U1000_DIAG_GPIO_PIN      13
#define BE_U1000_DIAG_SPI_BASE      BE_U1000_SPI0_BASE
#define BE_U1000_DIAG_SPI_BAUD_DIV  32
#define BE_U1000_DIAG_I2C_BASE      BE_U1000_I2C0_BASE
#define BE_U1000_DIAG_I2C_BUS_HZ    100000

/* ============================================================================
 * Stack configuration
 * ========================================================================= */
#define BE_U1000_MAIN_STACK_SIZE    4096    /* 4 KB main stack */
#define BE_U1000_ISR_STACK_SIZE     2048    /* 2 KB ISR stack */

#endif /* BOARD_CONFIG_BE_U1000_H */
