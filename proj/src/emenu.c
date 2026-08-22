//
// exec menu
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<string.h>
#include	<stdlib.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include    "message.h"
#include    "file.h"
#include	"disp.h"
#include    "flag.h"
#include	"json.h"
#include    "mylib.h"

extern void	scinit(void);
extern void	scsetup(void);
extern void	ms_init(void);
extern void	ms_exit(void);
extern void exitsc(void);

extern void	MakeNameBufferAll(int);
extern void	PutToneTable(int);
extern void PutOPMTable(int);
extern void	PutExecMenu(int);
extern void disp_setting(void);

extern void FileSave(int,int,int);

extern void check_file_status(char*);
extern int  proc_editflag(int,int,int);
extern int  stat_editflag(int,int,int);

extern int  load_scalekey_on_startup(int);
extern void unload_scalekey_on_exit(void);
extern int  get_scalekey_version(void);
extern EXECPATH *get_scalekey_exec_path(void);

extern void	C_MES(char*,char*,int);
extern void	C_MES2(char*,char*,int);
extern void	C_CLEAR(void);
extern void C_TITLE_VERSION(int);
extern void	C_TONEMEMO(int);
extern void GETSSS(char*,char*);

extern struct NAMECKBUF ExecFile;
extern int fileedit_flag[3];
extern int filelock_flag[2];         // filelock_flag

extern OED_FORMAT oeddata[3];
extern OPMDATA	Tone[3][TONENUM];
extern int      execmenu_keyflag;
extern TABLENUM tb[2];
extern int	    tnsw;
extern struct   NAMECKBUF FileInf[2];  // filename
extern int      template_all_lock;

void mev_ExecMenu(int,int,int,int,int*,int*);
void mev_EditToneMemo(int,int);
void mev_DispFilememo(int);
void mev_EditFilememo(int);
void mev_DispFileStatus(int,int);
void mev_disp_scalekey_exec_path(void);

void mev_exec_command_pre(char*);
int  mev_exec_command(char*);
int  mev_exec_command_num(int,int,int,int*,int*);
void mev_TemplateAllLock(int);

char str_exec_command[32];

