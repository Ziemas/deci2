#include "common.h"
#include "deci2.h"
#include "deci2_internal.h"
#include "intrman_internal.h"
#include "loadcore_internal.h"
#include "sif.h"
#include "sif_internal.h"
#include "sys/types.h"

#include <loadcore.h>
#include <string.h>

#define I_STAT 0xbf801070
#define IRQ_CTRL 0xbf801450
#define read32(a) (*(volatile u_int *)(a))
#define write32(a, b) (*(volatile u_int *)(a) = (b))

extern int D_A00003E0;
extern int D_A00003E4;

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
void func_00000E0C();

int (*drs_iface_func[])() = {
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
	struct deci2_iface *iface;
	int flag;
	int field_8;
	int field_c; // current protocol
	int field_10;
	int field_14; // current transfer size
	int field_18;
	int field_1c;
	int field_20;
	int field_24;
	int field_28;
	int field_2c;
	int field_30;
};

struct drv_sif sifdrv;

int func_000000DC(int func, struct drv_sif *drv, int a2, int a3);
int func_00000F50();

int
start(int a0)
{
	int *bootmode;
	int prid;

	asm volatile("mfc0 %0, $15" : "=r"(prid) :);
	if (prid < 16) {
		return 1;
	}

	if ((read32(IRQ_CTRL) & 8)) {
		return 1;
	}

	if (!a0) {
		bootmode = QueryBootMode(3);
		if (bootmode && (bootmode[1] & 0x40)) {
			return 1;
		}
	}

	memset(&sifdrv, 0, sizeof(sifdrv));
	sifdrv.flag |= 0x100000;
	sifdrv.iface = sceDeci2IfCreate(DECI2_NODE_EE, &sifdrv, func_000000DC, func_00000F50);
	DisableDispatchIntr(1);
	DisableDispatchIntr(34);
	EnableIntr(1);
	return 0;
}

int
func_000000DC(int func, struct drv_sif *drv, int a2, int a3)
{
	if ((drv->field_8 & 0x800) && func != 6) {
		sceDeci2ExPanic("\t\tsif2 func %s flag = %x\n", drs_iface_func_name[func], sifdrv.flag);
	}

	drs_iface_func[func](drv, a2, a3);
}

void
func_00000184(struct drv_sif *drv)
{
	u_int msflag;
	int spec, size;

	msflag = sceSifGetMSflg();
	if ((drv->field_8 & 0x800) && (msflag & 0xc0000000)) {
		sceDeci2ExPanic("\t\tsif2 read msflag for rcv %x\n", msflag);
	}

	if (!(drv->flag & 0x202) && (msflag & 0x80000000)) {
		spec = D_A00003E0;
		/*
		 * First write of transfer?
		 * int words : 8
		 * int dest  : 7
		 * int unk   : 1
		 * int proto : 16
		 *
		 * Further writes
		 * int words : 15
		 * int unk   : 1
		 * int proto : 16
		 */

		if ((drv->field_8 & 0x800)) {
			sceDeci2ExPanic("\t\tsif2 read spec %x and DECI2_ACCEPT\n", D_A00003E0);
		}

		drv->field_c = spec >> 16;
		if (spec & 0x8000) {
			size = (spec & 0x7fff);
			if (size) {
				size *= 4;
			} else {
				size = 0x20000;
			}

			drv->field_14 = size;
		} else {
			drv->field_10 = (spec >> 8) & 0x7f;
			size = (spec & 0xff);
			if (size) {
				size *= 4;
			} else {
				size = 0x400;
			}

			drv->field_14 = size;
		}

		drv->field_18 = 0;
		drv->flag = (drv->flag | 0x2200) & ~0x8000;
		sceSifSetMSflg(0x80000000);
		sceSifSetSMflg(0x40000000);
		sceSifIntrOther();
		return;
	}

	if (msflag & 0x80000000) {
		drv->flag |= 0x8000;
	}
}

void
func_000002B4(struct drv_sif *drv)
{
	u_int msflag;

	msflag = sceSifGetMSflg();
	if ((drv->field_8 & 0x800) && (msflag & 0xc0000000)) {
		sceDeci2ExPanic("\t\tsif2 read msflag for send %x\n", msflag);
	}

	if (!(drv->flag & 0x202) && (msflag & 0x40000000)) {
		drv->flag |= 2;
		sceSifSetMSflg(0x40000000);
		if ((drv->field_8 & 0x800)) {
			sceDeci2ExPanic("\t\tsif2 send dma start addr=%x, size=%x\n", drv->field_30,
			  drv->field_2c);
		}
		EnableIntr(34);
		sceSifDma2Transfer((void *)drv->field_30, drv->field_2c, 1);
	}
}

