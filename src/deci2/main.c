#include "common.h"
#include "introld.h"

#include <deci2.h>
#include <stdio.h>
#include <thread.h>

extern int D_00006708;
extern char D_00006568[];
extern char D_000065D0[];

INCLUDE_ASM("asm/deci2/nonmatchings/main", start);

int
func_000001F4()
{
	D_00006708 = 1;
	CpuEnableIntr();
	printf(D_00006568);
	printf(D_000065D0);
	return 0;
}

int
func_00000240(int *arg1)
{
	CpuEnableIntr();
	if (!*arg1) {
		ChangeThreadPriority(0, 126);
		while (1) {
			sceDeci2Poll();
			DelayThread(2000000);
		}
	}

	return 0;
}

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2Shutdown);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2GetStatus);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2SetDebugFormatRoutine);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2SetDebugFlags);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000360);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000524);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000558);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2Close);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExRecv);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00005AA0);

INCLUDE_RODATA("asm/deci2/nonmatchings/main", D_00005AB0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExReqSend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ReqSend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExSend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExLock);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExUnLock);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExRecvSuspend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExRecvUnSuspend);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExPanic);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000DA4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00000DE8);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2DbgPrintStatus);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2SetPollCallback);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExWakeupThread);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExSignalSema);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExSetEventFlag);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000121C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001364);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2IfCreate);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2IfEventHandler);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001BA0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001E20);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00001FC4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000214C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002234);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000231C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000025BC);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2ExPoll);

INCLUDE_ASM("asm/deci2/nonmatchings/main", sceDeci2Poll);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002904);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002914);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002A0C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002A40);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002A68);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002B10);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002C08);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002C9C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00002D14);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003170);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000338C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000033E0);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_0000352C);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003564);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_000035D4);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003600);

INCLUDE_ASM("asm/deci2/nonmatchings/main", func_00003668);
