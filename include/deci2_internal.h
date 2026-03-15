#ifndef DECI2_INTERNAL_H_
#define DECI2_INTERNAL_H_

#include "sys/types.h"

struct deci2_iface {
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
};

struct deci2_iface *sceDeci2IfCreate(char, void *, int (*)(), int (*)());
void sceDeci2IfEventHandler(int event, struct deci2_iface *iface, int len, int protocol,
  int node);
void sceDeci2ExPoll();

#endif // DECI2_INTERNAL_H_
