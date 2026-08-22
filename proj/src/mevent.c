//
//	mouse event
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<string.h>
#include	<stdlib.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include    "file.h"
#include	"disp.h"
#include	"ms_map.h"
#include    "message.h"
#include    "json.h"
#include    "scalekey.h"
#include    "opm.h"
#include	"minmax.h"
#include    "flag.h"
#include    "undo.h"
#include	"key_input.h"

#define MOUSE_WAIT_COUNT_BASE (17)

extern void ms_color(int);
extern int  stat_lockflag(int,int,int);

extern void	MakeNameBufferAll(int);
extern void	MakeNameBuffer(int,int);
extern void	PutToneChange(int);
extern void	PutToneTable(int);
extern void	PutVolume(void);
extern void	PutOPMData(int,int);
extern void	PutOPMChannel(void);
extern void PutOPMTable(int);
extern void	SetCurrentTone(int);

extern int  checkLockflag_para(int,int);
extern void ev_ChangeToneName(int,int);
extern void undo_invalidate(int);
extern int  proc_editflag(int,int,int);
extern int  proc_selectflag(int,int,int);
extern int  stat_selectflag(int,int,int);

extern void set_scalekey_opm_channel(int);
extern void set_scalekey_midi_channel_filter(int);
extern void set_unison_polyphonic_count(int,int);
extern void set_scalekey_channel_assign_policy(int);
extern int  set_opm_scale_offset(int);
extern void set_delay_count(int);
extern int  set_start_scan_channel(int);
extern void set_channel_detune(void);
extern void midi_keyoff_all(void);

extern void	C_MES(char *,char *,int);

extern int	fileedit_flag[3];
extern int	opmcopy_count[2];
extern int  tonememo_dispflag;
extern int  scalekey_opm_channel;
extern int  scalekey_unison_count;
extern int  scalekey_poly_count;
extern int  scalekey_channel_assign_policy;
extern int  scalekey_midi_channel_filter;
extern int  scalekey_midi_board_not_exist;
extern int  opm_scale_offset;
extern int  delay_count;
extern int  start_channel;
extern short channel_detune[8];

extern int      dispflag[3][TONENUM];
extern OPMDATA	Tone[3][TONENUM];
extern int      toneflag[3][TONENUM];
extern int      tonetable_dispflag[3];
extern int	    tt[3];
extern TABLENUM tb[2];
extern int	    tnsw;
extern int	    Vol[2][8];
extern CONFIG   config;

void mev_ToneCopy(int,int,int,int,int,int *,int *,int);
void mev_ToneSelect(int,int,int,int,int *,int *,int);
void mev_ToneMark(int,int,int,int,int *,int *,int);
void mev_OPMChSelect(int,int,int,int,int *,int *,int);
void mev_ChVolume(int,int,int,int,int *,int *,int);
void mev_MasterVolume(int,int,int,int,int *,int *,int);
void mev_OPMCopy(int,int,int,int,int *,int *,int);
void mev_OPMCopyUNDO(int,int,int,int,int *,int *,int);
void mev_ScalekeyOffset(int,int,int *,int *);
void mev_ScalekeyPlaymode(int,int,int *,int *,int);
void mev_ScalekeySelectPolicy(int,int,int *,int *);
void mev_ScalekeyMIDIChannelFilter(int,int,int *,int *);
void mev_ScalekeyOPMChSelect(int,int,int,int,int *,int *);
void mev_ScalekeyDelayCount(int,int,int *,int *);
void mev_ScalekeyStartChannel(int,int,int *,int *);
void mev_ScalekeyDetune(int,int,int,int,int *,int *,int);

int  recalc_effective_count(int,int);
void clip_poly_uni_count(void);

void tonecopy_disarm(void);
int  tonecopy_is_armed(void);
void tonecopy_get_src(int *,int *);
void ToneCopy(int,int,int,int);

UNDO_BUFFER undo;        // Undo buffer

// Upper limits the user has settled on, updated from mev_ScalekeyPlaymode().
int   user_max_poly_count   = MAX_SCALEKEY_POLY_COUNT;      // minmax.h
int   user_max_unison_count = DEFAULT_CFG_SCALEKEY_UNISON;  // json.h

//
// clip poly_count & uni_count
//
void clip_poly_uni_count(void)
{
	if( scalekey_poly_count   > user_max_poly_count   ) scalekey_poly_count   = user_max_poly_count;
	if( scalekey_unison_count > user_max_unison_count ) scalekey_unison_count = user_max_unison_count;
}

