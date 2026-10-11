#ifndef LOADCORE_INTERNAL_H_
#define LOADCORE_INTERNAL_H_

#include "sys/types.h"

#define BOOTMODE(id, count, value) (((id) << 16) | ((count) << 24) | (value))

int *QueryBootMode(int mode_id);
void RegisterBootMode(void *mode);
int AddRebootNotifyHandler(void *func, int priority, int *stat);

#endif // LOADCORE_INTERNAL_H_
