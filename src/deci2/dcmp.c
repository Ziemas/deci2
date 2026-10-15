#include "common.h"
#include "deci2_def.h"
#include "deci2_internal.h"
#include "loadcore_internal.h"
#include "thread_internal.h"

#include <deci2.h>

struct dcmp_state {
	/* 0x00 */ int unk0;
	/* 0x04 */ u_char unk4[48];
	/* 0x34 */ struct deci2_iface *unk34;
	/* 0x38 */ int unk38;
	/* 0x3C */ int unk3C;
	/* 0x40 */ char unk40[48];
	/* 0x70 */ int unk70;
	/* 0x74 */ int unk74;
	/* 0x78 */ int unk78;
	/* 0x7C */ int unk7C;
	/* 0x80 */ int unk80;
	/* 0x84 */ int unk84[40];
};

struct dcmp_state dcmp;

void func_00002C9C(struct dcmp_state *);
void func_00002D14(struct dcmp_state *, int);
void func_0000338C(struct dcmp_state *);
void func_000033E0(struct dcmp_state *);

void func_00003170(struct dcmp_state *d);
void func_000035D4(struct dcmp_state *d);
int func_00003668(struct dcmp_state *dcmp);
void func_00003564(struct dcmp_state *d, void *buf, int len);
struct deci2_socket *func_0000352C(int protocol);

extern struct deci2_manager d2m;

// dcmp init
void
func_00002A40()
{
	dcmp.unk0 = 0;
	dcmp.unk78 = 0;
	dcmp.unk7C = 0;
	dcmp.unk80 = 0;
	d2m.sock[0].opt = &dcmp;
}

// dcmp socket handler
void
func_00002A68(int event, int param, void *opt)
{
	struct dcmp_state *d = opt;

	switch (event) {
	case DECI2_READ:
		func_00002C9C(d);
		break;
	case DECI2_READDONE:
		func_00002D14(d, param);
		break;
	case DECI2_WRITE:
		func_0000338C(d);
		break;
	case DECI2_WRITEDONE:
		func_000033E0(d);
		break;
	case DECI2_CHSTATUS:
	case DECI2_ERROR:
	case DECI2Ex_RflagDone:
	case DECI2Ex_WriteStart:
	case DECI2Ex_WflagDone:
		sceDeci2ExPanic("DcmpHandler: unsupprt event 0x%x\n", event);
		break;
	default:
		sceDeci2ExPanic("DcmpHandler: unknown event 0x%x\n", event);
		break;
	}
}

void
func_00002B10(int error, void *buf)
{
	struct dcmp_state *d = &dcmp;
	sceDeci2Hdr *pkt = buf;
	uint i;

	struct {
		sceDeci2Hdr d2;
		sceDcmpHdr dcmp;
	} data;

	data.d2.len = sizeof(data);
	data.d2.rsvd = 0;
	data.d2.src = pkt->dest;
	data.d2.dest = pkt->src;
	data.d2.proto = DECI2_PROTO_DCMP;
	data.dcmp.type = DCMP_TYPE_ERROR;
	data.dcmp.code = error;
	data.dcmp.unused = 0;
	data.d2.len += sizeof(sceDeci2Hdr) + 16;

	// Send back header + first 16 bytes
	for (i = pkt->len; i < sizeof(sceDeci2Hdr) + 16; i++) {
		*((char *)buf + i) = 0;
	}

	if (((uint)data.d2.len + 3) >> 2 <= func_00003668(d)) {
		func_00003564(d, &data, sizeof(data) >> 2);
		func_00003564(d, buf, (sizeof(sceDeci2Hdr) + 16) >> 2);
		func_000035D4(d);
	} else {
		sceDeci2ExPanic("DCMPT_ERROR: buffer over flow\n");
	}
}

