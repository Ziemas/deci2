#ifndef INTRMAN_INTERNAL_H_
#define INTRMAN_INTERNAL_H_

enum {
	INUM_VBLANK = 0,
	INUM_GM = 1,
	INUM_CDROM = 2,
	INUM_DMA = 3,
	INUM_RTC0 = 4,
	INUM_RTC1 = 5,
	INUM_RTC2 = 6,
	INUM_SIO0 = 7,
	INUM_SIO1 = 8,
	INUM_SPU = 9,
	INUM_PIO = 10,
	INUM_EVBLANK = 11,
	INUM_DVD = 12,
	INUM_PCMCIA = 13,
	INUM_RTC3 = 14,
	INUM_RTC4 = 15,
	INUM_RTC5 = 16,
	INUM_SIO2 = 17,
	INUM_HTR0 = 18,
	INUM_HTR1 = 19,
	INUM_HTR2 = 20,
	INUM_HTR3 = 21,
	INUM_USB = 22,
	INUM_EXTR = 23,
	INUM_FWRE = 24,
	INUM_FDMA = 25,

	INUM_DMA_0 = 32,
	INUM_DMA_1 = 33,
	INUM_DMA_2 = 34,
	INUM_DMA_3 = 35,
	INUM_DMA_4 = 36,
	INUM_DMA_5 = 37,
	INUM_DMA_6 = 38,
	INUM_DMA_BERR = 39,
	INUM_DMA_7 = 40,
	INUM_DMA_8 = 41,
	INUM_DMA_9 = 42,
	INUM_DMA_10 = 43,
	INUM_DMA_11 = 44,
	INUM_DMA_12 = 45,

	INUM_SW1 = 62,
	INUM_SW2 = 63,

	INUM_MAX = 64,
};

int RegisterIntrHandler(int irq, int mode, int (*handler)(void *), void *arg);
int EnableIntr(int irq);
int CpuInvokeInKmode(void *function, ...);
void DisableDispatchIntr(int irq);

#endif // INTRMAN_INTERNAL_H_
