#include "common.h"

#include <loadcore.h>

ModuleInfo Module = { "Deci2_SIF2_interface_driver", 0x103 };

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
	"RCV_START",
	"RCV_READ",
	"RCV_END",
	"SEND_START",
	"SEND_WRITE",
	"SEND_END",
	"POLL",
	"RCV_OFF",
	"RCV_ON",
	"SEND_OFF",
	"SEND_ON",
	"DEBUG",
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
