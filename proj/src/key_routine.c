//
//	key routine
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include    "message.h"
#include	"disp.h"
#include	"flag.h"

extern void	scinit(void);
extern void	scsetup(void);
extern void	ms_init(void);
extern void C_MES(char *,char *,int);			// disp_sub.c
extern void C_TITLE_VERSION(int);

extern void	ev_ToneScroll(int);
extern void	ev_CurMove(int);
extern void	ev_OPMData(int);
extern void	ev_OPMDataT(int,int *);
extern void	ev_ToneINCDEC(int);
extern void	ev_ChangeToneName(int,int);
extern void ev_ToneLock(int);
extern void ev_ToneSelectMark(void);
extern void mev_EditToneMemo(int,int);
extern int  key_inputYN(int);                  // key_input.c
extern void key_buffer_clr(void);
extern void	PutCurrentTableMark(void);
extern void opmset_muteall(void);

extern int  tonecopy_is_armed(void);
extern void tonecopy_disarm(void);

int	key_routine();
int	stroke();
int	event();

//
// key_routine
//
int key_routine(void)
{
	return(stroke() && event());
}

/******

	キー入力（ストローク）

******/
//
// key input (for stroke)
//
int stroke(void)
{
	static int state = 0;
	int c;
	int shift;

	shift = BITSNS(0xE) & 1;
	c = INPOUT(0xff);

	if(c == 0x1b) {
		c = INPOUT(0xff);
		switch(c) {
			case 0x45:
				c = INPOUT(0xff);
				INPOUT(0xff);			// dummy
				switch(c) {
					case 0x2B:			// ROLL DOWN key
						state = 0;
						ev_OPMData(CURDOWN);
						break;
					case 0x2D:			// ROLL UP key
						state = 0;
						ev_OPMData(CURUP);
						break;
				}
				break;
			case 0x4A:
				state = 0;
				ev_CurMove(CURUP);
				break;
			case 0x53:
				state = 0;
				ev_CurMove(CURRIGHT);
				break;
			case 0x55:
				state = 0;
				ev_CurMove(CURDOWN);
				break;
		}
	} else {
		switch(c) {
			case 0x20:		// Space
				state = 0;
				ev_ToneINCDEC(CURNONE);
				PutCurrentTableMark();
				key_buffer_clr();
				break;

			case 0x08:		// BackSpace
				state = 0;
				ev_CurMove(CURLEFT);
				break;
			case 0x09:		// TAB
				state = 0;
				if(!shift) ev_ChangeToneName(-1,-1);
				else       mev_EditToneMemo(-1,-1);
				break;

			case 0x30:		// '0'-'9'
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x34:
			case 0x35:
			case 0x36:
			case 0x37:
			case 0x38:
			case 0x39:
				ev_OPMDataT(c, &state);
				break;

			case '!':       // normal Tone Lock
				state = 0;
			    ev_ToneLock(0);
				break;

			case 0x2A:      // '*' : toggle tone select mark
				state = 0;
				ev_ToneSelectMark();
				break;
		}
	}

	return 1;
}

//
// key input (for bit sense)
//
int event(void)
{
	static int xf1_flag = 0;
	static int xf2_flag = 0;
	static int f1_flag = 0;
	static int f2_flag = 0;
	static int key1_flag = 0;

	int c0, ca, cc, ce;
	int shift, ctrl, esc;
	int xf1, xf2;
	int f1, f2;
	int key1;

	c0 = BITSNS(0x0);
	ca = BITSNS(0xA);
	cc = BITSNS(0xC);
	ce = BITSNS(0xE);

	shift = ce & 0x01;
	ctrl  = ce & 0x02;
	esc   = c0 & 0x02;

	xf1   = ca & 0x20;
	xf2   = ca & 0x40;

	f1    = cc & 0x08;
	f2    = cc & 0x10;
	key1  = c0 & 0x04;

	// F1: re-disp
	{
		if(!f1_flag && f1) {   // check F1 key
			scinit();
			ms_init();
			scsetup();
			C_TITLE_VERSION(0);
			f1_flag = 1;

			return 1;
		}
	}

	// F2: OPM mute
	{
		if(!f2_flag && f2) {   // check F2 key
			opmset_muteall();
			f2_flag = 1;

			return 1;
		}
	}

	// ESC+SHIFT: oe exit
	{
		if(esc) {
			if(shift) {
				// check editflag
				if(!stat_editflag(STAT_FILE, 0, 0) && !stat_editflag(STAT_FILE, 1, 0) \
                                                   && !stat_editflag(STAT_FILE, 2, 0)) return 0;
				C_MES(C33, MES_EXIT, MESTIME_FOREVER);
				return(key_inputYN(1));
			}
			else {
				if(tonecopy_is_armed()) tonecopy_disarm();
				C_MES(C33, MES_KEYIN_ESC, MESTIME_SHORT);
			}
		}
	}

	// CTRL+!: force lock
	{
		if(!key1_flag && key1 && shift && ctrl) {
			ev_ToneLock(1);
			key1_flag = 1;
		}
	}

	// tone list scroll
	{
		if(shift) {
			if(xf1) {
				ev_ToneScroll(CURUP);       // XF1+SHIFT
				xf1_flag = 1;
				return 1;
			}
			else if(xf2) {
				ev_ToneScroll(CURDOWN);     // XF2+SHIFT
				xf2_flag = 1;
				return 1;
			}
		}

		if(ctrl) {
			if(xf1 && !xf1_flag) {	        // XF1+CTRL
				ev_ToneScroll(CURUP);
				xf1_flag = 1;
				return 1;
			}
			else if(xf2 && !xf2_flag) {     // XF2+CTRL
				ev_ToneScroll(CURDOWN);
				xf2_flag = 1;
				return 1;
			}
		}
	}

	// tone list inc/dec
	{
		if(xf1 && !shift && !ctrl && !xf1_flag) {	   // XF1
			ev_ToneINCDEC(CURDOWN);
			xf1_flag = 1;
			return 1;
		}
		else if(xf2 && !shift && !ctrl && !xf2_flag) { // XF2
			ev_ToneINCDEC(CURUP);
			xf2_flag = 1;
			return 1;
		}
	}

	if(!xf1)  xf1_flag  = 0;
	if(!xf2)  xf2_flag  = 0;
	if(!f1)   f1_flag   = 0;
	if(!f2)   f2_flag   = 0;
	if(!key1) key1_flag = 0;

	return 1;
}
