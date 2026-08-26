//
//	xevent.c
//

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<string.h>
#include    "_debug_.h"
#include	"oe.h"
#include	"disp.h"
#include	"key_input.h"
#include	"json.h"

extern void set_console(int, int);
extern void ex_scroll(int);
extern CONFIG config;

extern struct NAMECKBUF ExecFile;
extern struct NAMECKBUF jsonInf;
extern struct NAMECKBUF FileInf[2];
extern int fileedit_flag[3];
extern int filelock_flag[2];
extern int tnsw;
extern TABLENUM tb[2];
extern int tt[3];
extern int ln;

extern int delay_count;
extern int start_channel;
extern int opm_scale_offset;

extern int scalekey_channel_assign_policy;
extern int scalekey_poly_count;
extern int scalekey_unison_count;
extern int scalekey_opm_channel;
extern int scalekey_midi_channel_filter;

void disp_setting(void);
static void print_disp_setting(void);

//
// helpers
//
static char *octkey_str(int v)
{
	return v ? "SHIFT+/CTRL-" : "SHIFT-/CTRL+";
}

static char *onoff(int v)
{
	return v ? "on " : "off";
}

static char *repeat_str(int v)
{
	if(v == MOUSE_REPEAT_SPEED_FAST) return "fast";
	if(v == MOUSE_REPEAT_SPEED_SLOW) return "slow";
	return "medium";
}

static char *policy_str(int v)
{
	return v ? "RDR" : "SEQ";
}

static char *priority_str(int v)
{
	return v ? "OVER" : "HOLD";
}

static char *midifilter_str(int v, char *buf)
{
	if(v < 0)       strcpy(buf, "off");
	else if(v == 0) strcpy(buf, "any");
	else            sprintf(buf, "ch.%d", v);

	return buf;
}

// buf must hold at least 9 bytes (8 digits + NUL)
static char *bits8str(int v, char *buf)
{
	int i;
	for(i=0; i<8; i++)
		buf[i] = ((v >> (7-i)) & 1) ? '1' : '0';
	buf[8] = '\0';
	return buf;
}

//
// print disp setting
//
static void print_disp_setting(void)
{
	char mfbuf[16];
	char b0[16];
	char fa[sizeof(struct NAMECKBUF)];
	char fb[sizeof(struct NAMECKBUF)];
	char fe[sizeof(struct NAMECKBUF)];
	char fj[sizeof(struct NAMECKBUF)];

	printf("[ %s version %s - DISP Setting ]\n", OE_MYNAME, OE_VERSION);

	printf("\n");

	printf("                    author: \"%-20s\"\n", config.author);

	printf("              exec_command: \"%s\"\n", config.exec_command);

	printf("        offset_export_tone: %d\n", config.offset_export_tone_number);
	printf("    offset_export_template: %d\n", config.offset_export_template_number);

	printf("    auto_readonly:%-3s   mouse_repeat:%-6s   mouseover_help:%s\n",
		onoff(config.auto_readonly),
		repeat_str(config.mouse_repeat_speed),
		onoff(config.mouseover_help));

	printf("\n");

	printf("[scalekey]\n");

	printf("    offset:%-3d  octkey:%s\n",
		opm_scale_offset,
		octkey_str(config.scalekey.octkey));

	printf("    policy:%s  priority:%s  polyphony:%d  unison:%d\n",
		policy_str(scalekey_channel_assign_policy&1),
		priority_str(scalekey_channel_assign_policy>>1),
		scalekey_poly_count,
		scalekey_unison_count);

	printf("    opm_channel:%s  midi_channel_filter:%s  midi_enable:%s\n",
		bits8str(scalekey_opm_channel, b0),
		midifilter_str(scalekey_midi_channel_filter, mfbuf),
		onoff(config.scalekey.midi_enable));

	printf("    delay_count:%-3d        start_scan_channel:%c>\n",
		delay_count,
		start_channel+'A');

	printf("    load_on_startup: %-3s\n", onoff(config.scalekey.load_on_startup));
	printf("     unload_on_exit: %-3s\n", onoff(config.scalekey.unload_on_exit));

	printf("\n");

	printf("[opm_channel]\n");

	printf("    def:$%02X  MML:$%02X  OPM:$%02X  SND:$%02X  MDX:$%02X  ZMS:$%02X\n",
		config.opm_channel.def,
		config.opm_channel.mml,
		config.opm_channel.opm,
		config.opm_channel.snd,
		config.opm_channel.mdx,
		config.opm_channel.zms);

	printf("\n");

	printf("[auto_select_imported_tones]\n");

	printf("    OED:%-3s  MML:%-3s  OPM:%-3s  SND:%-3s  MDX:%-3s  ZMS:%-3s\n",
		onoff(config.auto_select.oed),
		onoff(config.auto_select.mml),
		onoff(config.auto_select.opm),
		onoff(config.auto_select.snd),
		onoff(config.auto_select.mdx),
		onoff(config.auto_select.zms));

	printf("\n");

	printf("[file]\n");

	sprintf(fe, "%s%s%s",
		ExecFile.drive, ExecFile.name, ExecFile.ext);
	printf("    xFile: %-32s\n", fe);

	sprintf(fj, "%s%s%s",
		jsonInf.drive, jsonInf.name, jsonInf.ext);
	printf("    jFile: %-32s\n", fj);

	sprintf(fa, "%s%s%s",
		FileInf[0].drive, FileInf[0].name, FileInf[0].ext);
	printf("    fileA: %-32s  edit:%d  lock:%d\n",
		fa, fileedit_flag[0], filelock_flag[0]);

	sprintf(fb, "%s%s%s",
		FileInf[1].drive, FileInf[1].name, FileInf[1].ext);
	printf("    fileB: %-32s  edit:%d  lock:%d\n",
		fb, fileedit_flag[1], filelock_flag[1]);

	printf("\n");

	printf("[internal]\n");

	printf("    tnsw:%d  tan:%d  ton:%d  ln:%d   tt[0]:%-3d tt[1]:%-3d tt[2]:%-3d\n",
		tnsw,
		tb[tnsw].tan,
		tb[tnsw].ton,
		ln,
		tt[0], tt[1], tt[2]);

	printf("\n");

	printf("press ENTER key to return");
}

//
// disp setting
//
void disp_setting(void)
{
	// 裏画面へ移動して表示する (ex_scroll と対で SCREEN_VWIDTH 行ぶん動く)
	set_console(SCREEN_XORIGIN, SCREEN_VWIDTH);
	B_CLR_AL();
	print_disp_setting();

	ex_scroll(0);

	while(key_input_bitsns(0, KEYMAP_EXDISP));
	key_input_bitsns(1, KEYMAP_EXDISP);

	set_console(SCREEN_XORIGIN, SCREEN_YORIGIN);
	ex_scroll(1);
}
