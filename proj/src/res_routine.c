//
// res_routine.c
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include    "_debug_.h"
#include	"oe.h"
#include    "escseq.h"
#include    "message.h"
#include	"opm.h"
#include    "ms_map.h"
#include    "disp.h"
#include    "scalekey.h"
#include    "json.h"
#include    "undo.h"
#include	"flag.h"

extern void	C_MES(char *,char *,int);
extern void C_MES_CLR(void);
extern void	C_FILENAME(int,int);
extern void C_TONEMEMO(int);
extern void C_TONECOPY(int,int);
extern void	C_OPMTONENAME(int,int);
extern void C_DETUNE(int);
extern void C_OTG(int);
extern void C_OPM_TIP(int);
extern void PutToneTable(int);
extern void PutOPMCopy(void);
extern void PutExecMenu(int);
extern void PutUNDO(int);

extern void PutOPMChannelScalekey(void);
extern int  tonecopy_is_armed(void);
extern void tonecopy_get_src(int*,int*);
extern void vwait(int);

extern int  mes_count;             // message count
extern int  map_value;             // map value

extern int	fileedit_flag[3];      // fileedit_flag
extern int  opmcopy_count[2];      // OPM copy count
extern int	toneflag[3][TONENUM];  // tone mark flag

extern int scalekey_available;
extern int scalekey_poly_count;
extern int scalekey_unison_count;
extern int scalekey_channel_assign_policy;
extern int scalekey_midi_channel_filter;
extern int scalekey_opm_channel;
extern int opm_scale_offset;
extern int delay_count;
extern int start_channel;
extern short channel_detune[8];

extern OPMDATA  Tone[3][TONENUM];		// OPM data
extern TABLENUM tb[2];
extern int      tnsw;
extern int      ln;			// cursor on OPM table

extern CONFIG config;
extern UNDO_BUFFER undo;
extern FUNC_PTR scalekey_main;

int  res_routine(void);
void check_toneflag(void);
void undo_invalidate(int);
#ifdef _debug_lock_
void debug_disp(void);
#endif

typedef struct {
	int   flag;   // disp mode
	char *color;
	int   map;
	char *mes;
} MAP_MESSAGE;

int tonetable_dispflag[3] = {-1, -1, -1};  // tonetable disp flag
int tonememo_dispflag = 0;         // tonememo disp flag
int execmenu_keyflag = -1;		   // exec menu disp flag

int redisp_req = 0;

// map_message.def
#include "map_message.def"

