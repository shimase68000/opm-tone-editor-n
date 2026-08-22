//
//  const.
//

#include	<stdio.h>
#include    "_debug_.h"
#include	"oe.h"

/*
 *	printing location of OPM reg.
 */

#define	X	0
#define Y	1

// カーソル表示位置。要素数は OPMD.data[] と一対一で対応する。
//   各行末の重複エントリは OPMREG.dummy / OPMDATA.dummy に対応する
//   ダミー。カーソルはそこに止まらないため直前と同じ座標を置いてある。
const CURSOR cur[REGNUM] = {
	{X+0,Y+2 },{X+4,Y+2 },{X+8,Y+2 },{X+12,Y+2},{X+16,Y+2},{X+20,Y+2},{X+24,Y+2},
	{X+28,Y+2},{X+32,Y+2},{X+36,Y+2},{X+40,Y+2},{X+44,Y+2},{X+48,Y+2},{X+48,Y+2},

	{X+0,Y+4 },{X+4,Y+4 },{X+8,Y+4 },{X+12,Y+4},{X+16,Y+4},{X+20,Y+4},
	{X+24,Y+4},{X+28,Y+4},{X+32,Y+4},{X+36,Y+4},{X+40,Y+4},{X+40,Y+4},
	{X+0,Y+5 },{X+4,Y+5 },{X+8,Y+5 },{X+12,Y+5},{X+16,Y+5},{X+20,Y+5},
	{X+24,Y+5},{X+28,Y+5},{X+32,Y+5},{X+36,Y+5},{X+40,Y+5},{X+40,Y+5},
	{X+0,Y+6 },{X+4,Y+6 },{X+8,Y+6 },{X+12,Y+6},{X+16,Y+6},{X+20,Y+6},
	{X+24,Y+6},{X+28,Y+6},{X+32,Y+6},{X+36,Y+6},{X+40,Y+6},{X+40,Y+6},
	{X+0,Y+7 },{X+4,Y+7 },{X+8,Y+7 },{X+12,Y+7},{X+16,Y+7},{X+20,Y+7},
	{X+24,Y+7},{X+28,Y+7},{X+32,Y+7},{X+36,Y+7},{X+40,Y+7},{X+40,Y+7}
};


/*
 *	limit value of OPM reg.
 */

// パラメータごとの上限値。OPMD の data[REGNUM] + data0[REGNUM0] に対応する。
const uchar OPMLimit[REGNUM+REGNUM0] = {
		7,	  // con
		7,	  // feedback
		15,	  // slotmask
		3,	  // pan
		7,	  // pms
		3,	  // ams

		1,	  // sync
		3,	  // waveform
		255,  // lfreq
		127,  // pmd
		127,  // amd
		1,	  // noise
		31,	  // nfreq
		0,    // *dummy*

     /* AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AMSE dummy */
		31, 31, 31, 15, 15,127,  3, 15,  7,  3,  1,  0,
		31, 31, 31, 15, 15,127,  3, 15,  7,  3,  1,  0,
		31, 31, 31, 15, 15,127,  3, 15,  7,  3,  1,  0,
		31, 31, 31, 15, 15,127,  3, 15,  7,  3,  1,  0,

		255,   // chmode (no limit)
		255,   // chflag (no limit)
		255,   // mark (no limit)
		127    // volume
};


