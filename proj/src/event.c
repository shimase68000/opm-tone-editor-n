//
//	event.c
//

#include	<stdio.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "message.h"
#include    "escseq.h"
#include	"disp.h"
#include    "undo.h"
#include    "flag.h"

extern void	C_MES(char*,char*,int);
extern void	C_FILENAME(int,int);

extern void	opmset(OPMDATA *,int);
extern void	MakeNameBufferAll(int);
extern void	MakeNameBuffer(int,int);
extern void	PutToneTable(int);
extern void PutToneChange(int);
extern void PutOPMTable(int);
extern void	PutOPMData(int,int);
extern void	cmove(int);
extern int  ChangeToneName(void);
extern void	SetCurrentTone(int);
extern void	vwait(int);
extern void undo_invalidate(int);
extern int  proc_editflag(int,int,int);
extern int  stat_editflag(int,int,int);

extern OPMDATA Tone[3][TONENUM];
extern OPMD	 *CT;
extern int   tonetable_dispflag[3];
extern int	 ln;
extern uchar OPMLimit[REGNUM+REGNUM0];
extern int	 tnsw;
extern int	 tt[3];
extern int   tonememo_dispflag;
extern TABLENUM tb[2];
extern UNDO_BUFFER undo;

void   ev_ToneScroll(int);
void   ev_ToneINCDEC(int);
void   ev_CurMove(int);
void   ev_OPMData(int);
void   ev_OPMDataT(int,int *);
void   ev_ChangeToneName(int,int);
void   ev_ToneLock(int);
void   ev_ToneSelectMark(void);
int    checkLockflag_para(int,int);

//
// tone list scroll
//
void ev_ToneScroll(int d)
{
	if( tb[tnsw].tan == 2 ) return;

	switch( d ) {
		case CURUP:
			tt[tb[tnsw].tan]--;
			if( tt[tb[tnsw].tan] < 0 ) tt[tb[tnsw].tan] = TONENUM-1;
			break;

		case CURDOWN:
			tt[tb[tnsw].tan]++;
			if( tt[tb[tnsw].tan] >= TONENUM ) tt[tb[tnsw].tan] = 0;
			break;
	}
	vwait(3);  // ajust scroll speed
	PutToneTable( tb[tnsw].tan );
}

//
// tone list inc/dec
//   d: CURUP or CURDOWN or CURNONE
//
void ev_ToneINCDEC(int d)
{
	int c_tonenum;
	int c_tonetablenum;
	int pre_tablenum;
	int pre_tonenum;

	c_tonenum      = (tb[tnsw].tan==2) ? TONENUM2 : TONENUM;
	c_tonetablenum = (tb[tnsw].tan==0) ? TONETABLE0NUM :
	                 (tb[tnsw].tan==1) ? TONETABLE1NUM : TONETABLE2NUM;

	pre_tablenum = tb[tnsw].tan;
	pre_tonenum  = tb[tnsw].ton;

	PutOPMData(0,-1);  // disp current OPM to normal status

	switch( d ) {
		case CURUP:
			tb[tnsw].ton = ( tb[tnsw].ton+1 )%c_tonenum;
			break;
		case CURDOWN:
			tb[tnsw].ton = ( tb[tnsw].ton+c_tonenum-1 )%c_tonenum;
			break;
		case CURNONE:
			tnsw = tnsw ^ 1;  // change current OPM table
			break;
	}

	// undo invalid
	if( d != CURNONE ) undo_invalidate(UNDO_SLOTNO);

	// make tone name buffer
	MakeNameBuffer( pre_tablenum, pre_tonenum );
	MakeNameBuffer( tb[tnsw].tan, tb[tnsw].ton );

	if( tb[tnsw].tan != 2 )
		if((tb[tnsw].ton-tt[tb[tnsw].tan]+c_tonenum)%c_tonenum >= c_tonetablenum )
			tt[tb[tnsw].tan] = ( tb[tnsw].ton+c_tonenum-(c_tonetablenum/2))%c_tonenum;

	SetCurrentTone(-1);	// set current OPM data
	vwait(3);
	PutToneChange( tnsw );
	tonememo_dispflag = 1;
}

//
// move cursor (on opm parameter table)
//
void ev_CurMove(int d)
{
	PutOPMData(0,-1);
	cmove( d );
	PutOPMData(1,-1);
}

//
// edit opm parameter (roll up/down)
//
void ev_OPMData(int cur)
{
	int tan, ton;
	int tan0, ton0;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;
	tan0 = tb[tnsw^1].tan;
	ton0 = tb[tnsw^1].ton;

	// check lockflag
	if(stat_lockflag(STAT_EFFECTIVE, tan, ton)) return;

	switch(cur) {
		case CURUP:
			if(CT->data[ln] >= OPMLimit[ln]) {CT->data[ln] = OPMLimit[ln]; return;}
			CT->data[ln]++;
			break;

		case CURDOWN:
			if(CT->data[ln] <= 0) {CT->data[ln] = 0; return;}
			CT->data[ln]--;
			break;
	}

	// invalid undo
	if(undo.src_tnsw == tnsw) undo_invalidate(UNDO_OPM_PARAM);

	// proc editflag
	proc_editflag(SET_EDITFLAG|EDIT_TONE_PARAM, tan, ton);

	PutOPMData(1,-1);

	// table0==table1?
	if(tan==tan0 && ton==ton0) PutOPMData(0, tnsw^1);

	opmset((OPMDATA *)CT, tnsw);
}