//
// recalc effective count
//    mode: see enum RECALC_MODE
//
int recalc_effective_count(int channel, int mode)
{
	int i;
	int c;

	c = 0;

	for(i=0;i<8;i++) {
		if(channel&(1u<<i)) c++;
	}
	if(mode == RECALC_COUNT_ONLY) return c;

	if(mode == RECALC_UNISON_PRIORITY) {
		if(user_max_unison_count > c) scalekey_unison_count = c;
		else                          scalekey_unison_count = user_max_unison_count;

		if(!scalekey_unison_count) {
			scalekey_unison_count = 1;
			scalekey_poly_count = user_max_poly_count;
		} else {
			scalekey_poly_count = c/scalekey_unison_count;
		}
	} else {
		if(user_max_poly_count > c) scalekey_poly_count = c;
		else                        scalekey_poly_count = user_max_poly_count;

		if(!scalekey_poly_count) {
			scalekey_unison_count = c;
		} else {
			scalekey_unison_count = c/scalekey_poly_count;
		}
	}

	clip_poly_uni_count();

	return -1;
}

//
// backup undo buffer
//
static void backup_undo_buffer(int fileno, int slotno, int src_tnsw, int src_fileno, int src_slotno)
{
	undo.flag = 1;

	undo.fileno = fileno;
	undo.slotno = slotno;

	undo.src_tnsw   = src_tnsw;
	undo.src_fileno = src_fileno;
	undo.src_slotno = src_slotno;

	// backup fileedit_flag
	undo.editflag = fileedit_flag[fileno];
	undo.toneflag = toneflag[fileno][slotno];

	// backup Tone data
	memcpy(&undo.data, &Tone[fileno][slotno], (int)sizeof(OPMDATA));
}

//
// restore undo buffer
//
static void restore_undo_buffer(void)
{
	int fileno;
	int slotno;

	fileno = undo.fileno;
	slotno = undo.slotno;

	// restore fileedit_flag
	fileedit_flag[fileno]    = undo.editflag;
	toneflag[fileno][slotno] = undo.toneflag;

	// restore Tone data
	memcpy(&Tone[fileno][slotno], &undo.data, (int)sizeof(OPMDATA));
}

//
// OPM Copy (between OPM parameter table0/1)
//
void mev_OPMCopy(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	int src, dest;
	int stan, ston;   // source tone tan, tone number
	int dtan, dton;   // dest   tone tan, tone number
	int i;

	(void)mx;
	(void)my;
	(void)br;
	(void)sbr;

	if(map_value == MAP_OPMCOPY0) {src = 1; dest = 0;}
	else                          {src = 0; dest = 1;}

	if(bl && !*sbl) {
		*sbl = -1;

		stan = tb[src].tan;
		ston = tb[src].ton;
		dtan = tb[dest].tan;
		dton = tb[dest].ton;

		if(checkLockflag_para(dtan, dton)) return;     // check Lock flag
		if((stan == dtan) && (ston == dton)) return;    // same tone then return;

		// backup undo buffer
		backup_undo_buffer(dtan, dton, src, stan, ston);

		// set opmcopy_count
		opmcopy_count[0] = opmcopy_count[1] = 0;
		opmcopy_count[dest] = OPMCOPY_COUNT;

		// copy Tone data
		Tone[dtan][dton].con      = Tone[stan][ston].con;
		Tone[dtan][dton].feedback = Tone[stan][ston].feedback;
		Tone[dtan][dton].slotmask = Tone[stan][ston].slotmask;
		Tone[dtan][dton].pan      = Tone[stan][ston].pan;
		Tone[dtan][dton].pms      = Tone[stan][ston].pms;
		Tone[dtan][dton].ams      = Tone[stan][ston].ams;
		Tone[dtan][dton].sync     = Tone[stan][ston].sync;
		Tone[dtan][dton].waveform = Tone[stan][ston].waveform;
		Tone[dtan][dton].lfreq    = Tone[stan][ston].lfreq;
		Tone[dtan][dton].pmd      = Tone[stan][ston].pmd;
		Tone[dtan][dton].amd      = Tone[stan][ston].amd;
		Tone[dtan][dton].noise    = Tone[stan][ston].noise;
		Tone[dtan][dton].nfreq    = Tone[stan][ston].nfreq;

		for(i=0; i<4; i++) {
			Tone[dtan][dton].reg[i].ar   = Tone[stan][ston].reg[i].ar;
			Tone[dtan][dton].reg[i].d1r  = Tone[stan][ston].reg[i].d1r;
			Tone[dtan][dton].reg[i].d2r  = Tone[stan][ston].reg[i].d2r;
			Tone[dtan][dton].reg[i].rr   = Tone[stan][ston].reg[i].rr;
			Tone[dtan][dton].reg[i].d1l  = Tone[stan][ston].reg[i].d1l;
			Tone[dtan][dton].reg[i].tl   = Tone[stan][ston].reg[i].tl;
			Tone[dtan][dton].reg[i].ks   = Tone[stan][ston].reg[i].ks;
			Tone[dtan][dton].reg[i].mul  = Tone[stan][ston].reg[i].mul;
			Tone[dtan][dton].reg[i].dt1  = Tone[stan][ston].reg[i].dt1;
			Tone[dtan][dton].reg[i].dt2  = Tone[stan][ston].reg[i].dt2;
			Tone[dtan][dton].reg[i].amse = Tone[stan][ston].reg[i].amse;
		}

		// set fileedit_flag
		proc_editflag(SET_EDITFLAG|EDIT_TONE_OPMCOPY, dtan, dton);

		// disp dest. OPM table
		PutOPMTable(dest);
		PutOPMData(1,-1);
		SetCurrentTone(-1);
	}
}

