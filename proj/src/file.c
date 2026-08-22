//
//	file.c
//

#define	__SIZE_T

#include	<stdio.h>
#include    <stat.h>
#include	<string.h>
#include	<iocslib.h>
#include	<doslib.h>
#include    <time.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include	"message.h"
#include    "file.h"
#include    "ferror.h"
#include    "disp.h"
#include    "json.h"
#include    "mylib.h"
#include    "flag.h"

extern int  chmod(char*,int);
extern int  isdigit(int);

extern void	C_MES(char*,char*,int);
extern void C_FILENAME(int,int);
extern void	C_MES_CLR(void);

extern void MakeNameBufferAll(int);
extern void PutToneTable(int);
extern void PutOPMTable(int);
extern void PutOPMData(int,int);
extern int	key_inputYN(int);       // key_input.c
extern void InitToneData(int);		// oe.c
extern int  CheckToneData(int);     // oe.c
extern void FileLock(int,int);      // mevent.c
extern void GETSSS(char*,char*);
extern void SetCurrentTone(int);
extern int  proc_editflag(int,int,int);

extern struct NAMECKBUF FileInf[2];	// ファイル名
extern OPMDATA Tone[3][TONENUM];
extern int toneflag[3][TONENUM];
extern int tonetable_dispflag[3];
extern int slot[4];
extern int Vol[2][8];		  // channel volume
extern int fileedit_flag[3];     // edit flag
extern int filelock_flag[2]; // filelock_flag
extern TABLENUM tb[2];
extern int tt[3];
extern CONFIG config;

int  fform(char*);
int  read_binary_file(char*,int);
int  check_samefile(int, struct NAMECKBUF*);
char *MakeStrFilename(char*);
char *MakeStrDateTime(int);

static int getn(char*,int*,int);
static int getfc(char*,int*,int);
static int conf_read_tempdata(char*);

static int oed2tone(int,char*);
static int ted2tone(int,char*);  // not implemented
static int snd2tone(int,char*);
static int mml2tone(int,char*,int);
static int opm2tone(int,char*,int);
static int mdx2tone(int,char*);
static int tone2oed(int,char*);
static int tone2ted(int,char*);  // not implemented
static int tone2mml(int,char*);
static int tone2opm(int,char*);
static int tone2snd(int,char*);
static int oed_validate(char*,int);

static void backup_tonedata(int);
static void restore_tonedata(int);

void FileLoad(int,int);
void FileSave(int,int,int);
void init_auto_readonly(int);
void check_file_status(char*);

int  filesize;
char *filebuf;

struct NAMECKBUF import_src[2];
int    auto_readonly;

// OED file header as read from disk (n=0,1: file A/B, n=2: template)
//   tonecount / tonesize are part of the on-disk layout but nothing
//   reads them back: loading clamps to TONENUM / TONENUM2 and saving
//   always writes TONENUM / TONENUM2.
OED_FORMAT oeddata[3];   // for loading n=0,1:OED 2:TED
OED_EX     oedexbuf;     // for saving

#define NBUFSIZE  256
#define U8(p) ((unsigned char)*(p))

//
// define file error message
//
static const char *ferror_message[] = {
	(const char*)0,              // index0: dummy
	MES_FERR_FILE_NOT_EXIST,
	MES_FERR_CANNOT_OPEN_FILE,
	MES_FERR_CANNOT_CREATE_FILE,
	MES_FERR_DISK_FULL,
	MES_FERR_UNABLE_MALLOC,
	MES_FERR_UNABLE_READ_FILE,
	MES_FERR_NOT_OED_FORMAT,
	MES_FERR_ILLEGAL_SNDFILE,
	MES_FERR_ILLEGAL_MMLFILE,
	MES_FERR_ILLEGAL_MDXFILE,
	MES_FERR_ILLEGAL_OEDFILE
};

//
// disp file error
//   *file: opened file name
//       n: ferror number
//
static void disp_ferror(char *file, int n)
{
	char mbuf[512];

	sprintf(mbuf, "[%s] %s", file, ferror_message[n]);
	C_MES(C32, mbuf, MESTIME_NORMAL);
}

//
// validate OED structure before reading (bounds & integrity)
//   buf: filebuf (body, 16byte header already stripped)
//   len: filesize (body bytes)
//   returns 0 = valid, else FERRCODE_ILLEGAL_OEDFILE
//
// 方針:
//   reject する = 破損の証拠のみ (tonesize不一致 / 宣言データがファイルを超過)
//   reject しない = tonecount の大小そのもの (スロット超過はクランプ読みで許容)
//
static int oed_validate(char *buf, int len)
{
	char *pt  = buf;
	char *end = buf + len;
	int   block;

	// fixed header : filememo + OED_EX + channelvol(2*8) + reserve0
	pt += OED_FILEMEMO_SIZE + (int)sizeof(OED_EX) + (2*8) + OED_RESERVE0_SIZE;
	if(pt > end) return FERRCODE_ILLEGAL_OEDFILE;

	// main block, then template block (both required)
	for(block=0; block<2; block++) {
		int tonecount;
		int tonesize;

		// tonecount(4) + tonesize(4) + reserve1(8) must be present
		if(pt + (int)sizeof(int)*2 + OED_RESERVE1_SIZE > end)
			return FERRCODE_ILLEGAL_OEDFILE;

		memcpy(&tonecount, pt, sizeof(int)); pt += sizeof(int);
		memcpy(&tonesize,  pt, sizeof(int)); pt += sizeof(int);
		pt += OED_RESERVE1_SIZE;

		// integrity : tonesize must match current struct layout
		if(tonesize != (int)OED_TONESIZE) return FERRCODE_ILLEGAL_OEDFILE;

		// declared tone data must fit within remaining bytes
		// (tonecount 自体はクランプ読みで許容。overflowは除算に逃がして判定)
		if(tonecount < 0) return FERRCODE_ILLEGAL_OEDFILE;
		if(tonecount > (int)(end - pt) / tonesize)
			return FERRCODE_ILLEGAL_OEDFILE;
		pt += tonecount * tonesize;
	}
	return 0;
}

//
//
//
static void set_oedexbuf(void)
{
    int  date;
    int  time;
    char nbuf[NBUFSIZE];

    // clear oedexbuf
    memset(&oedexbuf, 0, sizeof(OED_EX));

    // toolname
    sprintf(nbuf, OED_EX_TOOLNAME, OE_VERSION, OE_SUBVERSION);
    strncpy(oedexbuf.toolname, nbuf, sizeof(oedexbuf.toolname)-1);

    // environment / targetdevice / author
    strncpy(oedexbuf.environment,  OED_EX_ENVIRONMENT,  sizeof(oedexbuf.environment)-1);
    strncpy(oedexbuf.targetdevice, OED_EX_TARGETDEVICE, sizeof(oedexbuf.targetdevice)-1);
    strncpy(oedexbuf.author,       config.author,       sizeof(oedexbuf.author)-1);

    // datetime
    date = DATEBIN(BINDATEGET());
    time = TIMEBIN(TIMEGET());
    sprintf(nbuf, "%04d-%02d-%02d %02d:%02d:%02d", (date>>16) & 0xfff,
                                                   (date>>8)  & 0xff,
                                                   (date>>0)  & 0xff,
                                                   (time>>16) & 0xff,
                                                   (time>>8)  & 0xff,
                                                   (time>>0)  & 0xff);
    strncpy(oedexbuf.datetime, nbuf, sizeof(oedexbuf.datetime)-1);
}

