//
// file operation
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<string.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include    "file.h"
#include	"disp.h"
#include    "message.h"
#include    "opm.h"
#include    "flag.h"
#include	"key_input.h"

extern void	MakeNameBufferAll(int);
extern void	PutToneTable(int);
extern void PutOPMTable(int);
extern void	SetCurrentTone(int);
extern void	PutOPMData(int,int);
extern void	mev_EditFilememo(int);

extern void FileLoad(int,int);
extern void FileSave(int,int,int);

extern int  key_inputYN(int);
extern int  key_input_bitsns(int,int);

extern void	C_MES(char*,char*,int);
extern void C_FILENAME(int,int);
extern void GETSSS(char*,char*);

extern int  mes_count;
extern int	fileedit_flag[3];
extern int  filelock_flag[2];         // filelock_flag
extern int  tonememo_dispflag;

extern struct NAMECKBUF FileInf[2];  // filename

void mev_FileOperation(int,int,int*);
void FileLock(int,int);

//
// file operation
//   コマンドキー (ESC / ! / S / L / M / R) のいずれかが押されるまで
//   この関数から戻らない。その間エディタ全体が停止する。
//
void mev_FileOperation(int tan, int bl, int *sbl)
{
	int    k;

	if(tan >= 2||tan < 0) return;

	if(!(bl && !*sbl)) return;
	*sbl = -1;

	C_FILENAME(tan, 1);
	C_MES(C33, MES_FILEOP_COMMAND, MESTIME_FOREVER);
	while(1) {
		k = key_input_bitsns(1, KEYMAP_FILEOP);

		if((k & 0xff) == 0x1B) {         // ESC
			tonememo_dispflag = 1;
			while(key_input_bitsns(0, KEYMAP_FILEOP));
			return;
		}

		if((k & 0xff) == '!') {           // file lock / force lock
			if(k & (2<<8)) {            // CTRL down: force toggle
				FileLock(1, tan);
			} else {                    // normal toggle
				FileLock(0, tan);
			}
			while(key_input_bitsns(0, KEYMAP_FILEOP));
			return;
		}

		if(((k & 0xff) | 0x20) == 's') {      // file save
			if(k&0x100) FileSave(FORM_OED, tan, 1);  // 'S': edit filename mode
			else        FileSave(FORM_OED, tan, 0);  // 's': normal
			while(key_input_bitsns(0, KEYMAP_FILEOP));
            return;
		}

		if(((k & 0xff) | 0x20) == 'l') {      // file load
	       	FileLoad(tan, 0);

	       	// set tone-memo disp flag
	       	if(mes_count < 0) tonememo_dispflag = 1;

            // set current tone to OPM
			SetCurrentTone(-1);

			while(key_input_bitsns(0, KEYMAP_FILEOP));
	       	return;
		}

		if(((k & 0xff) | 0x20) == 'm') {      // edit file memo
			// check filelock
			if(filelock_flag[tan]) {
				C_MES(C33, MES_FILE_IS_LOCKED, MESTIME_NORMAL);
			} else {
				mev_EditFilememo(tan);
			}
			tonememo_dispflag = 1;
			while(key_input_bitsns(0, KEYMAP_FILEOP));
			return;
		}

		if(((k & 0xff) | 0x20) == 'r') {      // rename
			struct INPPTR ibuf;
			char nbuf[1024];
			char pbuf[1024];

			// check filelock
			if(filelock_flag[tan]) {
				C_MES(C33, MES_FILE_IS_LOCKED, MESTIME_NORMAL);
				while(key_input_bitsns(0, KEYMAP_FILEOP));
				return;
			}

			sprintf(nbuf, "%s%s%s", FileInf[tan].drive,
			                        FileInf[tan].name,
									FileInf[tan].ext );

	        sprintf(pbuf, "rename [%s]: ", nbuf);
	        C_MES(C33, pbuf, MESTIME_FOREVER);

	        ibuf.max = FILEPATH_LENGTH;
	        strcpy(ibuf.buffer, nbuf);
	        ibuf.length = strlen(ibuf.buffer);

	        C_CURON(); B_PRINT(C31);
	        GETSSS(nbuf, (char*)&ibuf);
	        C_CUROFF(); B_PRINT(C33);

			// set dispflag tonememo
	        tonememo_dispflag = 1;

	        if(ibuf.length) {
				struct NAMECKBUF fbuf;

	            NAMECK(ibuf.buffer, &fbuf);

				// no filename -> cancel
				if(!strlen(fbuf.name)) return;

				// no ext -> complement
				if(!strlen(fbuf.ext)) {
					strcpy(fbuf.ext, STR_FILEEXT_OED);
				}

				// if input has no path separator, inherit drive+path from FileInf[tan]
				// (only when FileInf[tan] has a valid path)
				if(!strchr(ibuf.buffer, '\\') && !strchr(ibuf.buffer, '/') && !strchr(ibuf.buffer, ':')) {
				    if(FileInf[tan].drive[0] && FileInf[tan].name[0]) {
				        strcpy(fbuf.drive, FileInf[tan].drive);
				    }
				}

				// rename FileInf[tan] (fbuf => FileInf)
                memcpy(&FileInf[tan], &fbuf, (int)sizeof(FileInf[tan]));
			}
			return;
		}
	}
}

//
// file lock
//     sw: 0=toggle (lock on/off)
//         1=force lock on/off
//    tan: file number 0 or 1
//
void FileLock(int sw, int tan)
{
	if(sw == 0) {                            // normal toggle
		if(!filelock_flag[tan]) {
			// unlock -> lock
			// -1(not loaded) or 1(edited): cannot lock
			if(fileedit_flag[tan] != 0) {
				C_FILENAME(tan, 0);
				C_MES(C33, MES_FILE_CANNOT_LOCK, MESTIME_LONG);
				return;
			}
			filelock_flag[tan] = 1;
			C_MES(C33, MES_FILE_LOCKED, MESTIME_NORMAL);
		} else {
			// lock -> unlock (confirm)
			C_MES(C33, MES_CONFIRM_FILEUNLOCK, MESTIME_FOREVER);
			if(key_inputYN(1)) {             // N: cancel
				C_FILENAME(tan, 0);
				PutToneTable(tan);
				return;
			}
			filelock_flag[tan] = 0;
			C_MES(C33, MES_FILE_UNLOCKED, MESTIME_NORMAL);
		}
	}
	else if(sw == 1) {                       // force toggle (CTRL+!)
		filelock_flag[tan] ^= 1;
		C_MES(C33, filelock_flag[tan] ? MES_FILE_LOCKED : MES_FILE_UNLOCKED,
		      MESTIME_NORMAL);
	}

	// re-disp. filename/ToneTable/OPMTable
	MakeNameBufferAll(tan);
	C_FILENAME(tan, 0);
	PutToneTable(tan);
	PutOPMTable(0);
	PutOPMTable(1);
	PutOPMData(1,-1);
}

