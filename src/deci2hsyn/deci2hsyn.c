#include "common.h"
#include "loadcore_internal.h"

#include <loadcore.h>

ModuleInfo Module = { "Deci2_host_sync", 0x101 };

/* TODO */
struct Deci2Status {
	int unk0x0;
	int unk0x4;
	int unk0x8;
	int unk0xc;
	int unk0x10;
	int unk0x14;
	int unk0x18;
	int unk0x1c;
	int unk0x20;
};

/* TODO */
struct Deci2Status *sceDeci2GetStatus();

int
start()
{
	int *bootmode;

	if (!sceDeci2GetStatus()->unk0x20) {
		return 1;
	}

	bootmode = QueryBootMode(1);
	if (!bootmode) {
		return 1;
	}

	if (*(unsigned short *)bootmode == 2) {
		while (!QueryBootMode(3))
			;
	}

	return 1;
}
