#include "common.h"
#include "deci2_internal.h"
#include "loadcore_internal.h"

#include <loadcore.h>

ModuleInfo Module = { "Deci2_host_sync", 0x101 };

int
start()
{
	int *bootmode;

	if (!sceDeci2GetStatus()->host_if) {
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