void
func_00002C08(int status, int protocol)
{
	struct dcmp_state *d = &dcmp;

	struct {
		sceDeci2Hdr d2;
		sceDcmpHdr dcmp;
		short protocol;
	} data;

	data.d2.len = sizeof(data);
	data.d2.rsvd = 0;
	data.d2.src = DECI2_NODE_IOP;
	data.d2.proto = DECI2_PROTO_DCMP;
	data.d2.dest = 0;
	data.dcmp.code = status;
	data.dcmp.type = DCMP_TYPE_STATUS;

	// ????
	// shouldn't this write to the next word after the dcmp header?
	// how is this correct wrt docs?
	data.dcmp.unused = protocol;

	if (data.d2.len >> 2 <= func_00003668(d)) {
		func_00003564(d, &data, sizeof(data) >> 2);
		func_000035D4(d);
	} else {
		sceDeci2ExPanic("DCMPT_STATUS: buffer over flow\n");
	}
}

// dcmp read
void
func_00002C9C(struct dcmp_state *d)
{
	if (!d->unk0) {
		d->unk34 = d2m.sock[0].read_if;
		d->unk0 += sceDeci2ExRecv(0, d->unk4, 0x30);
	} else {
		d->unk0 += sceDeci2ExRecv(0, &d->unk4[d->unk0], 0x30 - d->unk0);
	}
}

// dcmp readdone
// TODO try to clean this up, the raw pointer casting is nasty
// can we really not do casts to ad-hoc structs?
void
func_00002D14(struct dcmp_state *d, int param)
{
	struct deci2_socket *sock;
	int i;

	struct {
		u_int hdr;
		u_int param[2];
	} bm;

	struct {
		sceDeci2Hdr d2;
		sceDcmpHdr dcmp;
	} response;

	d->unk0 = 0;

	switch ((u_char)d->unk4[8]) {
	case DCMP_TYPE_CONNECT:

		switch (d->unk4[9]) {
		case DCMP_CODE_CONNECT:
			if (!d2m.host_route) {
				u_int tmp;
				d2m.host_route = d->unk34;
				if (*(u_short *)&d->unk4[0] == 32) {
					bm.hdr = BOOTMODE(2, 2, 0);
					bm.param[0] = *(u_int *)&d->unk4[16];
					bm.param[1] = *(u_int *)&d->unk4[20];
					RegisterBootMode(&bm);

					bm.hdr = BOOTMODE(3, 2, 0);
					bm.param[0] = *(u_int *)&d->unk4[24];
					bm.param[1] = *(u_int *)&d->unk4[28];
					RegisterBootMode(&bm);
				}

				response.d2.src = d->unk4[7];
				response.d2.dest = d->unk4[6];
				response.dcmp.unused = 0;
				tmp = d->unk4[6];
				d->unk4[6] = 'I';
				d->unk4[7] = 'I';
				*(u_short *)&d->unk4[0] = 14;
				*(u_short *)&d->unk4[4] = 1;
				*(u_char *)&d->unk4[8] = 2;
				*(u_char *)&d->unk4[9] = 0;
				*(u_int *)&d->unk4[12] = tmp;

				for (i = 3, sock = &d2m.sock[3]; i < MAX_SOCK; i++, sock++) {
					if (sock->handler) {
						sock->handler(DECI2_CHSTATUS, (int)d->unk4, sock->opt);
					}
				}

				sceDeci2ExSetEventFlag(0, GetSystemStatusFlag(), 0x40);
			} else {
				response.d2.src = d->unk4[7];
				response.d2.dest = d->unk4[6];
				response.dcmp.unused = 2;
			}

			response.d2.len = 16;
			response.d2.rsvd = 0;
			response.d2.proto = 1;
			response.dcmp.type = 0;
			response.dcmp.code = 1;

			func_00003564(d, &response, 3);
			func_000035D4(d);

			break;
		case DCMP_CODE_DISCONNECT:
			if (d2m.host_route) {
				response.dcmp.unused = 0;
			} else {
				response.dcmp.unused = 2;
			}

			response.d2.len = 16;
			response.d2.rsvd = 0;
			response.d2.src = d->unk4[7];
			response.d2.dest = d->unk4[6];
			response.d2.proto = 1;
			response.dcmp.type = 0;
			response.dcmp.code = 3;

			func_00003564(d, &response, 3);
			func_000035D4(d);

			break;
		default:
			sceDeci2ExPanic("DCMPT_CONNECT: unknown code 0x%x", d->unk4[9]);
			break;
		}
		break;

	case DCMP_TYPE_ECHO:
		switch (d->unk4[9]) {
		case 0:
			if ((*(u_short *)&d->unk4[0] + 3u) >> 2 <= func_00003668(d)) {
				response.d2.len = *(u_short *)&d->unk4[0];
				response.d2.rsvd = 0;
				response.d2.src = d->unk4[7];
				response.d2.dest = d->unk4[6];
				response.d2.proto = 1;
				response.dcmp.type = 1;
				response.dcmp.code = 1;
				response.dcmp.unused = 0;

				func_00003564(d, &response, 3);

				func_00003564(d, &d->unk4[12], ((*(u_short *)&d->unk4[0] + 3u) >> 2) - 3);
				func_000035D4(d);

			} else {
				sceDeci2ExPanic("DCMPT_ECHO: buffer over flow\n");
			}
			break;
		default:
			sceDeci2ExPanic("DCMPT_ECHO: unknown code 0x%x\n", d->unk4[9]);
			break;
		}
		break;

	case DCMP_TYPE_STATUS:
		switch (d->unk4[9]) {
		case 1:
			sock = func_0000352C(*(u_short *)&d->unk4[12]);
			if (sock && sock->handler) {
				sock->handler(DECI2_CHSTATUS, (int)d->unk4, sock->opt);
			}
			break;
		case 2:
		case 3:
			for (i = 3, sock = &d2m.sock[3]; i < MAX_SOCK; i++, sock++) {
				if (sock->handler) {
					sock->handler(DECI2_CHSTATUS, (int)d->unk4, sock->opt);
				}
			}
			break;
		case 0:
		case 4:
			break;
		default:
			sceDeci2ExPanic("DCMPT_STATUS: unknown code 0x%x\n", d->unk4[9]);
			break;
		}
		break;

	case DCMP_TYPE_ERROR:
		if (d->unk4[9] >= 5) {
			sceDeci2ExPanic("DCMPT_ERROR: unknown code 0x%x\n", d->unk4[9]);
			return;
		}

		sock = func_0000352C(*(u_short *)&d->unk4[16]);
		if (sock) {
			if (sock->handler) {
				sock->handler(DECI2_ERROR, (int)d->unk4, sock->opt);
			}
		} else {
			sceDeci2ExPanic("DCMPT_ERROR: unknown proto 0x%x\n", *(u_short *)&d->unk4[16]);
		}

		break;

	default:
		sceDeci2ExPanic("DCMPT_XXXX: unknown type 0x%x\n", d->unk4[8]);
		break;
	}
}

INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_00003170);

// dcmp write
void
func_0000338C(struct dcmp_state *d)
{
	d->unk3C += sceDeci2ExSend(0, &d->unk40[d->unk3C], ((sceDeci2Hdr *)d->unk40)->len - d->unk3C);
}

// dcmp writedone
INCLUDE_ASM("asm/deci2/nonmatchings/dcmp", func_000033E0);

struct deci2_socket *
func_0000352C(int protocol)
{
	struct deci2_socket *sock;
	int i;

	for (i = 3, sock = &d2m.sock[3]; i < MAX_SOCK; i++, sock++) {
		if (sock->proto == protocol) {
			return sock;
		}
	}

	return NULL;
}

void
func_00003564(struct dcmp_state *d, void *buf, int len)
{
	int *p = buf;
	int i;

	if (len + d->unk80 > 40) {
		return;
	}

	for (i = 0; i < len; i++) {
		d->unk84[d->unk78++] = p[i];
		d->unk80++;
		if (d->unk78 >= 40) {
			d->unk78 = 0;
		}
	}
}

void
func_000035D4(struct dcmp_state *d)
{
	d->unk74++;
	func_00003170(d);
}

void
func_00003600(struct dcmp_state *d, void *buf, int len)
{
	int *p = buf;
	int i;

	if (d->unk80 < len) {
		return;
	}

	while (len) {
		*p++ = d->unk84[d->unk7C++];
		d->unk80--;
		len--;
		if (d->unk7C >= 40) {
			d->unk7C = 0;
		}
	}
}

int
func_00003668(struct dcmp_state *d)
{
	return 0x28 - d->unk80;
}
