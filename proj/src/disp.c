//
//	disp.c
//

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<string.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include	"disp.h"
#include	"message.h"
#include	"json.h"
#include    "flag.h"
#include    "undo.h"

extern int      dispflag[3][TONENUM];
extern OPMDATA	Tone[3][TONENUM];		     // ＯＰＭデータ
extern OPMD	    *CT;				         // カレントＯＰＭデータ
extern int	    tt[3];
extern TABLENUM tb[2];
extern int	    tnsw;
extern int	    ln;				  // カーソル
extern CURSOR   cur[REGNUM];      // 表示位置データ
extern int	    Vol[2][8];		  // チャネル音量
extern CONFIG   config;
extern int      redisp_req;

extern void	C_CLEAR(void);
extern void	C_YELLOW(char *);
extern void	C_LBLUE(char *);
extern void	C_YELLOW_R(char *);
extern void	C_LBLUE_R(char *);
extern void	C_FILENAME(int,int);
extern void	C_OPMTONENAME(int,int);
extern int  stat_selectflag(int,int,int);

extern UNDO_BUFFER undo;

void	MakeNameBufferAll(int);
void	MakeNameBuffer(int,int);
void	scinit(void);
void	exitsc(void);
void	scsetup(void);
void	PutToneTable(int);
void	PutToneChange(int);
void	PutOPMTable(int);
void	MakeOPMData(int,int);
void	PutOPMData(int,int);
void	PutOPMChannel(void);
void	PutExecMenu(int);
void	PutVolume(void);
void	PutCurrentTableMark(void);
void	PutOPMCopy(void);
void    PutUNDO(int);
char    *MakeStrScalekeyMode(int,int,int);
void    PutOPMChannelScalekey(void);

char	NameBuffer[3][TONENUM][128];	 // 音色名表示用バッファ
char	buf[6][256];		// 表示用テンポラリバッファ
                            //   PutOPMTable() は MakeOPMData() で buf[0..4]
                            //   を作ってから表示する。それより前に呼ぶ
                            //   C_OPMTONENAME / PutOPMChannel / PutVolume も
                            //   buf[0..1] を使うため、呼び出し順を変えないこと。
int     opmcopy_count[2];   // opm copy disp count

int		template_all_lock = 1;


// for scalekey
extern int opm_scale_offset;
extern int delay_count;
extern int start_channel;
extern int scalekey_channel_assign_policy;
extern int scalekey_midi_board_not_exist;

int scalekey_poly_count;
int scalekey_unison_count;
int scalekey_opm_channel;
int scalekey_midi_channel_filter;

// disp.def
#include "disp.def"

//
// init screen
//
void scinit(void)
{
	CRTMOD(16);
	C_CUROFF();
}

//
// screen setup for exit
//
void exitsc(void)
{
	B_LOCATE( MES_XPOS, MES_YPOS );
	B_PRINT( "\n" );
	C_CURON();
}

//
// disp initial screen
//
void scsetup(void)
{
	C_CLEAR();

	redisp_req = 1;

	PutOPMTable(0);	 	 // OPMテーブルの表示
	PutOPMTable(1);

	opmcopy_count[0] = 0;     // clear opm copy disp count
	opmcopy_count[1] = 0;

	PutOPMData(1,-1);
	PutCurrentTableMark();
}

//
// PutOPMCopy
//
void PutOPMCopy(void)
{
	B_LOCATE(OPMCOPY0_XPOS, OPMCOPY0_YPOS);
	if(!opmcopy_count[0]) {B_PRINT(C31); B_PRINT(OPMCOPY0_CHARA0);}
	else                  {B_PRINT(C42); B_PRINT(OPMCOPY0_CHARA1);}

	B_LOCATE(OPMCOPY1_XPOS, OPMCOPY1_YPOS);
	if(!opmcopy_count[1]) {B_PRINT(C31); B_PRINT(OPMCOPY1_CHARA0);}
	else                  {B_PRINT(C42); B_PRINT(OPMCOPY1_CHARA1);}
}

