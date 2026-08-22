//
//	disp.h
//

#include    "escseq.h"

// screen origin
#define SCREEN_XORIGIN   (0)
#define SCREEN_YORIGIN   (0)
// screen width
#define SCREEN_HWIDTH    (96)   // horizontal width (character units)
#define SCREEN_VWIDTH    (32)   // vertical width (character units)

// space                      1234567890123456789
#define MES_SPACE19          "                   "
#define MES_SPACE19_LENGTH   (19)   //(strlen(MES_SPACE19))

// dispflag[][] : tone display state (bit-field, superposable)
#define DISP_NORMAL       (0)
#define DISP_TONECOPY     (1<<0)
#define DISP_SELECT       (1<<1)

// mev_ToneCopy dispatch selector
enum {
	TONECOPY_SET,
	TONECOPY_COPY,
	TONECOPY_CLEAR
};

#define SUB_MARK_WIDTH    (3)   // sub cursor marker: 右端反転セル数(2〜4)

#define MESTIME_BASE      (45)
#define	MESTIME_FOREVER   (0)                  // メッセージ表示時間 forever
#define MESTIME_BIT       (1)                  // メッセージ表示時間 bit
#define	MESTIME_NORMAL	  (10*(MESTIME_BASE))  // メッセージ表示時間 normal
#define	MESTIME_SHORT	  ( 5*(MESTIME_BASE))  // メッセージ表示時間 short
#define	MESTIME_LONG	  (30*(MESTIME_BASE))  // メッセージ表示時間 long
#define MESTIME_FILEMEMO  (10*(MESTIME_BASE))  // time for disp file memo
#define OPMCOPY_COUNT     (20)                 // OPM copy disp flash count

#define	TONETABLE0_XPOS	  (0)	    // pos of tone list
#define TONETABLE0_YPOS	  (1)
#define TONETABLE1_XPOS   (77)
#define TONETABLE1_YPOS	  (13)
#define TONETABLE2_XPOS   (TONETABLE1_XPOS)
#define TONETABLE2_YPOS	  (4)

#define OPMCHANNEL00_XPOS (TX0+1)   // pos of OPM Table
#define OPMCHANNEL00_YPOS (TY0+2)
#define OPMCHANNEL01_XPOS (OPMCHANNEL00_XPOS+19)
#define OPMCHANNEL01_YPOS (OPMCHANNEL00_YPOS)
#define OPMCHANNEL10_XPOS (TX1+1)
#define OPMCHANNEL10_YPOS (TY1+2)
#define OPMCHANNEL11_XPOS (OPMCHANNEL10_XPOS+19)
#define OPMCHANNEL11_YPOS (OPMCHANNEL10_YPOS)
#define OPMCHANNEL_CSIZE  (4)

#define OPMVOLUME00_XPOS  (OPMCHANNEL00_XPOS+13)
#define OPMVOLUME00_YPOS  (OPMCHANNEL00_YPOS+1)
#define OPMVOLUME01_XPOS  (OPMCHANNEL01_XPOS)
#define OPMVOLUME01_YPOS  (OPMCHANNEL01_YPOS+1)
#define OPMVOLUME10_XPOS  (OPMCHANNEL10_XPOS+13)
#define OPMVOLUME10_YPOS  (OPMCHANNEL10_YPOS+1)
#define OPMVOLUME11_XPOS  (OPMCHANNEL11_XPOS)
#define OPMVOLUME11_YPOS  (OPMCHANNEL11_YPOS+1)
#define OPMVOLUME_CSIZE   (3)

#define SCALEKEY_MIDIMODE_XPOS    (TX0+1)                        // [MIDI:Ch16]
#define SCALEKEY_MIDIMODE_YPOS    (0)
#define SCALEKEY_MIDIMODE_CSIZE   (11)

#define SCALEKEY_PLAYMODE_XPOS    (SCALEKEY_MIDIMODE_XPOS)       // UNI*/POLY*SEQ-HOLD
#define SCALEKEY_PLAYMODE_YPOS    (SCALEKEY_MIDIMODE_YPOS+1)
#define SCALEKEY_UNISON_CSIZE     (4)
#define SCALEKEY_POLYPHONIC_CSIZE (5)
#define SCALEKEY_POLICY_CSIZE     (8)

