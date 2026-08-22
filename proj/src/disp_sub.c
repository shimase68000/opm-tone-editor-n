//
//	disp_sub.c
//

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<string.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include	"disp.h"
#include	"opm.h"
#include	"message.h"
#include    "flag.h"

extern struct   NAMECKBUF FileInf[2];  // file name
extern OPMDATA	Tone[3][TONENUM];	   // ＯＰＭデータ
extern char     buf[6][256];		   // 表示用テンポラリバッファ
extern TABLENUM tb[2];
extern int      tnsw;
extern int      mes_count;             // message count
extern short    channel_detune[8];

void C_CLEAR(void);
void C_YELLOW(char*);
void C_LBLUE(char*);
void C_YELLOW_R(char*);
void C_LBLUE_R(char*);
void C_MES(char*,char*,int);
void C_MES2(char*,char*,int);
void C_MES_CLR(void);
void C_MES_CLR2(void);
void C_FILENAME(int,int);
void C_OPMTONENAME(int,int);
void C_TONEMEMO(int);
void C_TONECOPY(int,int);
void C_TITLE_VERSION(int);
void C_DETUNE(int);
void C_OTG(int);
void C_OPM_TIP(int);

const char *disp_alg[] = {
       DISP_ALG0,
       DISP_ALG1,
       DISP_ALG2,
       DISP_ALG3,
       DISP_ALG4,
       DISP_ALG5,
       DISP_ALG6,
       DISP_ALG7
};

const char *disp_slot[] = {
       DISP_SLOT0,
       DISP_SLOT1,
       DISP_SLOT2,
       DISP_SLOT3,
       DISP_SLOT4
};

const char *disp_opm[] = {
       DISP_OPM00,
       DISP_OPM01,
       DISP_OPM02,
       DISP_OPM03,
       DISP_OPM04,
       DISP_OPM05,
       DISP_OPM06,
       DISP_OPM07,
       DISP_OPM08,
       DISP_OPM09,
       DISP_OPM10,
       DISP_OPM11,
       DISP_OPM12,
       DISP_OPM20,
       DISP_OPM21,
       DISP_OPM22,
       DISP_OPM23,
       DISP_OPM24,
       DISP_OPM25,
       DISP_OPM26,
       DISP_OPM27,
       DISP_OPM28,
       DISP_OPM29,
       DISP_OPM30
};

//
//
//
void C_CLEAR(void)
{
	B_PRINT( C33 );
	C_CLS_AL();
}

void C_YELLOW(char *pbuf)
{
	B_PRINT( C32 );
	B_PRINT( pbuf );
	B_PRINT( C33 );
}

void C_LBLUE(char *pbuf)
{
	B_PRINT( C31 );
	B_PRINT( pbuf );
	B_PRINT( C33 );
}

void C_YELLOW_R(char *pbuf)
{
	B_PRINT( C42 );
	B_PRINT( pbuf );
	B_PRINT( C33 );
}

void C_LBLUE_R(char *pbuf)
{
	B_PRINT( C41 );
	B_PRINT( pbuf );
	B_PRINT( C33 );
}

//
// C_MES
//
void C_MES(char *color, char *pbuf, int count)
{
	B_LOCATE(MES_XPOS, MES_YPOS);
	B_PRINT(color);
	B_PRINT(pbuf);
	B_PRINT(CCLR0);
	B_PRINT(C33);
	mes_count = count;
}

//
// C_MES2
//
void C_MES2(char *color, char *pbuf, int count)
{
	B_PRINT(C33);
	C_MES_CLR2();
	B_PRINT(color);
	B_PRINT(pbuf);
	B_PRINT(C33);
	mes_count = count;
}

void C_MES_CLR(void)
{
	B_LOCATE(MES_XPOS, MES_YPOS);
	B_PRINT(CCLR2);
}

void C_MES_CLR2(void)
{
	B_LOCATE(MES2_XPOS, MES2_YPOS+2);
	B_PRINT(CCLR2);
	B_LOCATE(MES2_XPOS, MES2_YPOS+1);
	B_PRINT(CCLR2);
	B_LOCATE(MES2_XPOS, MES2_YPOS);
	B_PRINT(CCLR2);
}

