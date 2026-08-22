//
//	opmset
//

#include	<stdio.h>
#include	<iocslib.h>
#include    "_debug_.h"
#include	"oe.h"
#include	"escseq.h"
#include	"disp.h"
#include    "opm.h"
#include	"message.h"

extern void	C_MES(char*,char*,int);

extern void set_slotmask(void);
extern int	Vol[2][8];

extern int tnsw;
extern OPMDATA  Tone[3][TONENUM];  // OPM data
extern OPMD     *CT;               // curren OPM data
extern TABLENUM tb[2];

void SetCurrentTone(int);
void opmset(OPMDATA *,int);
void opmset_ch(OPMDATA *,int,int,int);
void opmset_muteall(void);

const int slot[4] = { 0, 2, 1, 3 };  // opm slot number
                                     // => M1(OP1),M2(OP3),C1(OP2),C2(OP4)
// アルゴリズム(con)ごとの、最初のキャリアのスロット位置。
//   slot[i] がこれ未満ならモジュレータなのでボリュームを掛けない。
const int carrier_slot_min[8] = { 3, 3, 3, 3, 2, 1, 1, 0 };

uchar channel_slotmask[8] = {0};
short channel_detune[8] = {0};
int   delay_count = 0;
int   start_channel = 0;
int   opm_scale_offset;

//
// set OPMDATA to OPM reg.
//   sw: OPM table number 0 or 1, or -1 for the current table
//
void SetCurrentTone(int sw)
{
	if( sw < 0 ) sw = tnsw;
	CT = (OPMD *)&Tone[tb[sw].tan][tb[sw].ton];  // カレントＯＰＭデータのセット
	opmset((OPMDATA *)CT, sw );
}

//
// set OPM data
//     p: OPMDATA
//    sw: OPM table number 0 or 1
//
void opmset(OPMDATA *p, int sw)
{
	int	i;
	int	r;
	int chflag;
	int	volume;

	OPMSET(OPMREG_NE_NFRQ, (int)(p->noise << 7 | p->nfreq));
	OPMSET(OPMREG_LFRQ,    (int)p->lfreq );
	OPMSET(OPMREG_CT_W,    (int)p->waveform );

	OPMSET(OPMREG_PMD_AMD, (int)(SEL_PMD_REG | p->pmd));
	OPMSET(OPMREG_PMD_AMD, (int)(SEL_AMD_REG | p->amd));

	chflag = (int)p->chflag;
	volume = (int)p->volume;

	r = 1;
	for(i=0; i<8; i++) {
		if(chflag & r) opmset_ch(p, sw, i, volume);
		r <<= 1;
	}
	set_slotmask();
}

//
// set OPM data (for channel)
//     p: OPMDATA
//    sw: OPM table number 0 or 1
//    ch: OPM channel
//     v: tone volume
//
void opmset_ch(OPMDATA *p, int sw, int ch, int v)
{
	int	i, tl;

    channel_slotmask[ch] = p->slotmask;

	OPMSET(OPMREG_RL_FB_CON+ch, (int)(p->pan << 6
						            | p->feedback << 3
						            | p->con ));

	OPMSET(OPMREG_PMS_AMS+ch,   (int)(p->pms << 4
						            | p->ams ));

	for(i=0; i<4; i++) {
		OPMSET(OPMREG_DT1_MUL+slot[i]*8+ch, (int)(p->reg[i].dt1 << 4
                                                | p->reg[i].mul ));

		if(slot[i] < carrier_slot_min[p->con])
			tl = 127-p->reg[i].tl;
		else
			tl = (((127-p->reg[i].tl) * v * Vol[sw][ch])/(127*127));

		OPMSET(OPMREG_TL+slot[i]*8+ch, 127-tl );

		OPMSET(OPMREG_KS_AR+slot[i]*8+ch,      (int)(p->reg[i].ks << 6
										           | p->reg[i].ar ));

		OPMSET(OPMREG_AMS_EN_D1R+slot[i]*8+ch, (int)(p->reg[i].amse << 7
										           | p->reg[i].d1r ));

		OPMSET(OPMREG_DT2_D2R+slot[i]*8+ch,    (int)(p->reg[i].dt2 << 6
										           | p->reg[i].d2r ));

		OPMSET(OPMREG_D1L_RR+slot[i]*8+ch,     (int)(p->reg[i].d1l << 4
										           | p->reg[i].rr ));
	}
}

//
// OPM mute
//
void opmset_muteall(void)
{
	int i;

	for(i=0; i<32; i++) OPMSET(OPMREG_TL+i,127);
	C_MES(C33, MES_OPM_MUTE, MESTIME_SHORT);
}
