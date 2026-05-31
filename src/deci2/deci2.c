#include "common.h"
#include "deci2_internal.h"
#include "intrman_internal.h"
#include "introld.h"
#include "loadcore.h"
#include "loadcore_internal.h"
#include "sysclib_internal.h"
#include "sysmem_internal.h"
#include "thread_internal.h"

#include <deci2.h>
#include <intrman.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <thread.h>

ModuleInfo Module = { "Deci2_Manager", 0x105 };

#define MAX_SOCK 35

struct deci2_socket {
	/* 0x000 */ void (*handler)();
	/* 0x004 */ void *opt;
	/* 0x008 */ u_int proto;
	/* 0x00c */ u_int unkC;
	/* 0x010 */ u_int unk10;
	/* 0x014 */ u_int unk14;
	/* 0x018 */ u_int unk18;
	/* 0x01c */ u_int unk1C;
	/* 0x020 */ u_int unk20;
	/* 0x024 */ struct deci2_iface *iface;
};

struct deci2_manager {
	/* 0x000 */ int unk0;
	/* 0x004 */ int unk4;
	/* 0x008 */ int unk8;
	/* 0x00c */ int unkC;
	/* 0x010 */ void (*dbg_print_fn)(void *, int);
	/* 0x014 */ void *dbg_print_opt;
	/* 0x018 */ char unk18[0xc];
	/* 0x024 */ int isdbgp_sock;
	/* 0x028 */ struct deci2_socket sock[MAX_SOCK];
	/* 0x6a0 */ struct deci2_iface iface[2];
	/* 0x600 */ struct stru_66F0 *unk600;
	/* 0x604 */ int (*unk604)()
};

struct stru_6D10 {
	/* 0x0 */ int unk0;
	/* 0x4 */ char unk4[0x4020];
};

struct stru_66F0 {
	/* 0x0 */ char unk0[0x8];
};

/* 0x6700 */ struct deci2_manager d2m;
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
int func_00002A0C(int s);
void func_00002C08(int a1, u_short proto);

#if 1
INCLUDE_ASM("asm/deci2/nonmatchings/deci2", start);
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
	d2m.sock[0].proto = 1;
	d2m.sock[0].handler = func_00002A68;
	d2m.sock[0].opt = NULL;
	for (i = 1; i < 2; i++) {
		d2m.sock[i].proto = -1;
		d2m.sock[i].handler = func_00001BA0;
		d2m.sock[i].opt = (void *)&unk6D10[i];
		d2m.sock[i].unkC = 1;
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

	struct deci2_manager *d2 = opt;

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
	struct deci2_iface *iface;
	int oldstat;
	int i;

	CpuSuspendIntr(&oldstat);

	for (i = 0; i < 2; i++) {
		iface = &d2m.iface[i];

		if (d2m.iface[i].handler)
			iface->handler(12, iface->opt, 0, 0);
	}

	CpuResumeIntr(oldstat);
	return 0;
}

struct deci2_manager *
sceDeci2GetStatus()
{
	return &d2m;
}

void
sceDeci2SetDebugFormatRoutine(int (*fn)(const char *, va_list))
{
	d2m.unk604 = fn;
}

void
sceDeci2SetDebugFlags(u_int flags)
{
	d2m.unkC = flags;
}

int
func_00000360(u_short proto, void *opt, void (*handler)(int event, int param, void *opt))
{
	int *bootmode;
	int i;

	if (proto == 0 || proto >= 0xf000) {
		return -1;
	}

	for (i = 0; i < MAX_SOCK; i++) {
		if (d2m.sock[i].proto == proto) {
			return -3;
		}
	}

	bootmode = QueryBootMode(4);

	for (i = 0; i < MAX_SOCK; i++) {
		if (d2m.sock[i].handler == NULL) {
			memset(&d2m.sock[i], 0, sizeof(d2m.sock[i]));

			d2m.sock[i].proto = proto;
			d2m.sock[i].handler = handler;
			d2m.sock[i].opt = opt;

			if (bootmode && *(u_short *)bootmode == 0) {
				if (proto <= DECI2_PROTO_I0TTYP || proto >= DECI2_PROTO_I0TTYP + 9 || d2m.unkC) {
					sceDeci2ExPanic(" socket %2d proto=0x%x handler=0x%x opt=0x%x\n", i, proto,
					  handler, opt);
				}
			}

			if (proto == DECI2_PROTO_ISDBGP) {
				d2m.isdbgp_sock = i;
			}

			if (d2m.unk8) {
				func_00002C08(1, proto);
			}

			return i;
		}
	}

	return -4;
}

