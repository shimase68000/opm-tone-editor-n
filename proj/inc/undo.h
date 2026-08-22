//
// undo.h
//

enum MODE_UNDO {
	UNDO_RESTORE,
	UNDO_OPM_PARAM,
	UNDO_TONE_NAME,
	UNDO_SLOTNO,
	UNDO_TONECOPY
};

typedef struct {
	int flag;
	int fileno;
	int slotno;
	int editflag;
	int toneflag;
	int src_tnsw;
	int src_fileno;
	int src_slotno;
	OPMDATA data;
} UNDO_BUFFER;
