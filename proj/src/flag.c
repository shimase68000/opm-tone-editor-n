//
//	flag.c
//

#include	<stdio.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "undo.h"
#include    "flag.h"

extern OPMDATA Tone[3][TONENUM];
extern int tonetable_dispflag[3];
extern UNDO_BUFFER undo;

extern void MakeNameBuffer(int,int);

int fileedit_flag[3];
int filelock_flag[2];
int toneflag[3][TONENUM];


int flag_select_stat(int tan, int ton)
{
	return GET_SELECT_MAIN(tan, ton);
}

void flag_select_init_main(int tan, int ton, int selected)
{
	if(selected) {
		Tone[tan][ton].mark |= MAINFLAG_SELECT;
	} else {
		Tone[tan][ton].mark &= ~(MAINFLAG_SELECT);
	}
}

void flag_select_set(int tan, int ton)
{
	Tone[tan][ton].mark |= MAINFLAG_SELECT;
	flag_fileedit_set(tan);
}

void flag_select_clr(int tan, int ton)
{
	Tone[tan][ton].mark &= ~(MAINFLAG_SELECT);
	flag_fileedit_set(tan);
}

void flag_select_toggle(int tan, int ton)
{
	if(GET_SELECT_MAIN(tan, ton)) {
		flag_select_clr(tan, ton);
	} else {
		flag_select_set(tan, ton);
	}
}


/*
 * LOCK helpers
 */
void flag_lock_set_main(int tan, int ton)
{
	Tone[tan][ton].mark |= MAINFLAG_LOCK;
}

void flag_lock_clr_main(int tan, int ton)
{
	Tone[tan][ton].mark &= ~(MAINFLAG_LOCK);
}

int flag_lock_stat_main(int tan, int ton)
{
	return GET_LOCK_MAIN(tan, ton);
}

int flag_lock_stat_effective(int tan, int ton)
{
	return GET_LOCK_EFFECTIVE(tan, ton);
}


/*
 * EDIT helpers
 */
void flag_edit_set_sub(int tan, int ton)
{
	toneflag[tan][ton] |= SUBFLAG_EDIT;
	fileedit_flag[tan] = 1;
}

void flag_edit_clr_sub(int tan, int ton)
{
	toneflag[tan][ton] &= ~(SUBFLAG_EDIT);
}

int flag_edit_stat_sub(int tan, int ton)
{
	return GET_EDIT_SUB(tan, ton);
}

void flag_fileedit_set(int tan)
{
	fileedit_flag[tan] = 1;
}

void flag_fileedit_clr(int tan)
{
	fileedit_flag[tan] = 0;
}

void flag_edit_clear_all_in_table(int tan, int ntones)
{
	int i;

	for(i=0; i<ntones; i++) {
		flag_edit_clr_sub(tan, i);
	}
	flag_fileedit_clr(tan);
}


/*
 * legacy API
 */
int proc_selectflag(int mode, int tan, int ton)
{
	if(mode & SET_SELECTFLAG) {
		flag_select_set(tan, ton);
	} else {
		flag_select_clr(tan, ton);
	}
	return flag_select_stat(tan, ton);
}

int stat_selectflag(int mode, int tan, int ton)
{
	(void)mode;
	return flag_select_stat(tan, ton);
}


//
// proc. lockflag
//
void proc_lockflag(int mode, int tan, int ton)
{
	if(mode & LOCK_TONE_KEYIN) {
		if((tan < 2) && GET_FILELOCK(tan)) return;

		if(mode & LOCK_LOCKFLAG) {
			if(!(mode & LOCK_FORCE_KEYIN)) {
				if(flag_edit_stat_sub(tan, ton)) return;
			}
			flag_lock_set_main(tan, ton);
		} else {
			flag_lock_clr_main(tan, ton);
		}
		flag_fileedit_set(tan);
	}
}


//
// status. lockflag
//
int stat_lockflag(int mode, int tan, int ton)
{
	// tone lock status
	if(mode & STAT_TONE) {
		return GET_LOCK_MAIN(tan, ton);
	}
	// effective status (file lock & tone lock)
	if(mode & STAT_EFFECTIVE) {
		return flag_lock_stat_effective(tan, ton);
	}
	// file lock status
	if(tan >= 2) return 0;
	return filelock_flag[tan];
}


//
//	proc. editflag
//
int proc_editflag(int mode, int tan, int ton)
{
	/* clear editflag? (= file SAVE/LOAD) */
	if(!(mode & SET_EDITFLAG)) {
		int n;

		n = (tan==2) ? TONENUM2 : TONENUM;
		flag_edit_clear_all_in_table(tan, n);
		return 0;
	}

	/* set editflag */
	{
		int pre_editflag = flag_edit_stat_sub(tan, ton);
		int pre_fileedit = fileedit_flag[tan];

		if(mode & (EDIT_TONE_PARAM | EDIT_TONE_RENAME | EDIT_TONE_VOLUME |
		           EDIT_TONE_OPMCOPY | EDIT_TONE_TONECOPY | EDIT_TONE_OPMCHSEL |
		           EDIT_TONE_MEMO)) {
			flag_edit_set_sub(tan, ton);
		}

		if(mode & (EDIT_FILE_RENAME | EDIT_FILE_MEMO)) {
			flag_fileedit_set(tan);
		}

		if(mode & EDIT_FILE_CHVOLUME) {
			flag_fileedit_set(0);
			flag_fileedit_set(1);
		}

		/* editflag changed? */
		if(!pre_editflag && flag_edit_stat_sub(tan, ton)) {
			MakeNameBuffer(tan, ton);
			tonetable_dispflag[tan] = 1;
		}
	}

	return 0;
}


//
// return editflag status
//
int stat_editflag(int mode, int tan, int ton)
{
	if(mode & STAT_TONE) {
		return flag_edit_stat_sub(tan, ton);
	}
	return fileedit_flag[tan];
}