//
// UNDO OPMCopy
//
void mev_OPMCopyUNDO(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	(void)mx;
	(void)my;
	(void)br;
	(void)sbr;

	if(bl && !*sbl) {
		*sbl = -1;

		// check tnsw
		if(undo.src_tnsw == 0 && map_value == MAP_OPMCOPY_UNDO0) return;
		if(undo.src_tnsw == 1 && map_value == MAP_OPMCOPY_UNDO1) return;

		// restore
		restore_undo_buffer();
		undo_invalidate(UNDO_RESTORE);

		tonetable_dispflag[undo.fileno] = 1;
		MakeNameBuffer(undo.fileno, undo.slotno);
		tonememo_dispflag = 1;

		// disp OPM table 0 and 1
		PutOPMTable(0);
		PutOPMTable(1);
		PutOPMData(1,-1);
		SetCurrentTone(-1);
	}
}

//
// select opm channel for scalekey
//
void mev_ScalekeyOPMChSelect(int mx, int my, int bl, int br, int *sbl, int *sbr)
{
	int	c;
	int x;

	(void)my;

    x = SCALEKEY_OPMCH_XPOS;

	if(bl && !*sbl) {
		*sbl = -1;

		c = ((mx/8)-x)/4;
		scalekey_opm_channel ^= (1<<c);

		scalekey_poly_count = MAX_SCALEKEY_POLY_COUNT;
		recalc_effective_count(scalekey_opm_channel, RECALC_UNISON_PRIORITY);

		set_scalekey_opm_channel(scalekey_opm_channel);
		set_unison_polyphonic_count(scalekey_unison_count, scalekey_poly_count);
	}
	if(br && !*sbr) {
		*sbr = -1;

		if( scalekey_opm_channel )
			scalekey_opm_channel = 0x00;    // 0b0000_0000;
		else
			scalekey_opm_channel = 0xff;    // 0b1111_1111;

		scalekey_poly_count = MAX_SCALEKEY_POLY_COUNT;
		recalc_effective_count(scalekey_opm_channel, RECALC_UNISON_PRIORITY);

		set_scalekey_opm_channel(scalekey_opm_channel);
		set_unison_polyphonic_count(scalekey_unison_count, scalekey_poly_count);
	}
}