//
// PutUNDO
//
void PutUNDO(int flag)
{
	// clear
	B_LOCATE(OPMCOPY_UNDO0_XPOS,OPMCOPY_UNDO0_YPOS);
	B_PRINT(OPMCOPY_UNDO_CHARA0);
	B_LOCATE(OPMCOPY_UNDO1_XPOS,OPMCOPY_UNDO1_YPOS);
	B_PRINT(OPMCOPY_UNDO_CHARA0);

	if(flag) {
		// disp
		if(undo.src_tnsw) B_LOCATE(OPMCOPY_UNDO0_XPOS,OPMCOPY_UNDO0_YPOS);
		else              B_LOCATE(OPMCOPY_UNDO1_XPOS,OPMCOPY_UNDO1_YPOS);
		B_PRINT(OPMCOPY_UNDO_CHARA1);
	}
}

//
//
//     tan: OPM table number
//
void PutToneChange(int tan)
{
	PutToneTable(0);     // disp tone table
	PutToneTable(1);
	PutToneTable(2);
	PutOPMTable(0);      // disp OPM table 0
	PutOPMTable(1);      // disp OPM table 1
	PutOPMData(1, tan);  // disp current OPM data
}

//
// put tone table
//    tan: 0=file A (left pane)
//         1=file B (right pane)
//         2=template slot
//
typedef struct {
	int xpos;
	int ypos;
} POS;

static POS tonelist_pos[] = {
	{TONETABLE0_XPOS, TONETABLE0_YPOS},
	{TONETABLE1_XPOS, TONETABLE1_YPOS},
	{TONETABLE2_XPOS, TONETABLE2_YPOS}
};

static int tonelist_num[] = {
	TONETABLE0NUM,
	TONETABLE1NUM,
	TONETABLE2NUM
};

static int tonenum[] = {
	TONENUM,
	TONENUM,
	TONENUM2
};

//
// put tone list (tone table)
//
void PutToneTable(int tan)
{
	int	i;
	int	n;

	for(i=0; i < tonelist_num[tan]; i++) {
		n = (tt[tan]+i)%tonenum[tan];
		B_LOCATE(tonelist_pos[tan].xpos, tonelist_pos[tan].ypos+i);
		B_PRINT(NameBuffer[tan][n]);
	}
}

//
// MakeNameBufferAll
//
void MakeNameBufferAll(int tan)
{
	int i;

	for(i=0; i<((tan==2)?TONENUM2:TONENUM); i++) {
		MakeNameBuffer(tan, i);
	}
}

