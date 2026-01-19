/**
 * @file os_pmp.c
 * @brief RISC-V PMP Implementation
 * 
 * Implements Physical Memory Protection for:
 * - Kernel/user isolation
 * - Task memory isolation
 * - Secure memory regions
 */

#include "os_pmp.h"
#include "riscv_csr.h"
#include "riscv_atomic.h"

#if OS_CFG_PMP_EN

/*===========================================================================*/
/* PMP CSR Definitions                                                        */
/*===========================================================================*/

/* PMP configuration register field positions */
#define PMPCFG_R        (1 << 0)
#define PMPCFG_W        (1 << 1)
#define PMPCFG_X        (1 << 2)
#define PMPCFG_A_SHIFT  3
#define PMPCFG_A_MASK   (0x3 << PMPCFG_A_SHIFT)
#define PMPCFG_L        (1 << 7)

/*===========================================================================*/
/* Private Data                                                               */
/*===========================================================================*/

/** Current PMP region configuration */
static os_pmp_region_t g_pmp_regions[OS_PMP_MAX_ENTRIES];

/** Kernel PMP context (default) */
static os_pmp_context_t g_kernel_pmp_ctx;

/** Lock for PMP operations */
static os_spinlock_t g_pmp_lock = OS_SPINLOCK_INIT;

/** Number of available PMP entries (detected at runtime) */
static uint8_t g_pmp_num_entries;

/*===========================================================================*/
/* CSR Access Functions                                                       */
/*===========================================================================*/

/**
 * @brief Write PMP address register
 */
static void pmp_write_addr(uint8_t entry, os_ubase_t addr) {
    switch (entry) {
        case 0:  csr_write(pmpaddr0, addr); break;
        case 1:  csr_write(pmpaddr1, addr); break;
        case 2:  csr_write(pmpaddr2, addr); break;
        case 3:  csr_write(pmpaddr3, addr); break;
        case 4:  csr_write(pmpaddr4, addr); break;
        case 5:  csr_write(pmpaddr5, addr); break;
        case 6:  csr_write(pmpaddr6, addr); break;
        case 7:  csr_write(pmpaddr7, addr); break;
#if OS_PMP_MAX_ENTRIES > 8
        case 8:  csr_write(pmpaddr8, addr); break;
        case 9:  csr_write(pmpaddr9, addr); break;
        case 10: csr_write(pmpaddr10, addr); break;
        case 11: csr_write(pmpaddr11, addr); break;
        case 12: csr_write(pmpaddr12, addr); break;
        case 13: csr_write(pmpaddr13, addr); break;
        case 14: csr_write(pmpaddr14, addr); break;
        case 15: csr_write(pmpaddr15, addr); break;
#endif
        default: break;
    }
}

/**
 * @brief Read PMP address register
 */
static os_ubase_t pmp_read_addr(uint8_t entry) {
    os_ubase_t addr = 0;
    switch (entry) {
        case 0:  addr = csr_read(pmpaddr0); break;
        case 1:  addr = csr_read(pmpaddr1); break;
        case 2:  addr = csr_read(pmpaddr2); break;
        case 3:  addr = csr_read(pmpaddr3); break;
        case 4:  addr = csr_read(pmpaddr4); break;
        case 5:  addr = csr_read(pmpaddr5); break;
        case 6:  addr = csr_read(pmpaddr6); break;
        case 7:  addr = csr_read(pmpaddr7); break;
#if OS_PMP_MAX_ENTRIES > 8
        case 8:  addr = csr_read(pmpaddr8); break;
        case 9:  addr = csr_read(pmpaddr9); break;
        case 10: addr = csr_read(pmpaddr10); break;
        case 11: addr = csr_read(pmpaddr11); break;
        case 12: addr = csr_read(pmpaddr12); break;
        case 13: addr = csr_read(pmpaddr13); break;
        case 14: addr = csr_read(pmpaddr14); break;
        case 15: addr = csr_read(pmpaddr15); break;
#endif
        default: break;
    }
    return addr;
}

/**
 * @brief Write PMP configuration byte
 */