//
// Select OPM channel
//
void mev_OPMChSelect(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	int sw;
	int	c;
	int x;
	int tan, ton;
	uchar pre;

	(void)my;

	if(map_value == MAP_CHANNEL0) {sw = 0; x = OPMCHANNEL01_XPOS;}
	else                          {sw = 1; x = OPMCHANNEL11_XPOS;}

	tan = tb[sw].tan;
	ton = tb[sw].ton;

    if(checkLockflag_para(tan, ton)) return;

	pre = Tone[tan][ton].chflag;

	if(bl && !*sbl) {
		*sbl = -1;

		c = ((mx/8)-x)/4;
		Tone[tan][ton].chflag ^= (1<<c);
	}
	if(br && !*sbr) {
		*sbr = -1;

		if(Tone[tan][ton].chflag) Tone[tan][ton].chflag = 0x00;
		else                      Tone[tan][ton].chflag = 0xff;
	}

	if(pre != Tone[tan][ton].chflag) {
		PutOPMChannel();
		SetCurrentTone(-1);
		// set fileedit_flag
		proc_editflag(SET_EDITFLAG|EDIT_TONE_OPMCHSEL, tan, ton);
	}
}

//
// tone copy : source (module-local single source of truth)
//
static int src_tan = -1;
static int src_ton = -1;

//
// clear src highlight (dispflag/namebuf/redisp)。src変数・ms_colorは触らない
//
static void clear_src_highlight(void)
{
	if(src_tan < 0) return;
	dispflag[src_tan][src_ton] &= ~DISP_TONECOPY;   // 他ビット保存（select共存に備える）
	MakeNameBuffer(src_tan, src_ton);
	tonetable_dispflag[src_tan] = 1;                // 再描画は res_routine に委ねる
}

//
// disarm (cancel/clear)。coordsを持たない全cases共通の解除口
//
void tonecopy_disarm(void)
{
	if(src_tan < 0) return;
	clear_src_highlight();
	src_tan = -1;
	src_ton = -1;
	ms_color(0);
}

//
// getters (armed判定・src取得)。ms_routine dispatch と res_routine表示が使う
//
int  tonecopy_is_armed(void)          { return (src_tan >= 0); }
void tonecopy_get_src(int *tan, int *ton) { *tan = src_tan; *ton = src_ton; }

//
// Tone Copy (executor)。責務はコピー動作のみ。armed判定は外(getter)に出す
//
void mev_ToneCopy(int action, int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	int tan, ton;
	int ypos;
	int tonemax;

	(void)mx;

	tan     = (map_value==MAP_TONETABLE0) ? 0 :
	          (map_value==MAP_TONETABLE1) ? 1 : 2;
	tonemax = (tan==2) ? TONENUM2 : TONENUM;
	ypos    = my/16 - ((tan==0) ? TONETABLE0_YPOS :
	                   (tan==1) ? TONETABLE1_YPOS : TONETABLE2_YPOS);
	ton     = (tt[tan] + ypos) % tonemax;

	switch(action) {
		case TONECOPY_SET:                                  // CTRL+左：src確定 / 差し替え
			if(!(bl && !*sbl)) break;
			*sbl = -1;

			if(src_tan==tan && src_ton==ton) break;         // 同一srcは無視
			clear_src_highlight();                          // 旧srcを消す
			dispflag[tan][ton] |= DISP_TONECOPY;            // 新srcを立てる
			MakeNameBuffer(tan, ton);
			tonetable_dispflag[tan] = 1;
			src_tan = tan;
			src_ton = ton;
			ms_color(1);
			break;

		case TONECOPY_COPY:                                 // armed中の左：dest確定
			if(!(bl && !*sbl)) break;
			*sbl = -1;

			if(src_tan < 0) break;
			if(src_tan==tan && src_ton==ton) break;         // src==dest は no-op（armed維持）
			if(stat_lockflag(STAT_EFFECTIVE, tan, ton)) {   // dest locked
				C_MES(C32, MES_TONE_IS_LOCKED2, MESTIME_NORMAL);
				break;
			}
			ToneCopy(src_tan, src_ton, tan, ton);
			proc_editflag(SET_EDITFLAG|EDIT_TONE_TONECOPY, tan, ton);
			if(undo.src_fileno==tan && undo.src_slotno==ton) undo_invalidate(UNDO_TONECOPY);

			clear_src_highlight();                          // src pane を戻す（src_tan有効なうちに）
			src_tan = -1;
			src_ton = -1;
			MakeNameBuffer(tan, ton);                       // dest name を更新
			tonetable_dispflag[tan] = 1;
			ms_color(0);

			PutOPMTable(0);      // disp OPM table 0
			PutOPMTable(1);      // disp OPM table 1
			PutOPMData(1,-1);    // disp current OPM data
			break;

		case TONECOPY_CLEAR:                                // armed中の右：取消
			if(!(br && !*sbr)) break;
			*sbr = -1;
			tonecopy_disarm();
			break;
	}
}