//
// EXEC Menu
//
void mev_ExecMenu(int mx, int my, int bl, int br, int *sbl, int *sbr)
{
	int	xpos, ypos;
	int execmenu_num;	// exec menu number 0-7
	int tan, ton;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;

	xpos = mx/8  - EXECMENU_XPOS;
	ypos = my/16 - EXECMENU_YPOS;
	execmenu_num = (xpos/(EXECMENU_STRLEN+1)+ypos*2);
	execmenu_num += (execmenu_keyflag * 10);

	// mouse left button click
	if(bl && !*sbl) {
		*sbl = -1;

		switch(execmenu_num) {
			//
			// normal No.0-7
			//
			case 0:
				break;
			case 1:                                     // save OED format
				FileSave(FORM_OED, tan, 0);
				break;
		    case 2:
			    break;
			case 3:                                     // save SND format
				FileSave(FORM_SND, tan, 0);
				break;
			case 4:
			    break;
			case 5:                                     // save MML format
				FileSave(FORM_MML, tan, 0);
				break;
			case 6:										// disp filememo
				mev_DispFilememo(tan);
				break;
			case 7:                                     // save OPM format
				FileSave(FORM_OPM, tan, 0);
				break;

			//
			// [SHIFT] No.10-17
			//
			case 10:
				mev_DispFileStatus(tan, 0);
				break;
			case 11:
				mev_TemplateAllLock(template_all_lock);
				template_all_lock ^= 1;
				PutExecMenu(1);
				break;
			case 12:
				break;
			case 13:
				break;
			case 14:         						// edit tone memo
				mev_EditToneMemo(-1,-1);
				break;
			case 15:
				break;
			case 16:								// edit file memo
			    if(tan >= 2) break;
				if(!filelock_flag[tan]) mev_EditFilememo(tan);
				else C_MES( C33, MES_FILE_IS_LOCKED, MESTIME_NORMAL );
				break;
			case 17:
				break;

            //
			// [CTRL] No.20-27
			//
			case 20:
			    mev_exec_command_num(1, bl, br, sbl, sbr);
			    break;
			case 21:
			    mev_exec_command_num(0, bl, br, sbl, sbr);
			    break;
		    case 22:
			    mev_exec_command_num(2, bl, br, sbl, sbr);
			    break;
			case 23:
			    break;
			case 24:
			    mev_exec_command_num(3, bl, br, sbl, sbr);
			    break;
			case 25:
			    break;
			case 26:
			    mev_exec_command_num(4, bl, br, sbl, sbr);
			    break;
			case 27:
			    break;

            //
			// [SHIFT+CTRL] No.30-37
			//
			case 30:
			    break;
			case 31:
				disp_setting();
			    break;
		    case 32:
			    break;
			case 33:
				mev_disp_scalekey_exec_path();
			    break;
			case 34:
			    break;
			case 35:
				mev_DispFileStatus(-1, 2);
			    break;
			case 36:
			    break;
			case 37:
			    C_TITLE_VERSION(1);  // force disp
			    break;

			default:
				return;
		}
	}

	// mouse right button click
	else if(br && !*sbr) {
		*sbr = -1;

		switch(execmenu_num) {
			//
			// normal No.0-7
			//
			case 0:
				break;
			case 1:                                     // save OED format
				FileSave(FORM_OED, tan, 1);
				break;
		    case 2:
			    break;
			case 3:                                     // save SND format
				FileSave(FORM_SND, tan, 1);
				break;
			case 4:
			    break;
			case 5:                                     // save MML format
				FileSave(FORM_MML, tan, 1);
				break;
			case 6:
				break;
			case 7:                                     // save OPM format
				FileSave(FORM_OPM, tan, 1);
				break;

			//
			// [SHIFT] No.10-17
			//
			case 10:
				mev_DispFileStatus(tan, 1);
				break;
			case 11:
				break;
			case 12:
				break;
			case 13:
				break;
			case 14:
				break;
			case 15:
				break;
			case 16:
				break;
			case 17:
				break;

            //
			// [CTRL] No.20-27
			//
			case 20:
			    mev_exec_command_num(1, bl, br, sbl, sbr);
			    break;
			case 21:
			    break;
		    case 22:
			    mev_exec_command_num(2, bl, br, sbl, sbr);
			    break;
			case 23:
			    break;
			case 24:
			    mev_exec_command_num(3, bl, br, sbl, sbr);
			    break;
			case 25:
			    break;
			case 26:
			    mev_exec_command_num(4, bl, br, sbl, sbr);
			    break;
			case 27:
			    break;

            default:
			    return;
		}
	}
}

//
// disp file comment
//
void mev_DispFilememo(int tan)
{
	char dbuf[256];
	char nbuf[256];
	int  len;

	if(tan == 2) return;

    memset(nbuf, 0, (int)sizeof(oeddata[tan].filememo));
	memcpy(nbuf, oeddata[tan].filememo, (int)sizeof(oeddata[tan].filememo));

    len = DISP_MAXSIZE-(int)sizeof(FileInf[tan].name)-2;
	nbuf[len] = '\0';
	sprintf(dbuf, "[%s%s%s] %s", C31, FileInf[tan].name, C33, nbuf);
	C_MES(C33, dbuf, MESTIME_FILEMEMO);
}

//
// edit file comment
//
void mev_EditFilememo(int tan)
{
	struct INPPTR ibuf;
	char nbuf[256];
	char prebuf[256];
	int  memosize;
	char *memopt;

	if(tan == 2) return;

	sprintf(nbuf, "[%s%s%s] ", C31, FileInf[tan].name, C33);
	C_MES2(C33, nbuf, MESTIME_FOREVER);

	ibuf.max = OED_FILEMEMO_SIZE;

    memosize = (int)sizeof(oeddata[tan].filememo);
	memopt = oeddata[tan].filememo;

    memset(prebuf, 0, (int)sizeof(prebuf));
	memcpy(prebuf, memopt, memosize );

	memset(nbuf, 0, (int)sizeof(nbuf));
	memcpy(nbuf, memopt, memosize );
	strcpy(ibuf.buffer, nbuf );

	ibuf.length = strlen(ibuf.buffer);
	B_PRINT(C31);
	C_CURON();
	GETSSS(nbuf, (char*)&ibuf);
	B_PRINT(C33);
	C_CUROFF();

    memset(nbuf, 0, (int)sizeof(nbuf));
	strcpy(nbuf, ibuf.buffer);
	memcpy(memopt, nbuf, memosize);

	if(memcmp(prebuf, memopt, memosize) && !stat_editflag(STAT_FILE, tan, 0)) {
		// set fileedit_flag
		proc_editflag(SET_EDITFLAG|EDIT_FILE_MEMO, tan, 0);
	}

    scinit();
    ms_init();
    scsetup();
    mev_DispFilememo(tan);
}

