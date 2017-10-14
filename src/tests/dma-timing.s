#include "macros.s"

.section .iwram
dmaIwram:
.space 0x80

.section .text
dmaRom:
.space 0x80

TEST_ALL(testTrivialDma, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001
),)

TEST_ALL(testTrivialDmaRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001
),)

TEST_ALL(testTrivialDmaToRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001
),)

TEST_ALL(testTrivialDmaRomRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001
),)

TEST_ALL(testTrivial32Dma, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001
),)

TEST_ALL(testTrivial32DmaRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001
),)

TEST_ALL(testTrivial32DmaToRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001
),)

TEST_ALL(testTrivial32DmaRomRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001
),)

TEST_ALL(testShortDma, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010
),)

TEST_ALL(testShortDmaRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010
),)

TEST_ALL(testShortDmaToRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010
),)

TEST_ALL(testShortDmaRomRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010
),)

TEST_ALL(testShort32Dma, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010
),)

TEST_ALL(testShort32DmaRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010
),)

TEST_ALL(testShort32DmaToRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010
),)

TEST_ALL(testShort32DmaRomRom, _(str r3, [r2, #8]), _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010
),)
