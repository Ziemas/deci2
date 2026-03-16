#ifndef DECI2_INTERNAL_H_
#define DECI2_INTERNAL_H_

#include "sys/types.h"

struct deci2_iface {
	int unk0;
	int unk4;
	int unk8;
	int unkC;
	int unk10;
	int unk14;
	int unk18;
	int unk1C;
	int unk20;
	int unk24;
	int unk28;
	int unk2C;
};

struct deci2_iface *sceDeci2IfCreate(char, void *, int (*)(), int (*)());
void sceDeci2IfEventHandler(int event, struct deci2_iface *iface, int len, int protocol,
  int node);
void sceDeci2ExPoll();

#endif // DECI2_INTERNAL_H_