//
// load file
//    tan: file number 0 or 1
//    sw : 0=normal 1=at startup
//
void FileLoad(int tan, int sw)
{
	struct NAMECKBUF fbuf;   // buffer for file pathname
	struct stat      sbuf;   // buffer for stat()

	char mbuf[512];
	char filename[sizeof(struct NAMECKBUF)];
	int  status;
	int  form;
	int  tonecheck;
	int  i;

	// set fbuf to filepath (at normal or startup)
	if(!sw) {
	    // check fileedit_flag
	    /* pre-load purge check; same file-level edit state query */
	    if(stat_editflag(STAT_FILE, tan, 0) > 0 || stat_editflag(STAT_FILE, 2, 0) > 0) {
	    	// data purge OK ?
	    	C_MES(C33, MES_PURGE_FILEDATA, MESTIME_FOREVER);
	    	if(key_inputYN(1)) return;  // keyin = 'N' then return
	    }

		// make current filename
	    sprintf(filename, "%s%s%s", FileInf[tan].drive,
		                            FileInf[tan].name,
									FileInf[tan].ext);

	    // edit load file name
		{
			struct INPPTR	 ibuf;   // buffer for GETSSS
			int nameck;

	    	C_MES(C33, MES_LOADFILE, MESTIME_FOREVER);

			ibuf.max = FILEPATH_LENGTH;
			strcpy(ibuf.buffer, filename);
			ibuf.length = strlen(ibuf.buffer);
			C_CURON();
			B_PRINT(C31);
			GETSSS(filename, (char*)&ibuf);
			B_PRINT(C33);
			C_CUROFF();
			C_MES_CLR();

	    	nameck = NAMECK(ibuf.buffer, &fbuf);

	    	if(nameck == 0xFF) {
	    		C_MES(C33, MES_LOADFILE_CANCEL, MESTIME_NORMAL);
				return;
			}
			if(nameck) {
				C_MES(C33, MES_ILLIGAL_FILENAME, MESTIME_NORMAL);
				return;
			}
			if(!strlen(fbuf.ext)) {
				strcpy(fbuf.ext, STR_FILEEXT_OED);
			}
		}
	} else {
		// at startup
		// copy FileInf[tan] => fbuf
		memcpy(&fbuf, &FileInf[tan], sizeof(struct NAMECKBUF));
	}

	// make new filename
	sprintf(filename, "%s%s%s", fbuf.drive,
                                fbuf.name,
                                fbuf.ext);

	// new file exist?
	{
		int fst;

		fst = stat(filename, &sbuf);

		if(fst) {
			// file not exist
			disp_ferror(filename, FERRCODE_FILE_NOT_EXIST);
			return;
		}
	}

	// backup tone data
	backup_tonedata(tan);

	// check file format and load file
	status = 0;
	switch(form = fform(filename)) {
		case FORM_OED:
			status = oed2tone(tan, filename);
			break;

		case FORM_SND:
			status = snd2tone(tan, filename);
			break;

		case FORM_MDX:
			status = mdx2tone(tan, filename);
			break;

		case FORM_MML:
		case FORM_MUS:
			status = mml2tone(tan, filename, form);
			break;

		case FORM_OPM:
		case FORM_ZMS:
			status = opm2tone(tan, filename, form);
			break;
	}
	if(status) {
		// restore tone data
		restore_tonedata(tan);
		// disp ferror message
		disp_ferror(filename, status);
		return;
	}

    // store import source for samefile check
	memcpy(&import_src[tan], &fbuf, sizeof(struct NAMECKBUF));

    // clear edit flag (main slot)
    //   Single clear point for every format. The template slot is
    //   cleared by oed2tone(), which knows whether it was replaced.
	proc_editflag(CLEAR_EDITFLAG|EDIT_FILE_LOAD, tan, 0);

    // clear filelock_flag
    filelock_flag[tan] = 0;

    // if file is read-only then file lock
    if(!(sbuf.st_mode & (1<<0))) {
		// file lock
		FileLock(1, tan);
	}

    // check tone data
	tonecheck = 0;
	for(i=0; i<3; i++) {
		if(CheckToneData(i)) tonecheck++;
	}

	if(!sw) {
	    // set filename
	    memcpy(&FileInf[tan], &fbuf, sizeof(struct NAMECKBUF));
	}

	// init tt, tb
	tt[tan] = 0;
	if(tb[0].tan == tan) tb[0].ton = 0;
	if(tb[1].tan == tan) tb[1].ton = 0;

	// disp new tone table
	MakeNameBufferAll(tan);
	MakeNameBufferAll(2);

	// re-disp
	C_FILENAME(tan, 0);
	PutToneTable(tan);	 // disp tone table
	PutToneTable(2);
	PutOPMTable(0);		 // disp OPM table
	PutOPMTable(1);
	PutOPMData(1, tan);	 // disp OPM data
	SetCurrentTone(-1);

	if(tonecheck) {
		sprintf(mbuf, MES_CORRECT_ILLEGAL_TONEDATA, filename);
		C_MES(C32, mbuf, MESTIME_LONG);
	} else {
		sprintf(mbuf, MES_SUCCESS_LOADING_FILE, filename);
		C_MES(C33, mbuf, MESTIME_NORMAL);
	}
}

//
// save file
//    form: file format (FORM_*)
//    tan : file number 0 or 1
//    sw  : 0=normal 1=edit filename befor save file
//
void FileSave(int form, int tan, int sw)
{
	struct NAMECKBUF fbuf;  // buffer for file pathname
	struct stat      sbuf;  // buffer for stat()

	char mbuf[512];
	char filename[sizeof(struct NAMECKBUF)];
	char *fileext;
	int  i;

	// check template slot
	if(tan == 2) return;

	// check not-loaded state
	if(fileedit_flag[tan] < 0) return;

	// check file format & set file ext
	fileext = STR_FILEEXT_OED;
	switch(form) {
		case FORM_OED:
			break;
		case FORM_SND:
			fileext = STR_FILEEXT_SND;
			break;
		case FORM_MML:
			fileext = STR_FILEEXT_MML;
			break;
		case FORM_OPM:
			fileext = STR_FILEEXT_OPM;
			break;
	}
	// check filename == NULL ?
	if(FileInf[tan].name[0] == '\0') {
		C_MES(C33, MES_FILENAME_IS_NULL, MESTIME_NORMAL);
		return;
	}

	// copy FileInf[tan] => fbuf
	memcpy(&fbuf, &FileInf[tan], sizeof(FileInf[tan]));

	// change file ext
	strcpy(fbuf.ext, fileext);

	// samefile check for MML/OPM export
	if((form == FORM_MML || form == FORM_OPM) && check_samefile(tan, &fbuf)) {
		sprintf(mbuf, MES_SAMEFILE, (form == FORM_MML) ? STR_FILEEXT_MML+1 : STR_FILEEXT_OPM+1);
		C_MES(C33, mbuf, MESTIME_FOREVER);
		if(key_inputYN(1)) return;
	}

	// make filename
	sprintf(filename, "%s%s%s", fbuf.drive,
                                fbuf.name,
                                fbuf.ext);

	// sw!=0: edit filename befor save file
	{
		struct INPPTR ibuf;  // buf for GETSSS
		int nameck;

		if(sw) {
			C_MES(C33, MES_SAVEFILE_EDIT, MESTIME_FOREVER);

			ibuf.max = FILEPATH_LENGTH;
			strcpy(ibuf.buffer, filename);
			ibuf.length = strlen(ibuf.buffer);
			C_CURON();
			B_PRINT(C31);
			GETSSS(filename, (char*)&ibuf);
			B_PRINT(C33);
			C_CUROFF();

			nameck = NAMECK(ibuf.buffer, &fbuf);
			if(nameck == 0xFF) {
				C_MES(C33, MES_SAVEFILE_CANCEL, MESTIME_NORMAL);
				return;
			}
			if(nameck) {
				C_MES(C33, MES_ILLIGAL_FILENAME, MESTIME_NORMAL);
				return;
			}

			// make new filename
			sprintf(filename,"%s%s%s", fbuf.drive,
			                           fbuf.name,
									   fbuf.ext);
		}
	}

	// file already exist? or readonly?
	{
		int fst;

		fst = stat(filename, &sbuf);

		if(!fst) {
			// if file is read-only then cannot write
			if(!(sbuf.st_mode & (1<<0))) {
				// file is readonly
				sprintf(mbuf, MES_FILE_IS_READONLY, filename);
				C_MES(C33, mbuf, MESTIME_NORMAL);
				return;
			} else {
				// overwrite?
			    sprintf(mbuf, MES_FILE_OVERWRITE, filename);
			    C_MES(C33, mbuf, MESTIME_FOREVER);
			    if(key_inputYN(1)) return;  // keyin = 'N' then return;
			    C_CUROFF();
			}
		} else {
			if(!sw) {
				// confirm save file?
				sprintf(mbuf, MES_CONF_SAVE_FILE, filename);
				C_MES(C33, mbuf, MESTIME_FOREVER);
				if(key_inputYN(1)) return;  // keyin = 'N' then return;
				C_CUROFF();
			}
		}
	}

	// save file and clear fileedit_flag
	{
		int st = 0;

		if(form == FORM_OED)      st = tone2oed(tan, filename);
		else if(form == FORM_SND) st = tone2snd(tan, filename);
		else if(form == FORM_MML) st = tone2mml(tan, filename);
		else if(form == FORM_OPM) st = tone2opm(tan, filename);

		if(st) {
			disp_ferror(filename, st);
			return;
		}

		if(form == FORM_OED) {
			// clear edit flag (main slot and template slot)
			//   OED is the only format that also writes the template slot
			//   (see tone2oed), so only an OED save clears it.
			proc_editflag(CLEAR_EDITFLAG|EDIT_FILE_SAVE,  tan, 0);
			proc_editflag(CLEAR_EDITFLAG|EDIT_FILE_SAVE2, 2,   0);
			// copy fbuf => FileInf[tan]
			memcpy(&FileInf[tan], &fbuf, (int)sizeof(FileInf[tan]));

			MakeNameBufferAll(tan);
			MakeNameBufferAll(2);

			tonetable_dispflag[tan] = 1;
			tonetable_dispflag[2] = 1;
		}
	}

	// auto_readonly & disp save file message
	{
		if((form == FORM_OED) && auto_readonly && filelock_flag[tan]) {
			chmod(filename, 0);
			sprintf(mbuf, MES_FILE_SAVED_AND_LOCKED, filename);
		} else {
			sprintf(mbuf, MES_FILE_SAVED, filename);
		}
		C_MES(C33, mbuf, MESTIME_NORMAL);
	}
}

