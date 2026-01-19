/**
 * @file os_config.h
 * @brief RTOS Configuration Header
 * 
 * This file contains all configurable parameters for the RTOS.
 * Modify these values to customize the system for your target platform.
 */

#ifndef OS_CONFIG_H
#define OS_CONFIG_H

/*===========================================================================*/
/* System Configuration                                                       */
/*===========================================================================*/

/** @brief OS Name */
#define OS_NAME                     "RV-AIOS"

/** @brief OS Version */
#define OS_VERSION_MAJOR            1
#define OS_VERSION_MINOR            0
#define OS_VERSION_PATCH            0

/** @brief Target architecture: 32 or 64 bit */
#ifndef OS_CFG_ARCH_BITS
#define OS_CFG_ARCH_BITS            64
#endif

/** @brief CPU clock frequency in Hz */
#define OS_CFG_CPU_FREQ_HZ          100000000UL

/** @brief System tick frequency in Hz */
#define OS_CFG_TICK_FREQ_HZ         1000

/*===========================================================================*/
/* Kernel Configuration                                                       */
/*===========================================================================*/

/** @brief Maximum number of priority levels (1-256) */
#define OS_CFG_PRIO_MAX             32

/** @brief Enable time slicing for equal priority tasks */
#define OS_CFG_TIME_SLICE_EN        1

/** @brief Default time slice in ticks */
#define OS_CFG_TIME_SLICE_TICKS     10

/** @brief Maximum number of tasks */
#define OS_CFG_TASK_MAX             32

/** @brief Idle task stack size in bytes */
#define OS_CFG_IDLE_STACK_SIZE      512

/** @brief Minimum task stack size in bytes */
#define OS_CFG_STACK_SIZE_MIN       256

/** @brief Enable stack overflow detection */
#define OS_CFG_STACK_CHECK_EN       1

/** @brief Stack guard pattern */
#define OS_CFG_STACK_GUARD_PATTERN  0xDEADBEEFUL

/*===========================================================================*/
/* Multi-Core Configuration                                                   */
/*===========================================================================*/

/** @brief Enable SMP support */
#define OS_CFG_SMP_EN               0

/** @brief Maximum number of CPU cores */
#define OS_CFG_CPU_MAX              1

/** @brief Number of active CPU cores */
#define OS_CFG_CPU_COUNT            1

/** @brief Enable AMP mode support */
#define OS_CFG_AMP_EN               1

/** @brief Enable dynamic load balancing */
#define OS_CFG_LOAD_BALANCE_EN      1

/** @brief Load balance interval in ticks */
#define OS_CFG_LOAD_BALANCE_TICKS   100

/*===========================================================================*/
/* Memory Configuration                                                       */
/*===========================================================================*/

/** @brief Enable static memory pools */
#define OS_CFG_MEM_POOL_EN          1

/** @brief Enable dynamic heap allocator */
#define OS_CFG_HEAP_EN              1

/** @brief Heap size in bytes (if enabled) */
#define OS_CFG_HEAP_SIZE            (64 * 1024)

/** @brief Memory pool block sizes */
#define OS_CFG_POOL_SMALL_SIZE      32
#define OS_CFG_POOL_SMALL_COUNT     64

#define OS_CFG_POOL_MEDIUM_SIZE     128
#define OS_CFG_POOL_MEDIUM_COUNT    32

#define OS_CFG_POOL_LARGE_SIZE      512
#define OS_CFG_POOL_LARGE_COUNT     16

/** @brief Enable PMP memory protection */
#define OS_CFG_PMP_EN               1

/** @brief Number of PMP regions available */
#define OS_CFG_PMP_REGIONS          16

/*===========================================================================*/
/* IPC Configuration                                                          */
/*===========================================================================*/

/** @brief Maximum semaphores */
#define OS_CFG_SEM_MAX              32

/** @brief Maximum mutexes */
#define OS_CFG_MUTEX_MAX            32

/** @brief Maximum message queues */
#define OS_CFG_QUEUE_MAX            16

/** @brief Maximum event flag groups */
#define OS_CFG_EVENT_MAX            16

/** @brief Enable priority inheritance for mutexes */
#define OS_CFG_MUTEX_PRIO_INHERIT   1

/*===========================================================================*/
/* Timer Configuration                                                        */
/*===========================================================================*/

/** @brief Maximum software timers */
#define OS_CFG_TIMER_MAX            16

/** @brief Timer task priority */
#define OS_CFG_TIMER_TASK_PRIO      2

/** @brief Timer task stack size */
#define OS_CFG_TIMER_STACK_SIZE     1024

