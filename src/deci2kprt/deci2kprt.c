#include "common.h"
#include "deci2_def.h"
#include "intrman_internal.h"
#include "loadcore_internal.h"
#include "sysclib_internal.h"
#include "sysmem_internal.h"

#include <deci2.h>
#include <loadcore.h>
#include <memory.h>

ModuleInfo Module = { "Deci2_Kprintf_driver", 0x101 };

struct kttyp_cmd {
	struct sceDeci2Hdr hdr;
	int pad;
	char buf[80];
};

struct kp_state {
	int sock;
	int wrpos;
	int rdpos;
	struct kttyp_cmd cmd;
	int unk68;
};

struct kp_state kp;

static void kp_handler(int event, int param, void *opt);
static void kp_putc(void *context, int c);
static int kp_print(void *context, const char *format, va_list ap);
static int kp_flush(struct kp_state *kp);

int
start()
{
	int *bootmode;

	bootmode = QueryBootMode(3);
	if (bootmode && (bootmode[1] & 8) != 0) {
		return 1;
	}

	memset(&kp, 0, sizeof(kp));

	kp.cmd.hdr.proto = DECI2_PROTO_IKTTYP;
	kp.cmd.hdr.src = DECI2_NODE_IOP;
	kp.cmd.hdr.dest = DECI2_NODE_HOST;

	kp.sock = sceDeci2Open(DECI2_PROTO_IKTTYP, &kp, kp_handler);
	if (kp.sock < 0) {
		return 1;
	}

	KprintfSet(kp_print, &kp);
	return 0;
}

static void
kp_putc(void *context, int c)
{
	struct kp_state *kp = context;

	if (c == 0x200) {
		kp->wrpos = 0;
		return;
	}

	if (c == 0x201) {
		if (kp->wrpos <= 0) {
			return;
		}

		kp_flush(kp);
		return;
	}

	if (c == '\n') {
		kp_putc(context, '\r');
	}

	kp->cmd.buf[kp->wrpos] = c;
	kp->wrpos++;

	if (kp->wrpos == 80) {
		kp_flush(kp);
	}
}

static int
kp_cb(void *context, const char *format, va_list ap)
{
	if (format) {
		prnt(kp_putc, context, format, ap);
	}

	return 0;
}

static int
kp_print(void *context, const char *format, va_list ap)
{
	return CpuInvokeInKmode(kp_cb, context, format, ap);
}

static int
kp_flush(struct kp_state *kp)
{
	kp->rdpos = 0;
	kp->cmd.hdr.len = kp->wrpos + 12;

	if (sceDeci2ReqSend(kp->sock, DECI2_NODE_HOST) >= 0) {
		while (kp->wrpos) {
			sceDeci2Poll();
		}
	}
}

static void
kp_handler(int event, int param, void *opt)
{
	struct kp_state *kp = opt;
	char buf[8];

	switch (event) {
	case DECI2_WRITE:
		kp->rdpos += sceDeci2ExSend(kp->sock, (char *)&kp->cmd + kp->rdpos,
		  kp->cmd.hdr.len - kp->rdpos);
		break;
	case DECI2_WRITEDONE:
		kp->wrpos = 0;
		break;
	case DECI2_READ:
		sceDeci2ExRecv(kp->sock, buf, 4);
		break;
	case DECI2_READDONE:
		break;
	case DECI2_CHSTATUS:
		break;
	case DECI2_ERROR:
		break;
	default:
		sceDeci2ExPanic("KpHandler: unknown event 0x%x\n", event);
	}
}
