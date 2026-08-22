//
// key_input.c
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include    "_debug_.h"
#include    "oe.h"
#include    "escseq.h"
#include	"disp.h"
#include    "message.h"
#include	"key_input.h"

extern void	C_MES(char*,char*,int);
extern void C_MES_CLR(void);

void key_buffer_clr(void);
int  keyin_wait(void);
int  key_inputYN(int);
int  key_input_bitsns(int,int);

static const int keymap0[] = {
	BITSNS_MAP_ESC,
	BITSNS_MAP_S,
	BITSNS_MAP_L,
	BITSNS_MAP_R,
	BITSNS_MAP_M,
	BITSNS_MAP_LOCK,
	0
};

static const int keymap1[] = {
	BITSNS_MAP_ESC,
	BITSNS_MAP_SPACE,
	BITSNS_MAP_CR,
	BITSNS_MAP_ENTER,
	BITSNS_MAP_MLEFT,
	0
};

static const int *bitsns_map[] = {
	keymap0,
	keymap1
};

//
// clear key buffer
//
void key_buffer_clr(void)
{
	while(INPOUT(0xff));
}

// is_keyin
//    0x0D: ENTER key
//    0x1B: ESC key
#define is_keyin(c)  (c==0x0D||c==' '||c==0x1B)

//
// keyin wait
//
int keyin_wait(void)
{
	int c;

	key_buffer_clr();
	while(!is_keyin((c=K_KEYINP())));

	return c;
}

//
// bitsns key-input
//
//   status bit8-15: BITSNS(0x0E)
//          bit0-7 : ASCII code
//
int key_input_bitsns(int sw, int map)
//  sw: 0=once
//      1=wait until valid input
// map: keymap number
{
	int group;
	int value;
	int ext;
	int	c;
	int d;
	int ce;
	int msdt;
	int i;

	if(map >= (int)(sizeof(bitsns_map)/sizeof(bitsns_map[0]))) return 0;

	c = 0;
	do {
		for(i=0; bitsns_map[map][i] != 0; i++) {
			d = bitsns_map[map][i];
			group = (d >> 24) & 0xff;
			value = (d >> 16) & 0xff;
			ext   = (d >> 8)  & 0xff;

			ce = BITSNS(0x0E);
			if((BITSNS(group) & value) && ((ce & ext) == ext)) {
				c = d & 0xff;
				c |= (ce << 8);
				break;
			}

			msdt = MS_GETDT();
			if((ext & 0x80)&&(msdt & 0xff00)) {
				c |= (ext << 8);
				break;
			}
			if((ext & 0x40)&&(msdt & 0xff)) {
				c |= (ext << 8);
				break;
			}
		}
	    key_buffer_clr();
	} while(sw==1 && c==0);

	return c;
}

//
//  key_inputYN
//      sw 0: disp message off
//         1: disp message on
//         2: disp message on without disp cansel message
//
//    stat 0: Y
//         1: N
//
int key_inputYN(int sw)
{
	int flag;
	int c;

	flag = 0;
	C_CURON();

	while(1) {
		c = K_KEYINP();

		if(((c|0x20)=='y') && !flag) {
			printf("%c", c);
			flag++;
		}

		if((c|0x20)=='n' || c==0x1B) {  // 0x1B: ESC
			C_CUROFF();
			if(sw==1) C_MES(C33, MES_KEYIN_CANCEL, MESTIME_SHORT);
			return 1;
		}

		if((c==0x0D) && flag) {         // 0x0D: ENTER
			if(sw!=2) {
				C_MES_CLR();
				C_CUROFF();
			}
			return 0;
		}

		if((c==0x08) && flag) {         // 0x08: BS or <- key
			printf("%c %c",  c, c);
			flag = 0;
		}
	}
}
