//
//	trap7.c
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	"_debug_.h"
#include	"oe.h"
#include	"message.h"
#include	"disp.h"
#include	"scalekey.h"
#include	"json.h"
#include	"trap7.h"
#include	"mylib.h"

extern int scalekey_midi_board_not_exist;
extern int scalekey_midi_channel_filter;
extern int scalekey_poly_count;
extern int scalekey_unison_count;
extern int scalekey_opm_channel;
extern int scalekey_channel_assign_policy;
extern int opm_scale_offset;
extern int delay_count;
extern int start_channel;
extern uchar channel_slotmask[];
extern short channel_detune[];
extern CONFIG config;

int	 check_trap7(void);
int  check_scalekey_available(int);
int  check_scalekey_version(void);
int  get_scalekey_version(void);
void *trap7(int,int,int);
void *init_scalekey_trap7(void);
int  check_midi_board(void);
int  set_start_scan_channel(int);
void set_channel_detune(void);
void set_slotmask(void);
void set_scalekey_opm_channel(int);
void set_unison_polyphonic_count(int,int);
void set_scalekey_channel_assign_policy(int);
void set_scalekey_midi_channel_filter(int);
void midi_keyoff_all(void);
int  set_opm_scale_offset(int);
int  set_delay_count(int);
void set_scalekey_octkey(int);
EXECPATH *get_scalekey_exec_path(void);

int scalekey_available = 0;

//
// check MIDI board
// return 0= MIDI board exist
//
int check_midi_board()
{
	if(scalekey_available) {
		return (int)trap7(TRAP7_CHECKBOARD, 0, 0);
	}
	return -1;
}

//
// get scalekey version
//
int get_scalekey_version(void)
{
	if(scalekey_available) {
		return (int)trap7(TRAP7_DISPONOFF, GETVERSION, 0);
	}
	return -2;
}

//
// get scalekey exec path
//
EXECPATH *get_scalekey_exec_path(void)
{
	if(scalekey_available) {
		return (EXECPATH*)trap7(TRAP7_EXEC_PATH, 0, 0);
	}
	return (EXECPATH *)0;
}

//
// set delay count
//
int set_delay_count(int count)
{
	if(scalekey_available) {
		return (int)trap7(TRAP7_DELAY_COUNT, count, 0);
	}
	return -1;
}

//
// set start scan channel
//
int set_start_scan_channel(int channel)
{
	if(scalekey_available) {
		return (int)trap7(TRAP7_START_CHANNEL, channel, 0);
	}
	return -1;
}

//
// set channel detune
//
void set_channel_detune(void)
{
	if(scalekey_available) {
		trap7(TRAP7_DETUNE, (int)channel_detune, 0);
	}
}

//
// set slotmask
//
void set_slotmask(void)
{
	if(scalekey_available) {
		trap7(TRAP7_SLOTMASK, (int)channel_slotmask, 0);
	}
}

//
// keyoff ALL for MIDI
//
void midi_keyoff_all(void)
{
	if(scalekey_available) {
		trap7(TRAP7_KEYOFFALL, 0, 0);
	}
}

//
// set MIDI channel filter
//
void set_scalekey_midi_channel_filter(int channel)
{
	if(scalekey_available) {
		trap7(TRAP7_CHANNELFILTER, channel, 0);
	}
}

//
// set unison/polyphonic count
//
void set_unison_polyphonic_count(int uni, int poly)
{
	if(scalekey_available) {
		trap7(TRAP7_UNIPOLYCOUNT, uni, poly);
	}
}

//
// set opm channel for scalekey
//
void set_scalekey_opm_channel(int channel)
{
	if(scalekey_available) {
		trap7(TRAP7_CHANNELSELECT, channel, 0);
	}
}

//
// set midi octkey (SHIFT/CTRL)
//
void set_scalekey_octkey(int val)
{
	if(scalekey_available) {
		trap7(TRAP7_SHIFTCTRL, val, 0);
	}
}

//
// set scale offset
//
int set_opm_scale_offset(int offset)
{
	if(scalekey_available) {
		return (int)trap7(TRAP7_NOTEOFFSET, offset, 0);
	}
	return -1;
}

//
// set channel_assign_policy
//
void set_scalekey_channel_assign_policy(int policy)
{
	if(scalekey_available) {
	    trap7(TRAP7_ASSIGNPOLICY, policy&1, (policy>>1)&1);
	}
}

