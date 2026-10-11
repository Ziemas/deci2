#ifndef DECI2_INTERNAL_H_
#define DECI2_INTERNAL_H_

#include "sys/types.h"

#define MAX_SOCK 35
#define MAX_INTERFACE 2

enum DECI2Ex {
	DECI2Ex_RflagDone = 7,
	DECI2Ex_WriteStart = 8,
	DECI2Ex_WflagDone = 9,
};

enum IFM {
	IFM_IN = 1,
	IFM_INDONE = 2,
	IFM_OUT = 3,
	IFM_OUTDONE = 4,

	// guessed names
	IFM_UP = 5,
	IFM_DOWN = 6,
};

enum IFFLG {
	IFLG_UP = 1,
};

enum IFFUNC {
	IFF_RCV_START = 0,
	IFF_RCV_READ = 1,
	IFF_RCV_END = 2,
	IFF_SEND_START = 3,
	IFF_SEND_WRITE = 4,
	IFF_SEND_END = 5,
	IFF_POLL = 6,
	IFF_RCV_OFF = 7,
	IFF_RCV_ON = 8,
	IFF_SEND_OFF = 9,
	IFF_SEND_ON = 10,
	IFF_DEBUG = 11,
	IFF_SHUTDOWN = 12,
};

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

struct deci2_iface {
	/* 0x0 */ int node;
	/* 0x4 */ int (*handler)();
	/* 0x8 */ void *opt;
	/* 0xc */ int flags;
	/* 0x10 */ struct deci2_socket *send;
	/* 0x14 */ int send_len;
	/* 0x18 */ int send_progress;
	/* 0x1c */ int rcv_state;
	/* 0x20 */ struct deci2_socket *rcv;
	/* 0x24 */ int rcv_len;
	/* 0x28 */ int rcv_progress;
	/* 0x2c */ void *rcv_buf;
};

struct deci2_manager {
	/* 0x000 */ int lock_holder;
	/* 0x004 */ int unk4;
	/* 0x008 */ int unk8;
	/* 0x00c */ int debug_flag;
	/* 0x010 */ void (*dbg_print_fn)(void *, int);
	/* 0x014 */ void *dbg_print_opt;
	/* 0x018 */ int (*poll_cb)();
	/* 0x01C */ struct deci2_iface *host_route;
	/* 0x020 */ struct deci2_iface *host_if;
	/* 0x024 */ int isdbgp_sock;
	/* 0x028 */ struct deci2_socket sock[MAX_SOCK];
	/* 0x5a0 */ struct deci2_iface iface[MAX_INTERFACE];
	/* 0x600 */ void **unk600;
	/* 0x604 */ int (*dbg_print)();
};

struct deci2_manager *sceDeci2GetStatus();
struct deci2_iface *sceDeci2IfCreate(short, void *, int (*)(), int (*)());
void sceDeci2IfEventHandler(int event, struct deci2_iface *iface, int len, int protocol, int node);
void sceDeci2ExPoll();
void sceDeci2DbgPrintStatus(void (*cb)(void *context, int c), void *param);

#endif // DECI2_INTERNAL_H_