//
// Make Name Buffer
//
void MakeNameBuffer(int tan, int ton)
{
	char namebuf[256];
	char nbuf[256];

    int  flag_tonecopy;
	int  flag_edit;
	int  flag_select;
	int  flag_lock;
	int  flag_effect;
	char mark_chara;
	char *mark_color;
	char *name_color;
	int  current_flag;      // current tone flag
	int  sub_flag;
	int  sub_mark;

	flag_tonecopy = (dispflag[tan][ton] & DISP_TONECOPY);
	current_flag  = ((tan == tb[tnsw].tan) && (ton == tb[tnsw].ton));
	sub_flag      = ((tan == tb[tnsw^1].tan) && (ton == tb[tnsw^1].ton));

	flag_edit   = stat_editflag(STAT_TONE, tan, ton);
	flag_lock   = stat_lockflag(STAT_TONE, tan, ton);
	flag_effect = stat_lockflag(STAT_EFFECTIVE, tan, ton);
	flag_select = stat_selectflag(STAT_TONE, tan, ton);

    mark_chara = flag_lock ? TONELOCK_CHARA :
	             flag_edit ? TONEEDIT_CHARA : ' ';

	// force locked : C36 (lock & edit)
	//       locked : C37 (lock)
	//       edited : C33 (edit)
	//       normal : C33
	mark_color = (flag_lock && flag_edit) ? C36 :
	              flag_lock ? C37 : C33;

	// current:
	//     tonecopy : C42
	//       locked : C41
	//       edited : C43
	//       normal : C43
	// not current:
	//     tonecopy : C42
	//       locked : C31
	//       edited : C33
	//       normal : C33
	name_color = current_flag ? (flag_tonecopy ? C42 : (flag_effect ? C41 : C43)) :
						        (flag_tonecopy ? C42 : (flag_effect ? C31 : C33)) ;

	sub_mark = (sub_flag && !current_flag && !flag_tonecopy);

	namebuf[0] = '\0';
	if(tan<2) {
	  if(flag_select) sprintf(namebuf, "%s%3d%s%c",
		                         (tan==0) ? C41 : C42, ton+1, mark_color, mark_chara);
	  else		      sprintf(namebuf, "%s%3d%s%c",
		                         (tan==0) ? C31 : C32, ton+1, mark_color, mark_chara);
	} else {
	  if(flag_select) sprintf(namebuf, "%s%c%02d%s%c",
		                         C42, LOCALTONE_CHARA, ton+1, mark_color, mark_chara);
	  else            sprintf(namebuf, "%s%c%02d%s%c",
		                         C32, LOCALTONE_CHARA, ton+1, mark_color, mark_chara);
	}

	memset(nbuf, 0, (int)sizeof(nbuf));
	memcpy(nbuf, Tone[tan][ton].name, (int)sizeof(Tone[tan][ton].name));
	nbuf[TONENAME_LENGTH] = '\0';

	NameBuffer[tan][ton][0] = '\0';

	// 末尾 SUB_MARK_WIDTH バイトを反転（sub cursor marker）。
	// 注: 全角(SJIS)が末尾境界に跨ると末尾が化けるが、F1で再描画復帰可能。
	//     文字境界対応は大規模になるため v1.20 では未対応（既知の制約）。
	if(sub_mark) {
		int len;

		sprintf(NameBuffer[tan][ton], "%s%s%s%s", namebuf, name_color, nbuf,
		        MES_SPACE19+strlen(nbuf)+(MES_SPACE19_LENGTH-TONENAME_LENGTH));

		len = (int)strlen(NameBuffer[tan][ton]);
		{
			char tail[8];

			memcpy(tail, NameBuffer[tan][ton]+len-SUB_MARK_WIDTH, SUB_MARK_WIDTH);
			tail[SUB_MARK_WIDTH] = '\0';
			sprintf(NameBuffer[tan][ton]+len-SUB_MARK_WIDTH, "%s%s%s", C7, tail, C33);
		}
	} else {
		sprintf(NameBuffer[tan][ton], "%s%s%s%s%s", namebuf, name_color, nbuf,
		        MES_SPACE19+strlen(nbuf)+(MES_SPACE19_LENGTH-TONENAME_LENGTH), C33);
	}
}

//
// disp OPM table
//    n: OPM table number: 0 or 1
//
void PutOPMTable(int n)
{
	char tempbuf[256];
	int  xpos, ypos;
	int  tan, ton;
	int  flag_locked;
	char *color;
	int  i;

	tan = tb[n].tan;
	ton = tb[n].ton;

	xpos = !n ? TX0 : TX1;
	ypos = !n ? TY0 : TY1;

	B_LOCATE(xpos, ypos++);
	C_OPMTONENAME(tan, ton);

	PutOPMChannel();

	PutVolume();
	ypos += 4;   // PutOPMChannel / PutVolume が使う行数分。位置は
	             // OPMCHANNEL** / OPMVOLUME** で絶対指定しているので、
	             // そちらを動かしたらこの値も合わせること。

	MakeOPMData(tan, ton);

    // check ToneLock & set color
	flag_locked = stat_lockflag(STAT_EFFECTIVE, tan, ton);
    color = flag_locked ? C31 : C33;

	B_LOCATE(xpos, ypos++);
	C_YELLOW(" CON  FL  SM  LR PMS AMS");
	C_LBLUE("  SY  WF LFQ PMD AMD  NE NFQ");
	B_LOCATE(xpos, ypos++);
	sprintf(tempbuf, "%s%s", color, buf[0]);
	B_PRINT(tempbuf);
	B_LOCATE(xpos, ypos++);
	C_YELLOW("  AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME");
	for(i=0; i<4; i++) {
		B_LOCATE(xpos, ypos++);
		sprintf(tempbuf, "%s%s", color, buf[i+1]);
		B_PRINT(tempbuf);
	}
}

