#include "common.h"
#include "introld.h"

#include <deci2.h>
#include <stdio.h>
#include <thread.h>

extern int D_00006708;
extern char D_00006568[];
extern char D_000065D0[];

INCLUDE_ASM("asm/deci2/nonmatchings/main", start);

int
func_000001F4()
{
	D_00006708 = 1;
	CpuEnableIntr();
	printf(D_00006568);
	printf(D_000065D0);
	return 0;
}

int
func_00000240(int *arg1)
{
	CpuEnableIntr();
	if (!*arg1) {
		ChangeThreadPriority(0, 126);
		while (1) {
			sceDeci2Poll();
			DelayThread(2000000);
		}
	}

	return 0;
}

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000029C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000330);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000340);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000350);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000360);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000524);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000558);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000678);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000006A4);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00005AA0);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00005AB0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000770);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000978);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000A18);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000B04);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000B6C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000C08);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000C8C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000D24);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000DA4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000DE8);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000E20);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000F08);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000F1C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001014);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001110);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000121C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001364);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000014AC);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000014E4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001BA0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001E20);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001FC4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000214C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002234);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000231C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000025BC);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000027F4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2Poll);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002904);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002914);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002A0C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002A40);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002A68);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002B10);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002C08);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002C9C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002D14);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003170);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000338C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000033E0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000352C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003564);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000035D4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003600);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003668);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000062C0);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000062E0);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000062E4);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000062FC);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00006314);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00006338);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", jtbl_00006358);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00006380);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000063A4);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000063C0);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_000063D8);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00006400);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_0000641C);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_0000643C);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", jtbl_00006460);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", jtbl_000064C0);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", jtbl_00006520);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", jtbl_00006540);