//
// check samefile
//   returns 1 if export destination matches import source, 0 otherwise
//
int check_samefile(int tan, struct NAMECKBUF *fbuf)
{
	char src[sizeof(struct NAMECKBUF)];
	char dst[sizeof(struct NAMECKBUF)];

	sprintf(src, "%s%s%s",
		import_src[tan].drive,
		import_src[tan].name, import_src[tan].ext);

	sprintf(dst, "%s%s%s",
		fbuf->drive,
		fbuf->name, fbuf->ext);

	my_strupr(src);
	my_strupr(dst);

	return !strcmp(src, dst);
}

//
// form validation
//   no match -> FORM_OED
//
int fform(char *filename)
{
	char e[5];

	stcgfe(e, filename);

	if(!strcmpi(e, "oed")) return FORM_OED;
	if(!strcmpi(e, "snd")) return FORM_SND;
	if(!strcmpi(e, "mdx")) return FORM_MDX;
	if(!strcmpi(e, "mus")) return FORM_MUS;
	if(!strcmpi(e, "mml")) return FORM_MML;
	if(!strcmpi(e, "opm")) return FORM_OPM;
	if(!strcmpi(e, "zms")) return FORM_ZMS;

	return FORM_OED;
}

//
// doslib-based file reader
//   書き込み側(wopen/wbytes/wputs/wclose)と対称。fopen/fclose を使わず
//   doslib の OPEN/READ/FGETC/SEEK/CLOSE で読み込む。
//   ・エラーは戻り値の負値で判定（doslib の流儀。EOF/エラーは -1）。
//   ・my_fgets は fgets 相当。\r を捨て \n だけ残す（\r\n → \n。読み書き対称）。
//
static int rfd = -1;   // 読み込み用ファイルハンドル（この経路専用）

// OPEN で開く。0=成功, 負=エラー
static int ropen(char *filename)
{
	rfd = OPEN(filename, 0x000);
	return (rfd < 0) ? rfd : 0;
}

// ファイルサイズを返す（負=エラー）。位置は先頭に戻す
static int rsize(void)
{
	int sz;
	if(rfd < 0) return -1;
	sz = SEEK(rfd, 0, SEEK_END);   // 末尾へ（戻り値=サイズ）
	SEEK(rfd, 0, SEEK_SET);        // 先頭へ戻す
	return sz;
}

// size バイト読む。実際に読めたバイト数を返す
static int rbytes(void *buf, int size)
{
	if(rfd < 0) return -1;
	return READ(rfd, buf, size);
}

// 閉じる
static void rclose(void)
{
	if(rfd >= 0) { CLOSE(rfd); rfd = -1; }
}

// fgets 相当（FGETC ベース）
//   ・1行（\n まで / EOF / size-1 まで）読む
//   ・\r は読み飛ばす（\r\n → \n）
//   ・\n は buffer に含める（stdio fgets 互換）
//   戻り値: 1バイト以上読めれば buf、EOFで何も読めなければ NULL
static char *my_fgets(char *buf, int size)
{
	int c;
	int n = 0;

	if(rfd < 0 || size <= 0) return NULL;

	while(n < size-1) {
		c = FGETC(rfd);
		if(c < 0) break;            // EOF (-1)
		if(c == '\r') continue;     // CR は捨てる
		buf[n++] = (char)c;
		if(c == '\n') break;
	}
	buf[n] = '\0';

	return (n > 0) ? buf : NULL;
}

//
// read binary file into filebuf
//      filename: filename
//      filetype: FORM_*
//
//      returns 0 = success (caller owns filebuf; must MFREE after use)
//              else FERRCODE_* (filebuf is not allocated or already freed here;
//                               caller must NOT MFREE)
//
int read_binary_file(char *filename, int filetype)
{
    int fsize;

	// open file (doslib OPEN)
    if(ropen(filename)) return FERRCODE_CANNOT_OPEN_FILE;

	// get file size
	fsize = rsize();

    // check size of SND format
	{
		if((filetype == FORM_SND) && (fsize != FILESIZE_SND)) {
			rclose();
			return FERRCODE_ILLEGAL_SNDFILE;
		}
	}

	// memory allocation
	{
		filebuf = MALLOC(fsize);
		if(filebuf < 0) {
			rclose();
			return FERRCODE_UNABLE_MALLOC;
		}
	}

    // check OED file header
	{
    	char *tempbuf;
		int i;

		if(filetype == FORM_OED) {
			tempbuf = OED_FILEHEADER_SJIS;
			for(i=0; i<OED_FILEHEADER_SIZE; i++) {
				if(*tempbuf++ != FGETC(rfd)) {
					rclose();
					MFREE(filebuf);
					return FERRCODE_NOT_OED_FORMAT;
				}
			}
			fsize -= OED_FILEHEADER_SIZE;
		}
	}

    filesize = rbytes(filebuf, fsize);
    rclose();

	if(filesize < fsize) {
		MFREE(filebuf);
		return FERRCODE_UNABLE_READ_FILE;
	}

    return 0;
}