/*===========================================================================*/
/* AI Runtime Configuration                                                   */
/*===========================================================================*/

/** @brief Enable AI runtime */
#define OS_CFG_AI_EN                1

/** @brief AI backend selection */
#define OS_CFG_AI_BACKEND_NATIVE    0
#define OS_CFG_AI_BACKEND_IREE      1

/** @brief AI tensor arena size */
#define OS_CFG_AI_ARENA_SIZE        (128 * 1024)

/** @brief Maximum loaded models */
#define OS_CFG_AI_MODEL_MAX         4

/** @brief Enable INT8 quantized inference */
#define OS_CFG_AI_INT8_EN           1

/** @brief AI task default priority */
#define OS_CFG_AI_TASK_PRIO         128

/** @brief Enable preemptible inference */
#define OS_CFG_AI_PREEMPT_EN        1

/*===========================================================================*/

/** @brief Enable AI runtime */
#define OS_CFG_AI_EN                1

/** @brief AI tensor arena size */
#define OS_CFG_AI_ARENA_SIZE        (128 * 1024)

/** @brief Maximum loaded models */
#define OS_CFG_AI_MODEL_MAX         4

/** @brief Enable INT8 quantized inference */
#define OS_CFG_AI_INT8_EN           1

/** @brief AI task default priority */
#define OS_CFG_AI_TASK_PRIO         128

/** @brief Enable preemptible inference */
#define OS_CFG_AI_PREEMPT_EN        1

/*===========================================================================*/
/* Federated Learning Configuration                                           */
/*===========================================================================*/

/** @brief Enable federated learning */
#define OS_CFG_FL_EN                1

/** @brief Local training batch size */
#define OS_CFG_FL_BATCH_SIZE        16

/** @brief Gradient buffer size */
#define OS_CFG_FL_GRAD_BUF_SIZE     (32 * 1024)

/** @brief Enable gradient compression */
#define OS_CFG_FL_COMPRESS_EN       1

/** @brief TopK sparsification ratio (0.0-1.0) */
#define OS_CFG_FL_TOPK_RATIO        0.1f

/** @brief Enable differential privacy */
#define OS_CFG_FL_DP_EN             1

/** @brief Differential privacy epsilon */
#define OS_CFG_FL_DP_EPSILON        1.0f

/*===========================================================================*/
/* Security Configuration                                                     */
/*===========================================================================*/

/** @brief Enable secure boot */
#define OS_CFG_SECURE_BOOT_EN       1

/** @brief Enable TEE support */
#define OS_CFG_TEE_EN               1

/** @brief Enable TLS support */
#define OS_CFG_TLS_EN               1

/** @brief Maximum key storage entries */
#define OS_CFG_KEY_STORE_MAX        8

/*===========================================================================*/
/* Communication Configuration                                                */
/*===========================================================================*/

/** @brief Enable network stack */
#define OS_CFG_NET_EN               1

/** @brief Enable MQTT client */
#define OS_CFG_MQTT_EN              1

/** @brief Enable CoAP client */
#define OS_CFG_COAP_EN              1

/** @brief Enable BLE support */
#define OS_CFG_BLE_EN               0

/** @brief Maximum network sockets */
#define OS_CFG_SOCKET_MAX           8

/*===========================================================================*/
/* Debug Configuration                                                        */
/*===========================================================================*/

/** @brief Enable debug output */
#define OS_CFG_DEBUG_EN             1

/** @brief Debug output level (0=none, 1=error, 2=warn, 3=info, 4=debug) */
#define OS_CFG_DEBUG_LEVEL          3

/** @brief Enable kernel statistics */
#define OS_CFG_STATS_EN             1

/** @brief Enable trace logging */
#define OS_CFG_TRACE_EN             0

/*===========================================================================*/
/* Platform Specific                                                          */
/*===========================================================================*/

/** @brief UART base address for debug output */
#define OS_CFG_UART_BASE            0x10000000UL

/** @brief CLINT base address */
#define OS_CFG_CLINT_BASE           0x02000000UL

/** @brief PLIC base address */
#define OS_CFG_PLIC_BASE            0x0C000000UL

/** @brief RAM start address */
#define OS_CFG_RAM_BASE             0x80000000UL

/** @brief RAM size */
#define OS_CFG_RAM_SIZE             (1024 * 1024)

/** @brief Flash start address */
#define OS_CFG_FLASH_BASE           0x20000000UL

/** @brief Flash size */
#define OS_CFG_FLASH_SIZE           (2 * 1024 * 1024)

#endif /* OS_CONFIG_H */