//
// disp filename
//   n : filename number: 0 or 1
//   sw: 0:normal(cyan) 1:yellow
//
void C_FILENAME(int n, int sw)
{
	char  nbuf[64];
	char  dbuf[256];
	char  headmark;
	char *str_color_headmark;
	char *str_color_filename;
	int   flag_locked;
	int   flag_edited;
	int   flag_edited2;

	if(n >= 2) return;

	// set disp. position
	if(n == 0) B_LOCATE(FILENAME0_XPOS, FILENAME0_YPOS);
	else       B_LOCATE(FILENAME1_XPOS, FILENAME1_YPOS);

	flag_locked  = stat_lockflag(STAT_FILE, n, 0);
	flag_edited  = stat_editflag(STAT_FILE, n, 0);
	flag_edited2 = stat_editflag(STAT_FILE, 2, 0);

	// set headmark
	if(flag_locked)
		headmark = TONELOCK_CHARA;
	else if(flag_edited > 0 || (flag_edited >= 0 && flag_edited2))
		headmark = TONEEDIT_CHARA;
	else
		headmark = '\0';

	// set headmark color
	str_color_headmark = (flag_edited && flag_locked) ? C46 :        // force locked
	                                     flag_locked  ? C45 : C41;   // locked : normal
	// set filename color
	str_color_filename = (sw==0) ? C41 : C42;  // normal : highlight

	// set filename string : nbuf
	memset(nbuf, 0, sizeof(nbuf));
	strncpy(nbuf, FileInf[n].name, sizeof(nbuf));
	strcat(nbuf, MES_SPACE19);

	// set disp. string : dbuf
	if(headmark=='\0') {
		nbuf[19] = '\0';
		sprintf(dbuf, "%s%s%s", str_color_filename, nbuf, C33);
	} else {
		nbuf[18] = '\0';
		sprintf(dbuf, "%s%c%s%s%s", str_color_headmark, headmark, str_color_filename, nbuf, C33);
	}

	B_PRINT(dbuf);
}

//
// disp tone name (OPM parameter table header)
//   tnum: tone table number 0/1/2
//   num : tone number
//
void C_OPMTONENAME(int tnum, int num)
{
	char nbuf[256];
	int  flag_select;
	int  flag_locked;
	int  flag_edited;
	char tmark;
	char *str_tmark_color;
	char *str_locked;
	char *str_locked_color;

	flag_locked = stat_lockflag(STAT_TONE, tnum, num);
	flag_edited = stat_editflag(STAT_TONE, tnum, num);
	flag_select = stat_selectflag(STAT_TONE, tnum, num);

    tmark = flag_locked ? TONELOCK_CHARA :
	        flag_edited ? TONEEDIT_CHARA : ' ';

	// force locked : C36 (lock & edit)
	//       locked : C37 (lock)
	//       edited : C33 (edit)
	//       normal : C33
	str_tmark_color = (flag_locked && flag_edited) ? C36 :
	                   flag_locked ? C37 : C33;

	str_locked = flag_locked ? STR_TONELOCK_MARK :
	             flag_edited ? STR_TONEEDIT_MARK : STR_TONELOCK_MARK_CLEAR;

	// force locked : C42 (lock & edit)
	//       locked : C41 (lock)
	//       edited : C41 (edit)
	//       normal : C33
	str_locked_color = (flag_locked && flag_edited) ? C42 :
	                   (flag_locked || flag_edited) ? C41 : C33;

    memset(nbuf, 0, sizeof(nbuf));
	memcpy(nbuf, Tone[tnum][num].name, sizeof(Tone[tnum][num].name));

	if(tnum == 0) {
	  sprintf(buf[0], " %s%3d%s%c%s%s%s %s%s%s", !flag_select ? C31 : C41, num+1,
	                  str_tmark_color, tmark, C33, nbuf,
					  MES_SPACE19+strlen(nbuf)+(MES_SPACE19_LENGTH-TONENAME_LENGTH),
					  str_locked_color, str_locked, C33);
	}
	if(tnum == 1) {
	  sprintf(buf[0], " %s%3d%s%c%s%s%s %s%s%s", !flag_select ? C32 : C42, num+1,
					  str_tmark_color, tmark, C33, nbuf,
					  MES_SPACE19+strlen(nbuf)+(MES_SPACE19_LENGTH-TONENAME_LENGTH),
					  str_locked_color, str_locked, C33);
	}
	if(tnum == 2) {
	  sprintf(buf[0], " %s%c%02d%s%c%s%s%s %s%s%s", !flag_select ? C32 : C42, LOCALTONE_CHARA, num+1,
					  str_tmark_color, tmark, C33, nbuf,
					  MES_SPACE19+strlen(nbuf)+(MES_SPACE19_LENGTH-TONENAME_LENGTH),
					  str_locked_color, str_locked, C33);
	}
	B_PRINT(buf[0]);
}

