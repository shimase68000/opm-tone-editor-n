//
//	oe header file
//

#include "_debug_.h"

typedef	unsigned char	uchar;
typedef	unsigned int	uint;
typedef void (*FUNC_PTR)(void);

#define FILEPATH_LENGTH (sizeof(struct NAMECKBUF))

#define OE_VERSION      "1.20"

#ifndef _debug_test_version_
  #define OE_SUBVERSION ""
#else
  #define OE_SUBVERSION "b-26.08.04"
#endif

#define OE_MYNAME       "OPM Tone Editor 'Ｎ'"
#define OE_TITLE        "%s version %s%s Copyright 1994,2023-26 UG.%s"

#define	TONENUM		    200	    // 音色数 (Table 0/1)
#define	TONENUM2	    8	    // 音色数 (Table 2)
#define SND_TONENUM     200     // 音色数 for SND format
#define	TONENAME_LENGTH 15	    // 音色名長さ(max)

#define CORRECT_OPM_NOTE_VALUE 3    // OPM clock: 4MHz

#define DEFAULT_TABLENUM0  0    // default of table number 0
#define DEFAULT_TONENUM0   0    // default of tone number 0
#define DEFAULT_TABLENUM1  2    // default of table number 1
#define DEFAULT_TONENUM1   0    // default of tone number 1

#define MOUSE_REPEAT_SPEED_FAST    (2)
#define MOUSE_REPEAT_SPEED_MEDIUM  (3)
#define MOUSE_REPEAT_SPEED_SLOW    (6)

#define	TONETABLE0NUM	30	// 音色テーブル０ 表示個数 tone table 0 : left
#define	TONETABLE1NUM	18	// 音色テーブル１ 表示個数 tone table 1 : right
#define	TONETABLE2NUM	8	// 音色テーブル２ 表示個数 tone table 2 : temp-right

#define	TX0		    22	    // ＯＰＭテーブル表示位置
#define	TY0		    6
#define	TX1		    22
#define	TY1		    19

// recalc_effective_count() mode
enum RECALC_MODE {
	RECALC_COUNT_ONLY = 0,   // just return the available channel count
	RECALC_UNISON_PRIORITY,  // recalc effective poly/unison, unison wins
	RECALC_POLY_PRIORITY     // recalc effective poly/unison, poly wins
};

enum MOVE_CURSOR {
	CURUP,
	CURDOWN,
	CURRIGHT,
	CURLEFT,
	CURNONE
};

#define STR_EXEC_COMMAND_EXT  ".BAT"     // exec command ext
#define EXEC_COMMAND_STR_SIZE (8)
#define EXEC_COMMAND0         ""         // no argument: run command.x itself

#define LOCALTONE_CHARA       'T'         // local tone (template slots) chara
#define DEFAULT_TONENAME      ""          // default tone name
#define DEFAULT_TONENAME2     "template"  // default template tone name
#define DEFAULT_MEMO          ""          // default tone memo
#define DEFAULT_FILEMEMO      ""          // default file memo

#define DEFAULT_CH_VOLUME     127	  // default channel volume
#define DEFAULT_OPM_CON       7
#define DEFAULT_OPM_FEEDBACK  0
#define DEFAULT_OPM_SLOTMASK  15
#define DEFAULT_OPM_PAN       3
#define DEFAULT_OPM_PMS       0
#define DEFAULT_OPM_AMS       0
#define DEFAULT_OPM_SYNC      0
#define DEFAULT_OPM_WAVEFORM  0
#define DEFAULT_OPM_LFREQ     0
#define DEFAULT_OPM_PMD       0
#define DEFAULT_OPM_AMD       0
#define DEFAULT_OPM_NOISE     0
#define DEFAULT_OPM_NFREQ     0

#define DEFAULT_OPM_CHMODE    1             // dummy value
#define DEFAULT_OPM_MARK      0				// lock/select
#define DEFAULT_OPM_VOLUME    127

#define DEFAULT_OPM_AR        31	// 0-31
#define DEFAULT_OPM_D1R       0		// 0-31
#define DEFAULT_OPM_D2R       0		// 0-31
#define DEFAULT_OPM_RR        15	// 0-15
#define DEFAULT_OPM_D1L       0		// 0-15
#define DEFAULT_OPM_TL        127	// 0-127
#define DEFAULT_OPM_KS        0		// 0-3
#define DEFAULT_OPM_MUL       0		// 0-15
#define DEFAULT_OPM_DT1       0		// 0-7
#define DEFAULT_OPM_DT2       0		// 0-3
#define DEFAULT_OPM_AMSE      0		// 0-1
#define DEFAULT_OPM_DUMMY     0     // 0

#define OED_FILEMEMO_SIZE     96                          // file comment size
#define TED_FILEMEMO_SIZE     (OED_FILEMEMO_SIZE)         // file comment size for template
#define OED_TONENAME_SIZE     (((TONENAME_LENGTH+1)/2)*2) // tone name size for OED format
#define OED_TONEMEMO_SIZE     64                          // tone memo size for OED format
#define SND_TONENAME_SIZE     10                          // tone name size for SND format

typedef struct {
	const int xpos;
	const int ypos;
} CURSOR;

typedef struct {
	unsigned char drive[2];
	unsigned char path[66];
	unsigned char name[24];
} EXECPATH;

typedef	struct {
	uchar	ar;     // 0
	uchar	d1r;    // 1
	uchar	d2r;    // 2
	uchar	rr;     // 3
	uchar	d1l;    // 4
	uchar	tl;     // 5
	uchar	ks;     // 6
	uchar	mul;    // 7
	uchar	dt1;    // 8
	uchar	dt2;    // 9
	uchar	amse;   // 10
	uchar   dummy;  // 11 *dummy*
} OPMREG;

typedef	struct {
	uchar	con;        // 0
	uchar	feedback;   // 1
	uchar	slotmask;   // 2
	uchar	pan;        // 3
	uchar	pms;        // 4
	uchar	ams;        // 5

	uchar	sync;		// 6
	uchar	waveform;	// 7
	uchar	lfreq;		// 8
	uchar	pmd;		// 9
	uchar	amd;		// 10
	uchar	noise;		// 11
	uchar	nfreq;		// 12
	uchar   dummy;      // 13 *dummy*

	OPMREG	reg[4];

	uchar	chmode;		// channel mode 0:POLY 1:LOCK
	uchar	chflag;		// channel flag (select flag for ch.A-H)
	uchar	mark;		// tone mark flag
	uchar	volume;		// tone volume

	char	name[OED_TONENAME_SIZE];  // tone name
	char	memo[OED_TONEMEMO_SIZE];  // tone memo
} OPMDATA;

#define REGNUM0    (4)  // sizeof(chmode)+sizeof(chflag)+sizeof(mark)+sizeof(volume)
#define REGNUM     (sizeof(OPMDATA)-OED_TONENAME_SIZE-OED_TONEMEMO_SIZE-REGNUM0)

typedef	struct {
	uchar	data[REGNUM];
	uchar   data0[REGNUM0];
	char	name[OED_TONENAME_SIZE];  // tone name
	char	memo[OED_TONEMEMO_SIZE];  // memo
} OPMD;					              // sizeof(OPMD) = sizeof(OPMDATA)

typedef struct {
	int		tan;
	int		ton;
} TABLENUM;

typedef struct {
	char	name[SND_TONENAME_SIZE];  // 音色名
	uchar	reg0[52];	              // データ.0
	uchar	dummy0[2];	              // 予備.0
	uchar	reg1[4];	              // データ.1
	uchar	dummy1[12];	              // 予備.1
} SNDFORM;