//
// init scalekey trap #7
//
void *init_scalekey_trap7(void)
{
	if(!scalekey_available) return (void*)0;

	// disp position
	trap7(TRAP7_DISPPOS, SCALEKEY_OPMCH_XPOS, SCALEKEY_OPMCH_YPOS);

	// note offset
	trap7(TRAP7_NOTEOFFSET, opm_scale_offset, 0);

	// octkey mode (shift/ctrl key)
	set_scalekey_octkey(config.scalekey.octkey);

	// channel assign policy
	set_scalekey_channel_assign_policy(scalekey_channel_assign_policy);

	// channel select
	trap7(TRAP7_CHANNELSELECT, scalekey_opm_channel, 0);

	// unison/poliphony
	trap7(TRAP7_UNIPOLYCOUNT, scalekey_unison_count, scalekey_poly_count);

	// midi channel filter
	set_scalekey_midi_channel_filter(scalekey_midi_channel_filter);

	// disp position of midi marker
	trap7(TRAP7_DISPPOS_MARK, SCALEKEY_MIDI_MARKER_XPOS, SCALEKEY_MIDI_MARKER_YPOS);

	// start scan channel
	trap7(TRAP7_START_CHANNEL, start_channel, 0);

	// OPM slotmask
	set_slotmask();

	// detune
	set_channel_detune();

	// delay count
	trap7(TRAP7_DELAY_COUNT, delay_count, 0);

	// disp mode and disp on
	trap7(TRAP7_DISPONOFF, DISPMODE, 0);

	// input enable (keyboard and MIDI input)
	trap7(TRAP7_ENABLE_KEYIN, 0b11, 0);

	// check MIDI board
	scalekey_midi_board_not_exist = check_midi_board();

	// main_loop address
	return (void*)trap7(TRAP7_MAINADRS, 0, 0);
}

//
// check_trap7
//    status 0: trap #7 vector is not hooked
//           1: trap #7 vector is hooked
//
int check_trap7(void)
{
	if(((unsigned int)INTVCG(0x27)&0xffffff) >= 0xfe0000) return 0;
	return 1;
}

//
// check scalekey availability
//    *trap #7 is hooked, then ask scalekey-compatible GETVERSION.
//        status: STAT_SCALEKEY_NOT_EXIST    // trap #7 is not hooked
//                STAT_SCALEKEY_UNKNOWN      // hooked, but not a scalekey
//                STAT_SCALEKEY_OLD100       // v1.00 (not available)
//                STAT_SCALEKEY_OLD101       // v1.01 (not available)
//                STAT_SCALEKEY_UNSUPPORTED  // version is too old
//                STAT_SCALEKEY_AVAILABLE    // scalekey is available
//
//    ver: >=0 で呼ぶと、その値を版数として判定する（TRAP #7 を再度叩かない）。
//         <0 で呼ぶと自分で取得する。
//
int check_scalekey_available(int ver)
{
	int iver;

	// trap #7 vector hooked?
	if(!check_trap7()) return STAT_SCALEKEY_NOT_EXIST;

	// ver2int
	if(ver<0) ver = (int)trap7(TRAP7_DISPONOFF, GETVERSION, 0);

	// scalekey v1.00/v1.01?
	//   v1.00 は版数を返さず、ディスパッチャの残留値 0x34 を返す。
	//   v1.01 は版数を返さず、ディスパッチャの残留値 0x38 を返す。
	//   詳細と、この判別が安全要件である理由は scalekey.h の
	//   SCALEKEY_V100_SIGNATURE を参照。
	//   下位ワードのみ比較すること（v1.00 は move.w で d0 を設定する）。
	if((ver & 0xffff) == SCALEKEY_V100_SIGNATURE) return STAT_SCALEKEY_OLD100;
	if((ver & 0xffff) == SCALEKEY_V101_SIGNATURE) return STAT_SCALEKEY_OLD101;

	iver = my_ver2int(ver);

	// hooked, but the handler did not answer as a scalekey at all.
	//   scalekey 以外の常駐が TRAP #7 を使っている場合がここに来る。
	//   v1.00/v1.01 は上の署名判定で既に分離済みなので、この状態は
	//   「scalekey ではない何か」を純粋に意味する。
	if(iver<0) return STAT_SCALEKEY_UNKNOWN;

	// supported version?
	if(iver < my_ver2int(my_str2ver(REQUIRED_SCALEKEY_VERSION))) return STAT_SCALEKEY_UNSUPPORTED;

	return STAT_SCALEKEY_AVAILABLE;
}

