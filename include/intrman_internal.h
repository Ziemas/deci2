#ifndef INTRMAN_INTERNAL_H_
#define INTRMAN_INTERNAL_H_

int EnableIntr(int irq);
int CpuInvokeInKmode(void *function, ...);
void DisableDispatchIntr(int irq);

#endif // INTRMAN_INTERNAL_H_
