//
//	opm editor oe.c
//

#ifndef __SIZE_T
#define __SIZE_T
#endif

#include	<stdio.h>
#include    <stat.h>
#include	<string.h>
#include	<iocslib.h>
#include	<doslib.h>

#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include	"disp.h"
#include	"message.h"
#include	"ms_map.h"
#include    "file.h"
#include    "json.h"
#include	"mylib.h"
#include	"trap7.h"

extern int  key_inputYN(int);
extern int  set_config_value(CONFIG*);
extern void correct_json_para(CONFIG*);
extern void print_config(const CONFIG*);
extern int  load_config(CONFIG*, struct NAMECKBUF);
extern void	scinit(void);
extern void	exitsc(void);
extern void	scsetup(void);
extern void C_MES(char*,char*,int);
extern void C_TITLE_VERSION(int);

extern int  load_scalekey_on_startup(int);
extern void unload_scalekey_on_exit(void);
extern unsigned int clear_mom(void);

extern void SetCurrentTone(int);
extern void	opmset(OPMDATA*,int);
extern void	MakeNameBufferAll(int);
extern void	MakeNameBuffer(int,int);

extern int	key_routine(void);
extern int	res_routine(void);

extern void	ms_init(void);
extern void	ms_exit(void);
extern int	ms_routine(void);

extern void FileLoad(int,int);
extern void GETSSS(char*,char*);

extern uchar  OPMLimit[66];
extern uchar  ms_map[96*32];
extern OED_FORMAT oeddata[3];

extern int fileedit_flag[3];	 // fileedit flag
extern int filelock_flag[2];     // filelock flag
extern int toneflag[3][TONENUM]; // tone mark flag

int	     main(int,char**);
void	 InitToneData(int);
void	 datainit(void);
void	 main_loop(void);
int 	 ChangeToneName(void);
void	 ms_map_init(void);
int      CheckToneData(int);
void     title(void);
void     init_scalekey_config_param(void);
void     init_midi_param(void);

struct   NAMECKBUF ExecFile;    // exec file
struct   NAMECKBUF jsonInf;     // json file
struct   NAMECKBUF FileInf[2];	// file name buffer
int      dispflag[3][TONENUM];  // tone display state (zero-init = DISP_NORMAL)
OPMDATA	 Tone[3][TONENUM];		// OPM data
OPMD	 *CT;					// curren OPM data
TABLENUM tb[2];
int	     tt[3];
int	     tnsw;
int	     ln;			// cursor
int	     Vol[2][8];		// channel volume
int      scalekey_channel_assign_policy;
int      scalekey_midi_board_not_exist;
int      scalekey_midi_disabled;        // scalekey started with -n

extern int scalekey_midi_channel_filter;
extern int opm_scale_offset;
extern int delay_count;
extern int start_channel;

CONFIG   config = {0};
FUNC_PTR scalekey_main;

//
//  disp. title
//
void title(void)
{
	printf(OE_TITLE, OE_MYNAME, OE_VERSION, OE_SUBVERSION, "\n");

	my_strupr(ExecFile.name);
	printf("usage: %s <filename>\n", ExecFile.name);

	return;
}

