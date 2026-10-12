#include "bits.h"
#include "common.h"
#include "deci2.h"
#include "deci2_internal.h"
#include "intrman_internal.h"
#include "iopregs.h"
#include "loadcore_internal.h"
#include "sif.h"
#include "sif_internal.h"
#include "sys/types.h"

#include <loadcore.h>
#include <string.h>

extern int D_A00003E0;
extern int D_A00003E4;

ModuleInfo Module = { "Deci2_SIF2_interface_driver", 0x103 };

struct drv_sif {
	struct deci2_iface *iface;
	int flag;
	int debug;

	// What the EE is sending us
	int in_proto;
	int in_node;
	int in_len;

	int rcv_progress;
	int rcv_len;
	void *rcv_buf;

	int send_proto;
	int send_node;
	int send_len;
	void *send_buf;
};

struct drv_sif sifdrv;

int drs_rcv_start(struct drv_sif *drv, int arg1, int arg2);
int drs_rcv_read(struct drv_sif *drv, int arg1, int arg2);
int drs_rcv_end(struct drv_sif *drv, int arg1, int arg2);
int drs_send_start(struct drv_sif *drv, int arg1, int arg2);
int drs_send_write(struct drv_sif *drv, int arg1, int arg2);
int drs_send_end(struct drv_sif *drv, int arg1, int arg2);
int drs_poll_1(struct drv_sif *drv, int arg1, int arg2);
int drs_rcv_off(struct drv_sif *drv, int arg1, int arg2);
int drs_rcv_on(struct drv_sif *drv, int arg1, int arg2);
int drs_send_off(struct drv_sif *drv, int arg1, int arg2);
int drs_send_on(struct drv_sif *drv, int arg1, int arg2);
int drs_debug(struct drv_sif *drv, int arg1, int arg2);
int drs_poll_2(struct drv_sif *drv, int arg1, int arg2);
int drs_shutdown(struct drv_sif *drv, int arg1, int arg2);

int func_000000DC(int func, struct drv_sif *drv, int a2, int a3);
int func_00000F50();

int (*drs_if_func[])(struct drv_sif *drv, int arg1, int arg2) = {
	drs_rcv_start,
	drs_rcv_read,
	drs_rcv_end,
	drs_send_start,
	drs_send_write,
	drs_send_end,
	drs_poll_1,
	drs_rcv_off,
	drs_rcv_on,
	drs_send_off,
	drs_send_on,
	drs_debug,
	drs_shutdown,
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
		return NO_RESIDENT_END;
	}

	if ((*IRQ_CTRL & 8)) {
		return NO_RESIDENT_END;
	}

	if (!a0) {
		bootmode = QueryBootMode(3);
		if (bootmode && (bootmode[1] & 0x40)) {
			return NO_RESIDENT_END;
		}
	}

	memset(&sifdrv, 0, sizeof(sifdrv));
	sifdrv.flag |= 0x100000;
	sifdrv.iface = sceDeci2IfCreate(DECI2_NODE_EE, &sifdrv, func_000000DC, func_00000F50);
	DisableDispatchIntr(1);
	DisableDispatchIntr(34);
	EnableIntr(1);
	return RESIDENT_END;
}