//
// SND to tone data
//
static int snd2tone(int tan, char *filename)
{
	SNDFORM	*pt;
	int  selectflag;
	int	 i, j;
	char *sfilename;

	sfilename = (char*)MakeStrFilename(filename);

	// get data then make file memo
	{
		int fn;
		int date;

		// get date status
		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		// make file memo
		memset(oeddata[tan].filememo, 0, sizeof(oeddata[tan].filememo));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
	                                          sfilename,
											  (char*)MakeStrDateTime(date));
	}

	// read file
	{
		int st;

		st = read_binary_file(filename, FORM_SND);
		if(st) return st;
	}

	// init tone data
	InitToneData(tan);

	// set select flag
	if(config.auto_select.snd) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	pt = (SNDFORM *)filebuf;


	{
		int tonecount;
		int namesize;

		// set tonecount
		tonecount = SND_TONENUM;
		if(tonecount > TONENUM) tonecount = TONENUM;

		// clear tone name
		namesize = (int)sizeof(Tone[tan][0].name);
		if(namesize > SND_TONENAME_SIZE) {
			for(i=0; i<tonecount; i++) {
				memset(Tone[tan][i].name, 0, namesize);
			}
		}

		//
		// main data
		//
		for(i=0; i < tonecount; i++) {
			// set tone name
			memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
			memcpy(Tone[tan][i].name, pt->name, SND_TONENAME_SIZE);
			// set tone memo
			memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
			sprintf(Tone[tan][i].memo, "%s @%d %s", sfilename, i+1, Tone[tan][i].name);

			Tone[tan][i].con      = pt->reg0[45];
			Tone[tan][i].feedback = pt->reg0[44];
			Tone[tan][i].slotmask = pt->reg1[1];
			Tone[tan][i].pan      = pt->reg1[2];
			Tone[tan][i].pms      = pt->reg0[48];
			Tone[tan][i].ams      = pt->reg0[49];
			Tone[tan][i].sync     = pt->reg1[0];
			Tone[tan][i].waveform = pt->reg0[46];
			Tone[tan][i].lfreq    = pt->reg0[47];
			Tone[tan][i].pmd      = pt->reg0[50];
			Tone[tan][i].amd      = pt->reg0[51];
			Tone[tan][i].noise    = DEFAULT_OPM_NOISE;
			Tone[tan][i].nfreq    = DEFAULT_OPM_NFREQ;
			Tone[tan][i].dummy    = DEFAULT_OPM_DUMMY;

			for( j = 0; j < 4; j++ ) {
				Tone[tan][i].reg[j].ar   = pt->reg0[   j];
				Tone[tan][i].reg[j].d1r  = pt->reg0[ 4+j];
				Tone[tan][i].reg[j].d2r  = pt->reg0[ 8+j];
				Tone[tan][i].reg[j].rr   = pt->reg0[12+j];
				Tone[tan][i].reg[j].d1l  = pt->reg0[16+j];
				Tone[tan][i].reg[j].tl   = pt->reg0[20+j];
				Tone[tan][i].reg[j].ks   = pt->reg0[24+j];
				Tone[tan][i].reg[j].mul  = pt->reg0[28+j];
				Tone[tan][i].reg[j].dt1  = pt->reg0[32+j];
				Tone[tan][i].reg[j].dt2  = pt->reg0[36+j];
				Tone[tan][i].reg[j].amse = pt->reg0[40+j];
				Tone[tan][i].reg[j].dummy = DEFAULT_OPM_DUMMY;
			}
			Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
			Tone[tan][i].chflag = config.opm_channel.snd;
			Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

			// init. select flag
			flag_select_init_main(tan, i, selectflag);

			pt++;
		}
	}

	MFREE(filebuf);
	return 0;
}

//
// confirm reading template data (OED file only)
//   0: skip template data
//   1: read template data
//

static int conf_read_tempdata(char *filename)
{
	char mbuf[512];

	sprintf(mbuf, MES_READ_TEMPLATE_DATA, filename);
	C_MES(C33, mbuf, MESTIME_FOREVER);
	return key_inputYN(0) ? 0 : 1;
}

//
// OED to tone data
//
static int oed2tone(int tan, char *filename)
{
	int	 i, j;
	char *pt_s;

	// file read
	{
		int st = read_binary_file(filename, FORM_OED);
		if(st) return st;
	}

	// validate structure before touching tone data (fail fast)
	{
		int vstat = oed_validate(filebuf, filesize);
		if(vstat) {MFREE(filebuf); return vstat;}
	}

	// initialize
	{
		// init tone data
		InitToneData(tan);

		// set pt_s
		pt_s = filebuf;

		// init oeddata
		memset(&oeddata[tan], 0, sizeof(oeddata[tan]));
	}

	// read header
	{
		// read file memo
		memcpy(oeddata[tan].filememo, pt_s, OED_FILEMEMO_SIZE);
		pt_s += OED_FILEMEMO_SIZE;

		// read OED_EX data
		memcpy(&oeddata[tan].oedex, pt_s, sizeof(OED_EX));
		pt_s += sizeof(OED_EX);

		// read channel volume ( 8ch x 2 )
		for(j=0; j<2; j++) {
			for(i=0; i<8; i++) {
				oeddata[tan].channelvol[j][i] = *pt_s++;
				Vol[j][i] = (int)oeddata[tan].channelvol[j][i];
			}
		}

		// skip reserve0 area (32byte)
		pt_s += OED_RESERVE0_SIZE;
	}

	//
	// for main data
	//
	{
		int tonecount;
		int tonesize;

		// read tonecount/tonesize
		memcpy(&tonecount, pt_s, sizeof(tonecount));
		oeddata[tan].tonecount = tonecount;
		pt_s += sizeof(tonecount);

		memcpy(&tonesize, pt_s, sizeof(tonesize));
		oeddata[tan].tonesize  = tonesize;
		pt_s += sizeof(tonesize);

		// skip reserve1 area (8byte)
		pt_s += OED_RESERVE1_SIZE;

		// read tone data
		for(i=0; i<tonecount; i++) {
			if(i >= TONENUM) {
				pt_s += sizeof(OPMDATA);
				continue;
			}
			memcpy(&Tone[tan][i], pt_s, sizeof(OPMDATA));
			pt_s += sizeof(OPMDATA);
		}

		// (main slot edit flag is cleared by FileLoad(), for all formats)
	}

	//
	// for template data
	//
	{
		int tonecount;
		int tonesize;

		// read tonecount/tonesize
		memcpy(&tonecount, pt_s, sizeof(tonecount));
		oeddata[2].tonecount = tonecount;
		pt_s += sizeof(tonecount);

		memcpy(&tonesize, pt_s, sizeof(tonesize));
		oeddata[2].tonesize  = tonesize;
		pt_s += sizeof(tonesize);

		// skip reserve area (8byte)
		pt_s += OED_RESERVE1_SIZE;

		// confirm reading template data
		if(conf_read_tempdata(filename)) {
			// read template tone data
			for(i=0; i<tonecount; i++) {
				if(i >= TONENUM2) break;
				memcpy(&Tone[2][i], pt_s, sizeof(OPMDATA));
				pt_s += sizeof(OPMDATA);
			}
			// clear edit flag (template slot)
			//   Cleared here, not in FileLoad(), because it depends on the
			//   answer above.
			proc_editflag(CLEAR_EDITFLAG|EDIT_FILE_LOAD2, 2, 0);
		 }
	}

	MFREE(filebuf);
	return 0;
}

//
//  TED => Tone   (for template file)
//
static int ted2tone(int tan, char *filename)
{
	(void)tan;
	(void)filename;

	// not implemented

	return 0;
}