#define SCALEKEY_OPMCH_XPOS       (SCALEKEY_MIDIMODE_XPOS+19)    // A B C D E F G H
#define SCALEKEY_OPMCH_YPOS       (SCALEKEY_MIDIMODE_YPOS+1)
#define SCALEKEY_OPMCH_CSIZE      (4)

#define SCALEKEY_DETUNE_XPOS      (SCALEKEY_OPMCH_XPOS)           // 0000 ...
#define SCALEKEY_DETUNE_YPOS      (SCALEKEY_OPMCH_YPOS-1)
#define SCALEKEY_DETUNE_CSIZE     (4)

#define SCALEKEY_START_CH_XPOS    (SCALEKEY_MIDIMODE_XPOS)        // [A>]
#define SCALEKEY_START_CH_YPOS    (SCALEKEY_MIDIMODE_YPOS+2)
#define SCALEKEY_START_CH_CSIZE   (4)

#define SCALEKEY_DELAY_XPOS       (SCALEKEY_MIDIMODE_XPOS+SCALEKEY_START_CH_CSIZE)  // [DLY:001]
#define SCALEKEY_DELAY_YPOS       (SCALEKEY_MIDIMODE_YPOS+2)
#define SCALEKEY_DELAY_CSIZE      (9)

#define SCALEKEY_KEYOFFSET_XPOS   (SCALEKEY_DELAY_XPOS+SCALEKEY_DELAY_CSIZE)  // [+45]
#define SCALEKEY_KEYOFFSET_YPOS   (SCALEKEY_MIDIMODE_YPOS+2)
#define SCALEKEY_KEYOFFSET_CSIZE  (5)

#define SCALEKEY_MIDI_MARKER_XPOS (SCALEKEY_MIDIMODE_XPOS+SCALEKEY_MIDIMODE_CSIZE)  // '*1'
#define SCALEKEY_MIDI_MARKER_YPOS (SCALEKEY_MIDIMODE_YPOS)

/*--------------------------------

[MIDI:Ch.1]
UNI9/POLY9SEQ-HOLD
[A>][DLY:000][+45]

--------------------------------*/

#define DISP_MAXSIZE        (93)

#define MES_XPOS	        (0)	          // message print position
#define MES_YPOS	        (31)
#define MES2_XPOS	        (MES_XPOS)    // message print position2
#define MES2_YPOS	        (MES_YPOS-2)

#define EXECMENU_XPOS       (77)        // exec menu disp position
#define EXECMENU_YPOS	    (0)

#define OPMCOPY0_XPOS       (54)		// OPM copy disp position
#define OPMCOPY0_YPOS       (19)
#define OPMCOPY0_CHARA0     "△△△"
#define OPMCOPY0_CHARA1     "▲▲▲"
#define OPMCOPY1_XPOS       (OPMCOPY0_XPOS+7)
#define OPMCOPY1_YPOS       (OPMCOPY0_YPOS)
#define OPMCOPY1_CHARA0     "▽▽▽"
#define OPMCOPY1_CHARA1     "▼▼▼"
#define OPMCOPY_CSIZE       (6)

#define OPMCOPY_UNDO0_XPOS  (OPMCOPY0_XPOS+14)
#define OPMCOPY_UNDO0_YPOS  (TY0)
#define OPMCOPY_UNDO1_XPOS  (OPMCOPY_UNDO0_XPOS)
#define OPMCOPY_UNDO1_YPOS  (TY1)
#define OPMCOPY_UNDO_CHARA0 "      "
#define OPMCOPY_UNDO_CHARA1 C42 "[UNDO]" C0
#define OPMCOPY_UNDO_CSIZE  (6)

#define TONELOCK_CHARA          '!'          // tone lock chara
#define TONEEDIT_CHARA          '*'          // tone edit chara
#define STR_TONEEDIT_MARK       "[EDITED]"   // tone edited mark
#define STR_TONELOCK_MARK       "[LOCKED]"   // tone locked mark
#define STR_TONELOCK_MARK_CLEAR "        "   // clear space for tone lock mark

