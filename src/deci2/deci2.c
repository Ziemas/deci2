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
#define MAX_INTERFACE 2

struct deci2_socket {
	/* 0x000 */ void (*handler)(int, int, void *);
	/* 0x004 */ void *opt;
	/* 0x008 */ u_int proto;
	/* 0x00c */ u_int ext_flags;
	/* 0x010 */ u_int dst_node;
	/* 0x014 */ int wakepup_type;
	/* 0x018 */ u_int wakeup_arg1;
	/* 0x01c */ u_int wakeup_arg2;
	/* 0x020 */ struct deci2_iface *send_if;
	/* 0x024 */ struct deci2_iface *read_if;
};

struct deci2_manager {
	/* 0x000 */ int lock_holder;
	/* 0x004 */ int unk4;
	/* 0x008 */ int unk8;
	/* 0x00c */ int debug_flag;
	/* 0x010 */ void (*dbg_print_fn)(void *, int);
	/* 0x014 */ void *dbg_print_opt;
	/* 0x018 */ int (*poll_cb)();
	/* 0x01C */ struct deci2_iface *unk1C;
	/* 0x020 */ struct deci2_iface *unk20;
	/* 0x024 */ int isdbgp_sock;
	/* 0x028 */ struct deci2_socket sock[MAX_SOCK];
	/* 0x5a0 */ struct deci2_iface iface[MAX_INTERFACE];
	/* 0x600 */ void **unk600;
	/* 0x604 */ int (*dbg_print)();
};

struct deci2_relay {
	/* 0x00 */ int flag;
	/* 0x04 */ int sock;
	/* 0x08 */ int protocol;
	/* 0x0c */ int unkC;
	/* 0x10 */ struct deci2_iface *unk10;
	/* 0x14 */ int unk14;
	/* 0x18 */ int unk18;
	/* 0x1c */ int rpos;
	/* 0x20 */ int wpos;
	/* 0x24 */ char buf[0x4000];
};

/* 0x6700 */ struct deci2_manager d2m;
/* 0x66f0 */ void *unk66F0[MAX_INTERFACE];
/* 0x6d10 */ struct deci2_relay relay[2];

extern libhead deci2api_stub;
extern libhead deci2log_stub;

void relay_handler();
void func_00002A68();
int func_0000121C(void *);
void func_00002A40();
int func_00003760(); // should be in sdb header
int func_000001F4(void *opt);
int func_00000240(void *opt);
void error_handler(int, int, void *);
void relay_rcv_packet(struct deci2_iface *src, struct deci2_iface *dst, int len, int protocol,
  int node);
void error_rcv_packet(struct deci2_iface *iface, int len, int protocol, int node, int unk);
void interface_Packet_send_done(struct deci2_iface *iface);
void bind_socket_interface();
void deliver_rcv_packet(struct deci2_iface *iface, int len, int protocol, int node);
void func_00002904();
int new_bind_poll();
int valid_socket(int s);
void func_00002B10(int, void *);
void func_00002C08(int, int);

