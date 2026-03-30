#include "bits.h"
#include "common.h"
#include "loadcore.h"
#include "sys/types.h"

ModuleInfo Module = { "Deci2_PIF_interface_driver", 0x101 };

#define PIFREG ((volatile struct pif_reg *)0xbf803800)

struct pif_reg {
	u_short unk0;
	u_short unk2;
	u_short unk4;
	u_short unk6;
	u_short unk8;
	u_short unkA;
	u_short unkC;
	u_short unkE;
	u_short unk10;
	u_short unk12;
	u_short unk14;
	u_short unk16;
	u_short unk18;
	u_short unk1A;
	u_short unk1C;
	u_short unk1E;
	u_short unk20;
	u_short unk22;
	u_short unk24;
	u_short unk26;
	u_short unk28;
	u_short unk2A;
	u_short unk2C;
	u_short unk2E;
	u_short unk30;
	u_short unk32;
	u_short unk34;
	u_short unk36;
	u_short unk38;
	u_short unk3A;
	u_short unk3C;
	u_short unk3E;
	u_int unk40;
};

struct drv_pif {
	struct deci2_iface *iface;
	int unk4;
	int unk8;
	char unkC;
	char unkD;
	char unkE;
	char unkF;
	u_short unk10;
	int unk14;
	int unk18;
	int unk1C;
	u_short unk20;
	u_char unk23;
	int unk24;
	int unk28;
	int unk2C;
};

int func_000008C4(struct drv_pif *drv, int, int);
int func_00000904(struct drv_pif *drv, int, int);
int func_00000A84(struct drv_pif *drv, int, int);
int func_00000B0C(struct drv_pif *drv, int, int);
int func_00000B6C(struct drv_pif *drv, int, int);
int func_00000BEC(struct drv_pif *drv, int, int);
int func_00000C58(struct drv_pif *drv, int, int);
int func_00000CC8(struct drv_pif *drv, int, int);
int func_00000D10(struct drv_pif *drv, int, int);
int func_00000D7C(struct drv_pif *drv, int, int);
int func_00000DBC(struct drv_pif *drv, int, int);
int func_00000E28(struct drv_pif *drv, int, int);
int func_00000E30(struct drv_pif *drv, int, int);

int (*drp_if_func[])(struct drv_pif *drv, int arg1, int arg2) = {
	func_000008C4,
	func_00000904,
	func_00000A84,
	func_00000B0C,
	func_00000B6C,
	func_00000BEC,
	func_00000C58,
	func_00000CC8,
	func_00000D10,
	func_00000D7C,
	func_00000DBC,
	func_00000E28,
	func_00000E30,
};

const char *drp_if_func_name[] = {
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

#if 1
INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", start);
#else
int
start(int a0)
{
	u_int v0, v1;
	int prid;

	v0 = read32(0xbf801004);
	v1 = read32(0xbf80101c);
	asm volatile("mfc0 %0, $15" : "=r"(prid) :);
	write32(0xbf801004, 0xbf802000);
	write32(0xbf80101c, 0xd3655);

	write32(0xbf801004, v0);
	write32(0xbf80101c, v1);
}
#endif

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_000002E8);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000410);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_000004C4);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000568);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000608);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000708);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_000008C4);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000904);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000A84);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000B0C);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000B6C);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000BEC);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000C58);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000CC8);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000D10);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000D7C);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000DBC);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000E28);

int
func_00000E30(struct drv_pif *drv, int a1, int a2)
{
}

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000E38);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000E5C);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000E7C);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000EA0);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000EC0);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000F44);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000FC4);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_000011A4);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00001378);

void
func_00001588(void)
{
	volatile struct pif_reg *pif = PIFREG;
	volatile u_int sp0; // needs volatile to make a stack frame...

	while (!(pif->unk10 & 1)) {
		sp0 = pif->unk40;
	}

	while (pif->unk18 != 0) {
		PIFREG->unk18 = 1;
	}
}

INCLUDE_RODATA("asm/deci2drp/nonmatchings/deci2drp", D_00001BF4);

INCLUDE_RODATA("asm/deci2drp/nonmatchings/deci2drp", D_00001BFC);

INCLUDE_RODATA("asm/deci2drp/nonmatchings/deci2drp", D_00001C00);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_000015FC);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00001820);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_0000183C);