static void pmp_write_cfg(uint8_t entry, uint8_t cfg) {
    /* pmpcfg registers pack 4 (RV32) or 8 (RV64) entries */
#if OS_CFG_ARCH_BITS == 64
    uint8_t reg = entry / 8;
    uint8_t idx = entry % 8;
    os_ubase_t shift = idx * 8;
    os_ubase_t mask = ~(0xFFUL << shift);
    os_ubase_t val;
    
    switch (reg) {
        case 0:
            val = csr_read(pmpcfg0);
            val = (val & mask) | ((os_ubase_t)cfg << shift);
            csr_write(pmpcfg0, val);
            break;
        case 1:
            val = csr_read(pmpcfg2);
            val = (val & mask) | ((os_ubase_t)cfg << shift);
            csr_write(pmpcfg2, val);
            break;
        default:
            break;
    }
#else
    uint8_t reg = entry / 4;
    uint8_t idx = entry % 4;
    uint32_t shift = idx * 8;
    uint32_t mask = ~(0xFFU << shift);
    uint32_t val;
    
    switch (reg) {
        case 0:
            val = csr_read(pmpcfg0);
            val = (val & mask) | ((uint32_t)cfg << shift);
            csr_write(pmpcfg0, val);
            break;
        case 1:
            val = csr_read(pmpcfg1);
            val = (val & mask) | ((uint32_t)cfg << shift);
            csr_write(pmpcfg1, val);
            break;
        case 2:
            val = csr_read(pmpcfg2);
            val = (val & mask) | ((uint32_t)cfg << shift);
            csr_write(pmpcfg2, val);
            break;
        case 3:
            val = csr_read(pmpcfg3);
            val = (val & mask) | ((uint32_t)cfg << shift);
            csr_write(pmpcfg3, val);
            break;
        default:
            break;
    }
#endif
}

/**
 * @brief Read PMP configuration byte
 */
static uint8_t pmp_read_cfg(uint8_t entry) {
#if OS_CFG_ARCH_BITS == 64
    uint8_t reg = entry / 8;
    uint8_t idx = entry % 8;
    os_ubase_t val;
    
    switch (reg) {
        case 0: val = csr_read(pmpcfg0); break;
        case 1: val = csr_read(pmpcfg2); break;
        default: return 0;
    }
    
    return (val >> (idx * 8)) & 0xFF;
#else
    uint8_t reg = entry / 4;
    uint8_t idx = entry % 4;
    uint32_t val;
    
    switch (reg) {
        case 0: val = csr_read(pmpcfg0); break;
        case 1: val = csr_read(pmpcfg1); break;
        case 2: val = csr_read(pmpcfg2); break;
        case 3: val = csr_read(pmpcfg3); break;
        default: return 0;
    }
    
    return (val >> (idx * 8)) & 0xFF;
#endif
}

/**
 * @brief Detect number of PMP entries
 */
static uint8_t pmp_detect_entries(void) {
    uint8_t count = 0;
    os_ubase_t test_val = 0xDEADBEEFUL;
    
    for (uint8_t i = 0; i < OS_PMP_MAX_ENTRIES; i++) {
        os_ubase_t orig = pmp_read_addr(i);
        pmp_write_addr(i, test_val);
        os_ubase_t readback = pmp_read_addr(i);
        pmp_write_addr(i, orig);
        
        if (readback == test_val) {
            count++;
        } else {
            break;
        }
    }
    
    return count;
}

/*===========================================================================*/
/* Public API                                                                 */
/*===========================================================================*/

void os_pmp_init(void) {
    os_spinlock_init(&g_pmp_lock);
    
    /* Detect available PMP entries */
    g_pmp_num_entries = pmp_detect_entries();
    if (g_pmp_num_entries > OS_PMP_MAX_ENTRIES) {
        g_pmp_num_entries = OS_PMP_MAX_ENTRIES;
    }
    
    /* Clear all regions */
    os_pmp_clear_all();
    
    /* Initialize kernel context */
    os_pmp_context_init(&g_kernel_pmp_ctx);
}

os_err_t os_pmp_set_region(uint8_t entry, os_ubase_t base, os_size_t size,
                           uint8_t perms, os_pmp_mode_t mode) {
    if (entry >= g_pmp_num_entries) {
        return OS_EINVAL;
    }
    
    /* Check if locked */
    if (pmp_read_cfg(entry) & PMPCFG_L) {
        return OS_EPERM;
    }
    
    /* Validate NAPOT size */
    if (mode == OS_PMP_NAPOT && !os_pmp_size_valid(size)) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_pmp_lock);
    
    /* Calculate address encoding based on mode */
    os_ubase_t pmp_addr;
    switch (mode) {
        case OS_PMP_TOR:
            pmp_addr = (base + size) >> 2;
            break;
        case OS_PMP_NA4:
            pmp_addr = base >> 2;
            break;
        case OS_PMP_NAPOT:
            pmp_addr = os_pmp_napot_addr(base, size);
            break;
        default:
            pmp_addr = 0;
            break;
    }
    
    /* Build config byte */
    uint8_t cfg = (perms & 0x7) | ((mode & 0x3) << PMPCFG_A_SHIFT);
    
    /* Write to CSRs */
    pmp_write_addr(entry, pmp_addr);
    pmp_write_cfg(entry, cfg);
    
    /* Update tracking structure */
    g_pmp_regions[entry].base = base;
    g_pmp_regions[entry].size = size;
    g_pmp_regions[entry].perms = perms;
    g_pmp_regions[entry].mode = mode;
    g_pmp_regions[entry].active = true;
    
    os_spinlock_irq_restore(&g_pmp_lock, flags);
    
    /* SFENCE to ensure PMP takes effect */
    __asm__ volatile("sfence.vma" ::: "memory");
    
    return OS_EOK;
}