#define OPM_CHANNEL_CHARA0      " Ａ "
#define OPM_CHANNEL_CHARA1      " Ｂ "
#define OPM_CHANNEL_CHARA2      " Ｃ "
#define OPM_CHANNEL_CHARA3      " Ｄ "
#define OPM_CHANNEL_CHARA4      " Ｅ "
#define OPM_CHANNEL_CHARA5      " Ｆ "
#define OPM_CHANNEL_CHARA6      " Ｇ "
#define OPM_CHANNEL_CHARA7      " Ｈ "

#define SCALEKEY_POLYMODE0      "POLY" C1 "0" C1
#define SCALEKEY_POLYMODE1      "POLY" C1 "1" C1
#define SCALEKEY_POLYMODE2      "POLY" C1 "2" C1
#define SCALEKEY_POLYMODE3      "POLY" C1 "3" C1
#define SCALEKEY_POLYMODE4      "POLY" C1 "4" C1
#define SCALEKEY_POLYMODE5      "POLY" C1 "5" C1
#define SCALEKEY_POLYMODE6      "POLY" C1 "6" C1
#define SCALEKEY_POLYMODE7      "POLY" C1 "7" C1
#define SCALEKEY_POLYMODE8      "POLY" C1 "8" C1

#define SCALEKEY_UNIMODE0       "UNI" C1 "0" C1
#define SCALEKEY_UNIMODE1       "UNI" C1 "1" C1
#define SCALEKEY_UNIMODE2       "UNI" C1 "2" C1
#define SCALEKEY_UNIMODE3       "UNI" C1 "3" C1
#define SCALEKEY_UNIMODE4       "UNI" C1 "4" C1
#define SCALEKEY_UNIMODE5       "UNI" C1 "5" C1
#define SCALEKEY_UNIMODE6       "UNI" C1 "6" C1
#define SCALEKEY_UNIMODE7       "UNI" C1 "7" C1
#define SCALEKEY_UNIMODE8       "UNI" C1 "8" C1

#define SCALEKEY_CH_ASSIGN0     "        "
#define SCALEKEY_CH_ASSIGN1     C41 "SEQ-HOLD" C0
#define SCALEKEY_CH_ASSIGN2     C41 "RDR-HOLD" C0
#define SCALEKEY_CH_ASSIGN3     C41 "SEQ-OVER" C0
#define SCALEKEY_CH_ASSIGN4     C41 "RDR-OVER" C0

#define SCALEKEY_START_CH1_INC  C31 "[A>]" C0
#define SCALEKEY_START_CH2_INC  C31 "[B>]" C0
#define SCALEKEY_START_CH3_INC  C31 "[C>]" C0
#define SCALEKEY_START_CH4_INC  C31 "[D>]" C0
#define SCALEKEY_START_CH5_INC  C31 "[E>]" C0
#define SCALEKEY_START_CH6_INC  C31 "[F>]" C0
#define SCALEKEY_START_CH7_INC  C31 "[G>]" C0
#define SCALEKEY_START_CH8_INC  C31 "[H>]" C0

#define SCALEKEY_START_CH1_DEC  C31 "[<A]" C0
#define SCALEKEY_START_CH2_DEC  C31 "[<B]" C0
#define SCALEKEY_START_CH3_DEC  C31 "[<C]" C0
#define SCALEKEY_START_CH4_DEC  C31 "[<D]" C0
#define SCALEKEY_START_CH5_DEC  C31 "[<E]" C0
#define SCALEKEY_START_CH6_DEC  C31 "[<F]" C0
#define SCALEKEY_START_CH7_DEC  C31 "[<G]" C0
#define SCALEKEY_START_CH8_DEC  C31 "[<H]" C0

#define SCALEKEY_DELAY_NONE     C32 "[DLY:" C1 "OFF" C1 "]" C0
#define SCALEKEY_DELAY_BASE     C32 "[DLY:" C1 "%3d" C1 "]" C0

