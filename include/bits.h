#ifndef BITS_H_
#define BITS_H_

#define read32(a) (*(volatile u_int *)(a))
#define write16(a, b) (*(volatile u_short*)(a) = (b))
#define write32(a, b) (*(volatile u_int *)(a) = (b))

#endif // BITS_H_