//
// resident routine
//
int res_routine(void)
{
	static int disp_map_message = 0;
	static int pre_scalekey_available = 0;
	int i;
	int tc_armed = tonecopy_is_armed();

    // scalekey_main (OPM key on/off)
	if(scalekey_available && (int)scalekey_main) scalekey_main();

	// disp scalekey
	{
		static int pre_start_channel = -1;
		static int pre_delay_count = -1;
		static int pre_opm_scale_offset = -1;
		static int pre_scalekey_poly_count = -1;
		static int pre_scalekey_unison_count= -1;
		static int pre_scalekey_channel_assign_policy = -1;
		static int pre_scalekey_midi_channel_filter = -1;
		static int pre_scalekey_opm_channel = -1;

		int scalekey_dispflag = 0;

		if(pre_start_channel != start_channel) {
			pre_start_channel = start_channel;
			scalekey_dispflag = 1;
		}
		if(pre_delay_count != delay_count) {
			pre_delay_count = delay_count;
			scalekey_dispflag = 1;
		}
		if(pre_opm_scale_offset != opm_scale_offset) {
			pre_opm_scale_offset = opm_scale_offset;
			scalekey_dispflag = 1;
		}
		if(pre_scalekey_poly_count != scalekey_poly_count) {
			pre_scalekey_poly_count = scalekey_poly_count;
			scalekey_dispflag = 1;
		}
		if(pre_scalekey_unison_count != scalekey_unison_count) {
			pre_scalekey_unison_count = scalekey_unison_count;
			scalekey_dispflag = 1;
		}
		if(pre_scalekey_channel_assign_policy != scalekey_channel_assign_policy) {
			pre_scalekey_channel_assign_policy = scalekey_channel_assign_policy;
			scalekey_dispflag = 1;
		}
		if(pre_scalekey_midi_channel_filter != scalekey_midi_channel_filter) {
			pre_scalekey_midi_channel_filter = scalekey_midi_channel_filter;
			scalekey_dispflag = 1;
		}
		if(pre_scalekey_opm_channel != scalekey_opm_channel) {
			pre_scalekey_opm_channel = scalekey_opm_channel;
			scalekey_dispflag = 1;
		}
		if(redisp_req || scalekey_dispflag) {
			// disp. "UNI1/POLY8", [MIDI:Ch10], [+00]
			PutOPMChannelScalekey();
		}
	}

	// disp channel detune
	{
		// [0] のみ -1。差分を見つけた時点で内側ループが残りを同期するので、
		// 初回を必ず発火させるにはこれで足りる。
		static short pre_channel_detune[8] = {-1};

		if(pre_scalekey_available && !scalekey_available) {
			// clear
			C_DETUNE(0);
		}
		else if(scalekey_available) {
			// disp
			for(i=0; i<8; i++) {
				if(redisp_req || pre_channel_detune[i] != channel_detune[i]) {
					for(; i<8; i++) pre_channel_detune[i] = channel_detune[i];
					C_DETUNE(1);
					break;
				}
			}
		}
	}

    // check toneflag
    check_toneflag();

	// disp alg.
	{
		static int pre_otg = -1;
		int otg;

		otg = Tone[tb[tnsw].tan][tb[tnsw].ton].con;
		if(redisp_req || pre_otg != otg) {
			pre_otg = otg;
			C_OTG(otg);
		}
	}

	// disp opm tip.
	{
		static int pre_opm_tip_dispflag = -1;

		if(redisp_req || pre_opm_tip_dispflag != ln) {
			pre_opm_tip_dispflag = ln;
			C_OPM_TIP(ln);
		}
	}

	// check exec menu disp flag
	{
		static int pre_execmenu_keyflag = 0;
		int bsE;

		bsE = BITSNS(0xE);
		if(bsE & 0x01) {
			if(bsE & 0x02) execmenu_keyflag = 3;   // SHIFT+CTRL
			else           execmenu_keyflag = 1;   // SHIFT
		} else {
			if(bsE & 0x02) execmenu_keyflag = 2;   // CTRL
			else           execmenu_keyflag = 0;   // Normal
		}
		if(redisp_req || pre_execmenu_keyflag != execmenu_keyflag) {
			pre_execmenu_keyflag = execmenu_keyflag;
			PutExecMenu(execmenu_keyflag);
		}
	}

	// check fileedit_flag => C_FILENAME
	{
		// [0] のみ -1。差分を見つけた時点で内側ループが残りを同期するので、
		// 初回を必ず発火させるにはこれで足りる。
		static int pre_fileedit_flag[3] = {-1};

		for(i=0; i<3; i++) {
			if(redisp_req || pre_fileedit_flag[i] != fileedit_flag[i]) {
				for(; i<3; i++) pre_fileedit_flag[i] = fileedit_flag[i];
				C_FILENAME(0,0);
				C_FILENAME(1,0);
				break;
			}
		}
	}

	// check tonetable_dispflag
	{
		for(i=0; i<3; i++) {
			if(redisp_req || tonetable_dispflag[i]) {
				PutToneTable(i);
				tonetable_dispflag[i] = 0;
			}
		}
	}

	// disp UNDO
	{
		static int pre_undo_flag = -1;
		static int pre_undo_tnsw = -1;

		if(redisp_req || pre_undo_flag != undo.flag || pre_undo_tnsw != undo.src_tnsw) {
			pre_undo_flag = undo.flag;
			pre_undo_tnsw = undo.src_tnsw;
			PutUNDO(undo.flag);
		}
	}

	// disp. opmcopy
	{
		static int pre_opmcopy_flag[2] = {0};   // 0:normal 1:opmcopy
		static int opmcopy_chara0 = 0;
		static int opmcopy_chara1 = 0;

		int opmcopy_dispflag = 0;

		// check opmcopy_count
		for(i=0; i<2; i++) {
			if(opmcopy_count[i] > 0) opmcopy_count[i]--;
			if((opmcopy_count[i] > 0) != (pre_opmcopy_flag[i] > 0)) {
				pre_opmcopy_flag[i] = (opmcopy_count[i] > 0) ? 1 : 0;
				opmcopy_dispflag = 1;
			}
		}

		// disp OPM Copy
		if(redisp_req || opmcopy_dispflag) PutOPMCopy();
		else {
			if(map_value == MAP_OPMCOPY0) {
				if(!opmcopy_chara0) {
					B_LOCATE( OPMCOPY0_XPOS, OPMCOPY0_YPOS );
					B_PRINT ( C31 ); B_PRINT( OPMCOPY0_CHARA1 );
					opmcopy_chara0 = 1;
				}
			} else {
				if(opmcopy_chara0) {
					B_LOCATE( OPMCOPY0_XPOS, OPMCOPY0_YPOS );
					B_PRINT ( C31 ); B_PRINT( OPMCOPY0_CHARA0 );
					opmcopy_chara0 = 0;
				}
			}
			if(map_value == MAP_OPMCOPY1) {
				if(!opmcopy_chara1) {
					B_LOCATE( OPMCOPY1_XPOS, OPMCOPY1_YPOS );
					B_PRINT ( C31 ); B_PRINT( OPMCOPY1_CHARA1 );
					opmcopy_chara1 = 1;
				}
			} else {
				if(opmcopy_chara1) {
					B_LOCATE( OPMCOPY1_XPOS, OPMCOPY1_YPOS );
					B_PRINT ( C31 ); B_PRINT( OPMCOPY1_CHARA0 );
					opmcopy_chara1 = 0;
				}
			}
		}
	}

	// check mes_count
	{
		// during tonecopy
		if(tc_armed && mes_count>0) mes_count = -1;

		// countdown mes_count
		//   MESTIME_FOREVER は 0。先にデクリメントして -1 にすることで、
		//   「期限切れ(==0)」と区別している。この2行の順序を入れ替えないこと。
		if(mes_count >= 0) mes_count--;

		// clear message
		if(mes_count == 0) C_MES_CLR();
	}

	// tonecopy message line (arm / disarm / re-arm)
	{
		static int pre_src_tan = -2;
		static int pre_src_ton = -2;
		int stan, ston;

		tonecopy_get_src(&stan, &ston);
		if(pre_src_tan != stan || pre_src_ton != ston) {
			pre_src_tan = stan;
			pre_src_ton = ston;
			if(stan >= 0) C_TONECOPY(stan, ston);
			else          C_MES_CLR();
		}
	}

    // disp. map_message
	{
    	static int pre_map_value = -1;
		static int flag_map_message = 0;

		if(pre_map_value != map_value) {
			pre_map_value = map_value;
			flag_map_message = 1;
			disp_map_message = 0;
		}

    	if(mes_count<0 && !tc_armed && flag_map_message && config.mouseover_help) {
			flag_map_message = 0;

       		for(i=0; i<(int)(sizeof(map_message)/sizeof(MAP_MESSAGE)); i++) {
			    if(map_value == map_message[i].map) {
					// undo marker message (exclusive display)
					if(map_message[i].flag == MAPMES_UNDO0 && (!undo.flag || undo.src_tnsw == 0)) continue;
					if(map_message[i].flag == MAPMES_UNDO1 && (!undo.flag || undo.src_tnsw == 1)) continue;
					// when scalekey not available
					if(map_message[i].flag == MAPMES_NOT_EXIST && !scalekey_available) continue;

					// disp map message
				    C_MES(map_message[i].color, map_message[i].mes, MESTIME_FOREVER);
					disp_map_message = 1;
					break;
			    }
	   		}
		}
	}

	// check tonememo count and flag
	{
		static int pre_mes_count = -2;
		static int flag_mes_count = 0;

		if(pre_mes_count != mes_count) {
			pre_mes_count = mes_count;
			flag_mes_count = 1;
		}

		if(!disp_map_message && !tc_armed && (tonememo_dispflag || (flag_mes_count && (mes_count<0)))) {
			tonememo_dispflag = 0;
			flag_mes_count = 0;
			mes_count = -1;

			// disp tonememo
			C_TONEMEMO(0);
		}
	}

#ifdef _debug_lock_
	debug_disp();
#endif

	// update pre_*
	pre_scalekey_available = scalekey_available;

	// clear redisp_req
	redisp_req = 0;

	// vwait
	vwait(1);

	return 1;
}