//
// edit tone memo
//
void mev_EditToneMemo(int tan, int ton)
{
	struct INPPTR ibuf;
	char nbuf[256];
	char prebuf[256];

	if(tan < 0 || ton < 0) {
		tan = tb[tnsw].tan;
		ton = tb[tnsw].ton;
	}

	// check lockflag
	if(stat_lockflag(STAT_EFFECTIVE, tan, ton)) {
		C_MES(C33, MES_TONE_IS_LOCKED, MESTIME_NORMAL);
		return;
	}

	C_TONEMEMO(1);

	ibuf.max = OED_TONEMEMO_SIZE;

	// backup
	memset(prebuf, 0, (int)sizeof(prebuf));
	memcpy(prebuf, Tone[tan][ton].memo, (int)sizeof(Tone[tan][ton].memo));

	memset(nbuf, 0, (int)sizeof(nbuf));
	memcpy(nbuf, Tone[tan][ton].memo, (int)sizeof(Tone[tan][ton].memo));
	strcpy(ibuf.buffer, nbuf);

	ibuf.length = strlen(ibuf.buffer);
	B_PRINT(C31);
	C_CURON();
	GETSSS(nbuf, (char*)&ibuf);
	B_PRINT(C33);
	C_CUROFF();

    memset(nbuf, 0, (int)sizeof(nbuf));
	strcpy(nbuf, ibuf.buffer);
	memcpy(Tone[tan][ton].memo, nbuf, (int)sizeof(Tone[tan][ton].memo));

	// prebuf != nbuf?
	if(memcmp(prebuf, nbuf, (int)sizeof(Tone[tan][ton].memo))) {
		//set fileedit_flag and toneflag
		proc_editflag(SET_EDITFLAG|EDIT_TONE_MEMO, tan, ton);
	}

    scinit();
    ms_init();
    scsetup();
	C_TONEMEMO(0);
}

//
// exec command (oe*.bat)
//
int mev_exec_command(char *exec_command)
{
	int stat;

	unload_scalekey_on_exit();
	ms_exit();
	exitsc();
	C_CLEAR();

	// exec command
	stat = system( exec_command );

	// loading scalekey
	load_scalekey_on_startup(1);

	// re-disp screen
	scinit();
	ms_init();
	scsetup();

	return stat;
}

//
// pre-process for exec command
//
void mev_exec_command_pre(char *vbuf)
{
	int len;

	len = (int)sizeof(str_exec_command);
	memset(str_exec_command, 0, len);

	if(!strlen(vbuf)) {
		strncpy(str_exec_command, DEFAULT_CFG_EXEC_COMMAND, len);
	} else {
		strncpy(str_exec_command, vbuf, len);
	}
	str_exec_command[len-1] = '\0';

	// Human68k のファイル名部は 8 文字。呼び出し時に番号 1 桁が付く
	// (mev_exec_command_num の "%s%d%s") ため、コマンド名は 7 文字までに切る。
	str_exec_command[EXEC_COMMAND_STR_SIZE-1] = '\0';
}

// INPPTR buffer.max for GETSSS
#define COMLINE_MAX   128