//
// edit opm parameter (ten key)
//     n: key number
//   *st: state
//
void ev_OPMDataT(int n, int *st)
{
	int	c, c1;
	int tan, ton;
	int tan0, ton0;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;
	tan0 = tb[tnsw^1].tan;
	ton0 = tb[tnsw^1].ton;

	// check lockflag
	if(stat_lockflag(STAT_EFFECTIVE, tan, ton)) return;

	c = n - 0x30;

	if(*st) c1 = CT->data[ln]*10 + c;
	else    c1 = c;

	if(c1 > OPMLimit[ln]) c1 = OPMLimit[ln];

	if(c1*10 > OPMLimit[ln]) *st = 0;  // disp color = reverse
	else                     *st = 1;  // disp color = modify

	if(CT->data[ln] == c1) return;

	CT->data[ln] = c1;

	// undo invalid
	if(undo.src_tnsw == tnsw) undo_invalidate(UNDO_OPM_PARAM);

	// proc editflag
	proc_editflag(SET_EDITFLAG|EDIT_TONE_PARAM, tan, ton);

	PutOPMData(*st+1,-1);

	// table0==table1?
	if(tan==tan0 && ton==ton0) PutOPMData(0, tnsw^1);

	opmset((OPMDATA*)CT, tnsw);
}

//
// change tone name
//
void ev_ChangeToneName(int tan, int ton)
{
	if(tan < 0 || ton < 0) {
		tan = tb[tnsw].tan;
		ton = tb[tnsw].ton;
	}

	// check lockflag
	if(stat_lockflag(STAT_EFFECTIVE, tan, ton)) {
		C_MES(C33, MES_TONE_IS_LOCKED, MESTIME_NORMAL);
		return;
	}

	if(ChangeToneName()) {
		// proc editflag
		proc_editflag(SET_EDITFLAG|EDIT_TONE_RENAME, tan, ton);
		MakeNameBuffer(tan,ton);
	}
    // set OPM data
	SetCurrentTone(-1);
	// disp all tonetable & OPMtable, disp current cursol
	PutToneChange(tnsw);
}

//
// Tone Lock
//
void ev_ToneLock(int force)
{
	int tan,  ton;
	int tan1, ton1;

    tan  = tb[tnsw].tan;
	ton  = tb[tnsw].ton;
	tan1 = tb[tnsw^1].tan;
	ton1 = tb[tnsw^1].ton;

	//
	// tone lock/unlock
	//

	// check filelock
	if(tan < 2) {
	    if(stat_lockflag(STAT_FILE, tan, 0)) {
			// filelock => return
		    C_MES(C33, MES_FILE_IS_LOCKED2, MESTIME_LONG);
			return;
	    }
	}

	if(stat_lockflag(STAT_TONE, tan, ton)) {
		// lock on -> off
		// unlock tonelock
		proc_lockflag(UNLOCK_LOCKFLAG|LOCK_TONE_KEYIN, tan, ton);

	} else {
		// lock off -> on
		// check edit flag
		if(!force && stat_editflag(STAT_TONE, tan, ton)) {
			// edit flag=on
			C_MES(C33, MES_TONE_CANNOT_LOCK, MESTIME_LONG);
			// exit func.
			return;
		}

		// set lock flag
		proc_lockflag(LOCK_LOCKFLAG|LOCK_TONE_KEYIN|(force ? LOCK_FORCE_KEYIN : 0), tan, ton);
	}

	// re-disp. OPM Table
	PutOPMTable(tnsw);
	if((tan == tan1)&&(ton == ton1)) {
		PutOPMTable(tnsw^1);
	}

	PutOPMData(1,-1);
	MakeNameBuffer(tan, ton);
   	PutToneTable(tan);
}

//
// toggle tone select mark (current tone)
//   select は音色データの編集ではないため、
//   file lock / tone lock のどちらの影響も受けない。
//
void ev_ToneSelectMark(void)
{
	int tan, ton;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;

	if(stat_selectflag(STAT_TONE, tan, ton)) {
		proc_selectflag(CLEAR_SELECTFLAG, tan, ton);
	} else {
		proc_selectflag(SET_SELECTFLAG, tan, ton);
	}

	// re-disp.
	tonetable_dispflag[tan] = 1;
	MakeNameBuffer(tan, ton);

	PutToneTable(tan);
	PutOPMTable(0);
	PutOPMTable(1);
	PutOPMData(1,-1);
}

//
// check lock flag para
//   tone data 由来のパラメータ用。file lock と tone lock の両方を見る。
//   channel volume は tone data ではなく実行時パラメータのため、
//   これを使わない (mev_ChVolume)。
//
int checkLockflag_para(int tablenum, int tonenum)
{
	return stat_lockflag(STAT_EFFECTIVE, tablenum, tonenum);
}