//
// mml to tone data
//   form (FORM_MML / FORM_MUS) is unused: both are parsed identically.
//
static int mml2tone(int tan, char *filename, int form)
{
	int	i, n, p;
	int	j;
	int	st;
	int len;
	int	d[47], dp;
	int selectflag;

	char  buf[READBUF];
	char *ad;
	char *sfilename;

	(void)form;

	sfilename = (char*)MakeStrFilename(filename);

    // init oeddata
	memset(&oeddata[tan], 0, (int)sizeof(oeddata[tan]));

	// get date status then make file memo
	{
		int fn;
		int date;

		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		// make file memo
		memset(&oeddata[tan].filememo, 0, (int)sizeof(oeddata[tan].filememo));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
	                                          sfilename,
								              (char*)MakeStrDateTime(date));
	}

	// open file (text mode)
	if(ropen(filename)) return FERRCODE_CANNOT_OPEN_FILE;

	// init tone data
	InitToneData(tan);

	// select flag
	if(config.auto_select.mml) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	//
	// main data
	//
	i  = 0; // OED tone number
	n  = 0; // MML @ tone number
	st = 0; // state
	while(i<TONENUM && (ad = my_fgets(buf, READBUF)) != NULL) {
		len = strlen(ad);
		if(!len) continue;

		p = 0; // index of buf

		switch(st) {
			case 0:
				// state 0: get tone number
				while(buf[p] && buf[p] != '@') p++;
				if(!buf[p]) break;
				st = 1;
				p++;
				if(0 > (n = getn(buf, &p, len))) {
					rclose();
					return FERRCODE_ILLEGAL_MMLFILE;
				}

				dp = 0; // index of MML tone param

				/* fallthrough - tone data may begin on this same line */

			case 1:
				// state 1: get tone data
				while(0 <= (d[dp] = getn(buf, &p, len))) {
					dp++;
					if(dp < 47) continue;

					// set tone name
                    memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
					sprintf(Tone[tan][i].name, "@%d", n);

					// set tone memo
					memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
					sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);

					dp = 0;
					for(j=0; j<4; j++) {
						Tone[tan][i].reg[j].ar   = d[dp++];
						Tone[tan][i].reg[j].d1r  = d[dp++];
						Tone[tan][i].reg[j].d2r  = d[dp++];
						Tone[tan][i].reg[j].rr   = d[dp++];
						Tone[tan][i].reg[j].d1l  = d[dp++];
						Tone[tan][i].reg[j].tl   = d[dp++];
						Tone[tan][i].reg[j].ks   = d[dp++];
						Tone[tan][i].reg[j].mul  = d[dp++];
						Tone[tan][i].reg[j].dt1  = d[dp++];
						Tone[tan][i].reg[j].dt2  = d[dp++];
						Tone[tan][i].reg[j].amse = d[dp++];
					}
					Tone[tan][i].con      = d[dp++];
					Tone[tan][i].feedback = d[dp++];
					Tone[tan][i].slotmask = d[dp++];
					Tone[tan][i].pan      = DEFAULT_OPM_PAN;

					Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
					Tone[tan][i].chflag = config.opm_channel.mml;
					Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

					// init. tone select flag
					flag_select_init_main(tan, i, selectflag);

					st = 0;
					i++;
					break;
				}
		}

	}

	oeddata[tan].tonecount = i;
	oeddata[tan].tonesize = sizeof(OPMDATA);

	rclose();
	return 0;
}


//
// OPM to tone data
//
static int opm2tone(int tan, char *filename, int form)
{
	int	 i, p;
	int	 j;
	int	 d[11*5+2], dp;
	int	 st;
	int  len;
	int  c;
	char buf[READBUF];
	char *ad;
	int  fn, date;
	int  chflag;
	int  selectflag;
	char *sfilename;

	sfilename = (char*)MakeStrFilename(filename);

    // init oeddata
	memset(&oeddata[tan], 0, sizeof(oeddata[tan]));

	// get date status then make file memo
	{
		// get date status
		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		// make file memo
		memset(&oeddata[tan].filememo, 0, sizeof(oeddata[tan].filememo));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
	                                          sfilename,
								              (char*)MakeStrDateTime(date));
	}

	// open file (text mode)
	if(ropen(filename)) return FERRCODE_CANNOT_OPEN_FILE;

	// init tone data
	InitToneData(tan);

	// chflag & select flag (OPM/ZMS)
	{
		if(form == FORM_OPM) {
			chflag = config.opm_channel.opm;
			if(config.auto_select.opm) selectflag = MAINFLAG_SELECT;
			else                       selectflag = 0;
		}
		else {
			chflag = config.opm_channel.zms;
			if(config.auto_select.zms) selectflag = MAINFLAG_SELECT;
			else                       selectflag = 0;
		}
	}

	//
	// main data
	//
	i  = 0;	// OED tone number
	st = 0; // state

	while(i<TONENUM && (ad = my_fgets(buf, READBUF)) != NULL) {
		len = strlen(ad);
		if(!len) continue;

		p = 0; // index of buf

		switch(st) {
			case 0:
				// state 0: get tone number
				if(0 > (c = getfc(buf, &p, len))) break;
				if(c != '(') break;

				if(0 > (c = getfc(buf, &p, len))) break;
				if(my_tolower(c) != 'v') break;

				st = 1;
				dp = 0; // index of MML tone param

				/* fallthrough - tone data may begin on this same line */

			case 1:
				// state 1: get tone data
				while(0 <= (d[dp] = getn(buf, &p, len))) {
					dp++;
					if(dp < 57) continue;

					// set tone name
                    memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
					sprintf(Tone[tan][i].name, "v%d", d[0]);

					// set tone memo
					memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
					sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);

					dp = 2;
					Tone[tan][i].con      = d[dp] & 0x7;
					Tone[tan][i].feedback = d[dp++] >> 3;
					Tone[tan][i].slotmask = d[dp++];
					Tone[tan][i].waveform = d[dp++];
					Tone[tan][i].sync     = d[dp++];
					Tone[tan][i].lfreq    = d[dp++];
					Tone[tan][i].pmd      = d[dp++];
					Tone[tan][i].amd      = d[dp++];
					Tone[tan][i].pms      = d[dp++];
					Tone[tan][i].ams      = d[dp++];
					Tone[tan][i].pan      = d[dp++];
					dp++;
					for(j=0; j<4; j++) {
						Tone[tan][i].reg[j].ar   = d[dp++];
						Tone[tan][i].reg[j].d1r  = d[dp++];
						Tone[tan][i].reg[j].d2r  = d[dp++];
						Tone[tan][i].reg[j].rr   = d[dp++];
						Tone[tan][i].reg[j].d1l  = d[dp++];
						Tone[tan][i].reg[j].tl   = d[dp++];
						Tone[tan][i].reg[j].ks   = d[dp++];
						Tone[tan][i].reg[j].mul  = d[dp++];
						Tone[tan][i].reg[j].dt1  = d[dp++];
						Tone[tan][i].reg[j].dt2  = d[dp++];
						Tone[tan][i].reg[j].amse = d[dp++];
					}
					Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
					Tone[tan][i].chflag = chflag;
					Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

					// init. selct flag
					flag_select_init_main(tan, i, selectflag);

					st = 0;
					i++;
					break;
				}
		}
	}

    oeddata[tan].tonecount = i;
	oeddata[tan].tonesize = sizeof(OPMDATA);

	rclose();
	return 0;
}

//
#define is_mml_comment(c)    ((c)=='/'||(c)=='*'||(c)=='#'||(c)==';')
#define is_opm_validchara(c) ((c)>=0x20 && (c)<=0x7f)

//
//	バッファ buf のindex *p から検索して、最初の文字を返す
//
static int getfc(char *buf, int *p, int len)
{
	int c;

	while(c = *(buf+(*p)++)) {
		if(is_mml_comment(c)) *p = len;
		if(*p >= len) return -1;
		if(is_opm_validchara(c)) return c;
	}
	return -1;
}

//
//	バッファ buf のindex (*p) から検索して、最初の数値を返す
//
static int getn(char *buf, int *p, int len)
{
	int n;
	int c;

	while(!isdigit(c = *(buf+(*p)++))) {
		if(is_mml_comment(c)) *p = len;
		if(*p >= len) return -1;
	}

	n = (int)(c-'0');
	while(isdigit(c = *(buf+*p))) {
		n = n*10+(int)(c-'0');
		if(++(*p) >= len) break;
	}
	return n;
}

