/**
 * @file os_pmp.h
 * @brief RISC-V PMP (Physical Memory Protection) Manager
 * 
 * Provides memory protection using RISC-V PMP registers:
 * - Region-based access control
 * - Task isolation
 * - Kernel/user space separation
 * - Secure memory regions for keys/secrets
 */

#ifndef OS_PMP_H
#define OS_PMP_H

#include "os_types.h"

#if OS_CFG_PMP_EN

/*===========================================================================*/
/* PMP Configuration                                                          */
/*===========================================================================*/

/** Maximum PMP entries (architecture dependent) */
#define OS_PMP_MAX_ENTRIES      OS_CFG_PMP_REGIONS

/*===========================================================================*/
/* PMP Permission Flags                                                       */
/*===========================================================================*/

/** PMP permission bits */
typedef enum {
    OS_PMP_R    = (1 << 0),     /**< Read permission */
    OS_PMP_W    = (1 << 1),     /**< Write permission */
    OS_PMP_X    = (1 << 2),     /**< Execute permission */
    OS_PMP_L    = (1 << 7),     /**< Lock bit - region locked until reset */
} os_pmp_perm_t;

/** Common permission combinations */
#define OS_PMP_RO       (OS_PMP_R)
#define OS_PMP_RW       (OS_PMP_R | OS_PMP_W)
#define OS_PMP_RX       (OS_PMP_R | OS_PMP_X)
#define OS_PMP_RWX      (OS_PMP_R | OS_PMP_W | OS_PMP_X)
#define OS_PMP_NONE     (0)

/*===========================================================================*/
/* PMP Address Matching Modes                                                 */
/*===========================================================================*/

/** PMP address matching modes (A field) */
typedef enum {
    OS_PMP_OFF      = 0,        /**< Null region (disabled) */
    OS_PMP_TOR      = 1,        /**< Top of Range */
    OS_PMP_NA4      = 2,        /**< Naturally aligned 4-byte region */
    OS_PMP_NAPOT    = 3,        /**< Naturally aligned power-of-2 region */
} os_pmp_mode_t;

/*===========================================================================*/
/* PMP Region Types                                                           */
/*===========================================================================*/

/** Predefined region types for common use cases */
typedef enum {
    OS_PMP_TYPE_KERNEL_CODE,    /**< Kernel code (RX) */
    OS_PMP_TYPE_KERNEL_DATA,    /**< Kernel data (RW) */
    OS_PMP_TYPE_KERNEL_RODATA,  /**< Kernel read-only data (R) */
    OS_PMP_TYPE_USER_CODE,      /**< User code (RX) */
    OS_PMP_TYPE_USER_DATA,      /**< User data (RW) */
    OS_PMP_TYPE_STACK,          /**< Task stack (RW) */
    OS_PMP_TYPE_PERIPHERAL,     /**< Memory-mapped I/O (RW, no cache) */
    OS_PMP_TYPE_SECURE,         /**< Secure region (no access from U-mode) */
    OS_PMP_TYPE_SHARED,         /**< Shared memory (RW) */
    OS_PMP_TYPE_DMA,            /**< DMA buffer (RW, no cache) */
} os_pmp_type_t;

/*===========================================================================*/
/* PMP Region Descriptor                                                      */
/*===========================================================================*/

/**
 * @brief PMP region descriptor
 */
typedef struct os_pmp_region {
    os_ubase_t      base;       /**< Base address */
    os_size_t       size;       /**< Region size (must be power of 2) */
    uint8_t         perms;      /**< Permission flags */
    os_pmp_mode_t   mode;       /**< Address matching mode */
    os_pmp_type_t   type;       /**< Region type */
    bool            active;     /**< Region is active */
    const char      *name;      /**< Region name (debug) */
} os_pmp_region_t;

/*===========================================================================*/
/* PMP Task Context                                                           */
/*===========================================================================*/

/**
 * @brief Per-task PMP configuration
 * 
 * Allows each task to have different memory access permissions.
 * Context is switched when task switches.
 */
typedef struct os_pmp_context {
    os_pmp_region_t regions[OS_PMP_MAX_ENTRIES];
    uint8_t         num_regions;
    bool            valid;
} os_pmp_context_t;

/*===========================================================================*/
/* Function Prototypes                                                        */
/*===========================================================================*/

/**
 * @brief Initialize PMP subsystem
 * 
 * Sets up default kernel protection and clears all regions.
 */
void os_pmp_init(void);