//
// check editflag
//
void check_toneflag(void)
{
	static int pre_toneflag[2] = {0};

	int i;
	int tan, ton;
	int xpos, ypos;

    for(i=0; i<2; i++) {
	    tan = tb[i].tan;
    	ton = tb[i].ton;

        if(pre_toneflag[i] == toneflag[tan][ton]) continue;
		pre_toneflag[i] = toneflag[tan][ton];

		xpos = (i==0) ? TX0 : TX1;
		ypos = (i==0) ? TY0 : TY1;

        B_LOCATE( xpos, ypos );
		C_OPMTONENAME( tan, ton );
	}
}

//
// undo invalidate
//
void undo_invalidate(int reason)
{
#ifndef _debug_undo_
	(void)reason;
#endif

	if(!undo.flag) return;
	undo.flag = 0;

#ifdef _debug_undo_
	B_LOCATE(83,30);
	printf( "UNDO:%02d", reason );
#endif
}

#ifdef _debug_lock_
//
// debug disp
//
void debug_disp(void)
{
	static int pre_tan = -1;
	static int pre_ton = -1;
	static int pre_tonemark = -1;
	static int pre_toneflag = -1;
	int tan;
	int ton;
	int xpos = 83;
	int ypos = 24;

	tan = tb[tnsw].tan;
	ton = tb[tnsw].ton;

	if(pre_tan==tan && pre_ton==ton && pre_tonemark==Tone[tan][ton].mark && pre_toneflag==toneflag[tan][ton]) return;
	pre_tan = tan;
	pre_ton = ton;
	pre_tonemark = Tone[tan][ton].mark;
	pre_toneflag = toneflag[tan][ton];

	B_LOCATE(xpos,ypos++);
	printf( "tan:%3d", tan );

	B_LOCATE(xpos,ypos++);
	printf( "ton:%3d", ton );

	B_LOCATE(xpos,ypos++);
	printf( "lock:%d", Tone[tan][ton].mark & MAINFLAG_LOCK );

	B_LOCATE(xpos,ypos++);
	printf( "edit:%d", toneflag[tan][ton] & SUBFLAG_EDIT );

	B_LOCATE(xpos,ypos++);
	printf( "sel:%d", Tone[tan][ton].mark & MAINFLAG_SELECT );

	B_LOCATE(xpos,ypos++);
	printf( "flag:%d", toneflag[tan][ton] );
}
#endif
