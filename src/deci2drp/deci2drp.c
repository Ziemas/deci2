#include "Old/timerold.h"
#include "bits.h"
#include "common.h"
#include "deci2.h"
#include "deci2_internal.h"
#include "intrman_internal.h"
#include "loadcore.h"
#include "memory.h"
#include "sys/types.h"
#include "timerman.h"

ModuleInfo Module = { "Deci2_PIF_interface_driver", 0x101 };

#define PIFREG ((volatile struct pif_reg *)0xbf803800)
#define UNKHW ((volatile struct pif_reg *)0xbf803200)

struct pif_reg {
	u_short board_id;
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
	int flag;
	char unkC;
	u_char unkD;
	char unkE;
	char unkF;
	short fifo_size;
	int irq_num;
	int (*unk18)();
	u_short unk1C; // rx len?
	u_short unk20;
	u_char unk23;
	int unk24;
	int unk28;
	int unk2C;
};

struct drv_pif pifdrv;

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

int func_000002E8(struct drv_pif *drv);
void func_00000410(void *context, int c);
int func_000004C4(int func, struct drv_pif *drv, int a2, int a3);
void func_00000568(struct drv_pif *drv, int a2);
void func_00000608(struct drv_pif *drv);
void func_00000708(struct drv_pif *drv);
void func_00000E38();
void func_00000E7C();
void func_00000EA0();
void func_00000E5C();
int func_00000EC0(struct drv_pif *drv);
void func_000015FC(struct drv_pif *drv, int a2, int a3);
void func_00001378(struct drv_pif *drv, int *a2, int a3, int a4);
void func_00001588(struct drv_pif *drv);
int func_00001820();
int func_0000183C();

int
start()
{
	volatile struct pif_reg *pif = NULL;
	u_int v0, v1, v4;
	int prid;
	volatile int unk;

	v0 = read32(0xbf801004);
	v1 = read32(0xbf80101c);
	asm volatile("mfc0 %0, $15  \n"
				 "nop           \n"
	  : "=r"(prid));

	write32(0xbf801004, 0xbf802000);
	asm volatile("" ::: "memory");
	write32(0xbf80101c, 0xd3655);
	asm volatile("" ::: "memory");

	// ?? why ??
	unk = 0;
	unk = 0;
	unk = 0;
	unk = 0;

	v4 = PIFREG->board_id;
	if ((v4 == 0x4126 || v4 == 0x4127) && (PIFREG->unk4 & 0x100) == 0) {
		PIFREG->board_id = 0;
		v4 = PIFREG->board_id;
		if (v4 == 0x4126 || v4 == 0x4127) {
			pif = PIFREG;
		}
	}

	if (!pif) {
		write32(0xbf80101c, 0);
		read32(0xbf80101c);
		write32(0xbf801004, v0);
		write32(0xbf80101c, v1);

		return NO_RESIDENT_END;
	}

	pif->unk24 = 0xe0c0;
	pif->unk2C = 5;
	memset(&pifdrv, 0, sizeof(pifdrv));
	pifdrv.unkD = 16;
	pifdrv.unkF = 8;
	pifdrv.unkE = 1;
	pifdrv.fifo_size = 0x7f;
	if (PIFREG->board_id == 0x4126) {
		pifdrv.fifo_size = 0x3ff;
	}

	if (!func_000002E8(&pifdrv)) {
		return NO_RESIDENT_END;
	}

	sceDeci2DbgPrintStatus(func_00000410, &pifdrv);
	if (prid < 16 || (read32(0xbf801450) & 8) != 0) {
		pifdrv.irq_num = 10;
		pifdrv.unk18 = func_0000183C;

		pifdrv.iface = sceDeci2IfCreate(DECI2_NODE_HOST, &pifdrv, func_000004C4, func_0000183C);
	} else {
		pifdrv.irq_num = 23;
		pifdrv.unk18 = func_00001820;
		pifdrv.unkC = 1;

		if ((*((u_char *)0xBF803100) & 8) == 0) {
			int unk = *((char *)0xBF803204) & 0xF0;
			if ((unk == 0x20) || (unk == 0x30) || (unk == 0x50) || (unk == 0x70) || (unk == 0x60)) {
				pifdrv.unkC = pifdrv.unkC | 6;
			}
			if ((*((char *)0xBF803204) & 0xF0) == 0x40) {
				pifdrv.unkC = pifdrv.unkC | 2;
			}
		}

		pifdrv.iface = sceDeci2IfCreate(DECI2_NODE_HOST, &pifdrv, func_000004C4, func_00001820);
	}

	DisableDispatchIntr(pifdrv.irq_num);
	func_00000E38();
	EnableIntr(pifdrv.irq_num);

	sceDeci2IfEventHandler(5, pifdrv.iface, 0, 0, 0);

	return RESIDENT_END;
}