//
// make str scalekey_mode
//
char *MakeStrScalekeyMode(int unison_cnt, int poly_cnt, int ch_assign)
{
	static char sbuf[64];

	sprintf(sbuf, "%s/%s%s", str_scalekey_unison[unison_cnt],
						     str_scalekey_poly[poly_cnt],
							 str_scalekey_channel_assign_policy[ch_assign]);
	return sbuf;
}

//
// disp OPM channel flag
//
void PutOPMChannel(void)
{
	int	c, r;
	int i, j;
	int locked;
	int x0, y0;
	int x1, y1;

	for(j=0; j<2; j++) {
		if(j == 0) {
			x0 = OPMCHANNEL00_XPOS;
			y0 = OPMCHANNEL00_YPOS;
			x1 = OPMCHANNEL01_XPOS;
			y1 = OPMCHANNEL01_YPOS;
		} else {
			x0 = OPMCHANNEL10_XPOS;
			y0 = OPMCHANNEL10_YPOS;
			x1 = OPMCHANNEL11_XPOS;
			y1 = OPMCHANNEL11_YPOS;
		}

		c = (int)Tone[tb[j].tan][tb[j].ton].chflag;

		if(!Tone[tb[j].tan][tb[j].ton].chmode) {
			sprintf(buf[0], "%s%s%s", C43,
						              MakeStrScalekeyMode(scalekey_unison_count,
						                                  scalekey_poly_count,
											              scalekey_channel_assign_policy), C33);
		} else {
			sprintf(buf[0], "%s%s%s", C33,
						              MakeStrScalekeyMode(scalekey_unison_count,
						                                  scalekey_poly_count,
											              scalekey_channel_assign_policy), C33);
		}

		B_LOCATE(x0, y0);
		B_PRINT(buf[0]);

		// channel select は tone data。ロック中はシアンで表示する。
		locked = stat_lockflag(STAT_EFFECTIVE, tb[j].tan, tb[j].ton);

		B_LOCATE(x1, y1);
		r = 1;
		for(i=0; i<8; i++) {
			if(c & r) B_PRINT(locked ? C41 : C43);
			else	  B_PRINT(locked ? C31 : C33);
			B_PRINT(str_opm_channel_chara[i]);
			r <<= 1;
		}
		B_PRINT(C33);
	}
}

//
// disp volume
//
void PutVolume(void)
{
	int j;
	int x0, y0;
	int x1, y1;

	for(j=0; j<2; j++) {
		if(j == 0) {
			x0 = OPMVOLUME00_XPOS;
			y0 = OPMVOLUME00_YPOS;
		    x1 = OPMVOLUME01_XPOS+1;
			y1 = OPMVOLUME01_YPOS;
		} else {
			x0 = OPMVOLUME10_XPOS;
			y0 = OPMVOLUME10_YPOS;
		    x1 = OPMVOLUME11_XPOS+1;
			y1 = OPMVOLUME11_YPOS;
		}

		sprintf(buf[0], "@v%3d", Tone[tb[j].tan][tb[j].ton].volume);

		sprintf(buf[1], "%3d %3d %3d %3d %3d %3d %3d %3d",
			Vol[j][0],Vol[j][1],Vol[j][2],Vol[j][3],Vol[j][4],Vol[j][5],Vol[j][6],Vol[j][7]);

		// master volume は tone data。ロック中はシアンで表示する。
		B_LOCATE(x0, y0);
		B_PRINT(stat_lockflag(STAT_EFFECTIVE, tb[j].tan, tb[j].ton) ? C31 : C33);
		B_PRINT(buf[0]);

		// channel volume は実行時パラメータ。ロックの対象外。
		B_LOCATE(x1, y1);
		B_PRINT(C33);
		B_PRINT(buf[1]);
	}
}