//
// main
//
int main(int argc, char **argv)
{
	char mbuf[256];
	int  i, j, k;

	// get exec file and path
	NAMECK(argv[0], &ExecFile);

	// set default json filename
	memcpy(&jsonInf, &ExecFile, (int)sizeof(jsonInf));
	memcpy(&jsonInf.ext, JSON_FILE_EXT, (int)sizeof(jsonInf.ext));

    j = 0;
	k = 0;
    for(i=1; i<argc; i++) {
		struct NAMECKBUF tempInf;
		char c;

		c = *argv[i];
		if((c=='/')||(c=='-')) {
			title();
			return 0;         // exit oe.x
		}
	    if(0 > NAMECK(argv[i], &tempInf)) {
			title();
			return 0;         // exit oe.x
		}
		// JSON file?
		if(!strcmpi(tempInf.ext, JSON_FILE_EXT)) {
			// json file
			if(k >= 1) {
				title();
				return 0;     // exit oe.x
			}
			memcpy(&jsonInf, &tempInf, (int)sizeof(jsonInf));
			k++;
		} else {
			// tone data file
			if(j >= 2) {
				title();
				return 0;     // exit oe.x
			}
			memcpy(&FileInf[j], &tempInf, (int)sizeof(FileInf[j]));
			j++;
		}
	}
	// load current path
	for(; j<2; j++) NAMECK("", &FileInf[j]);

    // load config file (oe.jsn)
	{
		int st;

		if(0>(st = load_config(&config, jsonInf))) {
			// warning
			printf("\n" );
			printf(MES_CONFIRM_START_ANYWAY, ExecFile.name, ExecFile.ext);
			if(key_inputYN(2)) {
				// no => exit
				printf("\n");
		    	C_CURON();
				return st;  // exit oe.x
			}
		} else if(0<st) {
			// error
			printf("\n");
			printf(MES_CONFIG_READ_ERROR, jsonInf.drive, jsonInf.name, jsonInf.ext);
			printf("\n");
			C_CURON();
			return st; // exit oe.x
		}
	}

	// check config.author
	if(!strlen(config.author)) {
		printf("\n");
		printf(MES_UNDEFINED_AUTHOR);
		if(key_inputYN(2)) {
			// no => exit
			sprintf(mbuf, MES_UNDEFINED_AUTHOR2, ExecFile.drive,
			                                     ExecFile.name, JSON_FILE_EXT);
			printf("\n%s\n", mbuf);
		    C_CURON();
			return 0;  // exit oe.x
		}
	}

	// check config.exec_command
	if(!strlen(config.exec_command)) {
		printf("\n");
		printf(MES_UNDEFINED_EXEC_COMMAND);
		if(key_inputYN(2)) {
			// no => exit
			sprintf(mbuf, MES_UNDEFINED_EXEC_COMMAND2, ExecFile.drive,
			                                           ExecFile.name, JSON_FILE_EXT);
			printf("\n%s\n", mbuf);
		    C_CURON();
			return -2;  // exit oe.x
		}
	}
	correct_json_para(&config);

#ifdef _debug_json_
	print_config(&config);
	DEBUG_KEYIN_WAIT();
#endif

	set_config_value(&config);

#ifdef _debug_json_
	printf( "\nexit? [Y/N]" );
	if(!key_inputYN(0)) return -2;
#endif

	// init scalekey param
	init_scalekey_config_param();
	init_midi_param();

	// load scalekey on startup
	if(load_scalekey_on_startup(0)) {
		clear_mom();
		unload_scalekey_on_exit();
		C_CURON();
		printf("\n");
		return 0;  // exit oe.x
	}

	// init Tone data
	InitToneData(0);
	InitToneData(1);
	InitToneData(2);

	// init oeddata
	memset(&oeddata, 0, sizeof(oeddata));
	for(i=0; i<3; i++) {
		strncpy(oeddata[i].filememo, DEFAULT_FILEMEMO, sizeof(oeddata[i].filememo));
	}

	for(k=0; k<2; k++)
		for(i=0; i<8; i++) Vol[k][i] = DEFAULT_CH_VOLUME;

	// reset fileedit_flag
	//   -1: not loaded
	//    0: loaded
	//    1: edited
	fileedit_flag[0] = 0;  // file A loaded (empty)
	fileedit_flag[1] = 0;  // file B loaded (empty)
	fileedit_flag[2] = 0;  // template is always initialized (empty)

	// reset filelock_flag
	filelock_flag[0] = 0;
	filelock_flag[1] = 0;

	B_SUPER(0);

	// init data
	datainit();
	ms_map_init();

    // init screen
	scinit();
	ms_init();
	scsetup();

	// display once
	res_routine();

	// load-file
    for(i=0; i<2; i++) {
	    if(FileInf[i].name[0]) {
			if(!FileInf[i].ext[0]) strcpy(FileInf[i].ext, STR_FILEEXT_OED);
	    	FileLoad(i, 1);
		}
	}

	// check tone data
	for(i=0; i < 3; i++) {
		if(CheckToneData(i) != 0) {
			sprintf(mbuf, "%s [%s%s]", MES_CORRECT_ILLEGAL_TONEDATA, FileInf[i].name, FileInf[i].ext);
			C_MES(C32, mbuf, MESTIME_LONG);
		}
	}

    // disp title & version
    C_TITLE_VERSION(0);

	// set current tone
	SetCurrentTone(-1);

	// main loop
	main_loop();

	clear_mom();
	unload_scalekey_on_exit();
	ms_exit();
	exitsc();

	KFLUSHIO(0xff);

	return 0;  // exit oe.x (normal)
}