//
// mdx to tone data
//
static int mdx2tone(int tan, char *filename)
{
	char nbuf[1024];
	char *p, *s, *r;
	int  tonecount;
	int  selectflag;
	int  i, j;
	char *sfilename;

	sfilename = (char*)MakeStrFilename(filename);

	// get file date then make file memo
	{
		int fn;
		int date;
		int st;

		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		st = read_binary_file(filename, FORM_MDX);
		if(st) return st;

		// make file memo to nbuf
		memset(nbuf, 0, (int)sizeof(nbuf));
		sprintf(nbuf, "%s %s ", sfilename,
                                (char*)MakeStrDateTime(date));
	}

	// set select flag
	if(config.auto_select.mdx) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	p = filebuf;
	r = filebuf + filesize;  // r: bottom of mdx data

    // add mdx title to filememo
	{
		int bufsize;
		int len;

		bufsize = (int)sizeof(nbuf);
		len = strlen((char*)nbuf);

		// get mdx title to nbuf
		while(len<bufsize && p<r) {
			if(0x0D == (nbuf[len++] = *p++)) {
				len--;
				break;
			}
		}
		if(len<bufsize && nbuf[len]==0x0D) nbuf[len] = '\0';
	}
///////////////////////////////////////////////////////////////////////////////////////

	// skip PCM file name
	while(p<r && *p++);

	// p -> BASE POINT (offset table top)
	// table: tone(2)+MML.A-H(2*8)+PCM(2) = 10 offsets = 20 bytes
	if((r-p) < 2*10) {
		MFREE(filebuf);
		return FERRCODE_ILLEGAL_MDXFILE;
	}
	{
		char *base = p;
		int   tone_offset = (U8(base)<<8) + U8(base+1);
		char *tone_end;
		int   k;

		// range check all 10 offsets
		for(k=0; k<10; k++) {
			int offset = (U8(base+k*2)<<8) + U8(base+k*2+1);
			if(!(2*10 <= offset && (base+offset) < (r-2))) {
				MFREE(filebuf);
				return FERRCODE_ILLEGAL_MDXFILE;
			}
		}
		// tone data end = nearest offset after tone_offset (else r)
		tone_end = r;
		for(k=1; k<10; k++) {
			int offset = (U8(base+k*2)<<8) + U8(base+k*2+1);
			if(tone_offset < offset && (base+offset) < tone_end)
				tone_end = base + offset;
		}
		s = base + tone_offset;  // top of tone data
		r = tone_end;            // narrow to tone data area
	}

	// no tone data?
	if(s==r) {
		MFREE(filebuf);
		C_MES(C33, MES_NO_TONE_DATA_MDXFILE, MESTIME_NORMAL);
		return 0;
	}

	// check tone data area
	if(s>r) {
		MFREE(filebuf);
		return FERRCODE_ILLEGAL_MDXFILE;
	}

	// calc tone count
	tonecount = (r-s)/27;
	if((r-s)%27) {
		MFREE(filebuf);
		return FERRCODE_ILLEGAL_MDXFILE;
	}

	// clip tonecount
	if(tonecount > TONENUM) tonecount = TONENUM;

	// init tone data
	InitToneData(tan);

    // init oeddata
	memset(&oeddata[tan], 0, sizeof(oeddata[tan]));

	// copy nbuf to file memo
	memcpy(oeddata[tan].filememo, nbuf, sizeof(oeddata[tan].filememo));

	for(i=0; i<tonecount; i++) {
		// set tone name
		memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
		sprintf(Tone[tan][i].name, "@%d", U8(s));
		// set tone memo
		memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
		sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);
		s++;

		Tone[tan][i].feedback = (U8(s) >> 3) & 0x7;
		Tone[tan][i].con      = U8(s) & 0x7;
		Tone[tan][i].pan      = 0x3;
		s++;

		Tone[tan][i].slotmask = U8(s);
		s++;

		for(j=0; j<4; j++) {
			Tone[tan][i].reg[slot[j]].dt1  = U8(s+j) >> 4;
			Tone[tan][i].reg[slot[j]].mul  = U8(s+j) & 0xF;
			Tone[tan][i].reg[slot[j]].tl   = U8(s+j+4);
			Tone[tan][i].reg[slot[j]].ks   = U8(s+j+8) >> 6;
			Tone[tan][i].reg[slot[j]].ar   = U8(s+j+8) & 0x1F;
			Tone[tan][i].reg[slot[j]].amse = U8(s+j+12) >> 7;
			Tone[tan][i].reg[slot[j]].d1r  = U8(s+j+12) & 0x1F;
			Tone[tan][i].reg[slot[j]].dt2  = U8(s+j+16) >> 6;
			Tone[tan][i].reg[slot[j]].d2r  = U8(s+j+16) & 0x1F;
			Tone[tan][i].reg[slot[j]].d1l  = U8(s+j+20) >> 4;
			Tone[tan][i].reg[slot[j]].rr   = U8(s+j+20) & 0xF;
		}
		s += 24;

		Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
		Tone[tan][i].chflag = config.opm_channel.mdx;
		Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

		// init. select flag
		flag_select_init_main(tan, i, selectflag);
	}

    oeddata[tan].tonecount = i;
	oeddata[tan].tonesize = sizeof(OPMDATA);

	MFREE(filebuf);
	return 0;
}

//
// tone data to OED
// 	  (OED file format v000)
//
//
// doslib-based file writer
//   stdio の fopen("wb"/"wt") が既存ファイルの上書きに失敗する環境のため、
//   書き込み方向は doslib の CREATE/WRITE/CLOSE で行う。
//   ・エラーは errno でなく戻り値の負値で判定する（doslib の流儀）。
//   ・wputs は "wt" 相当の改行変換（\n → \r\n）を行う。バイナリは wbytes を使う。
//
static int wfd = -1;   // 書き込み用ファイルハンドル（この経路専用）

// CREATE で作成（既存は 0 バイトにして作り直す）。0=成功, 負=エラー
static int wopen(char *filename)
{
	wfd = CREATE(filename, 0x20);   // 0x20: archive 属性
	return (wfd < 0) ? wfd : 0;
}

// size バイトそのまま書く（バイナリ）。0=成功, -1=失敗（DISK FULL 等）
static int wbytes(void *buf, int size)
{
	if(wfd < 0) return -1;
	if(WRITE(wfd, buf, size) != size) return -1;
	return 0;
}

// 文字列を書く。\n を \r\n に変換して出力（テキスト "wt" 相当）。0=成功, -1=失敗
static int wputs(char *s)
{
	char  wbuf[1024];
	int   n = 0;

	while(*s) {
		if(n >= (int)sizeof(wbuf)-2) {          // バッファ満杯前に掃き出す
			if(wbytes(wbuf, n)) return -1;
			n = 0;
		}
		if(*s == '\n') wbuf[n++] = '\r';        // \n の前に \r を挿入
		wbuf[n++] = *s++;
	}
	if(n) return wbytes(wbuf, n);
	return 0;
}

// 閉じる
static void wclose(void)
{
	if(wfd >= 0) { CLOSE(wfd); wfd = -1; }
}