int
func_000002E8(struct drv_pif *drv)
{
	volatile struct pif_reg *pif = PIFREG;
	int timer;
	int i;

	timer = AllocHardTimer(1, 16, 1);
	SetTimerCounter(timer, 0);
	SetTimerMode(timer, 0);
	SetTimerCompare(timer, 0);

	pif->unkC = 0x1300;
	pif->unk30 = 4;
	i = 0;

	if ((pif->unk20 & 4) == 0) {
		while (1) {
			if (GetTimerCounter(timer) > 0x8235u) {
				i++;
				SetTimerCounter(timer, 0);
				if (i > 100) {
					pif->unkC = 0;
					FreeHardTimer(timer);
					return 1;
				}
			}

			if (pif->unk20 & 4) {
				break;
			}
		}
	}

	if (pif->unk20 & 4) {
		pif->unk2C = 4;

		if ((pif->unk8 & 0xf00) == 0xf00) {
			FreeHardTimer(timer);
			return 0;
		}
	}

	FreeHardTimer(timer);
	return 1;
}

void
func_00000410(void *context, int c)
{
	struct drv_pif *drv = context;
	int i;

	if (c >= 255) {
		return;
	}

	if (drv->flag & 0x80000000) {
		i = 0;
		if (!(PIFREG->unk20 & 2)) {
			while (1) {
				if (i++ > 0x27FFFFF) {
					break;
				}

				if ((PIFREG->unk20 & 2)) {
					break;
				}
			}
		}

		drv->flag &= ~0x80000000;
	}

	PIFREG->unk2C = 2;
	PIFREG->unkC = c & 0xff;
	PIFREG->unk30 = 1;
	drv->flag |= 0x80000000;
}

int
func_000004C4(int func, struct drv_pif *drv, int a2, int a3)
{
	if ((drv->unk4 & 0x200) && func != 6) {
		sceDeci2ExPanic("pif func %s flag=%x\n", drp_if_func_name[func], drv->flag);
	}

	return drp_if_func[func](drv, a2, a3);
}

void
func_00000568(struct drv_pif *drv, int a2)
{
	volatile struct pif_reg *pif = PIFREG;

	if ((drv->flag & 1) == 0) {
		return;
	}

	if (a2 == 0x1000) {
		a2 = ((pif->unk10 >> 12) ^ 1) & 1;
	} else if (a2 == 0x200) {
		a2 = pif->unk10 & 0x200;
	} else {
		a2 = -1;
	}

	if (a2 > 0) {
		drv->flag |= 0x20;
		func_00000EA0();
	} else if (a2 == 0) {
		func_00000E7C();
	}
}

void
func_00000608(struct drv_pif *drv)
{
	volatile struct pif_reg *pif = PIFREG;

	if (!pif->unk18 && (pif->unk10 & 8) == 0) {
		return;
	}

	func_00000E5C();
	drv->flag |= 0x2000;

	if (drv->unk1C == 0) {
		if (drv->unk4 & 0x100) {
			sceDeci2ExPanic("pif new rcv packet found flag = %x\n", drv->flag);
		}

		func_00001378(drv, (int *)&drv->unk1C, 2, 1);
		drv->unk24 = 0;

		if (drv->unk1C < 8) {
			drv->flag &= ~0x2000;
			sceDeci2ExPanic("pif receive packet have illegal length\n");
			func_00001588(drv);
		}
	} else {
		if (drv->unk4 & 0x200) {
			sceDeci2ExPanic("pif  rcv packet continus flag = %x\n", drv->flag);
		}
	}
}

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000708);

int
func_000008C4(struct drv_pif *drv, int a2, int a3)
{
	drv->flag |= 0x100;
	if (drv->unk4 & 0x100) {
		sceDeci2ExPanic("\tpif receive start\n");
	}

	return 0;
}

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000904);
// int func_00000904(struct drv_pif *drv, int, int) {}

int
func_00000A84(struct drv_pif *drv, int a2, int a3)
{
	volatile struct pif_reg *pif = PIFREG;

	pif->unk18 = 1;
	if (drv->unk4 & 0x100) {
		sceDeci2ExPanic("\tpif RxCounter Dec\n");
	}

	drv->flag &= ~0x2100;
	drv->unk1C = 0;

	if ((drv->flag & 0x1000) == 0) {
		func_00000E38();
		func_00000608(drv);
	}

	return 0;
}