int
start()
{
	int oldstat, i;
	int *bootmode;
	int bm[2];

	memset(&d2m, 0, sizeof(d2m));
	memset(&relay, 0, sizeof(relay));
	memset(&unk66F0, 0, sizeof(unk66F0));
	d2m.unk600 = unk66F0;

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
	for (i = 0; i < 2; i++) {
		d2m.sock[i + 1].proto = -1;
		d2m.sock[i + 1].handler = relay_handler;
		d2m.sock[i + 1].opt = &relay[i];
		d2m.sock[i + 1].ext_flags = 1;

		relay[i].sock = i + 1;
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
	if (!d2->lock_holder) {
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

	for (i = 0; i < MAX_INTERFACE; i++) {
		iface = &d2m.iface[i];

		if (d2m.iface[i].handler)
			iface->handler(IFF_SHUTDOWN, iface->opt, 0, 0);
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
	d2m.dbg_print = fn;
}

void
sceDeci2SetDebugFlags(u_int flags)
{
	d2m.debug_flag = flags;
}

int
sceDeci2ExOpen(u_short proto, void *opt, void (*handler)(int event, int param, void *opt))
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
				if (proto <= DECI2_PROTO_I0TTYP || proto >= DECI2_PROTO_I0TTYP + 9 ||
				  d2m.debug_flag) {
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
	return CpuInvokeInKmode(sceDeci2ExOpen, proto, opt, handler);
}

int
sceDeci2ExClose(int s)
{
	if (!valid_socket(s) || s < 3) {
		return -2;
	}

	if (!d2m.sock[s].handler) {
		return -2;
	}

	d2m.sock[s].proto = -1;

	while (d2m.sock[s].read_if || d2m.sock[s].send_if) {
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
	return CpuInvokeInKmode(sceDeci2ExClose, s);
}

int
sceDeci2ExRecv(int s, void *buf, u_short len)
{
	struct deci2_iface *iface;

	if (!valid_socket(s)) {
		return -2;
	}

	iface = d2m.sock[s].read_if;
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

	return iface->handler(IFF_RCV_READ, iface->opt, buf, len);
}

int
sceDeci2ExReqSend(int s, char dest)
{
	struct deci2_socket *sock;
	struct deci2_iface *iface;
	int i;

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	sock = &d2m.sock[s];

	if (sock->send_if) {
		return DECI2_ERR_WOULDBLOCK;
	}

	if (dest == DECI2_NODE_HOST) {
		if (!d2m.unk1C) {
			if (!d2m.unk20) {
				return DECI2_ERR_NOHOSTIF;
			}

			return DECI2_ERR_NOROUTE;
		}

		if (d2m.debug_flag & 2) {
			sceDeci2ExPanic("socket=%d point to if=%d (host route)\n", sock - d2m.sock,
			  d2m.unk1C - d2m.iface);
		}

		sock->send_if = d2m.unk1C;
		sock->dst_node = DECI2_NODE_HOST;
		bind_socket_interface();
		return 1;
	}

	iface = d2m.iface;
	for (i = 0; i < MAX_INTERFACE; i++) {
		if (iface->node == dest) {
			if (!(iface->flags & 1)) {
				return DECI2_ERR_NOROUTE;
			}

			if (d2m.debug_flag & 2) {
				sceDeci2ExPanic("socket=%d point to if=%d\n", sock - d2m.sock, iface - d2m.iface);
			}

			sock->send_if = iface;
			sock->dst_node = dest;
			bind_socket_interface();
			return 1;
		}

		iface++;
	}

	bind_socket_interface();
	return 1;
}

int
sceDeci2ReqSend(int s, char dest)
{
	int ret;

	if (!valid_socket(s) || s < 3) {
		return DECI2_ERR_INVALSOCK;
	}

	ret = CpuInvokeInKmode(sceDeci2ExReqSend, s, dest);
	if (ret == 1) {
		CpuInvokeInKmode(new_bind_poll);
	}

	if (d2m.unk4 & 2) {
		func_00002904();
	}

	return ret;
}

int
sceDeci2ExSend(int s, void *buf, unsigned short len)
{
	struct deci2_iface *iface;

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	iface = d2m.sock[s].send_if;

	if (!iface) {
		return DECI2_ERR_INVALSOCK;
	}

	if (iface->send != &d2m.sock[s]) {
		return DECI2_ERR_INVALSOCK;
	}

	if (!(iface->flags & 8)) {
		return DECI2_ERR_WOULDBLOCK;
	}

	if ((u_int)buf & 3) {
		return DECI2_ERR_INVALADDR;
	}

	if (!iface->unk14) {
		if (len < 8) {
			return DECI2_ERR_PKTSIZE;
		}

		iface->unk14 = *(u_short *)buf;
	}

	return iface->handler(IFF_SEND_WRITE, iface->opt, buf, len);
}

int
sceDeci2ExLock(int s)
{
	if (d2m.lock_holder) {
		return DECI2_ERR_ALREADYLOCK;
	}

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	if (s > 2) {
		d2m.lock_holder = s;
		return 1;
	}

	return DECI2_ERR_INVALSOCK;
}

int
sceDeci2ExUnLock(int s)
{
	if (!d2m.lock_holder) {
		return DECI2_ERR_NOTLOCKED;
	}

	if (!valid_socket(s) || s <= 2) {
		return DECI2_ERR_INVALSOCK;
	}

	if (d2m.unk4 & 1) {
		func_00002C08(2, 0);
	}

	d2m.lock_holder = 0;
	d2m.unk4 &= ~1;
	bind_socket_interface();
	return 1;
}

int
sceDeci2ExRecvSuspend(int s)
{
	struct deci2_socket *sock;

	sock = &d2m.sock[s];

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	if (s <= 2) {
		return DECI2_ERR_INVALSOCK;
	}

	if (sock->read_if) {
		sock->read_if->handler(IFF_RCV_OFF, sock->read_if->opt, 0, 0);
		return 1;
	}

	return DECI2_ERR_INVALSOCK;
}

int
sceDeci2ExRecvUnSuspend(int s)
{
	struct deci2_socket *sock;

	sock = &d2m.sock[s];

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	if (s <= 2) {
		return DECI2_ERR_INVALSOCK;
	}

	if (sock->read_if) {
		sock->read_if->unk1C |= 2;
		sock->read_if->handler(IFF_RCV_ON, sock->read_if->opt, 0, 0);
		return 1;
	}

	return DECI2_ERR_INVALSOCK;
}

int
sceDeci2ExPanic(const char *fmt, ...)
{
	va_list va;

	if (d2m.dbg_print_fn) {
		va_start(va, fmt);

		if (d2m.dbg_print) {
			return d2m.dbg_print(d2m.dbg_print_fn, d2m.dbg_print_opt, fmt, va);
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
	  &d2m.debug_flag, &unk66F0);

	for (i = 0; i < MAX_SOCK; i++) {
		if (d2m.sock[i].handler) {
			sceDeci2ExPanic(" socket %2d proto=0x%x handler=0x%x opt=0x%x\n", i, d2m.sock[i].proto,
			  d2m.sock[i].handler, d2m.sock[i].opt);
		}
	}

	sceDeci2ExPanic("\r");
}

int (*sceDeci2SetPollCallback(int (*cb)()))()
{
	void *ret = d2m.poll_cb;

	d2m.poll_cb = cb;

	return ret;
}

int
sceDeci2ExWakeupThread(int s, int thid)
{
	struct deci2_socket *sock;
	struct deci2_socket *dbg_sock;

	sock = &d2m.sock[s];

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	if (sock->wakepup_type == 1 && sock->wakeup_arg1 == thid) {
		sock->wakeup_arg2++;
	} else {
		if (sock->wakepup_type) {
			return DECI2_ERR_INVALID;
		}

		sock->wakepup_type = 1;
		sock->wakeup_arg1 = thid;
		sock->wakeup_arg2 = 1;
	}

	if (d2m.isdbgp_sock) {
		d2m.unk4 |= 2;
		(&d2m.sock[d2m.isdbgp_sock])->handler(10, 0, (&d2m.sock[d2m.isdbgp_sock])->opt);
	}

	return 1;
}

int
sceDeci2ExSignalSema(int s, int semid)
{
	struct deci2_socket *sock;
	struct deci2_socket *dbg_sock;

	sock = &d2m.sock[s];

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	if (sock->wakepup_type == 2 && sock->wakeup_arg1 == semid) {
		sock->wakeup_arg2++;
	} else {
		if (sock->wakepup_type) {
			return DECI2_ERR_INVALID;
		}

		sock->wakepup_type = 2;
		sock->wakeup_arg1 = semid;
		sock->wakeup_arg2 = 1;
	}

	if (d2m.isdbgp_sock) {
		d2m.unk4 |= 2;
		(&d2m.sock[d2m.isdbgp_sock])->handler(10, 0, (&d2m.sock[d2m.isdbgp_sock])->opt);
	}

	return 1;
}

int
sceDeci2ExSetEventFlag(int s, int evfid, unsigned long bitpattern)
{
	struct deci2_socket *sock;
	struct deci2_socket *dbg_sock;

	sock = &d2m.sock[s];

	if (!valid_socket(s)) {
		return DECI2_ERR_INVALSOCK;
	}

	if (sock->wakepup_type == 3 && sock->wakeup_arg1 == evfid) {
		sock->wakeup_arg2 |= bitpattern;
	} else {
		if (sock->wakepup_type) {
			return DECI2_ERR_INVALID;
		}

		sock->wakepup_type = 3;
		sock->wakeup_arg1 = evfid;
		sock->wakeup_arg2 = bitpattern;
	}

	if (d2m.isdbgp_sock) {
		d2m.unk4 |= 2;
		(&d2m.sock[d2m.isdbgp_sock])->handler(10, 0, (&d2m.sock[d2m.isdbgp_sock])->opt);
	}

	return 1;
}

int
func_0000121C(void *arg)
{
	struct deci2_manager *d2 = arg;
	int i, j;

	d2->unk4 &= ~2;

	for (i = 0; i < MAX_SOCK; i++) {
		if (d2->sock[i].handler && d2->sock[i].wakepup_type > 0) {
			switch (d2->sock[i].wakepup_type) {
			case 1:
				for (j = 0; j < d2->sock[i].wakeup_arg2; j++) {
					iWakeupThread(d2->sock[i].wakeup_arg1);
				}
				break;
			case 2:
				for (j = 0; j < d2->sock[i].wakeup_arg2; j++) {
					iSignalSema(d2->sock[i].wakeup_arg1);
				}
				break;
			case 3:
				iSetEventFlag(d2->sock[i].wakeup_arg1, d2->sock[i].wakeup_arg2);
				break;
			}

			d2->sock[i].wakepup_type = 0;
		}
	}

	return 0;
}

struct if_param {
	u_short node;
	void *opt;
	void *handler;
	void *interrupt;
};

// sceDeci2ExIfCreate
struct deci2_iface *
sceDeci2ExIfCreate(struct if_param *ifp)
{
	struct deci2_iface *iface;
	int *bm;
	int i;

	for (i = 0; i < MAX_INTERFACE; i++) {
		if (!d2m.iface[i].handler) {

			if (ifp->node == DECI2_NODE_HOST) {
				d2m.unk20 = &d2m.iface[i];
			}

			d2m.iface[i].node = ifp->node;
			d2m.iface[i].handler = ifp->handler;
			d2m.iface[i].opt = ifp->opt;
			d2m.unk600[i] = ifp->interrupt;

			bm = QueryBootMode(4);
			if (bm && !*(u_short *)bm) {
				sceDeci2ExPanic(" interface %d dest=0x%x handler=0x%x opt=0x%x interface=0x%x\n\r",
				  i, ifp->node, ifp->handler, ifp->opt, &d2m.iface[i]);
			}

			iface = &d2m.iface[i];
			iface->handler(IFF_DEBUG, iface->opt, d2m.debug_flag, 0);
			return iface;
		}
	}

	return NULL;
}

struct deci2_iface *
sceDeci2IfCreate(short node, void *opt, int (*handler)(), int (*interrupt)())
{
	struct if_param ifp;

	ifp.node = node;
	ifp.opt = opt;
	ifp.handler = handler;
	ifp.interrupt = interrupt;

	return (struct deci2_iface *)CpuInvokeInKmode(sceDeci2ExIfCreate, &ifp);
}

void
sceDeci2IfEventHandler(int event, struct deci2_iface *iface, int len, int protocol, int node)
{
	int *bm;

	switch (event) {
	case IFM_IN:
		if (d2m.debug_flag & 3) {
			sceDeci2ExPanic("IFM_IN event from if driver %d\n", iface - d2m.iface);
		}

		if (!iface->rcv) {
			deliver_rcv_packet(iface, len, protocol, node);
			iface->handler(IFF_DEBUG, iface->opt, d2m.debug_flag, 0);
			iface->handler(IFF_RCV_START, iface->opt, 0, 0);
		}

		if (d2m.debug_flag & 2) {
			sceDeci2ExPanic("Send DECI2_READ event to socket=%d\n", iface->rcv - d2m.sock);
		}

		iface->unk1C |= 4;
		iface->rcv->handler(DECI2_READ, len, iface->rcv->opt);
		iface->unk1C &= ~4;
		break;
	case IFM_INDONE:
		if (d2m.debug_flag & 3) {
			sceDeci2ExPanic("IFM_INDONE event from if driver %d\n", iface - d2m.iface);
		}

		if (iface->rcv) {
			if (!iface->unk24) {
				// len from header?
				iface->unk24 = *(u_short *)iface->unk2C;
			}

			if (iface->unk24 < iface->unk28 + len) {
				if (iface->unk24 + 3 < iface->unk28 + len) {
					sceDeci2ExPanic("IFM_INDONE: Recieve packet too large %d>%d\n",
					  iface->unk28 + len, iface->unk24);
				}

				len = iface->unk24 - iface->unk28;
			}

			iface->unk28 += len;
			if (iface->rcv->ext_flags & 1) {
				if ((d2m.debug_flag & 2) != 0) {
					sceDeci2ExPanic("Send DECI2Ex_RflagDone event to socket=%d\n",
					  iface->rcv - d2m.sock);
				}

				iface->rcv->handler(DECI2Ex_RflagDone, len, iface->rcv->opt);
			}

			if (iface->unk24 <= iface->unk28) {
				if ((d2m.debug_flag & 2) != 0) {
					sceDeci2ExPanic("Send DECI2_READDONE event to socket=%d\n",
					  iface->rcv - d2m.sock);
				}

				iface->rcv->handler(DECI2_READDONE, d2m.debug_flag, iface->rcv->opt);
				iface->rcv->read_if = 0;
				iface->rcv = NULL;
				iface->handler(IFF_RCV_END, iface->opt, 0, 0);
			}
		} else {
			sceDeci2ExPanic("IFM_INDONE: Recieve Socket not found\n");
		}
		break;
	case IFM_OUT:
		if (d2m.debug_flag & 3) {
			sceDeci2ExPanic("IFM_OUT event from if driver %d\n", iface - d2m.iface);
		}

		if (iface->send) {
			iface->flags &= ~0x10;
			iface->flags |= 0x8;

			if (d2m.debug_flag & 2) {
				sceDeci2ExPanic("Send DECI2_WRITE event to socket=%d\n", iface->send - d2m.sock);
			}

			iface->send->handler(DECI2_WRITE, 0, iface->send->opt);
			iface->flags &= ~0x8;
		} else {
			sceDeci2ExPanic("IFM_OUT: Send Socket not found\n");
		}
		break;
	case IFM_OUTDONE:
		if (d2m.debug_flag & 3) {
			sceDeci2ExPanic("IFM_OUTDONE event from if driver %d\n", iface - d2m.iface);
		}

		if (iface->send) {
			iface->unk18 += len;
			if (len > 0) {
				if (iface->send->ext_flags & 1) {
					if (d2m.debug_flag & 2) {
						sceDeci2ExPanic("Send DECI2Ex_WflagDone event to socket=%d\n",
						  iface->send - d2m.sock);
					}

					iface->send->handler(DECI2Ex_WflagDone, len, iface->send->opt);
				}
			}

			if (iface->unk14 > 0 && iface->unk18 >= iface->unk14) {
				interface_Packet_send_done(iface);
			}
		} else {
			sceDeci2ExPanic("IFM_OUTDONE: Send Socket not found\n");
		}
		break;
	case IFM_UP:
		iface->flags |= 1;
		bm = QueryBootMode(4);
		if (bm && !*(u_short *)bm) {
			sceDeci2ExPanic(" interface %d active\n\r", iface - d2m.iface);
		}
		func_00002C08(4, iface->node);
		break;
	case IFM_DOWN:
		iface->flags &= ~1;
		break;
	}
}

// Relay socket handler
void
relay_handler(int event, int param, void *opt)
{
	struct deci2_relay *rly = opt;
	struct deci2_socket *sock = &d2m.sock[rly->sock];

	switch (event) {
	case 1:
		sceDeci2ExRecv(rly->sock, &rly->buf[rly->rpos], 0x4000 - rly->rpos);
		break;
	case 7:
		if (!rly->unk14) {
			rly->unk14 = *(u_short *)rly->buf;
		}

		rly->rpos += param;
		rly->unk18 += param;
		if (rly->unk18 >= rly->unk14 || rly->rpos >= 0x4000) {
			rly->unk10->handler(IFF_RCV_OFF, rly->unk10->opt, 0, 0);
			rly->flag |= 4;
			rly->wpos = 0;
			if (rly->flag & 2) {
				rly->flag &= ~2;
				sock->send_if->flags |= 2;
				sock->send_if->handler(IFF_SEND_ON, sock->send_if->opt, 0, 0);
			}
		}

		break;
	case 8:
		if (!rly->unk14 || (rly->unk18 < rly->unk14 && rly->rpos < 0x4000)) {
			rly->flag |= 2;
			sock->send_if->handler(IFF_SEND_OFF, sock->send_if->opt, 0, 0);
		}
		break;
	case 3:
		sceDeci2ExSend(rly->sock, &rly->buf[rly->wpos], rly->rpos - rly->wpos);
		break;
	case 9:
		rly->wpos += param;
		if (rly->wpos >= rly->rpos) {
			rly->rpos = 0;
			rly->flag &= ~4;
			rly->unk10->unk1C |= 2;
			rly->unk10->handler(IFF_RCV_ON, rly->unk10->opt, 0, 0);

			if (rly->unk14 > rly->unk18) {
				rly->flag |= 2;
				sock->send_if->handler(IFF_SEND_OFF, sock->send_if->opt, 0, 0);
			}
		}
		break;
	case 4:
		rly->flag &= ~1;
		break;
	case 2:
		break;
	default:
		sceDeci2ExPanic("RelayInOut: unknown event 0x%x\n", event);
		break;
	}
}

void
error_handler(int event, int param, void *opt)
{
	struct deci2_relay *rly = opt;
	uint unk18;

	switch (event) {
	case 1:
	case 2:
		if (rly->flag & 0x20) {
			func_00002B10(rly->unkC, rly->buf);
			rly->flag &= ~0x20;
		}

		if (event == 2) {
			rly->flag &= ~0x10;
		}

		if (event != 1) {
			return;
		}

		unk18 = rly->unk18;
		if (unk18 < 0x18) {
			if (d2m.debug_flag & 3) {
				sceDeci2ExPanic("ErrorIn: read error packet header\n");
			}

			rly->unk18 += sceDeci2ExRecv(rly->sock, rly->buf + rly->unk18, 0x4000 - rly->unk18);

		} else {
			if (d2m.debug_flag & 3) {
				sceDeci2ExPanic("ErrorIn: skip error packet\n");
			}

			rly->unk18 += sceDeci2ExRecv(rly->sock, rly->buf, 0x4000);
		}

		if (unk18 >= 0x18u) {
			return;
		}

		if (rly->unk18 < 0x18u && rly->unk18 < rly->unk14) {
			return;
		}

		rly->flag |= 0x20;
		break;
	case 7:
		break;
	default:
		sceDeci2ExPanic("ErrorIn: unknown event 0x%x\n", event);
		return;
	}
}

void
relay_rcv_packet(struct deci2_iface *src, struct deci2_iface *dst, int len, int protocol, int node)
{
	struct deci2_socket *sock;
	struct deci2_relay *rly;

	rly = &relay[src - d2m.iface];
	sock = &d2m.sock[rly->sock];

	if (rly->flag) {
		sceDeci2ExPanic("relay_rcv_packet: flag = 0x%x\n", rly->flag);
	}

	if (d2m.debug_flag & 2) {
		sceDeci2ExPanic("Relay start slot %d (sock=%d) %c->%c\n", src - d2m.iface, sock - d2m.sock,
		  src->node, dst->node);
	}

	rly->flag = 1;
	rly->unkC = 0;
	rly->rpos = 0;
	rly->wpos = 0;
	rly->unk14 = 0;
	rly->unk18 = 0;
	rly->protocol = protocol;
	rly->unk10 = src;

	sock->handler = relay_handler;
	sock->send_if = dst;
	sock->dst_node = node;
	sock->read_if = src;
	src->rcv = sock;

	bind_socket_interface();
}

void
error_rcv_packet(struct deci2_iface *iface, int len, int protocol, int node, int a4)
{
	struct deci2_socket *sock;
	struct deci2_relay *rly;

	rly = &relay[iface - d2m.iface];
	sock = &d2m.sock[rly->sock];

	if (rly->flag) {
		sceDeci2ExPanic("error_rcv_packet: flag = 0x%x\n", rly->flag);
	}

	rly->flag = 0x10;
	rly->unkC = a4;
	rly->rpos = 0;
	rly->wpos = 0;
	rly->unk14 = 0;
	rly->unk18 = 0;

	sock->handler = error_handler;
	sock->read_if = iface;
	iface->rcv = sock;
}

void
interface_Packet_send_done(struct deci2_iface *iface)
{
	struct deci2_socket *sock;

	if (iface->send->send_if != iface) {
		sceDeci2ExPanic("interface_packet_send_done: 0x%x != 0x%x\n", iface->send->send_if, iface);
	}

	sock = iface->send;

	iface->send->send_if = NULL;
	iface->send = NULL;

	iface->handler(IFF_SEND_END, iface->opt, 0, 0);

	if (d2m.debug_flag & 2) {
		sceDeci2ExPanic("Send DECI2_WRITEDONE event to socket=%d\n", sock - d2m.sock);
	}

	sock->handler(DECI2_WRITEDONE, 0, sock->opt);
	bind_socket_interface();
}

void
bind_socket_interface()
{
	struct deci2_iface *iface;
	struct deci2_socket *sock;
	int i, s;

	if (d2m.debug_flag & 2) {
		sceDeci2ExPanic("bind_socket_interface()\n");
	}

	for (i = 0, iface = d2m.iface; i < MAX_INTERFACE; i++, iface++) {
		if (iface->send) {
			continue;
		}

		for (s = 0, sock = d2m.sock; s < MAX_SOCK; s++, sock++) {
			if (d2m.lock_holder > 0 && s == 3) {
				sock = &d2m.sock[d2m.lock_holder];
			}

			if (sock->send_if == iface) {
				iface->send = sock;
				iface->unk14 = 0;
				iface->unk18 = 0;
				iface->flags |= 0x10;

				if (d2m.debug_flag & 2) {
					sceDeci2ExPanic("BIND socket=%d and if=%d\n", sock - d2m.sock,
					  iface - d2m.iface);
				}

				if (sock->ext_flags & 1) {
					if (d2m.debug_flag & 2) {
						sceDeci2ExPanic("Send DECI2Ex_WriteStart event to socket=%d\n",
						  sock - d2m.sock);
					}

					sock->handler(DECI2Ex_WriteStart, 0, sock->opt);
				}

				if (sock->proto != -1) {
					iface->handler(IFF_SEND_START, iface->opt, sock->proto, sock->dst_node);
				} else {
					struct deci2_relay *opt = sock->opt;
					iface->handler(IFF_SEND_START, iface->opt, opt->protocol, sock->dst_node);
				}

				break;
			}

			if (d2m.lock_holder > 0 && s == 3) {
				break;
			}
		}
	}

	if (d2m.debug_flag & 2) {
		sceDeci2ExPanic("bind_socket_interface() end\n");
	}
}

void
deliver_rcv_packet(struct deci2_iface *iface, int len, int protocol, int node)
{
	if (iface->rcv) {
		sceDeci2ExPanic("deliver_rcv_packet: rcvsocket %x\n", iface->rcv);
		return;
	}

	iface->unk24 = 0;
	iface->unk28 = 0;

	if (node == DECI2_NODE_HOST) {
		if (!d2m.unk1C) {
			error_rcv_packet(iface, len, protocol, node, 0);
		} else {
			relay_rcv_packet(iface, d2m.unk1C, len, protocol, node);
		}

		return;
	}

	if (node != DECI2_NODE_IOP) {
		int i;

		for (i = 0; i < MAX_INTERFACE; i++) {
			if (d2m.iface[i].node == node && d2m.iface[i].flags & 1) {
				break;
			}
		}

		if (i < MAX_INTERFACE && iface->flags & 1) {
			relay_rcv_packet(iface, &d2m.iface[i], len, protocol, node);
		} else {
			if (d2m.debug_flag & 3) {
				sceDeci2ExPanic("deliver_rcv_packet: no route error prot=%d dest=%c\n", protocol,
				  node);
			}

			error_rcv_packet(iface, len, protocol, node, 0);
		}
	} else {
		struct deci2_socket *sock;
		int i;

		for (i = 0, sock = d2m.sock; i < MAX_SOCK; i++, sock++) {
			if (sock->proto == protocol) {
				break;
			}
		}

		if (i < MAX_SOCK) {
			if (d2m.lock_holder && i != d2m.lock_holder) {
				d2m.unk4 |= 1;
				error_rcv_packet(iface, len, protocol, node, 2);
			} else {
				sock->read_if = iface;
				iface->rcv = sock;
			}

		} else {
			error_rcv_packet(iface, len, protocol, node, 1);
		}
	}
}

void
sceDeci2ExPoll()
{
	struct deci2_iface *iface;
	int i, again;

	again = 1;
	while (again) {
		again = 0;

		iface = d2m.iface;
		for (i = 0; i < MAX_INTERFACE; i++) {
			if (iface->handler) {
				iface->handler(IFF_POLL, iface->opt, 0, 0);
			}

			iface++;
		}

		iface = d2m.iface;
		for (i = 0; i < MAX_INTERFACE; i++) {
			if (iface->flags & 2 || iface->unk1C & 2) {
				again = 1;
				iface->flags &= ~2;
				iface->unk1C &= ~2;
			}

			iface++;
		}
	}
}

void
sceDeci2Poll()
{
	if (d2m.poll_cb) {
		d2m.poll_cb();
	}

	asm volatile("li $2, 1\n"
				 "syscall\n" ::
				   : "memory");
}

void
func_00002904()
{
	asm volatile("li $2, 2\n"
				 "syscall\n" ::
				   : "memory");
}

int
new_bind_poll()
{
	struct deci2_iface *iface;
	int i;

	iface = d2m.iface;

	if (d2m.debug_flag & 3) {
		sceDeci2ExPanic("new_bind_poll()\n");
	}

	for (i = 0; i < MAX_INTERFACE; i++, iface++) {
		if (iface->handler && iface->flags & 0x10) {
			if (d2m.debug_flag & 3) {
				sceDeci2ExPanic("  new_bind_poll() #%d\n", i);
			}

			iface->handler(IFF_POLL, iface->opt, 0, 0);
		}
	}

	if (d2m.debug_flag & 3) {
		sceDeci2ExPanic("new_bind_poll() end\n");
	}

	return 0;
}

int
valid_socket(int s)
{
	int ret = 0;

	if ((uint)s >= MAX_SOCK) {
		ret = 0;
	} else if (d2m.sock[s].handler) {
		ret = 1;
	}

	return ret;
}
