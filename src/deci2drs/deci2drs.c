#include "common.h"

#define IRQ_STAT 0xbf801070
#define DMA_DICR 0xbf8010f4

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", start);

INCLUDE_RODATA("asm/deci2drs/nonmatchings/deci2drs", D_000010B0);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000000DC);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000184);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000002B4);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000384);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000003F4);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000518);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_0000068C);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000006CC);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_0000079C);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000007E0);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000828);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000940);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000984);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000B80);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000CAC);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000CEC);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000D58);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000D98);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000E04);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000E0C);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000F50);

// Poll?
//int
//func_00000F50()
//{
//	unsigned int stat, dicr;
//	int res;
//
//	stat = *(volatile unsigned int *)IRQ_STAT;
//	res = stat & 2;
//
//	if (res) {
//		res = stat & 8;
//	}
//
//	if (res) {
//		dicr = *(volatile unsigned int *)DMA_DICR;
//		res = dicr & 0x4000000;
//	}
//
//	return res;
//}