int
sceDeci2Open(u_short proto, void *opt, void (*handler)(int event, int param, void *opt))
{
	return CpuInvokeInKmode(func_00000360, proto, opt, handler);
}

int
func_00000558(int s)
{
	if (!func_00002A0C(s) || s < 3) {
		return -2;
	}

	if (!d2m.sock[s].handler) {
		return -2;
	}

	d2m.sock[s].proto = -1;

	while (d2m.sock[s].iface || d2m.sock[s].unk20) {
		sceDeci2ExPoll();
	}

	if (d2m.sock[s].proto == DECI2_PROTO_ISDBGP) {
		d2m.isdbgp_sock = 0;
	}

	memset(&d2m.sock[s], 0, sizeof(d2m.sock[s]));
	return 1;
}

int
sceDeci2Close(int s)
{
	return CpuInvokeInKmode(func_00000558, s);
}

int
sceDeci2ExRecv(int s, void *buf, u_short len)
{
	struct deci2_iface *iface;

	if (!func_00002A0C(s)) {
		return -2;
	}

	iface = d2m.sock[s].iface;
	if (!iface) {
		return -7;
	}

	if (!(iface->unk1C & 4)) {
		return -7;
	}

	if ((u_int)buf & 3) {
		return -5;
	}

	if (!iface->unk24) {
		if (len < 8) {
			return -6;
		}

		iface->unk2C = buf;
	}

	return iface->handler(1, iface->opt, buf, len);
}

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExReqSend);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ReqSend);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExSend);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExLock);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExUnLock);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExRecvSuspend);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExRecvUnSuspend);

int
sceDeci2ExPanic(const char *fmt, ...)
{
	va_list va;

	if (d2m.dbg_print_fn) {
		va_start(va, fmt);

		if (d2m.unk604) {
			return d2m.unk604(d2m.dbg_print_fn, d2m.dbg_print_opt, fmt, va);
		}

		return prnt(d2m.dbg_print_fn, d2m.dbg_print_opt, fmt, va);
	}

	return 0;
}

int
func_00000DA4(void *opt, const char *fmt, va_list ap)
{
	struct deci2_manager *d2 = opt;

	if (d2m.dbg_print_fn && fmt) {
		prnt(d2->dbg_print_fn, d2->dbg_print_opt, fmt, ap);
	}

	return 0;
}

int
func_00000DE8(void *opt, const char *fmt, va_list ap)
{
	return CpuInvokeInKmode(func_00000DA4, opt, fmt, ap);
}

void
sceDeci2DbgPrintStatus(void (*fn)(void *, int), void *opt)
{
	int *bootmode, i;

	d2m.dbg_print_fn = fn;
	d2m.dbg_print_opt = opt;
	KprintfSet(func_00000DE8, &d2m);

	bootmode = QueryBootMode(4);
	if (!bootmode || *(u_short *)bootmode) {
		return;
	}

	sceDeci2ExPanic("\n\nDECI2 start d2manCB = 0x%x debugflag = 0x%x intrhandlers = 0x%x \n", &d2m,
	  &d2m.unkC, &unk66F0);

	for (i = 0; i < MAX_SOCK; i++) {
		if (d2m.sock[i].handler) {
			sceDeci2ExPanic(" socket %2d proto=0x%x handler=0x%x opt=0x%x\n", i, d2m.sock[i].proto,
			  d2m.sock[i].handler, d2m.sock[i].opt);
		}
	}

	sceDeci2ExPanic("\r");
}

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2SetPollCallback);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExWakeupThread);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExSignalSema);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExSetEventFlag);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_0000121C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00001364);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2IfCreate);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2IfEventHandler);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00001BA0);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00001E20);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00001FC4);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_0000214C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002234);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_0000231C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_000025BC);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2ExPoll);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", sceDeci2Poll);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002904);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002914);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002A0C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002A40);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002A68);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002B10);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002C08);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002C9C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00002D14);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00003170);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_0000338C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_000033E0);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_0000352C);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00003564);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_000035D4);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00003600);

INCLUDE_ASM("asm/deci2/nonmatchings/deci2", func_00003668);
