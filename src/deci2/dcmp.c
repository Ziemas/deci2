#include "common.h"
#include "deci2_internal.h"

#include <deci2.h>

struct dcmp { };

void func_00002C9C(struct dcmp *);
void func_00002D14(struct dcmp *, int);
void func_0000338C(struct dcmp *);
void func_000033E0(struct dcmp *);

// dcmp init
INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00002A40);

// dcmp socket handler
void
func_00002A68(int event, int param, void *opt)
{
	struct dcmp *d = opt;

	switch (event) {
	case DECI2_READ:
		func_00002C9C(d);
		break;
	case DECI2_READDONE:
		func_00002D14(d, param);
		break;
	case DECI2_WRITE:
		func_0000338C(d);
		break;
	case DECI2_WRITEDONE:
		func_000033E0(d);
		break;
	case DECI2_CHSTATUS:
	case DECI2_ERROR:
	case DECI2Ex_RflagDone:
	case DECI2Ex_WriteStart:
	case DECI2Ex_WflagDone:
		sceDeci2ExPanic("DcmpHandler: unsupprt event 0x%x\n", event);
		break;
	default:
		sceDeci2ExPanic("DcmpHandler: unknown event 0x%x\n", event);
		break;
	}
}

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00002B10);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00002C08);

// dcmp read
INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00002C9C);

// dcmp readdone
INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00002D14);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00003170);

// dcmp write
INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_0000338C);

// dcmp writedone
INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_000033E0);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_0000352C);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00003564);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_000035D4);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00003600);

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00003668);