//
// init scalekey
//
void init_scalekey_config_param(void)
{
	// init start scan channel
	start_channel = config.scalekey.start_scan_channel;

    // init note offset
    opm_scale_offset = config.scalekey.offset;

	// init scalekey assign policy
	scalekey_channel_assign_policy = ASSIGN_POLICY(config.scalekey.channel_assign_priority,
	                                               config.scalekey.channel_assign_policy);

	// init delay count
	delay_count = config.scalekey.delay_count;
}

//
// init midi param
//
void init_midi_param(void)
{
	// init scalekey_midi_channel_filter
	if(scalekey_midi_board_not_exist) {
		scalekey_midi_channel_filter = -1;	// MIDI:OFF
	} else {
		scalekey_midi_channel_filter = config.scalekey.midi_channel_filter;
	}
}

//
// init tone data
//
void InitToneData(int tan)
{
	char nbuf[8];
	int  i, j;

	for(i=0; i < ((tan==2) ? TONENUM2 : TONENUM); i++) {
		Tone[tan][i].con      = DEFAULT_OPM_CON;
		Tone[tan][i].feedback = DEFAULT_OPM_FEEDBACK;
		Tone[tan][i].slotmask = DEFAULT_OPM_SLOTMASK;
		Tone[tan][i].pan      = DEFAULT_OPM_PAN;
		Tone[tan][i].pms      = DEFAULT_OPM_PMS;
		Tone[tan][i].ams      = DEFAULT_OPM_AMS;
		Tone[tan][i].sync     = DEFAULT_OPM_SYNC;
		Tone[tan][i].waveform = DEFAULT_OPM_WAVEFORM;
		Tone[tan][i].lfreq    = DEFAULT_OPM_LFREQ;
		Tone[tan][i].pmd      = DEFAULT_OPM_PMD;
		Tone[tan][i].amd      = DEFAULT_OPM_AMD;
		Tone[tan][i].noise    = DEFAULT_OPM_NOISE;
		Tone[tan][i].nfreq    = DEFAULT_OPM_NFREQ;
		Tone[tan][i].dummy    = DEFAULT_OPM_DUMMY;

		Tone[tan][i].chmode   = DEFAULT_OPM_CHMODE;
		Tone[tan][i].chflag   = config.opm_channel.def;
		Tone[tan][i].mark     = DEFAULT_OPM_MARK;
		Tone[tan][i].volume   = DEFAULT_OPM_VOLUME;

		// clear name and memo buffer
		memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
		memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));

        strncpy(Tone[tan][i].memo, DEFAULT_MEMO, (int)sizeof(Tone[tan][i].memo));

		if(tan == 2) {
			strncpy((char*)Tone[tan][i].name, DEFAULT_TONENAME2, (int)sizeof(Tone[tan][i].name));
			sprintf(nbuf, " %d", i+1);
			strcat((char*)Tone[tan][i].name, nbuf);
		} else {
			strncpy((char*)Tone[tan][i].name, DEFAULT_TONENAME, (int)sizeof(Tone[tan][i].name));
		}

		for(j=0; j<4; j++) {
			Tone[tan][i].reg[j].ar    = DEFAULT_OPM_AR;
			Tone[tan][i].reg[j].d1r   = DEFAULT_OPM_D1R;
			Tone[tan][i].reg[j].d2r   = DEFAULT_OPM_D2R;
			Tone[tan][i].reg[j].rr    = DEFAULT_OPM_RR;
			Tone[tan][i].reg[j].d1l   = DEFAULT_OPM_D1L;
			Tone[tan][i].reg[j].tl    = DEFAULT_OPM_TL;
			Tone[tan][i].reg[j].ks    = DEFAULT_OPM_KS;
			Tone[tan][i].reg[j].mul   = DEFAULT_OPM_MUL;
			Tone[tan][i].reg[j].dt1   = DEFAULT_OPM_DT1;
			Tone[tan][i].reg[j].dt2   = DEFAULT_OPM_DT2;
			Tone[tan][i].reg[j].amse  = DEFAULT_OPM_AMSE;
			Tone[tan][i].reg[j].dummy = DEFAULT_OPM_DUMMY;
		}
	}
}

