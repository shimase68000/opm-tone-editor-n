//
// trap7.h
//

// scalekey(trap#7) API number
enum SCALEKEY_API_NUMBER {
	TRAP7_EXITTSR = 0,
	TRAP7_DISPONOFF,      // disp on/off and get scalekey version
	TRAP7_DISPPOS,        // set disp pos. and get scalekey version
	TRAP7_NOTEOFFSET,     // set note offset and get scalekey version
	TRAP7_SHIFTCTRL,
	TRAP7_ASSIGNPOLICY,
	TRAP7_CHANNELSELECT,
	TRAP7_MAINADRS,
	TRAP7_UNIPOLYCOUNT,
	TRAP7_CHANNELFILTER,
	TRAP7_DISPPOS_MARK,
	TRAP7_CHECKBOARD,
	TRAP7_KEYOFFALL,
	TRAP7_SLOTMASK,
	TRAP7_ENABLE_KEYIN,
	TRAP7_START_CHANNEL,
	TRAP7_DETUNE,
	TRAP7_DELAY_COUNT,
	TRAP7_ACCESS_PERMISSION,
	TRAP7_EXEC_PATH,
	TRAP7_MARK_OF_MASTER
};

// scalekey availability
//   v1.20: STAT_SCALEKEY_UNKNOWN を追加。
//   従来は「TRAP #7 が未フック」と「フックされているが scalekey として
//   応答しない」の 2 状態が STAT_SCALEKEY_NOT_EXIST に潰れていた。
//   前者は自動常駐を試みてよい状態、後者は絶対に試みてはいけない状態で、
//   取るべき行動が正反対のため分離した。
enum SCALEKEY_VERSION_STATUS {
	STAT_SCALEKEY_NOT_EXIST = 0,   // trap #7 is not hooked
	STAT_SCALEKEY_UNKNOWN,         // hooked, but did not answer as scalekey
	STAT_SCALEKEY_OLD100,          // v1.00 (identified by signature)
	STAT_SCALEKEY_OLD101,          // v1.01 (identified by signature)
	STAT_SCALEKEY_UNSUPPORTED,     // reported version is too old
	STAT_SCALEKEY_AVAILABLE        // scalekey is available
};

// check_scalekey_version() return value
//   v1.20: 従来は暗黙の -1 / 0 / 1 だったものを明示した。
//   既存の 3 値は数値を変えていないので、旧コードとの互換は保たれる。
enum SCALEKEY_CHECK_RESULT {
	SKCHK_UNKNOWN     = -2,        // resident, but not scalekey (do NOT auto-load)
	SKCHK_NOT_EXIST   = -1,        // not resident (the only auto-loadable state)
	SKCHK_UNSUPPORTED =  0,        // resident, but unusable
	SKCHK_AVAILABLE   =  1         // ready to use
};

// check MoM status
enum MOM_STATUS {
	MOM_ACQUIRED = 0,              // scalekey acquired
	MOM_ALREADY_IN_USE,            // scalekey is already in use by another master
	MOM_UNCHANGED                  // scakekey remains unchenged
};

// disp mode
enum {
	DISPMODE_TONE_EDITOR_N = 1
};

#define GETVERSION (-1)
#define DISPOFF    (0)
#define DISPMODE   (DISPMODE_TONE_EDITOR_N)