void
func_00000384(struct drv_sif *drv)
{
	int msflag;

	if (drv->field_8 & 0xc00) {
		msflag = sceSifGetMSflg();
		if (drv->flag & 2 && msflag & 0x40000000) {
			sceDeci2ExPanic("\t\tSIF2 SENDING unexpected DECI2_ACCEPT !\n");
		}
	}
}

void
func_000003F4(struct drv_sif *drv)
{
	if (drv->flag & 2) {
		if (drv->field_8 & 0x800) {
			sceDeci2ExPanic("\t\tsif2 send dma end %08x %08x %08x %08x ...\n",
			  ((uint *)drv->field_30)[0], ((uint *)drv->field_30)[1], ((uint *)drv->field_30)[2],
			  ((uint *)drv->field_30)[3]);
		}
		drv->flag = (drv->flag & ~0x2) | 0x40;
	} else if (drv->flag & 0x200) {
		if (drv->field_8 & 0x81c) {
			sceDeci2ExPanic("\t\tsif2 rcv dma end %08x %08x %08x %08x ...\n",
			  ((uint *)drv->field_20)[0], ((uint *)drv->field_20)[1], ((uint *)drv->field_20)[2],
			  ((uint *)drv->field_20)[3]);
		}

		drv->field_18 += drv->field_1c;
		drv->flag |= 0x4000;
		if (sceSifGetSMflg() & 0x80000000) {
			sceSifIntrOther();
		}
	} else {
		sceDeci2ExPanic("\t\tsif2 unexpected DMA interrupt\n");
	}
}

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000518);

int
func_0000068C(struct drv_sif *drv)
{
	drv->flag |= 0x100;

	if (drv->field_8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet receive start flag = %x\n", drv->flag);
	}

	return 0;
}

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000006CC);

int
func_0000079C(struct drv_sif *drv)
{
	drv->flag &= ~0x100;

	if (drv->field_8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet receive end flag = %x\n", drv->flag);
	}

	return 0;
}

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_000007E0);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000828);

int
func_00000940(struct drv_sif *drv)
{
	drv->flag &= ~0x21;

	if (drv->field_8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet send end  flag = %x\n", drv->flag);
	}

	return 0;
}

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000984);

INCLUDE_ASM("asm/deci2drs/nonmatchings/deci2drs", func_00000B80);

int
func_00000CAC(struct drv_sif *drv)
{
	drv->flag |= 0x1000;

	if (drv->field_8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 rcv off flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000CEC(struct drv_sif *drv)
{
	drv->flag &= ~0x1000;

	if (drv->field_8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 rcv on flag = %x\n", drv->flag);
	}

	if (drv->flag & 0x8000) {
		func_00000184(drv);
	}

	return 0;
}

int
func_00000D58(struct drv_sif *drv)
{
	drv->flag |= 0x10;

	if (drv->field_8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 send off flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000D98(struct drv_sif *drv)
{
	drv->flag &= ~0x10;

	if (drv->field_8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 send on flag = %x\n", drv->flag);
	}

	if (drv->flag & 1) {
		func_000002B4(drv);
	}

	return 0;
}

int
func_00000E04(struct drv_sif *drv, int a2, int a3)
{
	drv->field_8 = a2;
}

void
func_00000E0C(struct drv_sif *drv)
{
	uint msflg;

	if (drv->flag & 0x300000) {
		uint bits = 0x100000;
		drv->flag |= bits;
		drv->flag &= ~0x200000;

		return;
	}

	sceDeci2IfEventHandler(6, sifdrv.iface, 0, 0, 0);
	while (drv->flag & 0x101) {
		sceDeci2ExPoll();
	}

	D_A00003E4 = 0x200;
	sceSifSetSMflg(0x80000000);
	sceSifIntrOther();

	while (1) {
		if (!(read32(I_STAT) & 2)) {
			continue;
		}

		msflg = sceSifGetMSflg();
		write32(I_STAT, ~2);

		if (msflg & 0x40000000) {
			sceSifSetMSflg(0x40000000);
			drv->flag |= 0x100000;
			drv->flag &= ~0x200000;
			drs_iface_func[6] = func_00000B80;

            break;
		}
	}
}