//
// main loop
//
void main_loop(void)
{
	while(key_routine() && ms_routine() && res_routine());
}

//
// init mouse map
//
void ms_map_init(void)
{
	int i, j;
	int x, y;

	// filename 0
	x = FILENAME0_XPOS;
	y = FILENAME0_YPOS;
	for(i=0; i < FILENAME_STRLEN; i++) ms_map[x+i+y*96] = MAP_FILENAME0;

	// filename 1
	x = FILENAME1_XPOS;
	y = FILENAME1_YPOS;
	for(i=0; i < FILENAME_STRLEN; i++) ms_map[x+i+y*96] = MAP_FILENAME1;

	// exec menu
	x = EXECMENU_XPOS;
	y = EXECMENU_YPOS;
	for(j=0; j < 4; j++) {
		for(i=0; i < EXECMENU_STRLEN; i++)
			ms_map[x+i+(y+j)*96] = MAP_EXECMENU;
		ms_map[x+i+(y+j)*96] = MAP_NONE;
		i++;
		for(   ; i < EXECMENU_STRLEN*2+1; i++)
			ms_map[x+i+(y+j)*96] = MAP_EXECMENU;
	}

	// tone table 0
	x = TONETABLE0_XPOS;
	y = TONETABLE0_YPOS;
	for(j=0; j < TONETABLE0NUM; j++) {
		ms_map[x+0+(y+j)*96] = MAP_TONEMARK0;
		ms_map[x+1+(y+j)*96] = MAP_TONEMARK0;
		ms_map[x+2+(y+j)*96] = MAP_TONEMARK0;
		for(i=3; i < EXECMENU_STRLEN*2+1; i++)
			ms_map[x+i+(y+j)*96] = MAP_TONETABLE0;
	}

	// tone table 1
	x = TONETABLE1_XPOS;
	y = TONETABLE1_YPOS;
	for(j=0; j < TONETABLE1NUM; j++) {
		ms_map[x+0+(y+j)*96] = MAP_TONEMARK1;
		ms_map[x+1+(y+j)*96] = MAP_TONEMARK1;
		ms_map[x+2+(y+j)*96] = MAP_TONEMARK1;
		for(i=3; i < EXECMENU_STRLEN*2+1; i++)
			ms_map[x+i+(y+j)*96] = MAP_TONETABLE1;
	}

	// tone table 2
	x = TONETABLE2_XPOS;
	y = TONETABLE2_YPOS;
	for(j=0; j < TONETABLE2NUM; j++) {
		ms_map[x+0+(y+j)*96] = MAP_TONEMARK2;
		ms_map[x+1+(y+j)*96] = MAP_TONEMARK2;
		ms_map[x+2+(y+j)*96] = MAP_TONEMARK2;
		for(i=3; i < EXECMENU_STRLEN*2+1; i++)
			ms_map[x+i+(y+j)*96] = MAP_TONETABLE2;
	}

	// OPM table 0 channel mask, volume & master volume
	x = OPMCHANNEL01_XPOS;
	y = OPMCHANNEL01_YPOS;
	for(i=0; i<8*OPMCHANNEL_CSIZE; i++)
		ms_map[x++ + y*96] = MAP_CHANNEL0;

	x = OPMVOLUME00_XPOS+2;
	y = OPMVOLUME00_YPOS;
	for(i=0; i<OPMVOLUME_CSIZE; i++)
		ms_map[x++ + y*96] = MAP_MASTERVOL0;

	x = OPMVOLUME01_XPOS+1;
	y = OPMVOLUME01_YPOS;
	for(j=0; j<8; j++) {
		for(i=0; i<OPMVOLUME_CSIZE; i++) ms_map[x++ + y*96] = MAP_CHANNELVOL0;
	    ms_map[x++ + y*96] = MAP_NONE;
	}

	// OPM table 1 channel mask, volume & master volume
	x = OPMCHANNEL11_XPOS;
	y = OPMCHANNEL11_YPOS;
	for(i=0; i<8*OPMCHANNEL_CSIZE; i++)
		ms_map[x++ + y*96] = MAP_CHANNEL1;

	x = OPMVOLUME10_XPOS+2;
	y = OPMVOLUME10_YPOS;
	for(i=0; i<OPMVOLUME_CSIZE; i++)
		ms_map[x++ + y*96] = MAP_MASTERVOL1;

	x = OPMVOLUME11_XPOS+1;
	y = OPMVOLUME11_YPOS;
	for(j=0; j<8; j++) {
		for(i=0; i < OPMVOLUME_CSIZE; i++) ms_map[x++ + y*96] = MAP_CHANNELVOL1;
	    ms_map[x++ + y*96] = MAP_NONE;
	}

	// OPM copy
	x = OPMCOPY0_XPOS;
	y = OPMCOPY0_YPOS;
	for(i=0; i<OPMCOPY_CSIZE; i++) ms_map[x+i+y*96] = MAP_OPMCOPY0;

	x = OPMCOPY1_XPOS;
	y = OPMCOPY1_YPOS;
	for(i=0; i<OPMCOPY_CSIZE; i++) ms_map[x+i+y*96] = MAP_OPMCOPY1;

    x = OPMCOPY_UNDO0_XPOS;
	y = OPMCOPY_UNDO0_YPOS;
	for(i=0; i<OPMCOPY_UNDO_CSIZE; i++) ms_map[x+i+y*96] = MAP_OPMCOPY_UNDO0;

    x = OPMCOPY_UNDO1_XPOS;
	y = OPMCOPY_UNDO1_YPOS;
	for(i=0; i<OPMCOPY_UNDO_CSIZE; i++) ms_map[x+i+y*96] = MAP_OPMCOPY_UNDO1;

	// Scalekey OPM channel
	x = SCALEKEY_OPMCH_XPOS;
	y = SCALEKEY_OPMCH_YPOS;
	for(i=0; i<8*SCALEKEY_OPMCH_CSIZE; i++) ms_map[x+i+y*96] = MAP_SCALEKEY_OPMCHANNEL;

    //                  12345
	// Scalekey Offset "[+00]"
	x = SCALEKEY_KEYOFFSET_XPOS;
	y = SCALEKEY_KEYOFFSET_YPOS;
	for(i=0; i<SCALEKEY_KEYOFFSET_CSIZE; i++) ms_map[x+i+y*96] = MAP_SCALEKEY_KEYOFFSET;

	//                      123456789
	// Scalekey DelayCount "[DLY:000]"
	x = SCALEKEY_DELAY_XPOS;
	y = SCALEKEY_DELAY_YPOS;
	for(i=0; i<SCALEKEY_DELAY_CSIZE; i++) ms_map[x+i+y*96] = MAP_SCALEKEY_DELAY_COUNT;

	//                        1234
	// Scalekey StartChannel "[A>]"
	x = SCALEKEY_START_CH_XPOS;
	y = SCALEKEY_START_CH_YPOS;
	for(i=0; i<SCALEKEY_START_CH_CSIZE; i++) ms_map[x+i+y*96] = MAP_SCALEKEY_START_CHANNEL;

	//                     1234567890123
	// Scalekey Play mode "UNI0/POLY0SEQ"
	x = SCALEKEY_PLAYMODE_XPOS;
	y = SCALEKEY_PLAYMODE_YPOS;
	for(i=0; i<SCALEKEY_UNISON_CSIZE; i++) ms_map[x+i+y*96] = MAP_SCALEKEY_UNISON;
	i++;
	for(j=0; j<SCALEKEY_POLYPHONIC_CSIZE; j++) ms_map[x + i++ + y*96] = MAP_SCALEKEY_POLYPHONIC;
	for(j=0; j<SCALEKEY_POLICY_CSIZE; j++) ms_map[x + i++ + y*96] = MAP_SCALEKEY_ASSIGN_POLICY;

    //                               12345678901
	// Scalekey MIDI channel filter "[MIDI:Ch12]"
	x = SCALEKEY_MIDIMODE_XPOS;
	y = SCALEKEY_MIDIMODE_YPOS;
	for(i=0; i<SCALEKEY_MIDIMODE_CSIZE; i++) ms_map[x+i+y*96] = MAP_SCALEKEY_MIDI_CHANNEL;

	// Scalekey OPM channel detune
	x = SCALEKEY_DETUNE_XPOS;
	y = SCALEKEY_DETUNE_YPOS;
	for(i=0;i<8;i++) {
		for(j=0;j<SCALEKEY_DETUNE_CSIZE;j++) ms_map[x++ + y*96] = MAP_SCALEKEY_DETUNE;
	}
}

