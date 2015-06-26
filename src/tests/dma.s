#include "macros.s"

.section .iwram
dmaIwram:
.space 0x80

.section .text
dmaRom:
.space 0x80

TEST_ALL(testNullDma, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x00000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testNullDmaRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x00000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivialDma, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivialDmaRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivialDmaToRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivialDmaRomRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivial32Dma, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivial32DmaRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivial32DmaToRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testTrivial32DmaRomRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000001 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShortDma, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShortDmaRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShortDmaToRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShortDmaRomRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x80000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShort32Dma, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShort32DmaRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	ldr r3, =dmaIwram ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShort32DmaToRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaIwram ;
	str r3, [r2] ;
	ldr r3, =dmaRom ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010 ;
	str r3, [r2, #8]
), , )

TEST_ALL(testShort32DmaRomRom, _(
	ldr r2, =DMA3SAD ;
	ldr r3, =dmaRom ;
	str r3, [r2] ;
	add r3, #0x40 ;
	str r3, [r2, #4] ;
	ldr r3, =0x84000010 ;
	str r3, [r2, #8]
), , )