//
// tone copy sub. POD全コピー。mark/chmode は dest 維持
//
void ToneCopy(int stan, int ston, int dtan, int dton)
{
	OPMDATA keep = Tone[dtan][dton];

	Tone[dtan][dton]        = Tone[stan][ston];   // 構造体代入1行
	Tone[dtan][dton].mark   = keep.mark;          // lock/select : dest 維持
	Tone[dtan][dton].chmode = keep.chmode;        // channel mode : dest 維持（旧挙動踏襲）
}

//
// OPM Channel volume
//
void mev_ChVolume(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	static int cnt0 = 0;
	static int cnt1 = 0;
	int pre;
	int	c;
	int x, sw;
	int ce;
	int shift, ctrl;

	(void)my;

	ce = BITSNS(0x0E);
	shift = ce & 1;
	ctrl  = ce & 2;

	// sw = 0 or 1
	if(map_value == MAP_CHANNELVOL0) {sw = 0; x = OPMVOLUME01_XPOS;}
	else                             {sw = 1; x = OPMVOLUME11_XPOS;}

	// channel volume は tone data ではなく tone editor の実行時パラメータ。
	// OED には保存されるが、file lock / tone lock の対象外とする。

	c = ((mx/8)-x)/4;
	pre = Vol[sw][c];

	if(bl) {
		if(*sbl) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
		    *sbl = -1;
			cnt0 = 0;
			if(ctrl) Vol[sw][c] = MAX_CHANNEL_VOLUME;
		}

		if(!(*sbl<0 && !cnt0 && ctrl)) {
			if(shift) Vol[sw][c] += 10;
			else      Vol[sw][c]++;

			if(Vol[sw][c] > MAX_CHANNEL_VOLUME) {
				Vol[sw][c] = MAX_CHANNEL_VOLUME;
			}
		}
	}
	if(br) {
		if(*sbr) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
			*sbr = -1;
			cnt0 = 0;
			if(ctrl) Vol[sw][c] = MIN_CHANNEL_VOLUME;
		}

		if(!(*sbr<0 && !cnt0 && ctrl)) {
			if(shift) Vol[sw][c] -= 10;
			else      Vol[sw][c]--;

			if(Vol[sw][c] < MIN_CHANNEL_VOLUME) {
				Vol[sw][c] = MIN_CHANNEL_VOLUME;
			}
		}
	}

	if(pre != Vol[sw][c]) {
		PutVolume();
		SetCurrentTone(sw);
		// set fileedit_flag
		proc_editflag(SET_EDITFLAG|EDIT_FILE_CHVOLUME, 0, 0);
	}
}


//
// tone master volume
//
void mev_MasterVolume(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	static int cnt0 = 0;
	static int cnt1 = 0;
	uchar *volume;
	int pre;
	int intvol;
	int tan, ton;
	int sw;
	int ce;
	int shift, ctrl;

	(void)mx;
	(void)my;

	ce = BITSNS(0x0E);
	shift = ce & 1;
	ctrl  = ce & 2;

	// sw = 0 or 1
	sw = (map_value==MAP_MASTERVOL0) ? 0 : 1;
	tan = tb[sw].tan;
	ton = tb[sw].ton;

    if(checkLockflag_para(tan, ton)) return;

	volume = &Tone[tan][ton].volume;
	pre = *volume;
	intvol = *volume;

	if(bl) {
		if(*sbl) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
		    *sbl = -1;
			cnt0 = 0;
			if(ctrl) intvol = MAX_MASTER_TONE_VOLUME;
		}

		if(!(*sbl<0 && !cnt0 && ctrl)) {
			if(shift) intvol += 10;
			else      intvol++;

			if(intvol > MAX_MASTER_TONE_VOLUME) {
				intvol = MAX_MASTER_TONE_VOLUME;
			}
		}
	}
	if(br) {
		if(*sbr) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
			*sbr = -1;
			cnt0 = 0;
			if(ctrl) intvol = MIN_MASTER_TONE_VOLUME;
		}

		if(!(*sbr<0 && !cnt0 && ctrl)) {
			if(shift) intvol -= 10;
			else      intvol--;

			if(intvol < MIN_MASTER_TONE_VOLUME) {
				intvol = MIN_MASTER_TONE_VOLUME;
			}
		}
	}

	if(pre != intvol) {
		Tone[tan][ton].volume = (uchar)(intvol & 0xFF);
		PutVolume();
		SetCurrentTone(sw);
		// set toneedit flag
		proc_editflag(SET_EDITFLAG|EDIT_TONE_VOLUME, tan, ton);
	}
}