int
func_00000B0C(struct drv_pif *drv, int a2, int a3)
{
	drv->flag |= 0x1;
	func_00000568(drv, 0x1000);

	if (drv->unk4 & 0x100) {
		sceDeci2ExPanic("\tpif send start flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000B6C(struct drv_pif *drv, int a2, int a3)
{
	int unk;

	unk = func_00000EC0(drv);
	if (unk < a3) {
		a3 = unk;
	}

	if (a3 > 0) {
		drv->unk2C = a3;
		func_000015FC(drv, a2, ((u_int)a3 + 3) >> 2);
		drv->flag |= 0x40;
	}

	return a3;
}

int
func_00000BEC(struct drv_pif *drv, int a2, int a3)
{
	volatile struct pif_reg *pif = PIFREG;

	pif->unk14 = 1;
	if (drv->unk4 & 0x100) {
		sceDeci2ExPanic("\tpif TxCounter inc\n");
	}

	drv->flag &= ~0x21;
	func_00000EA0();
	return 0;
}

int
func_00000C58(struct drv_pif *drv, int a2, int a3)
{
	if (drv->unk18()) {
		write32(0xbf801070, ~(1 << drv->irq_num));
		func_00000568(drv, 512);
		func_00000608(drv);
	}

	func_00000708(drv);
	return 0;
}

int
func_00000CC8(struct drv_pif *drv, int a2, int a3)
{
	drv->flag |= 0x1000;

	if (drv->unk4 & 0x200) {
		sceDeci2ExPanic("\tpif rcv off flag = %x\n", drv->flag);
	}

	func_00000E5C();

	return 0;
}

int
func_00000D10(struct drv_pif *drv, int a2, int a3)
{
	drv->flag &= ~0x1000;

	if (drv->unk4 & 0x200) {
		sceDeci2ExPanic("\tpif rcv on flag = %x\n", drv->flag);
	}

	if ((drv->flag & 0x6000) == 0) {
		func_00000E38();
	}

	return 0;
}

int
func_00000D7C(struct drv_pif *drv, int a2, int a3)
{
	drv->flag |= 0x10;

	if (drv->unk4 & 0x200) {
		sceDeci2ExPanic("\tpif send off flag = %x\n", drv->flag);
	}

	return 0;
}

int
func_00000DBC(struct drv_pif *drv, int a2, int a3)
{
	drv->flag &= ~0x10;

	if (drv->unk4 & 0x200) {
		sceDeci2ExPanic("\tpif send on flag = %x\n", drv->flag);
	}

	if (drv->flag & 1) {
		func_00000E7C();
	}

	return 0;
}

int
func_00000E28(struct drv_pif *drv, int a1, int a2)
{
	drv->unk4 = a1;
}

int
func_00000E30(struct drv_pif *drv, int a1, int a2)
{
}

void
func_00000E38()
{
	volatile struct pif_reg *pif = PIFREG;
	int v = pif->unk28;

	pif->unk28 = 0;
	pif->unk28 = v | 0x210;
}

void
func_00000E5C()
{
	volatile struct pif_reg *pif = PIFREG;

	pif->unk28 &= ~0x210;
}

void
func_00000E7C()
{
	volatile struct pif_reg *pif = PIFREG;
	int tmp = pif->unk28;

	pif->unk28 = 0;
	pif->unk28 = tmp | 0x1000;
}

void
func_00000EA0()
{
	volatile struct pif_reg *pif = PIFREG;

	pif->unk28 &= ~0x1000;
}

int
func_00000EC0(struct drv_pif *drv)
{
	volatile struct pif_reg *pif = PIFREG;
	int unk = pif->unk10 ^ 0x1c03;

	if (unk & 0x100) {
		return 0x4000;
	}

	if (unk & 0x200) {
		return 4 * (0x1000 - drv->fifo_size);
	}

	if (unk & 0x400) {
		return 0x2000;
	}

	if (unk & 0x800) {
		return 4 * (drv->fifo_size + 1);
	}

	if (unk & 0x1000) {
		return 4;
	}

	return 0;
}

int
func_00000F44(struct drv_pif *drv)
{
	volatile struct pif_reg *pif = PIFREG;
	int unk = pif->unk10 ^ 0x1c03;

	if (unk & 0x10) {
		return 0x4000;
	}

	if (unk & 0x8) {
		return 4 * (0x1000 - drv->fifo_size);
	}

	if (unk & 0x4) {
		return 0x2004;
	}

	if (unk & 2) {
		return 4 * (drv->fifo_size + 1);
	}

	if (unk & 1) {
		return 4;
	}

	return 0;
}

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00000FC4);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_000011A4);

INCLUDE_ASM("asm/deci2drp/nonmatchings/deci2drp", func_00001378);

/* Completely drain FIFO? */
void
func_00001588(struct drv_pif *drv)
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
