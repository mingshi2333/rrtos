#include <stddef.h>
#include <stdint.h>

#include "../../include/hal_dma.h"

static uintptr_t g_dma_base;
static hal_dma_channel_t g_channel;

#define DMA_MSC_CFG_EN                 (1u << 0)
#define DMA_MSC_CH_EN_CH0_Pos          0u
#define DMA_MSC_CH_EN_WE_CH0_Pos       8u

#define DMA_CH_CTL0_INT_EN             (1u << 0)
#define DMA_CH_CTL0_DTW_8              (0u << 1)
#define DMA_CH_CTL0_STW_8              (0u << 4)
#define DMA_CH_CTL0_DINC_INC           (0u << 7)
#define DMA_CH_CTL0_SINC_INC           (0u << 9)
#define DMA_CH_CTL0_DST_MSIZE_4        (1u << 11)
#define DMA_CH_CTL0_SRC_MSIZE_4        (1u << 14)
#define DMA_CH_CTL0_TT_FC_M2M          (0u << 20)

#define DMA_CH_CTL1_DONE               (1u << 12)

#define DMA_CH_CFG0_HS_SEL_DST_SW      (1u << 10)
#define DMA_CH_CFG0_HS_SEL_SRC_SW      (1u << 11)
#define DMA_CH_CFG0_PRIOR_7            (7u << 5)

#define DMA_CH_CFG1_PROTCTL_1          (1u << 2)
#define DMA_CH_CFG1_FIFO_MODE_0        (0u << 1)

#define DMA_CH_STRIDE                  0x58u
#define DMA_CH_SAR                     0x00u
#define DMA_CH_DAR                     0x08u
#define DMA_CH_LLP                     0x10u
#define DMA_CH_CTL0                    0x18u
#define DMA_CH_CTL1                    0x1Cu
#define DMA_CH_CFG0                    0x40u
#define DMA_CH_CFG1                    0x44u

#define DMA_INT_STAT_TFR               0x2E8u
#define DMA_INT_CLR_TFR                0x338u
#define DMA_MSC_CFG                    0x398u
#define DMA_MSC_CH_EN                  0x3A0u

static volatile uint32_t *dma_reg(uint32_t offset)
{
    return (volatile uint32_t *)(g_dma_base + offset);
}

static uint32_t dma_channel_offset(hal_dma_channel_t channel, uint32_t offset)
{
    return ((uint32_t)channel * DMA_CH_STRIDE) + offset;
}

static uint32_t dma_channel_enable_mask(hal_dma_channel_t channel)
{
    return ((uint32_t)1u << (DMA_MSC_CH_EN_WE_CH0_Pos + (uint32_t)channel)) |
           ((uint32_t)1u << (DMA_MSC_CH_EN_CH0_Pos + (uint32_t)channel));
}

int hal_dma_init_mem2mem(uintptr_t base, const hal_dma_mem2mem_config_t *config)
{
    if (base == 0 || config == NULL || config->block_size == 0u) {
        return -1;
    }

    g_dma_base = base;
    g_channel = config->channel;

    *dma_reg(DMA_MSC_CFG) = DMA_MSC_CFG_EN;
    *dma_reg(DMA_INT_CLR_TFR) = (1u << (uint32_t)g_channel);

    *dma_reg(dma_channel_offset(g_channel, DMA_CH_SAR)) = (uint32_t)config->src;
    *dma_reg(dma_channel_offset(g_channel, DMA_CH_DAR)) = (uint32_t)config->dst;
    *dma_reg(dma_channel_offset(g_channel, DMA_CH_LLP)) = 0u;
    *dma_reg(dma_channel_offset(g_channel, DMA_CH_CTL0)) = DMA_CH_CTL0_INT_EN |
                                                           DMA_CH_CTL0_DTW_8 |
                                                           DMA_CH_CTL0_STW_8 |
                                                           DMA_CH_CTL0_DINC_INC |
                                                           DMA_CH_CTL0_SINC_INC |
                                                           DMA_CH_CTL0_DST_MSIZE_4 |
                                                           DMA_CH_CTL0_SRC_MSIZE_4 |
                                                           DMA_CH_CTL0_TT_FC_M2M;
    *dma_reg(dma_channel_offset(g_channel, DMA_CH_CTL1)) = ((uint32_t)config->block_size & 0xFFu);
    *dma_reg(dma_channel_offset(g_channel, DMA_CH_CFG0)) = DMA_CH_CFG0_HS_SEL_DST_SW |
                                                           DMA_CH_CFG0_HS_SEL_SRC_SW |
                                                           DMA_CH_CFG0_PRIOR_7;
    *dma_reg(dma_channel_offset(g_channel, DMA_CH_CFG1)) = DMA_CH_CFG1_PROTCTL_1 | DMA_CH_CFG1_FIFO_MODE_0;
    return 0;
}

int hal_dma_enable_channel(void)
{
    if (g_dma_base == 0u) {
        return -1;
    }

    *dma_reg(DMA_MSC_CH_EN) = dma_channel_enable_mask(g_channel);
    return 0;
}

int hal_dma_poll_done(uint32_t timeout)
{
    uint32_t i;

    if (g_dma_base == 0u) {
        return -1;
    }

    for (i = 0; i < timeout; ++i) {
        if ((*dma_reg(DMA_INT_STAT_TFR) & (1u << (uint32_t)g_channel)) != 0u) {
            *dma_reg(DMA_INT_CLR_TFR) = (1u << (uint32_t)g_channel);
            return 0;
        }
    }

    return -1;
}

int hal_dma_mem2mem_transfer(uint32_t timeout)
{
    if (hal_dma_enable_channel() != 0) {
        return -1;
    }

    return hal_dma_poll_done(timeout);
}