//
// select tone
//
void mev_ToneSelect(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	int	ypos;  // ton
	int tan, ton;
	int pre_tan, pre_ton;
	int tonemax;

	(void)mx;

	tan  = (map_value==MAP_TONETABLE0) ? 0 :
	       (map_value==MAP_TONETABLE1) ? 1 : 2;
	ypos = my/16 - ((tan==0) ? TONETABLE0_YPOS :
	                (tan==1) ? TONETABLE1_YPOS : TONETABLE2_YPOS);

	if((tan == 0) && (fileedit_flag[0]<0)) return;
	if((tan == 1) && (fileedit_flag[1]<0)) return;
	if((tan == 2) && (fileedit_flag[0]<0) && (fileedit_flag[1]<0)) return;

	tonemax = (tan==2) ? TONENUM2 : TONENUM;
	ton = (tt[tan]+ypos)%tonemax;

	pre_tan = tb[tnsw].tan;
	pre_ton = tb[tnsw].ton;

	// left click => select tone
	if(bl && !*sbl) {
		*sbl = -1;

		// check same tone
		if(pre_tan==tan && pre_ton==ton) return;

		tb[tnsw].tan = tan;
		tb[tnsw].ton  = ton;

		// undo invalid
		undo_invalidate(UNDO_SLOTNO);

		// re-disp.
		MakeNameBuffer(pre_tan, pre_ton);
		MakeNameBuffer(tan, ton);

		SetCurrentTone(-1);
		PutToneChange(tnsw);
		tonememo_dispflag = 1;
	}

	// right click => change tone name
	//   No "same tone" guard here, unlike the left click: renaming does
	//   not depend on the selection, and there is no undo buffer to
	//   protect.
	if(br && !*sbr) {
		*sbr = -1;

		tb[tnsw].tan = tan;
		tb[tnsw].ton  = ton;

		MakeNameBuffer(pre_tan, pre_ton);
		MakeNameBuffer(tan, ton);

		SetCurrentTone(-1);
		PutToneChange(tnsw);

		ev_ChangeToneName(-1,-1);
	}
}

//
// tone select mark (for export tone data)
//
void
mev_ToneMark(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	int tan;	// table number 0-2
	int	ypos;	// tone number
	int tonemax;
	TABLENUM n;

	(void)mx;
	(void)br;
	(void)sbr;

	tan  = (map_value==MAP_TONEMARK0) ? 0 :
	       (map_value==MAP_TONEMARK1) ? 1 : 2;
	ypos = my/16 - ((tan==0) ? TONETABLE0_YPOS :
	                (tan==1) ? TONETABLE1_YPOS : TONETABLE2_YPOS);
	tonemax = (tan==2) ? TONENUM2 : TONENUM;

	if(bl && !*sbl) {
		*sbl = -1;

		n.tan = tan;
		n.ton  = (tt[tan]+ypos)%tonemax;

		// select は音色データの編集ではないため、
		// file lock / tone lock のどちらの影響も受けない。
		if(stat_selectflag(STAT_TONE, n.tan, n.ton)) {
			proc_selectflag(CLEAR_SELECTFLAG, n.tan, n.ton);
		} else {
			proc_selectflag(SET_SELECTFLAG, n.tan, n.ton);
		}

		// re-disp.
		tonetable_dispflag[n.tan] = 1;
		MakeNameBuffer(n.tan, n.ton);

		PutToneTable(n.tan);
		PutOPMTable(0);
		PutOPMTable(1);
		PutOPMData(1,-1);
	}
}

