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

#define I_STAT		0xbf801070
#define DMA_DICR	0xbf8010f4
#define IRQ_CTRL	0xbf801450
#define DMA_DMACEN	0xbf801578

#define read32(a) (*(volatile u_int *)(a))
#define write32(a, b) (*(volatile u_int *)(a) = (b))

extern int D_A00003E0;
extern int D_A00003E4;

ModuleInfo Module = { "Deci2_SIF2_interface_driver", 0x103 };

struct drv_sif {
	struct deci2_iface *iface;
	int flag;
	int unk8;
	int unkC; // current protocol
	int unk10;
	int unk14; // current transfer size
	int unk18;
	int unk1C;
	void *unk20;
	int unk24;
	int unk28;
	int unk2C;
	int unk30;
};

struct drv_sif sifdrv;

int func_0000068C(struct drv_sif *drv, int arg1, int arg2);
int func_000006CC(struct drv_sif *drv, int arg1, int arg2);
int func_0000079C(struct drv_sif *drv, int arg1, int arg2);
int func_000007E0(struct drv_sif *drv, int arg1, int arg2);
int func_00000828(struct drv_sif *drv, int arg1, int arg2);
int func_00000940(struct drv_sif *drv, int arg1, int arg2);
int func_00000B80(struct drv_sif *drv, int arg1, int arg2);
int func_00000CAC(struct drv_sif *drv, int arg1, int arg2);
int func_00000CEC(struct drv_sif *drv, int arg1, int arg2);
int func_00000D58(struct drv_sif *drv, int arg1, int arg2);
int func_00000D98(struct drv_sif *drv, int arg1, int arg2);
int func_00000E04(struct drv_sif *drv, int arg1, int arg2);
int func_00000984(struct drv_sif *drv, int arg1, int arg2);
int func_00000E0C(struct drv_sif *drv, int arg1, int arg2);

int func_000000DC(int func, struct drv_sif *drv, int a2, int a3);
int func_00000F50();