/**
 * @brief Configure a PMP region
 * 
 * @param entry     PMP entry index (0 to OS_PMP_MAX_ENTRIES-1)
 * @param base      Region base address
 * @param size      Region size (must be power of 2 for NAPOT)
 * @param perms     Permission flags (OS_PMP_R/W/X)
 * @param mode      Address matching mode
 * @return OS_EOK on success
 */
os_err_t os_pmp_set_region(uint8_t entry, os_ubase_t base, os_size_t size,
                           uint8_t perms, os_pmp_mode_t mode);

/**
 * @brief Configure a PMP region with type
 * 
 * @param entry     PMP entry index
 * @param base      Region base address
 * @param size      Region size
 * @param type      Region type (determines permissions)
 * @return OS_EOK on success
 */
os_err_t os_pmp_set_region_type(uint8_t entry, os_ubase_t base, 
                                 os_size_t size, os_pmp_type_t type);

/**
 * @brief Clear a PMP region
 * 
 * @param entry     PMP entry index
 * @return OS_EOK on success
 */
os_err_t os_pmp_clear_region(uint8_t entry);

/**
 * @brief Clear all PMP regions
 */
void os_pmp_clear_all(void);

/**
 * @brief Lock a PMP region
 * 
 * Once locked, region cannot be modified until reset.
 * 
 * @param entry     PMP entry index
 * @return OS_EOK on success
 */
os_err_t os_pmp_lock_region(uint8_t entry);

/**
 * @brief Check if address is accessible with given permissions
 * 
 * @param addr      Address to check
 * @param perms     Required permissions
 * @return true if accessible
 */
bool os_pmp_check_access(os_ubase_t addr, uint8_t perms);

/**
 * @brief Get current PMP configuration for an entry
 * 
 * @param entry     PMP entry index
 * @param region    Output region descriptor
 * @return OS_EOK on success
 */
os_err_t os_pmp_get_region(uint8_t entry, os_pmp_region_t *region);

/**
 * @brief Setup default kernel PMP configuration
 * 
 * Configures PMP for kernel mode:
 * - Kernel code: RX
 * - Kernel data: RW
 * - Peripheral space: RW
 * - Everything else: no access from U-mode
 */
void os_pmp_setup_kernel(void);

/**
 * @brief Create PMP context for a task
 * 
 * @param ctx       Context to initialize
 * @return OS_EOK on success
 */
os_err_t os_pmp_context_init(os_pmp_context_t *ctx);

/**
 * @brief Add region to task PMP context
 * 
 * @param ctx       Task PMP context
 * @param base      Region base address
 * @param size      Region size
 * @param perms     Permissions
 * @return OS_EOK on success
 */
os_err_t os_pmp_context_add_region(os_pmp_context_t *ctx, os_ubase_t base,
                                    os_size_t size, uint8_t perms);

/**
 * @brief Switch to task's PMP context
 * 
 * Called during context switch to load task-specific PMP config.
 * 
 * @param ctx       Task PMP context (NULL for kernel context)
 */
void os_pmp_context_switch(os_pmp_context_t *ctx);

/**
 * @brief Protect task stack with PMP
 * 
 * Sets up guard regions around stack to detect overflow.
 * 
 * @param stack_base    Stack base address
 * @param stack_size    Stack size
 * @return PMP entry used, or -1 on error
 */
int os_pmp_protect_stack(os_ubase_t stack_base, os_size_t stack_size);

/**
 * @brief Create secure memory region
 * 
 * Region is only accessible from M-mode (not U/S-mode).
 * Used for keys, secrets, secure computation.
 * 
 * @param base      Region base
 * @param size      Region size
 * @param lock      Lock region permanently
 * @return PMP entry used, or -1 on error
 */
int os_pmp_create_secure_region(os_ubase_t base, os_size_t size, bool lock);

/**
 * @brief Dump PMP configuration (debug)
 */
void os_pmp_dump(void);

/*===========================================================================*/
/* Inline Helpers                                                             */
/*===========================================================================*/

/**
 * @brief Check if size is valid for NAPOT mode
 */
static inline bool os_pmp_size_valid(os_size_t size) {
    /* Must be power of 2 and >= 4 bytes */
    return size >= 4 && (size & (size - 1)) == 0;
}

/**
 * @brief Calculate NAPOT address encoding
 */
static inline os_ubase_t os_pmp_napot_addr(os_ubase_t base, os_size_t size) {
    /* NAPOT encoding: addr[XLEN-1:2] with low bits encoding size */
    os_ubase_t napot = (base >> 2) | ((size >> 3) - 1);
    return napot;
}

#endif /* OS_CFG_PMP_EN */

#endif /* OS_PMP_H */