//
// EXEC batch number
//
int mev_exec_command_num(int num, int bl, int br, int *sbl, int *sbr)
{
	struct INPPTR ibuf;
	char  com_namebuf[32];
	char  com_extbuf[256];
	char  com_buf[512];
	char  nbuf[256];
	int   stat;

	(void)sbl;
	(void)sbr;

	memset(com_buf, 0, (int)sizeof(com_buf));
	memset(com_extbuf, 0, (int)sizeof(com_extbuf));

	if(!num) {
		strcpy(com_buf, "command.x");
		stat = mev_exec_command(EXEC_COMMAND0);
	} else {
	    sprintf(com_namebuf, "%s%d%s", str_exec_command, num, STR_EXEC_COMMAND_EXT);

		if(!bl && br) {
			strcat(com_namebuf, " ");
			C_MES(C33, com_namebuf, MESTIME_FOREVER);

	        ibuf.max = COMLINE_MAX;
			memset(com_extbuf, 0, (int)sizeof(com_extbuf));

			ibuf.length = strlen(com_extbuf);
			B_PRINT( C31 );
			C_CURON();
			GETSSS(com_extbuf, (char*)&ibuf);
			B_PRINT(C33);
			C_CUROFF();

			memset(com_extbuf, 0, (int)sizeof(com_extbuf));
			strncpy(com_extbuf, ibuf.buffer, (int)sizeof(com_extbuf));
		}

		sprintf(com_buf, "%s%s%s", ExecFile.drive,
								   com_namebuf,
								   com_extbuf);

		stat = mev_exec_command(com_buf);
	}

	if(stat < 0) {
		sprintf(nbuf, MES_EXEC_ERROR, C32, com_buf, stat);
	} else {
		sprintf(nbuf, MES_EXEC_NORMAL, C33, com_buf, stat);
	}
	C_MES(C33, nbuf, MESTIME_NORMAL);

    return stat;
}

//
// Template All Lock/Unlock
//    sw: 0=unlock, 1=lock
//
void mev_TemplateAllLock(int sw)
{
	int i;

	for(i=0;i<TONENUM2;i++) {
		if(stat_editflag(STAT_TONE, 2, i)) continue;
		if(sw) {
			// lock
			proc_lockflag(LOCK_LOCKFLAG|LOCK_TONE_KEYIN, 2, i);
		} else {
			// unlock
			proc_lockflag(UNLOCK_LOCKFLAG|LOCK_TONE_KEYIN, 2, i);
		}
	}

	// re-disp. OPMTable & ToneTable
	MakeNameBufferAll(2);
	PutOPMTable(0);
	PutOPMTable(1);
	PutToneTable(2);
}

//
// Disp File Status
//   num: file number 0/1, 2=template, <0=not used (sw=2 only)
//   sw : 0=disp, 1=edit path then disp, 2=exec file (oe.x)
//
void mev_DispFileStatus(int num, int sw)
{
	struct NAMECKBUF inf;
	struct INPPTR ibuf;
	char   nbuf[512];

	// sw==2 を先に見る。num は参照しないため負値で呼ばれる。
	if(sw == 2) {
		sprintf(nbuf, "%s%s%s", ExecFile.drive,
								ExecFile.name,
								ExecFile.ext);
	} else if((num >= 0) && (num < 2) && strlen(FileInf[num].name)) {
	    if(!strlen(FileInf[num].ext)) {
			strcpy(FileInf[num].ext, STR_FILEEXT_OED);
		}
		sprintf(nbuf, "%s%s%s", FileInf[num].drive,
								FileInf[num].name,
								FileInf[num].ext);
	} else {
		NAMECK("*.*", &inf);
		sprintf(nbuf, "%s", inf.drive);
	}

	if(sw == 1) {
		ibuf.max = (int)sizeof(struct NAMECKBUF);
		strcpy(ibuf.buffer, nbuf);

		ibuf.length = strlen(ibuf.buffer);
		C_MES2(C33, "", MESTIME_BIT);
		B_PRINT(C31);
		C_CURON();
		GETSSS(nbuf, (char*)&ibuf);
		B_PRINT(C33);
		C_CUROFF();
		strcpy(nbuf, ibuf.buffer);

	    scinit();
   		ms_init();
   		scsetup();

	} else if(num == 2) {
		C_MES(C33, MES_FILESTATUS_TEMPDATA, MESTIME_NORMAL);
		return;
	}

	check_file_status(nbuf);
}

//
// disp scalekey exec path
//
void mev_disp_scalekey_exec_path(void)
{
	char mbuf0[128];
	char mbuf1[128];
	int  scalekey_version;
	EXECPATH *exec_path;

	scalekey_version = get_scalekey_version();
	exec_path = get_scalekey_exec_path();
	if(!exec_path) {
		C_MES(C33, "SCALEKEY is " C31 "unavailable" C33, MESTIME_LONG);
		return;
	}

	sprintf(mbuf0, "[%s%s] ", exec_path->drive, exec_path->name);

	sprintf(mbuf1, "SCALEKEY version %s " C32 "available" C33, my_ver2str(scalekey_version));
	strcat(mbuf0, mbuf1);

	C_MES(C33, mbuf0, MESTIME_LONG);
}
