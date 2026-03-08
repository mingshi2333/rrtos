#include "os_kernel.h"
#include "riscv_csr.h"
#include "hal_clint.h"
#include "hal_plic.h"
#include "hal_irq.h"
#include "hal_uart.h"

#include "os_config.h"
#if OS_CFG_SMP_EN
#include "os_smp.h"
#endif

extern void os_tick_handler(void);

#if OS_CFG_IRQ_MODEL_CLIC
extern void hal_clic_dispatch(uint32_t irq_num);
#endif

void os_trap_handler(void *sp) {
    (void)sp;
    
    os_reg_t mcause = csr_read(mcause);
    uint32_t cpu = os_cpu_id();
    
    if (mcause & ((os_ubase_t)1 << (OS_CFG_ARCH_BITS - 1))) {
        uint32_t irq = mcause & 0x3FF;
        
        switch (irq) {
            case 7:
                {
                    uint64_t next = hal_clint_mtime_get() + 
                                   (OS_CFG_TIMER_FREQ_HZ / OS_CFG_TICK_FREQ_HZ);
                    hal_clint_mtimecmp_set(cpu, next);
                    os_tick_handler();
                }
                break;
                
            case 3:
#if OS_CFG_SMP_EN
                os_ipi_handler();
#endif
                break;
                
            case 11:
#if !OS_CFG_IRQ_MODEL_CLIC
                {
                    uint32_t ext_irq = hal_plic_claim(cpu);
                    if (ext_irq > 0) {
                        hal_plic_complete(cpu, ext_irq);
                    }
                }
#endif
                break;
                
            default:
#if OS_CFG_IRQ_MODEL_CLIC
                hal_clic_dispatch(irq);
#endif
                break;
        }
    } else {
        os_reg_t mepc = csr_read(mepc);
        os_reg_t mtval = csr_read(mtval);
        os_print("\nEXCEPTION: mcause=0x%x, mepc=0x%x, mtval=0x%x\n", mcause, mepc, mtval);
        while(1);
    }
}
