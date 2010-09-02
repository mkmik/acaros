#ifndef _MM_DMA_H_
#define _MM_DMA_H_

/** 1MB */
#define DMA_POOL_SIZE 0x80000

/** initializes the direct mapped pool suited
 * for dma transfers */
void mm_dma_init();

#endif
