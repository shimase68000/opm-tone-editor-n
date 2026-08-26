//
//	scalekey.c
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<stdlib.h>
#include    "_debug_.h"
#include	"oe.h"
#include	"disp.h"
#include    "json.h"
#include    "trap7.h"
#include    "scalekey.h"
#include	"escseq.h"
#include	"key_input.h"
#include    "message.h"
#include    "mylib.h"

extern int check_scalekey_version(void);
extern int get_scalekey_midi_status(void);
extern int scalekey_available;
extern FUNC_PTR scalekey_main;
extern CONFIG config;

int  load_scalekey_on_startup(int);
void unload_scalekey_on_exit(void);

// 並びは enum SCALEKEY_MESSAGE と一対一で対応する。
// 片方だけ並べ替えないこと。
static const char *mes_scalekey_table[][2] = {
	{MES_SCALEKEY_NOT_EXIST0,      MES_SCALEKEY_NOT_EXIST1     },
	{MES_SCALEKEY_UNSUPPORTED0,    MES_SCALEKEY_UNSUPPORTED1   },
	{MES_SCALEKEY_ALREADY_IN_USE0, MES_SCALEKEY_ALREADY_IN_USE1},
	{MES_SCALEKEY_UNKNOWN0,        MES_SCALEKEY_UNKNOWN1       }
};

//
// notify scalekey problem
//        sw: 0=at start up
//            1=upon return
//    status: 0=continue
//            1=exit
//
static int notify_scalekey_problem(int sw, int pre, int stat)
{
    if(sw == 0) {
        // at start up: printf + [Y/N]
        printf(mes_scalekey_table[stat][0]);
        return key_inputYN(0);  // 'Y'=continue, 'N'=exit
    }
    // upon return: C_MES + keyin wait
    if(pre) {
        C_MES(C33, mes_scalekey_table[stat][1], MESTIME_FOREVER);
        keyin_wait();
    }
    return 0;
}

//
// load scalekey on startup
//        sw: 0=at start up
//            1=upon returning from exec-menu
//    status: 0=continue
//            1=exit
//
int load_scalekey_on_startup(int sw)
{
	int stat;
	int pre;

	pre = scalekey_available;

	// check scalekey and set scalekey_available
	stat = check_scalekey_version();

	// 自動常駐は「未常駐」のときだけ試みる。
	//
	//   v1.12 までは stat<0 を条件にしていたため、SKCHK_UNKNOWN
	//   (TRAP #7 に scalekey 以外が居る) でも scalekey.x を起動して
	//   いた。また当時は v1.00 常駐時も stat が -1 になっていたため、
	//   v1.00 の上に新版が二重常駐する経路が存在した。
	//   scalekey.x は proc_mark の位置で常駐を検出しており、その位置が
	//   版によって異なるため、旧版を認識できずに上書き常駐してしまう。
	//   結果として旧版のブロックがリブートまで回収されない。
	//
	//   SKCHK_NOT_EXIST に限定することで、この経路を塞ぐ。
	if(stat == SKCHK_NOT_EXIST && config.scalekey.load_on_startup) {
		// exec scalekey.r
		//   midi_enable が false なら -n を付けて常駐させる。scalekey は
		//   YM3802 を一切初期化しないので、MIDI 割り込みを持つ外部サウンド
		//   ドライバと共存できる。
		if(config.scalekey.midi_enable) {
			system(COMMAND_EXEC_LOAD_SCALEKEY);
		} else {
			system(COMMAND_EXEC_LOAD_SCALEKEY_N);
		}

		// re-check scalekey and set scalekey_available
		stat = check_scalekey_version();
	}

	if(stat == SKCHK_UNKNOWN) {
		// trap #7 is in use by something that is not scalekey
		return notify_scalekey_problem(sw, pre, MESSTAT_SCALEKEY_UNKNOWN);
	}

	if(stat == SKCHK_NOT_EXIST) {
		// scalekey is not exist
		return notify_scalekey_problem(sw, pre, MESSTAT_SCALEKEY_NOT_EXIST);
	}

	if(stat == SKCHK_UNSUPPORTED) {
		// unsupoprted (v1.00 もここに来る)
		return notify_scalekey_problem(sw, pre, MESSTAT_SCALEKEY_UNSUPPORTED);
	}

	// scalekey available (stat == SKCHK_AVAILABLE)
	{
		int mstat;

		mstat = check_mom();

		// unchenged?
		if(mstat == MOM_UNCHANGED) return 0;

		// in use by another master?
		if(mstat == MOM_ALREADY_IN_USE) {
			scalekey_available = 0;
			return notify_scalekey_problem(sw, pre, MESSTAT_SCALEKEY_ALREADY_IN_USE);
		}

		// scalekey acquired (mstat == MOM_ACQUIRED)
		scalekey_main = init_scalekey_trap7();
	}

	// midi_enable=false を指定したが、常駐中の scalekey が古く、
	// -n を解釈できない（= MIDI を初期化済みかもしれない）場合の警告。
	// 起動は妨げない。
	if(sw == 0 && !config.scalekey.midi_enable) {
		if(my_ver2int(get_scalekey_version()) <
		   my_ver2int(my_str2ver(MIDI_STATUS_SCALEKEY_VERSION))) {
			printf(MES_SCALEKEY_MIDI_ENABLE_UNSUPPORTED,
			       my_ver2str(my_str2ver(MIDI_STATUS_SCALEKEY_VERSION)));
			keyin_wait();
			printf("\n");
		}
	}

	return 0;
}

//
// unload scalekey on exit
//
void unload_scalekey_on_exit(void)
{
	if(!config.scalekey.unload_on_exit) return;
	if(!scalekey_available) return;

	system(COMMAND_EXEC_UNLOAD_SCALEKEY);
}