os_err_t os_pmp_set_region_type(uint8_t entry, os_ubase_t base, 
                                 os_size_t size, os_pmp_type_t type) {
    uint8_t perms;
    
    switch (type) {
        case OS_PMP_TYPE_KERNEL_CODE:
        case OS_PMP_TYPE_USER_CODE:
            perms = OS_PMP_RX;
            break;
        case OS_PMP_TYPE_KERNEL_DATA:
        case OS_PMP_TYPE_USER_DATA:
        case OS_PMP_TYPE_STACK:
        case OS_PMP_TYPE_SHARED:
        case OS_PMP_TYPE_DMA:
            perms = OS_PMP_RW;
            break;
        case OS_PMP_TYPE_KERNEL_RODATA:
            perms = OS_PMP_RO;
            break;
        case OS_PMP_TYPE_PERIPHERAL:
            perms = OS_PMP_RW;
            break;
        case OS_PMP_TYPE_SECURE:
            perms = OS_PMP_NONE;  /* No access from U/S mode */
            break;
        default:
            return OS_EINVAL;
    }
    
    os_err_t err = os_pmp_set_region(entry, base, size, perms, OS_PMP_NAPOT);
    if (err == OS_EOK) {
        g_pmp_regions[entry].type = type;
    }
    return err;
}

os_err_t os_pmp_clear_region(uint8_t entry) {
    if (entry >= g_pmp_num_entries) {
        return OS_EINVAL;
    }
    
    if (pmp_read_cfg(entry) & PMPCFG_L) {
        return OS_EPERM;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_pmp_lock);
    
    pmp_write_addr(entry, 0);
    pmp_write_cfg(entry, 0);
    
    g_pmp_regions[entry].active = false;
    g_pmp_regions[entry].base = 0;
    g_pmp_regions[entry].size = 0;
    g_pmp_regions[entry].perms = 0;
    
    os_spinlock_irq_restore(&g_pmp_lock, flags);
    
    return OS_EOK;
}

void os_pmp_clear_all(void) {
    for (uint8_t i = 0; i < g_pmp_num_entries; i++) {
        /* Skip locked entries */
        if (!(pmp_read_cfg(i) & PMPCFG_L)) {
            pmp_write_addr(i, 0);
            pmp_write_cfg(i, 0);
            g_pmp_regions[i].active = false;
        }
    }
    
    __asm__ volatile("sfence.vma" ::: "memory");
}

os_err_t os_pmp_lock_region(uint8_t entry) {
    if (entry >= g_pmp_num_entries) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_pmp_lock);
    
    uint8_t cfg = pmp_read_cfg(entry);
    cfg |= PMPCFG_L;
    pmp_write_cfg(entry, cfg);
    
    os_spinlock_irq_restore(&g_pmp_lock, flags);
    
    return OS_EOK;
}

bool os_pmp_check_access(os_ubase_t addr, uint8_t perms) {
    /* Check each active region */
    for (uint8_t i = 0; i < g_pmp_num_entries; i++) {
        if (!g_pmp_regions[i].active) continue;
        
        os_ubase_t base = g_pmp_regions[i].base;
        os_size_t size = g_pmp_regions[i].size;
        
        if (addr >= base && addr < base + size) {
            /* Address is in this region - check permissions */
            return (g_pmp_regions[i].perms & perms) == perms;
        }
    }
    
    /* No matching region - M-mode has full access, others denied */
    return false;
}

os_err_t os_pmp_get_region(uint8_t entry, os_pmp_region_t *region) {
    if (entry >= g_pmp_num_entries || !region) {
        return OS_EINVAL;
    }
    
    *region = g_pmp_regions[entry];
    return OS_EOK;
}