//
// C_TONEMEMO
//   sw: 0: display tone memo
//       1: no display tone memo
//
void C_TONEMEMO(int sw)
{
	static char mesbuf[256];  // buffer for disp tone-memo
	int  tan, ton;       // tan, ton
	char tname[256];     // Tone.name
	char tmemo[256];     // Tone.memo
	int  len;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;

	memset(tname, 0, sizeof(tname));
	memcpy(tname, Tone[tan][ton].name, sizeof(Tone[tan][ton].name));
	memset(tmemo, 0, sizeof(tmemo));
	memcpy(tmemo, Tone[tan][ton].memo, sizeof(Tone[tan][ton].memo));

    len = sw ? 0 : DISP_MAXSIZE-strlen(Tone[tan][ton].name)-2;
	tmemo[len] = '\0';

	if(tan == 0)
		  sprintf(mesbuf, "[%s%3d%s %s] %s", C31, ton+1, C33, tname, tmemo);
	else if(tan == 1)
		  sprintf(mesbuf, "[%s%3d%s %s] %s", C32, ton+1, C33, tname, tmemo);
	else if(tan == 2)
		  sprintf(mesbuf, "[%s%c%02d%s %s] %s", C32, LOCALTONE_CHARA, ton+1, C33, tname, tmemo);

	B_PRINT(C33);
	C_MES_CLR();
	B_LOCATE(MES_XPOS, MES_YPOS);
	if(sw==1) C_MES_CLR2();
	B_PRINT(mesbuf);
}

//
// C_TONECOPY
//
void C_TONECOPY(int tan, int ton)
{
	char nbuf0[256];
	char nbuf1[256];
	char tname[128];

	memset(tname, 0, sizeof(tname));
	memcpy(tname, Tone[tan][ton].name, sizeof(Tone[tan][ton].name));

	nbuf0[0] = '\0';
	nbuf1[0] = '\0';

	if(tan == 0)
		sprintf(nbuf1, "%s [%s%3d%s %s] ", C33, C31, ton+1, C33, tname);
	else if(tan == 1)
		sprintf(nbuf1, "%s [%s%3d%s %s] ", C33, C32, ton+1, C33, tname);
	else if(tan == 2)
		sprintf(nbuf1, "%s [%s%c%02d%s %s] ", C33, C32, LOCALTONE_CHARA, ton+1, C33, tname);

	sprintf(nbuf0, "%s%s", MES_TONECOPY0, nbuf1);
	C_MES(C42, nbuf0, MESTIME_FOREVER);
}

#define DETUNE_HI(x) ((x<<2) & 0xff00)>>8
#define DETUNE_LO(x) (x & 0x3f)
//
// C_DETUNE
//   sw: 0=clear
//       1=disp
//
void C_DETUNE(int sw)
{
	char mbuf[256];

	if(sw) {
		sprintf(mbuf, "%s%02X%02d%s%02X%02d%s%02X%02d%s%02X%02d%s%02X%02d%s%02X%02d%s%02X%02d%s%02X%02d",
				C33, DETUNE_HI(channel_detune[0]), DETUNE_LO(channel_detune[0]),
				C31, DETUNE_HI(channel_detune[1]), DETUNE_LO(channel_detune[1]),
				C33, DETUNE_HI(channel_detune[2]), DETUNE_LO(channel_detune[2]),
				C31, DETUNE_HI(channel_detune[3]), DETUNE_LO(channel_detune[3]),
				C33, DETUNE_HI(channel_detune[4]), DETUNE_LO(channel_detune[4]),
				C31, DETUNE_HI(channel_detune[5]), DETUNE_LO(channel_detune[5]),
				C33, DETUNE_HI(channel_detune[6]), DETUNE_LO(channel_detune[6]),
				C31, DETUNE_HI(channel_detune[7]), DETUNE_LO(channel_detune[7])
		);
	} else {
		strcpy(mbuf, "                                ");   // 8ch x 4 chara
	}

	B_LOCATE(SCALEKEY_DETUNE_XPOS, SCALEKEY_DETUNE_YPOS);
	B_PRINT(mbuf);
}

//
// disp OPM tip.
//
void C_OPM_TIP(int ln)
{
	int ll, lc;

	ll = (ln<14) ? ln : (ln-14)%12+13;
	lc = (ln<14) ?  0 : (ln-14)/12+1;
	B_LOCATE(DISP_OPMTIP_XPOS, DISP_OPMTIP_YPOS);
	B_PRINT(C33);
	B_PRINT(disp_slot[lc]);
	B_PRINT(disp_opm[ll]);
}

//
// disp OTG
//
void C_OTG(int n)
{
	B_LOCATE( DISP_ALG_XPOS, DISP_ALG_YPOS );
	B_PRINT( C33 );
	B_PRINT( DISP_ALG_BASE );
	B_PRINT( disp_alg[n] );
}
//
// disp title & version
//    sw: 0=check mes_count
//        1=force disp
//
void C_TITLE_VERSION(int sw)
{
	char mbuf[256];

	// sw=0 then check mes_count
	if(!sw && mes_count>0) return;

	sprintf(mbuf, OE_TITLE, OE_MYNAME, OE_VERSION, OE_SUBVERSION, "");
	C_MES(C33, mbuf, MESTIME_NORMAL);
}