//
// make OPM param table disp data
//
void MakeOPMData(int tan, int ton)
{
	int	i;

/*                    CON  FL  SM  LR PMS AMS SYN  WF LFQ PMD AMD  NE NFQ */
	sprintf(buf[0], " %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d",
			Tone[tan][ton].con,
			Tone[tan][ton].feedback,
			Tone[tan][ton].slotmask,
			Tone[tan][ton].pan,
			Tone[tan][ton].pms,
			Tone[tan][ton].ams,
			Tone[tan][ton].sync,
			Tone[tan][ton].waveform,
			Tone[tan][ton].lfreq,
			Tone[tan][ton].pmd,
			Tone[tan][ton].amd,
			Tone[tan][ton].noise,
			Tone[tan][ton].nfreq);

	for(i=0; i<4; i++) {
/*				             AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME */
		sprintf(buf[i+1], " %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d",
				  Tone[tan][ton].reg[i].ar,
				  Tone[tan][ton].reg[i].d1r,
				  Tone[tan][ton].reg[i].d2r,
				  Tone[tan][ton].reg[i].rr,
				  Tone[tan][ton].reg[i].d1l,
				  Tone[tan][ton].reg[i].tl,
				  Tone[tan][ton].reg[i].ks,
				  Tone[tan][ton].reg[i].mul,
				  Tone[tan][ton].reg[i].dt1,
				  Tone[tan][ton].reg[i].dt2,
				  Tone[tan][ton].reg[i].amse);
	}
}

//
// disp OPM data at OPM parameter table
//     num: 0 normal
//          1 reverce
//          2 modified
//     sw:  0 or 1 (disp opm table sw)
//          <0 (tnsw)
//
void PutOPMData(int num, int sw)
{
	int   x, y;          // 表示基準位置
	int   lockflag;
	char  *color;
	char  tempbuf[256];

	if(sw<0) sw = tnsw;

	x = (sw==0) ? TX0+1 : TX1+1;
	y = (sw==0) ? TY0+3 : TY1+3;

	sprintf(buf[0], "%3d", CT->data[ln]);

	B_LOCATE(x + (int)cur[ln].xpos, y + (int)cur[ln].ypos);

	// check ToneLock
	lockflag = stat_lockflag(STAT_EFFECTIVE, tb[sw].tan, tb[sw].ton);

	switch (num) {
		case 0:
		    color = lockflag ? C31 : C33;
		    sprintf(tempbuf, "%s%s%s", color, buf[0], C33);
			B_PRINT(tempbuf);
			break;
		case 1:
		    color = lockflag ? C41 : C43;
		    sprintf(tempbuf, "%s%s%s", color, buf[0], C33);
			B_PRINT(tempbuf);
			break;
		case 2:
		    sprintf(tempbuf, "%s%s%s", C41, buf[0], C33);
			B_PRINT(tempbuf);
			break;
	}
}

//
// disp exec menu
//    sw: 0=normal
//        1=SHIFT
//        2=CTRL
//        3=SHIFT+CTRL
//
void PutExecMenu(int sw)
{
	int ypos;
	int num;

	ypos = EXECMENU_YPOS;
    num = sw*10;

	// 11: str_execmenu[] 内の EXECMENU14 の位置 (disp.def の並びに直結)。
	//     この1行のために str_execmenu[] だけ非 const にしてある。
	str_execmenu[11] = template_all_lock ? EXECMENU14 : EXECMENU14_;

	B_LOCATE( EXECMENU_XPOS, ypos++ );
	C_YELLOW_R( str_execmenu[num++] );
	C_YELLOW_R( "|" );
	C_YELLOW_R( str_execmenu[num++] );
	B_LOCATE( EXECMENU_XPOS, ypos++ );
	C_LBLUE_R ( str_execmenu[num++] );
	C_LBLUE_R ( "|" );
	C_LBLUE_R ( str_execmenu[num++] );
	B_LOCATE( EXECMENU_XPOS, ypos++ );
	C_YELLOW_R( str_execmenu[num++] );
	C_YELLOW_R( "|" );
	C_YELLOW_R( str_execmenu[num++] );
	B_LOCATE( EXECMENU_XPOS, ypos++ );
	C_LBLUE_R ( str_execmenu[num++] );
	C_LBLUE_R ( "|" );
	C_LBLUE_R ( str_execmenu[num++] );
}

