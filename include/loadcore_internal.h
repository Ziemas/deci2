#ifndef LOADCORE_INTERNAL_H_
#define LOADCORE_INTERNAL_H_

#include "sys/types.h"

int AddRebootNotifyHandler(void *func, int priority, int *stat);

// doesn't seem possible to construct it by using an actual struct instance
#define BOOTMODE(key, value, extwords) (((key) << 16) | (value) | ((extwords) << 24))

typedef struct bootparam {
	unsigned short value;
	unsigned char key;
	unsigned char extwords;
	unsigned long eword[0];
} BootParam;

void RegisterBootMode(int *param);
BootParam *QueryBootMode(int key);
#define QueryBootParam(key) QueryBootMode(key)

/* key */
#define KEY_DECItype 1
#define KEY_EEbootparam 2
#define KEY_IOPbootparam 3

/* IOPbootparam */
#define IBP_no_sif_init (1 << 0)
#define IBP_no_iop_service (1 << 1)
#define IBP_no_auto_load (1 << 2)

#endif // LOADCORE_INTERNAL_H_