//
// change scalekey offset
//
void mev_ScalekeyOffset(int bl, int br, int *sbl, int *sbr)
{
	static int cnt0 = 0;
	static int cnt1 = 0;
	int ce;
	int shift, ctrl;

	ce = BITSNS(0x0E);
	shift = ce & 1;
	ctrl  = ce & 2;

    if(bl) {
		if(*sbl) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
		    *sbl = -1;
			cnt0 = 0;
			if(ctrl) opm_scale_offset = config.scalekey.offset;
		}

		if(!(*sbl<0 && !cnt0 && ctrl)) {
			if(shift) opm_scale_offset += 12;
			else      opm_scale_offset++;

			if(opm_scale_offset > MAX_SCALEKEY_OFFSET) {
				opm_scale_offset = MAX_SCALEKEY_OFFSET;
			}
		}
		set_opm_scale_offset(opm_scale_offset);
	}
	if(br) {
		if(*sbr) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
			*sbr = -1;
			cnt0 = 0;
			if(ctrl) opm_scale_offset = config.scalekey.offset;
		}

		if(!(*sbr<0 && !cnt0 && ctrl)) {
			if(shift) opm_scale_offset -= 12;
			else      opm_scale_offset--;

			if(opm_scale_offset < MIN_SCALEKEY_OFFSET) {
				opm_scale_offset = MIN_SCALEKEY_OFFSET;
			}
		}
		set_opm_scale_offset(opm_scale_offset);
	}
}

//
// change delay_count
//
void mev_ScalekeyDelayCount(int bl, int br, int *sbl, int *sbr)
{
	static int cnt0 = 0;
	static int cnt1 = 0;
	int ce;
	int shift, ctrl;

	ce = BITSNS(0x0E);
	shift = ce & 1;
	ctrl  = ce & 2;

    if(bl) {
		if(*sbl) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
		    *sbl = -1;
			cnt0 = 0;
			if(ctrl) delay_count = 0;
		}

		if(!(*sbl<0 && !cnt0 && ctrl)) {
			if(shift) delay_count += 10;
			else      delay_count++;
		}

		if(delay_count > MAX_SCALEKEY_DELAY_COUNT) {
			delay_count = MAX_SCALEKEY_DELAY_COUNT;
		}
		set_delay_count(delay_count);
	}
	if(br) {
		if(*sbr) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
		} else {
			*sbr = -1;
			cnt0 = 0;
			if(ctrl) delay_count = 0;
		}

		if(!(*sbr<0 && !cnt0 && ctrl)) {
			if(shift) delay_count -= 10;
			else      delay_count--;
		}

		if(delay_count < MIN_SCALEKEY_DELAY_COUNT) {
			delay_count = MIN_SCALEKEY_DELAY_COUNT;
		}
		set_delay_count(delay_count);
	}
}

//
// change start_channel
//
void mev_ScalekeyStartChannel(int bl, int br, int *sbl, int *sbr)
{
	static int cnt0 = 0;
	static int cnt1 = 0;

    if(bl && !*sbl) {
		*sbl = -1;

		start_channel++;
		if(start_channel > MAX_SCALEKEY_START_CHANNEL) {
			start_channel = MIN_SCALEKEY_START_CHANNEL;
		}
		set_start_scan_channel(start_channel);
	}
	if(br && !*sbr) {
		*sbr = -1;

		start_channel--;
		if(start_channel < MIN_SCALEKEY_START_CHANNEL) {
			start_channel = MAX_SCALEKEY_START_CHANNEL;
		}
		set_start_scan_channel(start_channel);
	}
}

//
// change scalekey MIDI channel filter
//
void mev_ScalekeyMIDIChannelFilter(int bl, int br, int *sbl, int *sbr)
{
	if(scalekey_midi_board_not_exist) return;

    if(bl && !*sbl) {
		*sbl = -1;

		if(scalekey_midi_channel_filter < MAX_MIDI_CHANNEL_FILTER) {
			scalekey_midi_channel_filter++;
			set_scalekey_midi_channel_filter(scalekey_midi_channel_filter);
			midi_keyoff_all();
		}
	}
	else if(br && !*sbr) {
		*sbr = -1;

		if(scalekey_midi_channel_filter > MIN_MIDI_CHANNEL_FILTER) {
			scalekey_midi_channel_filter--;
			set_scalekey_midi_channel_filter(scalekey_midi_channel_filter);
			midi_keyoff_all();
		}
	}
}

//
// change scalekey channel assign policy
//
void mev_ScalekeySelectPolicy(int bl, int br, int *sbl, int *sbr)
{
    if(bl && !*sbl) {
		*sbl = -1;
		scalekey_channel_assign_policy ^= 1;
		set_scalekey_channel_assign_policy(scalekey_channel_assign_policy);
	}
	else if(br && !*sbr) {
		*sbr = -1;
		scalekey_channel_assign_policy ^= 2;
		set_scalekey_channel_assign_policy(scalekey_channel_assign_policy);
	}
}