int (*drs_if_func[])(struct drv_sif *drv, int arg1, int arg2) = {
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

const char *drs_if_func_name[] = {
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
	if ((drv->unk8 & 0x800) && func != 6) {
		sceDeci2ExPanic("\t\tsif2 func %s flag = %x\n", drs_if_func_name[func], sifdrv.flag);
	}

	return drs_if_func[func](drv, a2, a3);
}

void
func_00000184(struct drv_sif *drv)
{
	u_int msflag;
	int spec, size;

	msflag = sceSifGetMSflg();
	if ((drv->unk8 & 0x800) && (msflag & 0xc0000000)) {
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

		if ((drv->unk8 & 0x800)) {
			sceDeci2ExPanic("\t\tsif2 read spec %x and DECI2_ACCEPT\n", D_A00003E0);
		}

		drv->unkC = spec >> 16;
		if (spec & 0x8000) {
			size = (spec & 0x7fff);
			if (size) {
				size *= 4;
			} else {
				size = 0x20000;
			}

			drv->unk14 = size;
		} else {
			drv->unk10 = (spec >> 8) & 0x7f;
			size = (spec & 0xff);
			if (size) {
				size *= 4;
			} else {
				size = 0x400;
			}

			drv->unk14 = size;
		}

		drv->unk18 = 0;
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
	if ((drv->unk8 & 0x800) && (msflag & 0xc0000000)) {
		sceDeci2ExPanic("\t\tsif2 read msflag for send %x\n", msflag);
	}

	if (!(drv->flag & 0x202) && (msflag & 0x40000000)) {
		drv->flag |= 2;
		sceSifSetMSflg(0x40000000);
		if ((drv->unk8 & 0x800)) {
			sceDeci2ExPanic("\t\tsif2 send dma start addr=%x, size=%x\n", drv->unk30,
			  drv->unk2C);
		}
		EnableIntr(34);
		sceSifDma2Transfer((void *)drv->unk30, drv->unk2C, 1);
	}
}

void
func_00000384(struct drv_sif *drv)
{
	int msflag;

	if (drv->unk8 & 0xc00) {
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
		if (drv->unk8 & 0x800) {
			sceDeci2ExPanic("\t\tsif2 send dma end %08x %08x %08x %08x ...\n",
			  ((uint *)drv->unk30)[0], ((uint *)drv->unk30)[1], ((uint *)drv->unk30)[2],
			  ((uint *)drv->unk30)[3]);
		}
		drv->flag = (drv->flag & ~0x2) | 0x40;
	} else if (drv->flag & 0x200) {
		if (drv->unk8 & 0x81c) {
			sceDeci2ExPanic("\t\tsif2 rcv dma end %08x %08x %08x %08x ...\n",
			  ((uint *)drv->unk20)[0], ((uint *)drv->unk20)[1], ((uint *)drv->unk20)[2],
			  ((uint *)drv->unk20)[3]);
		}

		drv->unk18 += drv->unk1C;
		drv->flag |= 0x4000;
		if (sceSifGetSMflg() & 0x80000000) {
			sceSifIntrOther();
		}
	} else {
		sceDeci2ExPanic("\t\tsif2 unexpected DMA interrupt\n");
	}
}

void
func_00000518(struct drv_sif *drv)
{
	int again;

	do {
		again = 0;

		if (drv->flag & 0x40) {
			again = 1;
			drv->flag = drv->flag & ~0x40;
			sceDeci2IfEventHandler(4, drv->iface, drv->unk2C, 0, 0);
			if (drv->flag & 1) {
				drv->flag |= 0x20;
			}
		}

		if (!(drv->flag & 0x210)) {
			if ((drv->flag & 0x21) == 0x21) {
				again = 1;
				drv->flag &= ~0x20;
				sceDeci2IfEventHandler(3, drv->iface, 0, 0, 0);
			}
		}

		if (drv->flag & 0x4000) {
			again = 1;
			drv->flag &= ~0x4000;
			if (drv->unk14 <= drv->unk18) {
				drv->flag &= ~0x200;
			} else {
				drv->flag |= 0x2000;
			}

			sceDeci2IfEventHandler(2, drv->iface, drv->unk1C, 0, 0);
		}

		if (!(drv->flag & 0x1000)) {
			if (drv->flag & 0x2000) {
				again = 1;
				drv->flag &= ~0x2000;
				sceDeci2IfEventHandler(1, drv->iface, drv->unk14 - drv->unk18, drv->unkC,
				  drv->unk10);
			}
		}
	} while (again);
}

int
func_0000068C(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag |= 0x100;

	if (drv->unk8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet receive start flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_000006CC(struct drv_sif *drv, int arg1, int arg2)
{
	void *ptr = (void *)arg1;
	int size = arg2;

	if (drv->flag & 2) {
		drv->flag |= 0x2000;
		return 0;
	}

	size = (size + 3) & ~3;

	if (drv->unk14 - drv->unk18 < size) {
		size = drv->unk14 - drv->unk18;
	}

	if (size <= 0) {
		return size;
	}

	if (drv->unk8 & 0x800) {
		sceDeci2ExPanic("\t\tsif2 rcv dma start %d byte\n", size);
	}

	drv->unk1C = size;
	drv->unk20 = ptr;
	EnableIntr(34);
	sceSifDma2Transfer(ptr, size, 16);

	return size;
}

int
func_0000079C(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x100;

	if (drv->unk8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet receive end flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_000007E0(struct drv_sif *drv, int a2, int a3)
{
	drv->flag |= 0x25;
	drv->unk24 = a2;
	drv->unk28 = a3;
	if (drv->unk8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet send start flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000828(struct drv_sif *drv, int arg1, int arg2)
{
	int s0 = drv->unk24 << 0x10;

	if (drv->flag & 0x4) {
		int v0;
		drv->flag &= ~0x4;

		if (arg2 > 0x400) {
			arg2 = 0x400;
		}

		s0 |= (drv->unk28 & 0x7f) << 0x8;
		v0 = (arg2 + 3) / 4;
		s0 |= (v0 & 0xff);
	} else {
		int v0_;
		if (arg2 > 0x20000) {
			arg2 = 0x20000;
		}

		v0_ = (arg2 + 3) / 4;
		s0 |= (v0_ & 0x7fff);
		s0 |= 0x8000;
	}

	if (drv->unk8 & 0x800) {
		sceDeci2ExPanic("\t\tsif2 write spec %x and DECI2_START\n", s0);
	}

	D_A00003E4 = s0;
	sceSifSetSMflg(0x80000000);
	sceSifIntrOther();

	drv->unk30 = arg1;
	drv->unk2C = arg2;
	return arg2;
}

int
func_00000940(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x21;

	if (drv->unk8 & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet send end  flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000984(struct drv_sif* drv, int arg1, int arg2)
{
	if (func_00000F50(drv) != 0) {
		if ((read32(DMA_DICR) & 0x4000000) != 0 && (read32(I_STAT) & 0x8) != 0) {
			while (read32(DMA_DMACEN) != 0) {
				write32(DMA_DMACEN, 0);
			}

			write32(I_STAT, ~0x8);
			write32(DMA_DICR, read32(DMA_DICR) & ~0xfb840000);

			while ((read32(DMA_DICR) & 0x800000) != 0);

			while (read32(DMA_DMACEN) != 1) {
				write32(DMA_DMACEN, 1);
			}

			func_000003F4(drv);

			while (read32(DMA_DMACEN) != 0) {
				write32(DMA_DMACEN, 0);
			}

			write32(DMA_DICR, (read32(DMA_DICR) & 0xffffff) | 0x800000);

			while (read32(DMA_DMACEN) != 1) {
				write32(DMA_DMACEN, 1);
			}
		}

		func_00000518(drv);

		if ((read32(I_STAT) & 0x2) != 0) {
			write32(I_STAT, ~0x2);
			func_000002B4(drv);
			func_00000184(drv);
			func_00000384(drv);
		}

		func_00000518(drv);
	}

	func_00000518(drv);
	return 0;
}

int
func_00000B80(struct drv_sif *drv, int arg1, int arg2)
{
	uint msflg;

	if (drv->flag & 0x100000 && sceSifCheckInit()) {
		sceSifDma2Init();
		D_A00003E4 = 0x100;
		drv->flag = (drv->flag & ~0x100000) | 0x200000;
		sceSifSetSMflg(0x80000000);
		sceSifIntrOther();
	} else if (drv->flag & 0x200000 && read32(I_STAT) & 2) {
		msflg = sceSifGetMSflg();
		write32(I_STAT, ~2);
		if (msflg & 0x40000000) {
			sceSifSetMSflg(0x40000000);
			drv->flag &= ~0x200000;
			drs_if_func[6] = func_00000984;
			sceDeci2IfEventHandler(5, sifdrv.iface, 0, 0, 0);
		}
	}

	return 0;
}

int
func_00000CAC(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag |= 0x1000;

	if (drv->unk8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 rcv off flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000CEC(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x1000;

	if (drv->unk8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 rcv on flag = %x\n", drv->flag);
	}

	if (drv->flag & 0x8000) {
		func_00000184(drv);
	}

	return 0;
}

int
func_00000D58(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag |= 0x10;

	if (drv->unk8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 send off flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000D98(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x10;

	if (drv->unk8 & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 send on flag = %x\n", drv->flag);
	}

	if (drv->flag & 1) {
		func_000002B4(drv);
	}

	return 0;
}

int
func_00000E04(struct drv_sif *drv, int arg1, int arg2)
{
	drv->unk8 = arg1;
	// BUG: missing return
}

int
func_00000E0C(struct drv_sif *drv, int arg1, int arg2)
{
	uint msflg;

	if (drv->flag & 0x300000) {
		uint bits = 0x100000;
		drv->flag |= bits;
		drv->flag &= ~0x200000;

		// BUG: wrong return
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
			drs_if_func[6] = func_00000B80;

			break;
		}
	}

	// BUG: missing return
}