#define MIDI_CHANNEL_FILTER_NONE   C43 "[MIDI:None]" C0
#define MIDI_CHANNEL_FILTER_BASE   C42 "[MIDI:" C1 "%s" C1 "]" C0
#define MIDI_CHANNEL_FILTER_OFF    "OFF "
#define MIDI_CHANNEL_FILTER_ANY    "Any "
#define MIDI_CHANNEL_FILTER_CH1    "Ch.1"
#define MIDI_CHANNEL_FILTER_CH2    "Ch.2"
#define MIDI_CHANNEL_FILTER_CH3    "Ch.3"
#define MIDI_CHANNEL_FILTER_CH4    "Ch.4"
#define MIDI_CHANNEL_FILTER_CH5    "Ch.5"
#define MIDI_CHANNEL_FILTER_CH6    "Ch.6"
#define MIDI_CHANNEL_FILTER_CH7    "Ch.7"
#define MIDI_CHANNEL_FILTER_CH8    "Ch.8"
#define MIDI_CHANNEL_FILTER_CH9    "Ch.9"
#define MIDI_CHANNEL_FILTER_CH10   "Ch10"
#define MIDI_CHANNEL_FILTER_CH11   "Ch11"
#define MIDI_CHANNEL_FILTER_CH12   "Ch12"
#define MIDI_CHANNEL_FILTER_CH13   "Ch13"
#define MIDI_CHANNEL_FILTER_CH14   "Ch14"
#define MIDI_CHANNEL_FILTER_CH15   "Ch15"
#define MIDI_CHANNEL_FILTER_CH16   "Ch16"

#define CTABLEMARK0_XPOS    (TX0-2)  // current opm table mark disp position
#define CTABLEMARK0_YPOS    (TY0)
#define CTABLEMARK0         " "
#define CTABLEMARK0_LENGTH  (12)
#define CTABLEMARK1_XPOS    (TX1-2)
#define CTABLEMARK1_YPOS    (TY1)
#define CTABLEMARK1         CTABLEMARK0
#define CTABLEMARK1_LENGTH  (CTABLEMARK0_LENGTH)

#define FILENAME0_XPOS	    (0)   // filename disp position
#define FILENAME0_YPOS	    (0)
#define FILENAME1_XPOS    	(77)
#define FILENAME1_YPOS	    (12)
#define FILENAME_STRLEN     (19)  // filename max string length for disp

// Width of one exec-menu cell, in characters.
//   ms_map_init() also uses EXECMENU_STRLEN*2+1 as the tone list width,
//   so changing this moves both.
#define EXECMENU_STRLEN (9)

#define EXECMENU__  "         "   // *dummy*
#define EXECMENU00	"         "   // normal
#define EXECMENU01	"         "
#define EXECMENU02	"         "
#define EXECMENU03	"DISP " C1 "F" C1 "mmo"
#define EXECMENU04	"   OED   "
#define EXECMENU05	"   SND   "
#define EXECMENU06	"   MML   "
#define EXECMENU07  "   OPM   "
#define EXECMENU06_ "   MUS   "   // exp .mus

#define EXECMENU10	"DISP " C1 "F" C1 "sts"  // +[SHIFT] key
#define EXECMENU11	"         "
#define EXECMENU12	"EDIT " C1 "T" C1 "mmo"
#define EXECMENU13	"EDIT " C1 "F" C1 "mmo"
#define EXECMENU14	C1 "LOCK" C1 " Temp"
#define EXECMENU15	"         "
#define EXECMENU16	"         "
#define EXECMENU17  "         "
#define EXECMENU14_	C1 "UNLOCK" C1 "Tmp"

#define EXECMENU20	"  EXEC" C1 "1" C1 "  "  // +[CTRL] key
#define EXECMENU21	"  EXEC" C1 "2" C1 "  "
#define EXECMENU22	"  EXEC" C1 "3" C1 "  "
#define EXECMENU23  "  EXEC" C1 "4" C1 "  "
#define EXECMENU24	"command.x"
#define EXECMENU25	"         "
#define EXECMENU26	"         "
#define EXECMENU27	"         "

#define EXECMENU30	"         "  // +[SHIFT]+[CTRL] key
#define EXECMENU31	"         "
#define EXECMENU32	"         "
#define EXECMENU33	"         "
#define EXECMENU34	"DISP Sett"
#define EXECMENU35	"DISP " C1 "SKEY" C1
#define EXECMENU36	"DISP " C1 "OE.X" C1
#define EXECMENU37  "DISP " C1 "Ver." C1