//
// init data
//
void datainit(void)
{
	int	i;

	ln = 0;			 		                 // init cursor location for OPM table

	tnsw = 0;		                         // OPM table switch
	tt[0] = tt[1] = tt[2] = 0;               // top number of tone list
	tb[0].tan = DEFAULT_TABLENUM0;   // current tone number
	tb[0].ton  = DEFAULT_TONENUM0;
	tb[1].tan = DEFAULT_TABLENUM1;
	tb[1].ton  = DEFAULT_TONENUM1;

	// clear tone name buffer and toneflag
	for(i = 0; i < TONENUM; i++) {
		toneflag[0][i] = 0;
		toneflag[1][i] = 0;
		toneflag[2][i] = 0;
	}

	MakeNameBufferAll(0);
	MakeNameBufferAll(1);
	MakeNameBufferAll(2);
}

//
// change tone name
//
int ChangeToneName(void)
{
	struct INPPTR buf;
	char nbuf[256];
	char prebuf[256];
	int  len;
	int  namesize;
	char *namept;
	int  x, y;
	int  tan, ton;
	int  tonenum;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;

	x       = (tan==0) ? TONETABLE0_XPOS :
	          (tan==1) ? TONETABLE1_XPOS : TONETABLE2_XPOS;
	y       = (tan==0) ? TONETABLE0_YPOS :
	          (tan==1) ? TONETABLE1_YPOS : TONETABLE2_YPOS;
	tonenum = (tan==2) ? TONENUM2 : TONENUM;

	B_LOCATE(x+4, y+(ton-tt[tan]+tonenum)%tonenum);
	B_PRINT(C31);
	B_PRINT(MES_SPACE19+(MES_SPACE19_LENGTH-TONENAME_LENGTH));

    namesize = sizeof(Tone[tan][ton].name);
	namept = Tone[tan][ton].name;

    memset(prebuf, 0, sizeof(prebuf));
	memcpy(prebuf, namept, namesize);

	memset(buf.buffer, 0, sizeof(buf.buffer));
	memcpy(buf.buffer, namept, namesize);

    memset(nbuf, 0, sizeof(nbuf));
	memcpy(nbuf, namept, namesize);

	buf.max = TONENAME_LENGTH;
	buf.length = strlen(buf.buffer);
	B_LOCATE(x+4, y+(ton-tt[tan]+tonenum)%tonenum);

	C_CURON();
	GETSSS(nbuf, (char*)&buf);
	C_CUROFF();
	B_PRINT(C33);

    len = strlen(buf.buffer);
	if(len > namesize) len = namesize;

    memset(namept, 0, namesize);
	memcpy(namept, buf.buffer, len);

    return memcmp(namept, prebuf, namesize);
}

//
// check tone data
//   status 0: no error
//          1: data error
//
int CheckToneData(int tan)
{
	OPMD *t;     // tone data
	int i, j;
	int errcount;

	errcount = 0;

	for(j=0; j<(tan==2 ? TONENUM2 : TONENUM); j++) {
		t = (OPMD*)&Tone[tan][j];

		// check OPM Reg. data
		for(i=0; i < REGNUM; i++) {
			if(t->data[i] > OPMLimit[i]) {
				t->data[i] &= OPMLimit[i];
				errcount++;
			}
		}
		// check OPM Data
		for(i=0; i < REGNUM0; i++) {
			if(t->data0[i] > OPMLimit[REGNUM+i]) {
				t->data0[i] &= OPMLimit[REGNUM+i];
				errcount++;
			}
		}
	}
	return errcount;
}