static int tone2oed(int tan, char *filename)
{
	char fbuf[1024];
	int  tonecount;
	int  tonesize;
	int  wcount;
	int  i, j;

	// open file (doslib CREATE)
	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

    // set OED_EX oedexbuf
	set_oedexbuf();

	// write OED file header
	wbytes(OED_FILEHEADER_SJIS, OED_FILEHEADER_SIZE);

	// write OED file memo
	memcpy(fbuf, oeddata[tan].filememo, (int)sizeof(oeddata[tan].filememo));
	wbytes(fbuf, (int)sizeof(oeddata[tan].filememo));

    // write OED_EX data
    wbytes(&oedexbuf, (int)sizeof(OED_EX));

	// write channel volume (8ch x 2)
	{
		char vbuf[16];
		int  k = 0;
		for(i=0; i<2; i++)
			for(j=0; j<8; j++) vbuf[k++] = (char)Vol[i][j];
		wbytes(vbuf, 16);
	}

    // write reserve (32byte)
	memset(fbuf, 0, OED_RESERVE0_SIZE);
	wbytes(fbuf, OED_RESERVE0_SIZE);

	// write tone count (TONENUM)
	tonecount = TONENUM;
	wbytes(&tonecount, (int)sizeof(tonecount));

	// wite tone data size (= OPMDATA)
	tonesize = (int)sizeof(OPMDATA);
	wbytes(&tonesize, (int)sizeof(tonesize));

    // write reserve (8byte)
	memset(fbuf, 0, OED_RESERVE1_SIZE);
	wbytes(fbuf, OED_RESERVE1_SIZE);

	// write tone data
	wcount = (int)sizeof(OPMDATA);
	for(i=0; i<tonecount; i++) {
		if(wbytes(&Tone[tan][i], wcount)) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	// write tone count for template (= TONENUM2)
	tonecount = TONENUM2;
	wbytes(&tonecount, (int)sizeof(tonecount));

	// write tone data size for template (= sizeof(OPMDATA))
	tonesize = (int)sizeof(OPMDATA);
	wbytes(&tonesize, (int)sizeof(tonesize));

    // write reserve1 (8byte)
	memset(fbuf, 0, OED_RESERVE1_SIZE);
	wbytes(fbuf, OED_RESERVE1_SIZE);

	// write template date
	wcount = (int)sizeof(OPMDATA);
	for(i=0; i<tonecount; i++) {
		if(wbytes(&Tone[2][i], wcount)) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	wclose();
	return 0;
}

//
// tone data to TED (for template file)
//    TED file format v000
//
static int tone2ted(int tan, char *filename)
{
	(void)tan;
	(void)filename;

	// not implemented

	return 0;
}

//
// tone data to MML
//
static int tone2mml(int tan, char *filename)
{
	char buf[256];
	int	 i, j;

	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

	sprintf(buf, MML_FILECOMMENT, OE_MYNAME, OE_VERSION, OE_SUBVERSION);
	strcat(buf, "\n");
	wputs(buf);

	// output tone data
	for(i=0; i<TONENUM; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, tan, i)) continue;

		sprintf(buf, "\n/* %s */\n", Tone[tan][i].name);
		wputs(buf);

		sprintf(buf, "@%d = {\n", i+config.offset_export_tone_number+1);
		wputs(buf);

		wputs("/*  AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME  */\n");

		for(j=0; j<4; j++) {
			sprintf(buf, "   %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,\n",
					Tone[tan][i].reg[j].ar,
					Tone[tan][i].reg[j].d1r,
					Tone[tan][i].reg[j].d2r,
					Tone[tan][i].reg[j].rr,
					Tone[tan][i].reg[j].d1l,
					Tone[tan][i].reg[j].tl,
					Tone[tan][i].reg[j].ks,
					Tone[tan][i].reg[j].mul,
					Tone[tan][i].reg[j].dt1,
					Tone[tan][i].reg[j].dt2,
					Tone[tan][i].reg[j].amse);
			wputs(buf);
		}

		wputs("/* CON  FL  SM */\n");

		sprintf(buf, "   %3d,%3d,%3d\n",
					Tone[tan][i].con,
					Tone[tan][i].feedback,
					Tone[tan][i].slotmask  );
		wputs(buf);
		if(wputs("}\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	// output template tone data
	for(i=0; i<TONENUM2; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, 2, i)) continue;

		sprintf(buf, "\n/* %s */\n", Tone[2][i].name);
		wputs(buf);

		sprintf(buf, "@%d = {\n", i+config.offset_export_template_number+1);
		wputs(buf);

		wputs("/*  AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME  */\n");

		for(j=0; j<4; j++) {
			sprintf(buf, "   %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,\n",
					Tone[2][i].reg[j].ar,
					Tone[2][i].reg[j].d1r,
					Tone[2][i].reg[j].d2r,
					Tone[2][i].reg[j].rr,
					Tone[2][i].reg[j].d1l,
					Tone[2][i].reg[j].tl,
					Tone[2][i].reg[j].ks,
					Tone[2][i].reg[j].mul,
					Tone[2][i].reg[j].dt1,
					Tone[2][i].reg[j].dt2,
					Tone[2][i].reg[j].amse );
			wputs(buf);
		}

		wputs("/* CON  FL  SM */\n");

		sprintf(buf, "   %3d,%3d,%3d\n",
					Tone[2][i].con,
					Tone[2][i].feedback,
					Tone[2][i].slotmask  );
		wputs(buf);
		if(wputs("}\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}
	wclose();
	return 0;
}

//
// tone data to OPM
//
static int tone2opm(int tan, char *filename)
{
	int	 i, j;
	char buf[256];

	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

	sprintf(buf, MML_FILECOMMENT, OE_MYNAME, OE_VERSION, OE_SUBVERSION);
	strcat (buf, "\n");
	wputs(buf);

	// output tone data
	for(i=0; i<TONENUM; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, tan, i)) continue;

		sprintf(buf, "\n/* %s\n", Tone[tan][i].name);
		wputs(buf);

		sprintf(buf, "(v%d,0\n", i+config.offset_export_tone_number+1);
		wputs(buf);

		wputs("/*    F/A  SM  WF  SY  SP PMD AMD PMS AMS  LR\n");
		sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,  0\n",
				(Tone[tan][i].feedback << 3) + Tone[tan][i].con,
				Tone[tan][i].slotmask,
				Tone[tan][i].waveform,
				Tone[tan][i].sync,
				Tone[tan][i].lfreq,
				Tone[tan][i].pmd,
				Tone[tan][i].amd,
				Tone[tan][i].pms,
				Tone[tan][i].ams,
				Tone[tan][i].pan);
		wputs(buf);

		wputs("/*     AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME\n");
		for(j=0; j<4; j++) {
			sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d\n",
				Tone[tan][i].reg[j].ar,
				Tone[tan][i].reg[j].d1r,
				Tone[tan][i].reg[j].d2r,
				Tone[tan][i].reg[j].rr,
				Tone[tan][i].reg[j].d1l,
				Tone[tan][i].reg[j].tl,
				Tone[tan][i].reg[j].ks,
				Tone[tan][i].reg[j].mul,
				Tone[tan][i].reg[j].dt1,
				Tone[tan][i].reg[j].dt2,
				Tone[tan][i].reg[j].amse);
			wputs(buf);
		}
		if(wputs(")\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	// output local tone data
	for(i=0; i<TONENUM2; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, 2, i)) continue;

		sprintf(buf, "\n/* %s\n", Tone[2][i].name);
		wputs(buf);

		sprintf(buf, "(v%d,0\n", i+config.offset_export_template_number+1);
		wputs(buf);

		wputs("/*    F/A  SM  WF  SY  SP PMD AMD PMS AMS  LR\n");
		sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,  0\n",
				(Tone[2][i].feedback << 3) + Tone[2][i].con,
				Tone[2][i].slotmask,
				Tone[2][i].waveform,
				Tone[2][i].sync,
				Tone[2][i].lfreq,
				Tone[2][i].pmd,
				Tone[2][i].amd,
				Tone[2][i].pms,
				Tone[2][i].ams,
				Tone[2][i].pan);
		wputs(buf);

		wputs("/*     AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME\n");
		for(j=0; j<4; j++) {
			sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d\n",
				Tone[2][i].reg[j].ar,
				Tone[2][i].reg[j].d1r,
				Tone[2][i].reg[j].d2r,
				Tone[2][i].reg[j].rr,
				Tone[2][i].reg[j].d1l,
				Tone[2][i].reg[j].tl,
				Tone[2][i].reg[j].ks,
				Tone[2][i].reg[j].mul,
				Tone[2][i].reg[j].dt1,
				Tone[2][i].reg[j].dt2,
				Tone[2][i].reg[j].amse);
			wputs(buf);
		}
		if(wputs(")\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}
	wclose();
	return 0;
}

//
// tone data to SND
//
static int tone2snd(int tan, char *filename)
{
	SNDFORM p;
	int tonecount;
	int wcount;
	int i, j;

	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

    // upper limit
    tonecount = SND_TONENUM;
	if(tonecount > TONENUM) tonecount = TONENUM;

	wcount = (int)sizeof(SNDFORM);
	for(i=0; i < tonecount; i++) {
        // clear SNDFORM p
        memset(&p, 0, sizeof(SNDFORM));

		// set tone name
		strncpy(p.name, Tone[tan][i].name, SND_TONENAME_SIZE);

		// set OPM param.
		p.reg0[45] = Tone[tan][i].con;
		p.reg0[44] = Tone[tan][i].feedback;
		p.reg1[1]  = Tone[tan][i].slotmask;
		p.reg1[2]  = Tone[tan][i].pan;
		p.reg0[48] = Tone[tan][i].pms;
		p.reg0[49] = Tone[tan][i].ams;
		p.reg1[0]  = Tone[tan][i].sync;
		p.reg0[46] = Tone[tan][i].waveform;
		p.reg0[47] = Tone[tan][i].lfreq;
		p.reg0[50] = Tone[tan][i].pmd;
		p.reg0[51] = Tone[tan][i].amd;

		for(j=0; j<4; j++) {
			p.reg0[   j] = Tone[tan][i].reg[j].ar;
			p.reg0[ 4+j] = Tone[tan][i].reg[j].d1r;
			p.reg0[ 8+j] = Tone[tan][i].reg[j].d2r;
			p.reg0[12+j] = Tone[tan][i].reg[j].rr;
			p.reg0[16+j] = Tone[tan][i].reg[j].d1l;
			p.reg0[20+j] = Tone[tan][i].reg[j].tl;
			p.reg0[24+j] = Tone[tan][i].reg[j].ks;
			p.reg0[28+j] = Tone[tan][i].reg[j].mul;
			p.reg0[32+j] = Tone[tan][i].reg[j].dt1;
			p.reg0[36+j] = Tone[tan][i].reg[j].dt2;
			p.reg0[40+j] = Tone[tan][i].reg[j].amse;
		}

		if(wbytes(&p, wcount)) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	wclose();
	return 0;
}

//
// make string for file name
//
char *MakeStrFilename(char *fname)
{
	static char fbuf[64];
	struct NAMECKBUF f;

	NAMECK(fname, &f);
	sprintf(fbuf, "%s%s", f.name,  // file.name
                          f.ext    // file.ext
	);
	return fbuf;
}

//
// make string for date-time
//
char *MakeStrDateTime(int date)
{
	static char fbuf[64];

	sprintf(fbuf, "%04d-%02d-%02d %02d:%02d:%02d",
		((date >> 25) & 0x7f) + 1980, // year
		(date >> 21) & 0x0f,          // month
		(date >> 16) & 0x1f,          // day
		(date >> 11) & 0x1f,          // hour
		(date >>  5) & 0x3f,          // minute
		(date & 0x1f) * 2             // second
	);
	return fbuf;
}

//
// check file status
//
void check_file_status(char *filename)
{
	struct stat sbuf;   // for stat
	char nbuf[512];
	char *str_rw;
	int  fst;
	int  fn;
	int  date;
	int  str_offset, str_offset2;
	char *str_date;

    fst = stat(filename, &sbuf);
	if(0 > fst) {
		sprintf(nbuf, "[%s] %s", filename, MES_FILESTATUS_NOT_EXIST);
		C_MES(C33, nbuf, MESTIME_LONG);
		return;
	}

	// get file date
	//   stat() succeeds for a directory but OPEN() does not.
	//   date < 0 means "no date" and prints as "****-**-**" below.
	date = -1;
	if(0 <= (fn = OPEN(filename, 0x000))) {
		date = FILEDATE(fn, 0);
		CLOSE(fn);
	}

	// S_IFREG: 0x20
	// S_IEXEC: 0x40
	// str_offset2 = bytes of str_rw that occupy no space on screen
	//   (colour sequences - see STR_FILESTATUS_* in file.h)
	if(sbuf.st_mode & S_IFDIR) {
		str_rw = STR_FILESTATUS_DIR;
		str_offset2 = STR_FILESTATUS_NONPRINT;
	} else if(sbuf.st_mode & (1<<0)) {
		str_rw = STR_FILESTATUS_RW;
		str_offset2 = 0;
	} else {
		str_rw = STR_FILESTATUS_RO;
		str_offset2 = STR_FILESTATUS_NONPRINT;
	}

	// no date available (OPEN failed, e.g. a directory) or date unset
	if(date < 0 || ((date>>25)&0x7f) == 0x7f) {
		str_date = "****-**-** **:**:**";
	} else {
		str_date = MakeStrDateTime(date);
	}
	sprintf(nbuf, "[%s] %s %d bytes [%s]", filename,
                                           str_date,
                                           sbuf.st_size,
                                           str_rw);

	str_offset = strlen(nbuf) - DISP_MAXSIZE - str_offset2 - 3;
	if(str_offset < 0) str_offset = 0;

	C_MES(C33, &nbuf[str_offset], MESTIME_LONG);
	return;
}

// for backup tone data
//   TONENUM for all three slots; the template slot uses only TONENUM2.
//   Relies on TONENUM2 <= TONENUM (oe.h).
static OPMDATA Tone2[3][TONENUM];
static int toneflag2[3][TONENUM];
static int Vol2[2][8];
static int fileedit_flag2[3];
static OED_FORMAT oeddata2[3];

//
// backup tone data
//   Covers the target slot and the template slot, plus oeddata,
//   toneflag, fileedit_flag and the channel volumes - everything a
//   load can touch. restore_tonedata() makes a failed load atomic.
//
static void backup_tonedata(int tan)
{
	int tmp = 2;  // index of template slot

	memcpy(Tone2[tan], Tone[tan], (int)sizeof(Tone2[tan]));
    memcpy(Tone2[tmp], Tone[tmp], (int)sizeof(Tone2[tmp]));

    memcpy(&oeddata2[tan], &oeddata[tan], (int)sizeof(oeddata2[tan]));
    memcpy(&oeddata2[tmp], &oeddata[tmp], (int)sizeof(oeddata2[tmp]));

    memcpy(toneflag2[tan], toneflag[tan], (int)sizeof(toneflag2[tan]));
    memcpy(toneflag2[tmp], toneflag[tmp], (int)sizeof(toneflag2[tmp]));

    fileedit_flag2[tan] = fileedit_flag[tan];
    fileedit_flag2[tmp] = fileedit_flag[tmp];

    memcpy(Vol2, Vol, sizeof(Vol2));
}

//
// restore tone data
//
static void restore_tonedata(int tan)
{
	int tmp = 2;  // index of template slot

	memcpy(Tone[tan], Tone2[tan], (int)sizeof(Tone[tan]));
    memcpy(Tone[tmp], Tone2[tmp], (int)sizeof(Tone[tmp]));

    memcpy(&oeddata[tan], &oeddata2[tan], (int)sizeof(oeddata[tan]));
    memcpy(&oeddata[tmp], &oeddata2[tmp], (int)sizeof(oeddata[tmp]));

    memcpy(toneflag[tan], toneflag2[tan], (int)sizeof(toneflag[tan]));
    memcpy(toneflag[tmp], toneflag2[tmp], (int)sizeof(toneflag[tmp]));

    fileedit_flag[tan] = fileedit_flag2[tan];
    fileedit_flag[tmp] = fileedit_flag2[tmp];

    memcpy(Vol, Vol2, sizeof(Vol));
}
