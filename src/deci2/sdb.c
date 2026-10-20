#include "common.h"
#include "deci2.h"
#include "deci2_internal.h"
#include "excepman.h"

struct sdb {
	int unk0;
	int unk4;
	int unk8;
	int unkC;
	int unk10;
	int unk14;
	int unk18;
	int unk1C;
	int unk20;
	int unk24;
	int unk28;
	int unk2C;
	int unk30;
	int unk34;
	int unk38;
	int unk3C;
	int unk40;
	int unk44;
	int unk48;
	int unk4C;
	int unk50;
	int unk54;
	int unk58;
	int unk5C;
};

struct sdb sdbg;

void func_00003C58(struct sdb *);
void func_00003E6C(struct sdb *);
void func_00003F68(struct sdb *);
void func_00004064(struct sdb *);
void func_00004EBC(struct sdb *);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00003760);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000038C8);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00003950);

// INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00003B78);
void
func_00003B78(int event, int param, void *opt)
{

	struct sdb *sdb = opt;

	switch (event) {
	case DECI2_READ:
		func_00003C58(sdb);
		break;
	case DECI2_READDONE:
		sdb->unk4 = param;
		func_00003E6C(sdb);
		break;
	case DECI2_WRITE:
		func_00003F68(sdb);
		break;
	case DECI2_WRITEDONE:
		func_00004064(sdb);
		break;

	case DECI2_CHSTATUS:
	case DECI2_ERROR:
		if (sdb->unk4 & 0x2000) {
			sceDeci2ExPanic("DbgHandler: unsupprt event 0x%x\n", event);
		}
		break;

	case DECI2Ex_RflagDone:
	case DECI2Ex_WriteStart:
	case DECI2Ex_WflagDone:
		break;

	case 10:
		if (sdbg.unk8 > 0) {
			func_00004EBC(sdb);
		}
		break;
	default:
		sceDeci2ExPanic("DbgHandler: unknown event 0x%x\n", event);
		break;
	}
}

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00003C58);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00003E6C);

INCLUDE_RODATA("asm/deci2/nonmatchings/sdb", D_000063D8);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00003F68);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004064);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000040FC);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000041C8);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004214);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004398);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004490);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000044CC);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_0000453C);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_0000458C);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000046FC);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004774);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000048E4);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_000049E4);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004BA0);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004D2C);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004D84);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004E14);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004E6C);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004E94);

INCLUDE_ASM("asm/deci2/nonmatchings/sdb", func_00004EBC);
