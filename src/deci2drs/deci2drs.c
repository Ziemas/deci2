#include "common.h"

#include <loadcore.h>

extern char D_000010B0[];
ModuleInfo Module = { D_000010B0, 0x103 };

extern char D_00001138[];
extern char D_0000112C[];
extern char D_00001124[];
extern char D_00001118[];
extern char D_0000110C[];
extern char D_00001100[];
extern char D_000010F8[];
extern char D_000010F0[];
extern char D_000010E8[];
extern char D_000010DC[];
extern char D_000010D4[];
extern char D_000010CC[];

int func_0000068C();
int func_000006CC();
int func_0000079C();
int func_000007E0();
int func_00000828();
int func_00000940();
int func_00000B80();
int func_00000CAC();
int func_00000CEC();
int func_00000D58();
int func_00000D98();
int func_00000E04();
int func_00000E0C();

void *drs_iface_func[] = {
	func_0000068C,
	func_000006CC,
	func_0000079C,
	func_000007E0,
	func_00000828,
	func_00000940,

	func_00000B80,
	func_00000CAC,
	func_00000CEC,
	func_00000D58,
	func_00000D98,
	func_00000E04,
	func_00000E0C,
};

const char *drs_iface_func_name[] = {
	D_00001138,
	D_0000112C,
	D_00001124,
	D_00001118,
	D_0000110C,
	D_00001100,
	D_000010F8,
	D_000010F0,
	D_000010E8,
	D_000010DC,
	D_000010D4,
	D_000010CC,
};

struct drv_sif {
	int iface;
	int flag;
	int field_8;
	int field_c;
	int field_10;
	int field_14;
	int field_18;
	int field_1c;
	int field_20;
	int field_24;
	int field_28;
	int field_2c;
	int field_30;
};

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