//
// change scalekey playmode (poly count & unison count)
//
//   The left and right paths are asymmetric on purpose:
//     - raising unison lets poly fall along with it, down to 1
//     - lowering unison lets poly rise again, up to user_max_poly_count
//   Do not make them symmetric.
//
void mev_ScalekeyPlaymode(int bl, int br, int *sbl, int *sbr, int map_value)
{
    if(bl && !*sbl) {
		*sbl = -1;
		if(map_value == MAP_SCALEKEY_UNISON) {
			if(scalekey_unison_count < MAX_SCALEKEY_UNISON_COUNT) {
				scalekey_unison_count++;
				user_max_unison_count = scalekey_unison_count;

				recalc_effective_count(scalekey_opm_channel, RECALC_UNISON_PRIORITY);
				set_unison_polyphonic_count(scalekey_unison_count, scalekey_poly_count);
			}
		} else {
			if(scalekey_poly_count < recalc_effective_count(scalekey_opm_channel, RECALC_COUNT_ONLY)) {
				scalekey_poly_count++;
				user_max_poly_count = scalekey_poly_count;

				recalc_effective_count(scalekey_opm_channel, RECALC_POLY_PRIORITY);
				set_unison_polyphonic_count(scalekey_unison_count, scalekey_poly_count);
			}
		}
	}
	else if(br && !*sbr) {
		*sbr = -1;
		if(map_value == MAP_SCALEKEY_UNISON) {
			if(scalekey_unison_count > MIN_SCALEKEY_UNISON_COUNT) {
				scalekey_unison_count--;
				user_max_unison_count = scalekey_unison_count;
				scalekey_poly_count = user_max_poly_count;

				recalc_effective_count(scalekey_opm_channel, RECALC_UNISON_PRIORITY);
				set_unison_polyphonic_count(scalekey_unison_count, scalekey_poly_count);
			}
		} else {
			if(scalekey_poly_count > MIN_SCALEKEY_POLY_COUNT) {
				scalekey_poly_count--;
				user_max_poly_count = scalekey_poly_count;

				recalc_effective_count(scalekey_opm_channel, RECALC_POLY_PRIORITY);
				user_max_unison_count = scalekey_unison_count;
				set_unison_polyphonic_count(scalekey_unison_count, scalekey_poly_count);
			}
		}
	}
}

//
// Scalekey Channel Detune
//
void mev_ScalekeyDetune(int mx, int my, int bl, int br, int *sbl, int *sbr, int map_value)
{
	static int cnt0 = 0;
	static int cnt1 = 0;
	int	c;
	int x;
	int ce;
	int shift, ctrl;

	(void)my;

	x = SCALEKEY_DETUNE_XPOS;
	c = ((mx/8)-x)/4;

	ce = BITSNS(0x0E);
	shift = ce & 1;
	ctrl  = ce & 2;

	if(bl) {
		if(*sbl) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
			ctrl = 0;
		} else {
		    *sbl = -1;
			cnt0 = 0;

			if(ctrl) {
				if(channel_detune[c] & 0b111111) channel_detune[c] &= ~(0b111111);
				else                             channel_detune[c] = 0;
			}
		}

		if(!ctrl) {
			if(shift) channel_detune[c] += 64;
			else      channel_detune[c]++;
		}

		if(channel_detune[c] > MAX_SCALEKEY_DETUNE) {
			channel_detune[c] = MAX_SCALEKEY_DETUNE;
		}
		set_channel_detune();
	}
	if(br) {
		if(*sbr) {
			if(++cnt0 < MOUSE_WAIT_COUNT_BASE) return;
			cnt0--;
			if(++cnt1 < config.mouse_repeat_speed) return;
			cnt1 = 0;
			ctrl = 0;
		} else {
			*sbr = -1;
			cnt0 = 0;

			if(ctrl) {
				if(channel_detune[c] & 0b111111) channel_detune[c] &= ~(0b111111);
				else                             channel_detune[c] = 0;
			}
		}

		if(!ctrl) {
			if(shift) channel_detune[c] -= 64;
			else      channel_detune[c]--;
		}

		if(channel_detune[c] < MIN_SCALEKEY_DETUNE) {
			channel_detune[c] = MIN_SCALEKEY_DETUNE;
		}
		set_channel_detune();
	}
}
