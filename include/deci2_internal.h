#ifndef DECI2_INTERNAL_H_
#define DECI2_INTERNAL_H_

#include "sys/types.h"

struct deci2_socket;

enum DECI2Ex {
	DECI2Ex_RflagDone = 7,
	DECI2Ex_WriteStart= 8,
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

struct deci2_iface {
	/* 0x0 */ int node;
	/* 0x4 */ int (*handler)();
	/* 0x8 */ void *opt;
	/* 0xc */ int flags;
	/* 0x10 */ struct deci2_socket *send;
	/* 0x14 */ int unk14;
	/* 0x18 */ int unk18;
	/* 0x1c */ int unk1C;
	/* 0x20 */ struct deci2_socket *rcv;
	/* 0x24 */ int unk24;
	/* 0x28 */ int unk28;
	/* 0x2c */ void *unk2C;
};

struct deci2_iface *sceDeci2IfCreate(short, void *, int (*)(), int (*)());
void sceDeci2IfEventHandler(int event, struct deci2_iface *iface, int len, int protocol, int node);
void sceDeci2ExPoll();
void sceDeci2DbgPrintStatus(void (*cb)(void *context, int c), void *param);

#endif // DECI2_INTERNAL_H_
