//
//	flag.h
//

/*
 * Flag ownership (authoritative rules)
 * ====================================
 *
 * mainflag : Tone[tan][ton].mark
 *            persistent state (saved in OED)
 * subflag  : toneflag[tan][ton]
 *            local working state (runtime only)
 *
 * [LOCK]
 *   storage : mainflag only
 *   read    : mainflag
 *   write   : mainflag only
 *
 * [SELECT]
 *   storage : mainflag only
 *   read    : mainflag
 *   write   : mainflag only
 *
 * [EDIT]
 *   storage : subflag only
 *   read    : subflag
 *   write   : subflag only
 *
 * [FILE EDIT]
 *   storage : fileedit_flag[tan] (local)
 *
 * [FILE LOCK]
 *   storage : filelock_flag[tan] (local)
 *
 * Important:
 *   - do not store LOCK in subflag
 *   - do not store EDIT in mainflag
 *
 * Flag storage rules:
 *
 *   MAINFLAG_*  -> stored in Tone[tan][ton].mark
 *   SUBFLAG_*   -> stored in toneflag[tan][ton]
 *
 * Never mix them.
 *
 * CLEAR_* is represented by absence of SET_* bit.
 * UNLOCK_* is represented by absence of LOCK_* bit.
 *
 * Note: CLEAR_* and UNLOCK_* are defined as (0<<N) = 0.
 * They are documentary macros only - they express caller intent in code,
 * but must NOT be used as bitmask operands (e.g. "mode & CLEAR_EDITFLAG"
 * always evaluates to 0). Use the absence of the corresponding SET_* / LOCK_*
 * bit to check for these states instead.
 *
 * STAT_TONE, STAT_FILE, STAT_EFFECTIVE are distinct query mode bits.
 * Pass exactly one of them to stat_lockflag() / stat_editflag().
 *
 */

/*
 * EDIT_TONE_* / EDIT_FILE_*:
 * These IDs are currently used mainly as caller-origin context tags.
 * They are not intended to be a fully generalized operation taxonomy.
 *
 * Primary purpose:
 *   - debug / trace
 *   - lightweight caller classification
 *
 * Example:
 *   ev_OPMData()/ev_OPMDataT()      -> EDIT_TONE_PARAM
 *   ev_ChangeToneName()             -> EDIT_TONE_RENAME
 *   mev_ChVolume()/mev_MasterVolume() -> EDIT_TONE_VOLUME
 */

// main flags (persistent)
#define MAINFLAG_SELECT     (1<<0)  // tone select flag (main)
#define MAINFLAG_LOCK       (1<<1)  // tone lock flag (main)

// sub flags (runtime)
#define SUBFLAG_EDIT        (1<<8)  // tone edited flag (sub)

#define STAT_TONE           (1<<31) // tone lock status only
#define STAT_FILE           (1<<30) // file lock status only
#define STAT_EFFECTIVE      (1<<29) // effective status (tone lock & file lock)

#define SET_EDITFLAG        (1<<23)
#define CLEAR_EDITFLAG      (0<<23)
#define LOCK_LOCKFLAG       (1<<22)
#define UNLOCK_LOCKFLAG     (0<<22)

#define SET_SELECTFLAG      (1<<21)
#define CLEAR_SELECTFLAG    (0<<21)

#define EDIT_TONE_PARAM     (1<<0)
#define EDIT_TONE_RENAME    (1<<1)
#define EDIT_TONE_VOLUME    (1<<2)
#define EDIT_TONE_OPMCOPY   (1<<3)
#define EDIT_TONE_TONECOPY  (1<<4)
#define EDIT_TONE_OPMCHSEL  (1<<5)
#define EDIT_TONE_MEMO      (1<<6)

#define EDIT_FILE_RENAME    (1<<12)
#define EDIT_FILE_CHVOLUME  (1<<13)
#define EDIT_FILE_MEMO      (1<<14)
#define EDIT_FILE_SAVE      (1<<15)
#define EDIT_FILE_LOAD      (1<<16)
#define EDIT_FILE_LOAD2     (1<<17)    // for template
#define EDIT_FILE_SAVE2     (1<<18)    // for template

#define LOCK_TONE_KEYIN     (1<<0)
#define LOCK_FORCE_KEYIN    (1<<1)  // force lock: allow locking when tone edit = ON


/* LOCK read helper (main only) */
#define GET_LOCK_MAIN(tan, ton) \
    (Tone[(tan)][(ton)].mark & MAINFLAG_LOCK)

/* effective lock (tone lock + file lock for file table 0/1) */
#define GET_LOCK_EFFECTIVE(tan, ton) \
    (((tan) < 2) ? (GET_LOCK_MAIN((tan), (ton)) || filelock_flag[(tan)]) \
                 :  GET_LOCK_MAIN((tan), (ton)))

/* SELECT read helper (main only) */
#define GET_SELECT_MAIN(tan, ton) \
    (Tone[(tan)][(ton)].mark & MAINFLAG_SELECT)

/* EDIT read helper (sub only) */
#define GET_EDIT_SUB(tan, ton) \
    (toneflag[(tan)][(ton)] & SUBFLAG_EDIT)

/* file flags */
#define GET_FILEEDIT(tan) (fileedit_flag[(tan)])
#define GET_FILELOCK(tan) (filelock_flag[(tan)])


/* helper API */
void flag_select_set(int tan, int ton);
void flag_select_clr(int tan, int ton);
void flag_select_toggle(int tan, int ton);
int  flag_select_stat(int tan, int ton);
void flag_select_init_main(int tan, int ton, int selected);

void flag_lock_set_main(int tan, int ton);
void flag_lock_clr_main(int tan, int ton);
int  flag_lock_stat_main(int tan, int ton);
int  flag_lock_stat_effective(int tan, int ton);

void flag_edit_set_sub(int tan, int ton);
void flag_edit_clr_sub(int tan, int ton);
int  flag_edit_stat_sub(int tan, int ton);
void flag_edit_clear_all_in_table(int tan, int ntones);
void flag_fileedit_set(int tan);
void flag_fileedit_clr(int tan);

/* legacy API used by existing callers */
int  proc_selectflag(int mode, int tan, int ton);
int  stat_selectflag(int mode, int tan, int ton);
void proc_lockflag(int mode, int tan, int ton);
int  stat_lockflag(int mode, int tan, int ton);
int  proc_editflag(int mode, int tan, int ton);
int  stat_editflag(int mode, int tan, int ton);