//
// check scalekey version
//     SKCHK_UNKNOWN     (-2): resident, but not scalekey
//     SKCHK_NOT_EXIST   (-1): not resident
//     SKCHK_UNSUPPORTED  (0): resident, but unusable
//     SKCHK_AVAILABLE    (1): ready to use
//

/*--------------------------------

  ex.1)
  scalekey v1.23
  ->
  ver = '1.23'

  ex.2)
  scalekey v1.23a
  ->
  ver = '1a23'

---------------------------------*/

int check_scalekey_version(void)
{
	int ver;
	int vstat;

	scalekey_available = 0;

	// TRAP #7 未フック = scalekey は常駐していない。
	// 自動常駐を試みてよい唯一の状態であり、伝えるべき異常も無いので
	// メッセージを出さずに戻る。
	// ここで先に確定させることで、以降は「TRAP #7 に何か居る」ことが
	// 保証され、状態の意味が単純になる。
	if(!check_trap7()) return SKCHK_NOT_EXIST;

	// 版数を 1 回だけ取得し、判定と表示の両方でその値を使う。
	// v1.12 までは判定用と表示用で TRAP #7 を 2 回叩いていた。
	ver   = (int)trap7(TRAP7_DISPONOFF, GETVERSION, 0);
	vstat = check_scalekey_available(ver);

#ifdef _debug_scalekey_
	printf("\nvstat:%d, raw_ver:%08X, iver:%08X, limit:%08X\n",
	       vstat, ver, my_ver2int(ver), my_str2ver(REQUIRED_SCALEKEY_VERSION));
	KEYIN_WAIT();
#endif

	// TRAP #7 は使われているが scalekey ではない
	if(vstat == STAT_SCALEKEY_UNKNOWN) {
		printf(MES_CHECK_SCALEKEY_UNKNOWN);
		printf("\n");
		return SKCHK_UNKNOWN;
	}

	if(vstat == STAT_SCALEKEY_OLD100) {
		printf(MES_CHECK_SCALEKEY_VERSION0, OLD_SCALEKEY_VERSION100);
	}
	else if(vstat == STAT_SCALEKEY_OLD101) {
		printf(MES_CHECK_SCALEKEY_VERSION0, OLD_SCALEKEY_VERSION101);
	}
	else {
		printf(MES_CHECK_SCALEKEY_VERSION0, my_ver2str(ver));
	}
	printf("\n");

	if(vstat != STAT_SCALEKEY_AVAILABLE) {
		printf(MES_CHECK_SCALEKEY_VERSION1, my_ver2str(my_str2ver(REQUIRED_SCALEKEY_VERSION)));
		printf("\n");
		return SKCHK_UNSUPPORTED;
	}

	scalekey_available = 1;

	return SKCHK_AVAILABLE;
}

//
// check MoM
//   *Assumes that the scale key version has been verified.
//       status: MOM_ACQUIRED              // scalekey acquired
//	             MOM_ALREADY_IN_USE        // scalekey is already in use by another master
//	             MOM_UNCHANGED             // scakekey remains unchenged
//
int check_mom(void)
{
	unsigned int pdb;
	unsigned int mom;

	// get my PDB
	pdb = (unsigned int)GETPDB();

    //
    //  ACQUIRE MoM.
    //  return:
    //      0     : MoM was default/restarted, now acquired by me
    //      myPDB : already owned by me
    //      other : already owned by another master
    //
	mom = (unsigned int)trap7(TRAP7_MARK_OF_MASTER, pdb, 0);

	// scalekey restarted? (or default state?)
	if(mom == 0) {
		return MOM_ACQUIRED;
	}

	// Is scalekey already in use by another master?
	if(mom != pdb) {
		return MOM_ALREADY_IN_USE;
	}

	return MOM_UNCHANGED;
}

//
// clear MoM
//
unsigned int clear_mom(void)
{
    unsigned int pdb;

	// scalekey is not available -> return
    if(!scalekey_available) return 0;

    pdb = (unsigned int)GETPDB();

	// check PDB and clear MoM
    return (unsigned int)trap7(TRAP7_MARK_OF_MASTER, 0, pdb);
}