int
func_000000DC(int func, struct drv_sif *drv, int a2, int a3)
{
	if ((drv->debug & 0x800) && func != 6) {
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
	if ((drv->debug & 0x800) && (msflag & 0xc0000000)) {
		sceDeci2ExPanic("\t\tsif2 read msflag for rcv %x\n", msflag);
	}

	if (!(drv->flag & (0x200 | 0x2)) && (msflag & 0x80000000)) {
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

		if ((drv->debug & 0x800)) {
			sceDeci2ExPanic("\t\tsif2 read spec %x and DECI2_ACCEPT\n", D_A00003E0);
		}

		drv->in_proto = spec >> 16;
		if (spec & 0x8000) {
			size = (spec & 0x7fff);
			if (size) {
				size *= 4;
			} else {
				size = 0x20000;
			}

			drv->in_len = size;
		} else {
			drv->in_node = (spec >> 8) & 0x7f;
			size = (spec & 0xff);
			if (size) {
				size *= 4;
			} else {
				size = 0x400;
			}

			drv->in_len = size;
		}

		drv->rcv_progress = 0;
		drv->flag = (drv->flag | (0x2000 | 0x200)) & ~0x8000;
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
	if ((drv->debug & 0x800) && (msflag & 0xc0000000)) {
		sceDeci2ExPanic("\t\tsif2 read msflag for send %x\n", msflag);
	}

	if (!(drv->flag & (0x200 | 0x2)) && (msflag & 0x40000000)) {
		drv->flag |= 2;
		sceSifSetMSflg(0x40000000);
		if ((drv->debug & 0x800)) {
			sceDeci2ExPanic("\t\tsif2 send dma start addr=%x, size=%x\n", drv->send_buf, drv->send_len);
		}
		EnableIntr(34);
		sceSifDma2Transfer(drv->send_buf, drv->send_len, 1);
	}
}

void
func_00000384(struct drv_sif *drv)
{
	int msflag;

	if (drv->debug & 0xc00) {
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
		if (drv->debug & 0x800) {
			sceDeci2ExPanic("\t\tsif2 send dma end %08x %08x %08x %08x ...\n",
			  ((uint *)drv->send_buf)[0], ((uint *)drv->send_buf)[1], ((uint *)drv->send_buf)[2],
			  ((uint *)drv->send_buf)[3]);
		}
		drv->flag = (drv->flag & ~0x2) | 0x40;
	} else if (drv->flag & 0x200) {
		if (drv->debug & 0x81c) {
			sceDeci2ExPanic("\t\tsif2 rcv dma end %08x %08x %08x %08x ...\n",
			  ((uint *)drv->rcv_buf)[0], ((uint *)drv->rcv_buf)[1], ((uint *)drv->rcv_buf)[2],
			  ((uint *)drv->rcv_buf)[3]);
		}

		drv->rcv_progress += drv->rcv_len;
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
			sceDeci2IfEventHandler(IFM_OUTDONE, drv->iface, drv->send_len, 0, 0);
			if (drv->flag & 1) {
				drv->flag |= 0x20;
			}
		}

		if (!(drv->flag & (0x200 | 0x10))) {
			if ((drv->flag & (0x20 | 0x1)) == (0x20 | 0x1)) {
				again = 1;
				drv->flag &= ~0x20;
				sceDeci2IfEventHandler(IFM_OUT, drv->iface, 0, 0, 0);
			}
		}

		if (drv->flag & 0x4000) {
			again = 1;
			drv->flag &= ~0x4000;
			if (drv->in_len <= drv->rcv_progress) {
				drv->flag &= ~0x200;
			} else {
				drv->flag |= 0x2000;
			}

			sceDeci2IfEventHandler(IFM_INDONE, drv->iface, drv->rcv_len, 0, 0);
		}

		if (!(drv->flag & 0x1000)) {
			if (drv->flag & 0x2000) {
				again = 1;
				drv->flag &= ~0x2000;
				sceDeci2IfEventHandler(IFM_IN, drv->iface, drv->in_len - drv->rcv_progress, drv->in_proto,
				  drv->in_node);
			}
		}
	} while (again);
}

int
drs_rcv_start(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag |= 0x100;
	if (drv->debug & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet receive start flag = %x\n", drv->flag);
	}

	return 0;
}

int
drs_rcv_read(struct drv_sif *drv, int arg1, int arg2)
{
	void *ptr = (void *)arg1;
	int size = arg2;

	if (drv->flag & 2) {
		drv->flag |= 0x2000;
		return 0;
	}

	size = (size + 3) & ~3;

	if (drv->in_len - drv->rcv_progress < size) {
		size = drv->in_len - drv->rcv_progress;
	}

	if (size <= 0) {
		return size;
	}

	if (drv->debug & 0x800) {
		sceDeci2ExPanic("\t\tsif2 rcv dma start %d byte\n", size);
	}

	drv->rcv_len = size;
	drv->rcv_buf = ptr;
	EnableIntr(34);
	sceSifDma2Transfer(ptr, size, 16);

	return size;
}

int
drs_rcv_end(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x100;

	if (drv->debug & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet receive end flag = %x\n", drv->flag);
	}

	return 0;
}

int
drs_send_start(struct drv_sif *drv, int a2, int a3)
{
	drv->flag |= 0x25;
	drv->send_proto = a2;
	drv->send_node = a3;
	if (drv->debug & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet send start flag = %x\n", drv->flag);
	}

	return 0;
}

int
drs_send_write(struct drv_sif *drv, int arg1, int arg2)
{
	int spec = drv->send_proto << 0x10;

	if (drv->flag & 0x4) {
		drv->flag &= ~0x4;

		if (arg2 > 0x400) {
			arg2 = 0x400;
		}

		spec |= (drv->send_node & 0x7f) << 0x8;
		spec |= ((arg2 + 3) / 4 & 0xff);
	} else {
		if (arg2 > 0x20000) {
			arg2 = 0x20000;
		}

		spec |= ((arg2 + 3) / 4 & 0x7fff);
		spec |= 0x8000;
	}

	if (drv->debug & 0x800) {
		sceDeci2ExPanic("\t\tsif2 write spec %x and DECI2_START\n", spec);
	}

	D_A00003E4 = spec;
	sceSifSetSMflg(0x80000000);
	sceSifIntrOther();

	drv->send_buf = (void *)arg1;
	drv->send_len = arg2;
	return arg2;
}

int
drs_send_end(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x21;

	if (drv->debug & 0x400) {
		sceDeci2ExPanic("\t\tsif2 packet send end  flag = %x\n", drv->flag);
	}

	return 0;
}

int
drs_poll_2(struct drv_sif *drv, int arg1, int arg2)
{
	if (func_00000F50(drv) != 0) {
		if ((*D_ICR & 0x4000000) != 0 && (*I_STAT & 0x8) != 0) {
			while (*D_DMACEN != 0) {
				*D_DMACEN = 0;
			}

			*I_STAT = ~0x8;
			*D_ICR &= ~0xfb840000;

			while ((*D_ICR & 0x800000) != 0)
				;

			while (*D_DMACEN != 1) {
				*D_DMACEN = 1;
			}

			func_000003F4(drv);

			while (*D_DMACEN != 0) {
				*D_DMACEN = 0;
			}

			*D_ICR = (*D_ICR & 0xffffff) | 0x800000;

			while (*D_DMACEN != 1) {
				*D_DMACEN = 1;
			}
		}

		func_00000518(drv);

		if ((*I_STAT & 0x2) != 0) {
			*I_STAT = ~0x2;
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
drs_poll_1(struct drv_sif *drv, int arg1, int arg2)
{
	uint msflg;

	if (drv->flag & 0x100000 && sceSifCheckInit()) {
		sceSifDma2Init();
		D_A00003E4 = 0x100;
		drv->flag = (drv->flag & ~0x100000) | 0x200000;
		sceSifSetSMflg(0x80000000);
		sceSifIntrOther();
	} else if (drv->flag & 0x200000 && *I_STAT & 2) {
		msflg = sceSifGetMSflg();
		*I_STAT = ~2;
		if (msflg & 0x40000000) {
			sceSifSetMSflg(0x40000000);
			drv->flag &= ~0x200000;
			drs_if_func[6] = drs_poll_2;
			sceDeci2IfEventHandler(IFM_UP, sifdrv.iface, 0, 0, 0);
		}
	}

	return 0;
}

int
drs_rcv_off(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag |= 0x1000;

	if (drv->debug & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 rcv off flag = %x\n", drv->flag);
	}

	return 0;
}

int
drs_rcv_on(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x1000;

	if (drv->debug & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 rcv on flag = %x\n", drv->flag);
	}

	if (drv->flag & 0x8000) {
		func_00000184(drv);
	}

	return 0;
}

int
drs_send_off(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag |= 0x10;

	if (drv->debug & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 send off flag = %x\n", drv->flag);
	}

	return 0;
}

int
drs_send_on(struct drv_sif *drv, int arg1, int arg2)
{
	drv->flag &= ~0x10;

	if (drv->debug & 0x800) {
		sceDeci2ExPanic("\t\t\tsif2 send on flag = %x\n", drv->flag);
	}

	if (drv->flag & 1) {
		func_000002B4(drv);
	}

	return 0;
}

int
drs_debug(struct drv_sif *drv, int arg1, int arg2)
{
	drv->debug = arg1;
	// BUG: missing return
}

int
drs_shutdown(struct drv_sif *drv, int arg1, int arg2)
{
	uint msflg;

	if (drv->flag & 0x300000) {
		uint bits = 0x100000;
		drv->flag |= bits;
		drv->flag &= ~0x200000;

		// BUG: wrong return
		return;
	}

	sceDeci2IfEventHandler(IFM_DOWN, sifdrv.iface, 0, 0, 0);
	while (drv->flag & (0x100 | 0x1)) {
		sceDeci2ExPoll();
	}

	D_A00003E4 = 0x200;
	sceSifSetSMflg(0x80000000);
	sceSifIntrOther();

	while (1) {
		if (!(*I_STAT & 2)) {
			continue;
		}

		msflg = sceSifGetMSflg();
		*I_STAT = ~2;

		if (msflg & 0x40000000) {
			sceSifSetMSflg(0x40000000);
			drv->flag |= 0x100000;
			drv->flag &= ~0x200000;
			drs_if_func[6] = drs_poll_1;

			break;
		}
	}

	// BUG: missing return
}
