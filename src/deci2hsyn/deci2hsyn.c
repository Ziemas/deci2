#include "common.h"
#include "deci2_internal.h"
#include "loadcore_internal.h"

#include <loadcore.h>

ModuleInfo Module = { "Deci2_host_sync", 0x101 };

int
start()
{
	BootParam *bp;

	if (!sceDeci2GetStatus()->host_if) {
		return 1;
	}

	bp = QueryBootMode(KEY_DECItype);
	if (!bp) {
		return 1;
	}

	if (bp->value == 2) {
		while (!QueryBootMode(KEY_IOPbootparam))
			;
	}

	return 1;
}