//
// disp. current table mark
//
void PutCurrentTableMark(void)
{
	int xpos, ypos;
	int i;

	xpos = CTABLEMARK0_XPOS;
	ypos = CTABLEMARK0_YPOS;

	B_PRINT((tnsw==0) ? C41 : C33);			   // esc-seq
	for(i=0; i<CTABLEMARK0_LENGTH; i++) {
	  B_LOCATE(xpos, ypos);
	  B_PRINT(CTABLEMARK0);
	  B_LOCATE(xpos+55, ypos++);   // 55: OPM table size
	  B_PRINT(CTABLEMARK0);
	}

	xpos = CTABLEMARK1_XPOS;
	ypos = CTABLEMARK1_YPOS;

	B_PRINT((tnsw==1 ) ? C41 : C33);			// esc-seq
	for(i=0; i<CTABLEMARK1_LENGTH; i++) {
	  B_LOCATE(xpos, ypos);
	  B_PRINT(CTABLEMARK1);
	  B_LOCATE(xpos+55, ypos++);   // 55: OPM table size
	  B_PRINT(CTABLEMARK1);
	}
	B_PRINT(C33);
}

//
// disp opm channel for scalekey
//
void PutOPMChannelScalekey(void)
{
	char *sbuf0;       // "UNI1/POLY8RRN"
	char sbuf1[32];    // "[MIDI:Ch10]"
	char sbuf2[32];    // "[DLY:000]"
	char sbuf3[32];    // "offset:[+00]"


	sbuf0 = MakeStrScalekeyMode(scalekey_unison_count,
	                            scalekey_poly_count,
								scalekey_channel_assign_policy);

    sbuf3[0] = '\0';
	if(opm_scale_offset < 0) {
		sprintf(sbuf3, "%s[" C1 "-%2d" C1 "] ", C33, (-1)*opm_scale_offset);
	} else {
		sprintf(sbuf3, "%s[" C1 "+%2d" C1 "] ", C33, opm_scale_offset);
	}

	sbuf1[0] = '\0';
	if(scalekey_midi_board_not_exist) {
		sprintf(sbuf1, MIDI_CHANNEL_FILTER_NONE);
	} else {
		sprintf(sbuf1, MIDI_CHANNEL_FILTER_BASE, str_scalekey_midi_channel_filter[scalekey_midi_channel_filter+1]);
	}
	sbuf2[0] = '\0';
	if(delay_count) {
		sprintf(sbuf2, SCALEKEY_DELAY_BASE, delay_count);
	} else {
		strcpy(sbuf2, SCALEKEY_DELAY_NONE);
	}

	// disp midi mode "[MIDI:Ch.1]"
	B_LOCATE(SCALEKEY_MIDIMODE_XPOS, SCALEKEY_MIDIMODE_YPOS);
	B_PRINT(sbuf1);

	B_PRINT(C33);

	// disp play mode "SEQ-HOLD"
    B_LOCATE(SCALEKEY_PLAYMODE_XPOS, SCALEKEY_PLAYMODE_YPOS);
	B_PRINT(sbuf0);

	// disp key offset "[+45]"
	B_LOCATE(SCALEKEY_KEYOFFSET_XPOS, SCALEKEY_KEYOFFSET_YPOS);
	B_PRINT(sbuf3);

	// disp start channel "[A>]"
	B_LOCATE(SCALEKEY_START_CH_XPOS, SCALEKEY_START_CH_YPOS);
	B_PRINT(str_start_channel[start_channel]);

	// disp delay count "[DLY:000]"
	B_LOCATE(SCALEKEY_DELAY_XPOS, SCALEKEY_DELAY_YPOS);
	B_PRINT(sbuf2);
}