void os_pmp_setup_kernel(void) {
    uint8_t entry = 0;
    
    /* Entry 0: Kernel code - RX */
    extern uint8_t _stext[], _etext[];
    os_ubase_t kcode_base = (os_ubase_t)_stext;
    os_size_t kcode_size = (os_ubase_t)_etext - kcode_base;
    /* Round up to power of 2 */
    os_size_t pmp_size = 1;
    while (pmp_size < kcode_size) pmp_size <<= 1;
    os_pmp_set_region_type(entry++, kcode_base, pmp_size, OS_PMP_TYPE_KERNEL_CODE);
    
    /* Entry 1: Kernel data - RW */
    extern uint8_t _sdata[], _end[];
    os_ubase_t kdata_base = (os_ubase_t)_sdata;
    os_size_t kdata_size = (os_ubase_t)_end - kdata_base;
    pmp_size = 1;
    while (pmp_size < kdata_size) pmp_size <<= 1;
    os_pmp_set_region_type(entry++, kdata_base, pmp_size, OS_PMP_TYPE_KERNEL_DATA);
    
    /* Entry 2: RAM - RW for all kernel operations */
    os_pmp_set_region_type(entry++, OS_CFG_RAM_BASE, OS_CFG_RAM_SIZE, 
                           OS_PMP_TYPE_KERNEL_DATA);
    
    /* Entry 3: Peripheral space - RW */
    os_pmp_set_region(entry++, 0x00000000, 0x80000000, OS_PMP_RW, OS_PMP_TOR);
    
    /* Entry 4-15: Available for user tasks */
}

os_err_t os_pmp_context_init(os_pmp_context_t *ctx) {
    if (!ctx) {
        return OS_EINVAL;
    }
    
    ctx->num_regions = 0;
    ctx->valid = false;
    
    for (uint8_t i = 0; i < OS_PMP_MAX_ENTRIES; i++) {
        ctx->regions[i].active = false;
    }
    
    return OS_EOK;
}

os_err_t os_pmp_context_add_region(os_pmp_context_t *ctx, os_ubase_t base,
                                    os_size_t size, uint8_t perms) {
    if (!ctx || ctx->num_regions >= OS_PMP_MAX_ENTRIES) {
        return OS_ENOMEM;
    }
    
    uint8_t idx = ctx->num_regions;
    ctx->regions[idx].base = base;
    ctx->regions[idx].size = size;
    ctx->regions[idx].perms = perms;
    ctx->regions[idx].mode = OS_PMP_NAPOT;
    ctx->regions[idx].active = true;
    ctx->num_regions++;
    ctx->valid = true;
    
    return OS_EOK;
}

void os_pmp_context_switch(os_pmp_context_t *ctx) {
    if (!ctx || !ctx->valid) {
        /* Restore kernel context */
        return;
    }
    
    /* Load task's PMP configuration */
    /* Note: First few entries are reserved for kernel, start from entry 4 */
    uint8_t start_entry = 4;
    
    for (uint8_t i = 0; i < ctx->num_regions && (start_entry + i) < g_pmp_num_entries; i++) {
        if (ctx->regions[i].active) {
            os_pmp_set_region(start_entry + i,
                             ctx->regions[i].base,
                             ctx->regions[i].size,
                             ctx->regions[i].perms,
                             ctx->regions[i].mode);
        } else {
            os_pmp_clear_region(start_entry + i);
        }
    }
    
    __asm__ volatile("sfence.vma" ::: "memory");
}

int os_pmp_protect_stack(os_ubase_t stack_base, os_size_t stack_size) {
    /* Find a free PMP entry */
    for (uint8_t i = 4; i < g_pmp_num_entries; i++) {
        if (!g_pmp_regions[i].active) {
            /* Set stack region with RW permissions */
            os_err_t err = os_pmp_set_region_type(i, stack_base, stack_size,
                                                   OS_PMP_TYPE_STACK);
            if (err == OS_EOK) {
                g_pmp_regions[i].name = "stack";
                return i;
            }
        }
    }
    return -1;
}

int os_pmp_create_secure_region(os_ubase_t base, os_size_t size, bool lock) {
    /* Find a free PMP entry */
    for (uint8_t i = 0; i < g_pmp_num_entries; i++) {
        if (!g_pmp_regions[i].active) {
            /* Set secure region - no permissions (M-mode only access) */
            os_err_t err = os_pmp_set_region(i, base, size, OS_PMP_NONE, OS_PMP_NAPOT);
            if (err == OS_EOK) {
                g_pmp_regions[i].type = OS_PMP_TYPE_SECURE;
                g_pmp_regions[i].name = "secure";
                
                if (lock) {
                    os_pmp_lock_region(i);
                }
                return i;
            }
        }
    }
    return -1;
}

void os_pmp_dump(void) {
    /* Debug function - prints PMP configuration */
    /* Would use os_printf if available */
    for (uint8_t i = 0; i < g_pmp_num_entries; i++) {
        if (g_pmp_regions[i].active) {
            /* Print region info */
            /* os_printf("PMP[%d]: 0x%lx - 0x%lx, perms=%d, mode=%d\n",
                      i, g_pmp_regions[i].base,
                      g_pmp_regions[i].base + g_pmp_regions[i].size,
                      g_pmp_regions[i].perms, g_pmp_regions[i].mode); */
        }
    }
}

#endif /* OS_CFG_PMP_EN */
