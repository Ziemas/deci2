#include "common.h"
#include "intrman_internal.h"
#include "introld.h"
#include "loadcore.h"
#include "loadcore_internal.h"
#include "thread_internal.h"

#include <deci2.h>
#include <intrman.h>
#include <stdio.h>
#include <string.h>
#include <thread.h>

ModuleInfo Module = { "Deci2_Manager", 0x105 };

struct d2_unk_28 {
	/* 0x000 */ void (*unk0)();
	/* 0x004 */ char *unk4;
	/* 0x008 */ u_int unk8;
	/* 0x008 */ u_int unkC;
	/* 0x010 */ char unk10[0x18];
};

struct d2_iface {
	/* 0x0 */ int unk0;
	/* 0x0 */ int (*unk4)();
	/* 0x8 */ void *unk8;
	/* 0xc */ char unkC[0x24];
};

struct d2_mgr {
	/* 0x000 */ int unk0;
	/* 0x004 */ int unk4;
	/* 0x008 */ int unk8;
	/* 0x00c */ int unkC;
	/* 0x010 */ char unk10[0x18];
	/* 0x028 */ struct d2_unk_28 unk28[2];
	/* 0x07c */ char unk7C[0x528];
	/* 0x6a0 */ struct d2_iface iface[2];
	/* 0x600 */ struct stru_66F0 *unk600;
	/* 0x604 */ u_int unk604;
};

struct stru_6D10 {
	/* 0x0 */ int unk0;
	/* 0x4 */ char unk4[0x4020];
};

struct stru_66F0 {
	/* 0x0 */ char unk0[0x8];
};

/* 0x6700 */ struct d2_mgr d2m;
/* 0x66f0 */ struct stru_66F0 unk66F0;
/* 0x6d10 */ struct stru_6D10 unk6D10[2];
//* 0x6d10 */ char unk6D10[0x8048];

extern libhead deci2api_stub;
extern libhead deci2log_stub;

void func_00001BA0();
void func_00002A68();
int func_0000121C(void *);
void func_00002A40();
int func_00003760(); // should be in sdb header
int func_000001F4(void *opt);
int func_00000240(void *opt);

#if 1
INCLUDE_ASM("asm/deci2/nonmatchings/main", start);
#else
int
start()
{
	int oldstat, i;
	int *bootmode;
	int bm[2];

	memset(&d2m, 0, sizeof(d2m));
	memset(&unk6D10, 0, sizeof(unk6D10));
	memset(&unk66F0, 0, sizeof(unk66F0));
	d2m.unk600 = &unk66F0;

	bootmode = QueryBootMode(1);
	if (bootmode) {
		return 1;
	}

	if (RegisterLibraryEntries(&deci2api_stub)) {
		return 1;
	}

	if (RegisterLibraryEntries(&deci2log_stub)) {
		return 1;
	}

	CpuSuspendIntr(&oldstat);
	d2m.unk28[0].unk8 = 1;
	d2m.unk28[0].unk0 = func_00002A68;
	d2m.unk28[0].unk4 = NULL;
	for (i = 1; i < 2; i++) {
		d2m.unk28[i].unk8 = -1;
		d2m.unk28[i].unk0 = func_00001BA0;
		d2m.unk28[i].unk4 = (void *)&unk6D10[i];
		d2m.unk28[i].unkC = 1;
		unk6D10[i].unk0 = i;
	}

	bm[0] = 0x10002;
	bm[1] = 0;
	RegisterBootMode(bm);

	RegisterIntrHandler(0x3e, 1, func_0000121C, &d2m);
	func_00002A40();
	func_00003760();
	AddRebootNotifyHandler(func_000001F4, 2, 0);
	AddRebootNotifyHandler(func_00000240, 3, 0);
	CpuResumeIntr(oldstat);

	SetEventFlag(GetSystemStatusFlag(), 0x20);
	return 0;
}
#endif

int
func_000001F4(void *opt)
{
	static char header[] = "\nIOP DECI2 manager Version 0.9.4"
						   "\n    Copyright 1999,2000,2003 (C) Sony Computer Entertainment Inc. \n";
	static char start_msg[] = "\nDECI2 manager start.\n";

	d2m.unk8 = 1;
	CpuEnableIntr();
	printf(header);
	printf(start_msg);
	return 0;
}

int
func_00000240(void *opt)
{

	struct d2_mgr *d2 = opt;

	CpuEnableIntr();
	if (!d2->unk0) {
		ChangeThreadPriority(0, 126);
		while (1) {
			sceDeci2Poll();
			DelayThread(2000000);
		}
	}

	return 0;
}

int
sceDeci2Shutdown()
{
	struct d2_iface *iface;
	int oldstat;
	int i;

	CpuSuspendIntr(&oldstat);

	for (i = 0; i < 2; i++) {
		iface = &d2m.iface[i];

		if (d2m.iface[i].unk4)
			iface->unk4(12, iface->unk8, 0, 0);
	}

	CpuResumeIntr(oldstat);
	return 0;
}

struct d2_mgr *
sceDeci2GetStatus()
{
	return &d2m;
}

void
sceDeci2SetDebugFormatRoutine(u_int a1)
{
	d2m.unk604 = a1;
}

void
sceDeci2SetDebugFlags(u_int flags)
{
	d2m.unkC = flags;
}

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000360);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000524);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000558);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2Close);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExRecv);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00005AB0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExReqSend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ReqSend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExSend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExLock);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExUnLock);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExRecvSuspend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExRecvUnSuspend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExPanic);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000DA4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000DE8);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2DbgPrintStatus);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2SetPollCallback);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExWakeupThread);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExSignalSema);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExSetEventFlag);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000121C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001364);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2IfCreate);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2IfEventHandler);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001BA0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001E20);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001FC4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000214C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002234);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000231C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000025BC);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExPoll);

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
