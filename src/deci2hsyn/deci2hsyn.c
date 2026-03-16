#include "common.h"
#include "loadcore_internal.h"

#include <loadcore.h>

ModuleInfo Module = { "Deci2_host_sync", 0x101 };

/* TODO */
struct Deci2Status {
	int unk0;
	int unk4;
	int unk8;
	int unkC;
	int unk10;
	int unk14;
	int unk18;
	int unk1C;
	int unk20;
};

/* TODO */
struct Deci2Status *sceDeci2GetStatus();

int
start()
{
	int *bootmode;

	if (!sceDeci2GetStatus()->unk20) {
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
