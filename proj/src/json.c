//
// json.c
//

#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <iocslib.h>
#include   <doslib.h>
#include   "_debug_.h"
#include   "oe.h"
#include   "json.h"
#include   "mylib.h"

// offsetof
#ifndef offsetof
  #define offsetof(type, member) ((size_t) &(((type *) 0)->member))
#endif

#define ENABLE_JSON_ERROR_MESSAGE

#define _enable_old_version_
#undef  _enable_old_version_

#define _local_debug_json_
#undef  _local_debug_json_


extern int key_inputYN(int);

int  load_config(CONFIG *, struct NAMECKBUF);
void set_default_config_data(void);
void store_config_data(CONFIG *);
void print_config(const CONFIG *);
void correct_json_para(CONFIG *);

// UTF-8 BOM
static const uchar json_BOM[] = { 0xEF, 0xBB, 0xBF };

// boolean value
static const JSON_BOOLEAN json_boolean[] = {
	// true
	{JSON_STR_TRUE,    JSON_TRUE_VALUE},
	{JSON_STR_ENABLE,  JSON_TRUE_VALUE},
	{JSON_STR_ON,      JSON_TRUE_VALUE},
	{JSON_STR_YES,     JSON_TRUE_VALUE},
	// false
	{JSON_STR_FALSE,   JSON_FALSE_VALUE},
	{JSON_STR_DISABLE, JSON_FALSE_VALUE},
	{JSON_STR_OFF,     JSON_FALSE_VALUE},
	{JSON_STR_NO,      JSON_FALSE_VALUE}
};

// keyword sets for keys that take a fixed vocabulary.
// A value not listed here falls back to the key default, the same
// rule json_bool2val() applies to unrecognised boolean tokens.
static const JSON_ENUM json_repeat_speed[] = {
	{JSON_STR_REPEAT_FAST,   MOUSE_REPEAT_SPEED_FAST},
	{JSON_STR_REPEAT_MEDIUM, MOUSE_REPEAT_SPEED_MEDIUM},
	{JSON_STR_REPEAT_SLOW,   MOUSE_REPEAT_SPEED_SLOW}
};

static const JSON_ENUM json_assign_policy[] = {
	{JSON_STR_POLICY_SEQ,    JSON_VAL_POLICY_SEQ},
	{JSON_STR_POLICY_RDR,    JSON_VAL_POLICY_RDR}
};

static const JSON_ENUM json_assign_priority[] = {
	{JSON_STR_PRIORITY_HOLD, JSON_VAL_PRIORITY_HOLD},
	{JSON_STR_PRIORITY_OVER, JSON_VAL_PRIORITY_OVER}
};

static const MIDI_CHANNEL_FILTER midi_channel_filter_value[] = {
	{ "off", -1 },
	{ "any",  0 },
	{ "-1" , -1 },
	{ "0",    0 },
	{ "1",    1 },
	{ "2",    2 },
	{ "3",    3 },
	{ "4",    4 },
	{ "5",    5 },
	{ "6",    6 },
	{ "7",    7 },
	{ "8",    8 },
	{ "9",    9 },
	{ "10",  10 },
	{ "11",  11 },
	{ "12",  12 },
	{ "13",  13 },
	{ "14",  14 },
	{ "15",  15 },
	{ "16",  16 }
};

// json_error.def
#include "json_error.def"

// json_hier.def
#include "json_hier.def"

// hierval
static uint hierval;

#ifdef _debug_json_
  static int  push_count;
  static int  max_push_count = (int)(sizeof(hierval)*8)/HIERVAL_BITWIDTH;
#endif

//
#define is_channel_sep(c) ((c)=='_'  || (c)==' ' || (c)=='\t' || (c)=='~' || (c)=='|')

// '{' に対応する「pushしたか」スタック
// 0: pushしてない（未知のkeyで階層を下げた=> hierval シフトしない）
// 1: pushした    （既知のkeyで階層を下げた=> hierval シフトする）
// sp: '{' を認識した回数
static int  pushstk[64] = {0};
static int  max_sp = (int)sizeof(pushstk)/sizeof(pushstk[0]);
static int  sp = 0;
static char keybuf[256];
static char keybuf_temp[256];
static char valbuf_temp[256];

//
// json boolean to value
//
static int json_bool2val(char *s)
{
	int i;

	for(i=0; i<(int)(sizeof(json_boolean)/sizeof(JSON_BOOLEAN)); i++) {
		if(!my_stricmp(s, json_boolean[i].str)) {
			return json_boolean[i].value;
		}
	}

	return JSON_IGNORED_VALUE;
}

//
// my_atoi for json.c
//
static int json_my_atoi(const char *s)
{
	const char *p = s;

	if(s == NULL) return JSON_IGNORED_VALUE;

	if(*p == '-' || *p == '+') p++;
	if(*p == '\0') return JSON_IGNORED_VALUE;

	while(*p) {
		if(!my_isdigit((int)*p)) return JSON_IGNORED_VALUE;
		p++;
	}
	return atoi((const char*)s);
}

//
// disp json_error
//    *file: json file name
//       ln: line number
//        n: error number
//
static void disp_json_error(char *file, int ln, int n)
{
#ifdef ENABLE_JSON_ERROR_MESSAGE
	static int fg = 0;
	int dispsw = json_error[n].dispsw;

	if(!fg) {
		printf("\n");
		fg = 1;
	}

	// disp filename
	printf("[%s] ", file);

	// disp line number
	if(dispsw & LINE) printf("line:%d ", ln);

	// disp error or warning
	if(json_error[n].mode==ERROR) printf("error: ");
	else                          printf("warning: ");

	// disp message
	if(dispsw & MESS) printf("%s ", json_error[n].str);

	// disp key
	if(dispsw & KEY) printf("key:%s ", keybuf_temp);

	// disp value
	if(dispsw & VALUE) printf("value:%s ", valbuf_temp);

	printf("\n");
#endif
}
//
//
//
static int parse_channel_filter_value(const char *s)
{
	int i;

	for(i=0; i<(int)(sizeof(midi_channel_filter_value)/sizeof(MIDI_CHANNEL_FILTER)); i++) {
		if(!my_stricmp(s, midi_channel_filter_value[i].key)) {
			return midi_channel_filter_value[i].value;
		}
	}
	return JSON_IGNORED_VALUE;
}

//
//
//
static int parse_channel_value(const char *s, int digit)
{
	int  value;
	int  i;

	if(!*s || digit <= 0 || digit > 32) return JSON_IGNORED_VALUE;

    value = 0;
	i = 0;
	while((*s != '\0') && (i < digit)) {
		char c = *s++;
		if(is_channel_sep(c)) continue;
		if(c == '1') value |= (1<<i);
		else if(c != '0') return JSON_IGNORED_VALUE;
		i++;
	}

	return value;
}

//
//
//
static int trim_quotes(char *str)
{
    int len = strlen(str);

    if((len >= 2) && (str[0] == '\"') && (str[len-1] == '\"')) {
        my_memmove(str, str+1, len-2);
        str[len-2] = '\0';
		return 1;
    }
	return 0;
}

static void skip_spaces(char **pp)
{
    char  *p = *pp;
    while(*p && my_isspace(*p)) p++;
    *pp = p;
}

// "..." を読む（開始は '"', 成功時 out に格納して終端 '"' の次を返す）
// 失敗時は NULL
static char *read_quoted(char *s, char *out, int outsz)
{
    int i = 0;

	out[0] = '\0';

    if(*s != '\"') return NULL;  // "
    s++; // skip first "

    while(*s && *s != '\"') {    // "
        // NOTE: \" などのエスケープは未対応（必要なら後で追加）
        if(i < outsz-1) out[i++] = *s;
        s++;
    }
    out[i] = '\0';
    if(*s != '\"') return NULL;  // "

    return s+1; // skip closing "
}

// クオート無しトークンを読む：',' または '}' または行末まで
// 先頭空白は読み飛ばし、末尾空白は rtrim(out) で削除する。
static char *read_token(char *s, char *out, int outsz)
{
    int i = 0;

    // 先頭空白を読み飛ばす（memmove 不要）
    while(*s && my_isspace(*s)) s++;

    while(*s && *s != ',' && *s != '}' && *s != ']') {
        if(i < outsz-1) out[i++] = *s;
        s++;
    }
    out[i] = '\0';
    rtrim(out);

    return s; // delimiter位置（',' or '}' or '\0'）
}

//
// define state
//
// state:
//    KEY -> COL -> VAL -> KEY -> ...
//
#define JSON_NEXTSTATE_KEY 0
#define JSON_NEXTSTATE_COL 1
#define JSON_NEXTSTATE_VAL 2
static int json_nextstate = JSON_NEXTSTATE_KEY;

//
// func parse_line
//
static int parse_line(char *line)
{
    char valbuf[256];
    char *p = line;
	int  st = 0;

	keybuf_temp[0] = '\0';
	valbuf_temp[0] = '\0';

    while(*p) {
        skip_spaces(&p);
		if(!*p) return st;

		//
		// nextstate == KEY
		//
		if(json_nextstate == JSON_NEXTSTATE_KEY) {

        	// 区切り・構造文字
        	if(*p == ',') { p++; continue; }	// (next => key)

        	// 値側の '{'（= key処理で判定するので、ここでは「単独 '{'」を許容）
        	if(*p == '{') {
        	    // ここに来るのは、
        	    // - topの '{'
        	    // - あるいは未知キーの後で parse を継続した結果の '{'
        	    // など。pushは伴わないので 0 を積む。
        	    if(sp < max_sp) pushstk[sp++] = 0;
				else return JSON_ERR_STACK_OVERFLOW;  // stack overflow
        	    p++;
        	    continue;		// (next => key)
        	}

        	if(*p == '}') {
        	    // 1段戻る。ただし「pushした時だけ hierval を戻す」
        	    if(sp > 0) {
        	        sp--;
        	        if(pushstk[sp]) {
        	            if(hierval) hierval >>= HIERVAL_BITWIDTH;
#ifdef _debug_json_
						push_count--;
#endif
        	        }
        	    } else return JSON_ERR_MISSING_BRACES;  // missing braces
        	    p++;
        	    continue;	// (next => key)
        	}

			//
        	// --- key を読む（"key" 前提）---
			//
        	if(*p != '\"') {										// "
        	    // 想定外：クオート無しキー等。次の区切りまで捨てる
        	    while(*p && *p != ',' && *p != '}' ) p++;
        	    continue;	// (next => key)
        	}

			// keybuf <= key
        	{
        	    char *np = read_quoted(p, keybuf, (int)sizeof(keybuf));
				// backup keybuf to keybuf_temp
				strncpy(keybuf_temp, keybuf, (int)sizeof(keybuf_temp));
        	    if(!np) return JSON_ERR_NO_KEY_QUOTE;	// no key quote
        	    p = np;
        	}

#ifdef _debug_json_
	printf( "sp:%d, key:%s, ", sp, keybuf );
#endif

			// next => check ':'
			json_nextstate = JSON_NEXTSTATE_COL;
			continue;
		}

		//
		// nextstate == COL
		//
		if(json_nextstate == JSON_NEXTSTATE_COL) {
        	if(*p != ':') {
        	    // ':' が無ければ破損とみなして終了
        	    return JSON_ERR_NO_COLON;	// no colon
        	}
        	p++; // skip ':'

			// next => get value
			json_nextstate = JSON_NEXTSTATE_VAL;
			continue;
		}

		//
		// nextstate == VAL
		//

		// next => key
		json_nextstate = JSON_NEXTSTATE_KEY;

        if(*p == '{') {
            // 階層を下る（key が階層キーとして登録されている場合のみ push）
            int i;
            int pushed = 0;

            for(i=0; i<(int)(sizeof(json_hier_value)/sizeof(JSON_HIER_VALUE)); i++) {
                if(!my_stricmp(keybuf, json_hier_value[i].key)) {
                    hierval <<= HIERVAL_BITWIDTH;
                    hierval |= json_hier_value[i].hierval;
                    pushed = 1;
#ifdef _debug_json_
					push_count++;
					if(push_count > max_push_count) {
						return JSON_ERR_OVER_HIERARCHY_DEPTH;
					}
#endif
                    break;
                }
            }

            // この '{' に対して「pushしたかどうか」を積む
            if(sp < max_sp) pushstk[sp++] = pushed;
			else return JSON_ERR_STACK_OVERFLOW;  // stack over

#ifdef _debug_json_
	printf( "\nsp:%d, hierval:%08X\n", sp, hierval );
#endif

            p++; // consume '{'
            continue; // (next => key of child)
        }

        {
            char c_flag = 0;  // 1: quoted string, 0: unquoted token

			//
			// valbuf <= value
			//
            if(*p == '\"') {  // "
                char *np = read_quoted(p, valbuf, (int)sizeof(valbuf));
				// backup valbuf to valbuf_temp
				strncpy(valbuf_temp, valbuf, (int)sizeof(valbuf_temp));
				// np == NULL?
                if(!np) return JSON_ERR_NO_VALUE_QUOTE;  // no value quote
                p = np;
                c_flag = 1;
            } else {
                p = read_token(p, valbuf, (int)sizeof(valbuf));
                c_flag = 0;
				// backup valbuf to valbuf_temp
				strncpy(valbuf_temp, valbuf, (int)sizeof(valbuf_temp));
            }

#ifdef _debug_json_
	printf( "value:%s, c_flag:%d\n", valbuf, c_flag );
	key_inputYN(0);
#endif
			//
            // --- json_hier_keylist に格納 ---
			//
            {
				int flag = 0;
                int i, n;

                for(i=0; i<(int)(sizeof(json_hier_keylist)/sizeof(JSON_HIER_KEYLIST)); i++) {
                    if(!my_stricmp(keybuf, json_hier_keylist[i].key)
                        && (json_hier_keylist[i].hierval == hierval)) {

                        json_hier_keylist[i].c_flag = c_flag;

                        // 方針：整数項目で "1" を許さない → quoted の場合 i_val は無視
                        if(!c_flag) {
							int value = json_bool2val(valbuf);

							if(value != JSON_IGNORED_VALUE) {
								json_hier_keylist[i].i_val = value;
							} else {
								json_hier_keylist[i].i_val = json_my_atoi(valbuf);
							}
                        } else {
                                json_hier_keylist[i].i_val = JSON_IGNORED_VALUE;
                        }

                        n = (int)sizeof(json_hier_keylist[i].c_val);
                        strncpy(json_hier_keylist[i].c_val, valbuf, n-1);
                        json_hier_keylist[i].c_val[n-1] = '\0';

						flag = 1;
						if(json_hier_keylist[i].type==JSON_TYPE_STR && !c_flag) {
							return JSON_ERR_STRING_VALUE_NOT_QUOTED;
						}

						break;
                    }
                }
				if(!flag) st = JSON_WRN_UNKNOWN_KEY;
			}
            continue;
        }
    }
    return st;
}

//
// func load_config
//
//   ret: >0 error
//         0 success
//        <0 warning
//
int load_config(CONFIG *cfg, struct NAMECKBUF jsonfile)
{
	char  nbuf[128];
    char  line[1024];
    FILE *fp;
	int   stat = 0;
	int   wstat = 0;
	int   ln;

    // make config file path & name
	sprintf(nbuf, "%s%s%s", jsonfile.drive,
							jsonfile.name,
							JSON_FILE_EXT );

#ifdef _debug_json_
	printf( "\nJSON PATH [%s]\n", nbuf );
#endif

	set_default_config_data();

	fp = fopen( nbuf, "r" );
    if(!fp) {
		store_config_data(cfg);
		stat = JSON_ERR_NO_JSONFILE;
		disp_json_error(nbuf, 0, stat);
		return stat;  // file not found.
	}

	// init. parse value
	json_nextstate = JSON_NEXTSTATE_KEY;
	sp = 0;
	keybuf[0] = '\0';

	ln = 0;
	hierval = 0;
	memset(pushstk, 0, (int)sizeof(pushstk));

#ifdef _debug_json_
	push_count = 0;
#endif

    while(fgets(line, (int)sizeof(line), fp)) {
		ln++;

		// skip BOM
		{
			int len;

			len = strlen(line);
			if(len>=3 && (uchar)line[0]==json_BOM[0] && (uchar)line[1]==json_BOM[1] && (uchar)line[2]==json_BOM[2]) {
				my_memmove(line, line+3, len-3);
			}
		}

		// skip comment line
		{
			char *p;

        	if(my_strstr(line, JSON_COMMENT_PATTERN1)) continue;
			if(p = my_strstr(line, "//")) {
				// even or odd?
				if(!(my_strncnt(line, '\"', (int)(p-line))&1)) *p = '\0';
				// not closed
				else if(!strchr(p, '\"')) *p = '\0';
			}
		}

		// parse line
        if(stat = parse_line(line)) {
			// error or warning?
			if(json_error[stat].mode == ERROR) break;  // error
			else {
				disp_json_error(nbuf, ln, stat);  // warning
				wstat = stat;
				stat = 0;
			}
		}
    }

    fclose(fp);

	if(!stat && sp) stat = JSON_ERR_MISSING_BRACES; // missing braces
	if(stat) {
		disp_json_error(nbuf, ln, stat);
		return stat;
	}

	store_config_data(cfg);
    return wstat ? -1 : 0;
}

//
// get keyval
//
static JSON_HIER_KEYLIST *get_keyval(int hier, char *key)
{
	int i;

	for(i=0; i<(int)(sizeof(json_hier_keylist)/sizeof(JSON_HIER_KEYLIST)); i++ ) {
 		if(!my_stricmp(key, json_hier_keylist[i].key)
			&& (json_hier_keylist[i].hierval == hier)) {
			return &json_hier_keylist[i];
		}
	}
	return NULL;
}

//
// Returns values from numbers and strings
//
static int json_tovalue(int v, char *s, int d, char c_flag)
{
	if(v != JSON_IGNORED_VALUE) return v;

	if(c_flag) v = json_bool2val(s);

	return v==JSON_IGNORED_VALUE ? d : v;
}

//
// Returns a plain numeric value
//   Same shape as json_tovalue(), minus the token lookup: a key
//   written as a string has no numeric meaning, so the default wins.
//
static int json_toint(int v, int d, char c_flag)
{
	if(c_flag || (v == JSON_IGNORED_VALUE)) return d;

	return v;
}

//
// Returns a value from a keyword table
//   Anything not in the table - including a numeric form - falls
//   back to the default.
//
#define JSON_TOENUM(tbl,key) \
	json_toenum((tbl), (int)(sizeof(tbl)/sizeof(JSON_ENUM)), \
	            (key)->c_val, (key)->i_def, (key)->c_flag)

static int json_toenum(const JSON_ENUM *tbl, int n, char *s, int d, char c_flag)
{
	int i;

	if(c_flag) {
		for(i=0; i<n; i++) {
			if(!my_stricmp(tbl[i].str, s)) return tbl[i].value;
		}
	}

	return d;
}

//
// Returns an OPM channel bit mask from its string form
//
static int json_tochannel(char *s, int d, char c_flag)
{
	int v;

	if(!c_flag) return d;

	v = parse_channel_value(s, JSON_OPM_CHANNEL_NUM);

	return (v == JSON_IGNORED_VALUE) ? d : v;
}

//
// set config string data
//
static void set_config_string_data(char **t, char *s, const char *d, char c_flag)
{
	*t = (char *)(c_flag ? s : d);
}

//
// set default config data
//
void set_default_config_data(void)
{
	JSON_HIER_KEYLIST *key;
	int i;

	for(i=0; i<(int)(sizeof(json_hier_keylist)/sizeof(JSON_HIER_KEYLIST)); i++) {
		key = &json_hier_keylist[i];

		key->i_val = key->i_def;
		key->c_val[0] = '\0';
		key->c_flag = 0;

		if(key->type == JSON_TYPE_STR) strcpy(key->c_val, key->c_def);
	}
}

//
// store config data
//
void store_config_data(CONFIG *cfg)
{
	JSON_HIER_KEYLIST *key;
	int val;

	// top.author
	key = get_keyval(JSON_HIERVAL_TOP, JSON_KEY_AUTHOR);
	set_config_string_data(&cfg->author, key->c_val, key->c_def, key->c_flag);

	// top.exec_command
	key = get_keyval(JSON_HIERVAL_TOP, JSON_KEY_EXEC_COMMAND);
	set_config_string_data(&cfg->exec_command, key->c_val, key->c_def, key->c_flag);

	// top.auto_readonly
	key = get_keyval(JSON_HIERVAL_TOP, JSON_KEY_AUTO_READONLY);
	cfg->auto_readonly = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// top.mouseover_help
	key = get_keyval(JSON_HIERVAL_TOP, JSON_KEY_MOUSEOVER_HELP);
	cfg->mouseover_help = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// file_io.offset_export_tone_number
	key = get_keyval(JSON_HIERVAL_FILE_IO, JSON_KEY_EXPORT_TONE_NUMBER);
	cfg->offset_export_tone_number = json_toint(key->i_val, key->i_def, key->c_flag);

	// file_io.offset_export_template_number
	key = get_keyval(JSON_HIERVAL_FILE_IO, JSON_KEY_EXPORT_TEMPLATE_NUMBER);
	cfg->offset_export_template_number = json_toint(key->i_val, key->i_def, key->c_flag);

	// top.mouse_repeat_speed
	key = get_keyval(JSON_HIERVAL_TOP, JSON_KEY_MOUSE_REPEAT_SPEED);
	cfg->mouse_repeat_speed = JSON_TOENUM(json_repeat_speed, key);

	// scalekey.offset
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_OFFSET);
	cfg->scalekey.offset = json_toint(key->i_val, key->i_def, key->c_flag);

	// scalekey.octkey
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_OCTKEY);
	cfg->scalekey.octkey = json_toint(key->i_val, key->i_def, key->c_flag);

	// scalekey.channel_assign_policy
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_POLICY);
	cfg->scalekey.channel_assign_policy = JSON_TOENUM(json_assign_policy, key);

	// scalekey.channel_assign_priority
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_PRIORITY);
	cfg->scalekey.channel_assign_priority = JSON_TOENUM(json_assign_priority, key);

	// scalekey.start_scan_channel
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_START_CHANNEL);
	cfg->scalekey.start_scan_channel = json_toint(key->i_val, key->i_def, key->c_flag);

	// scalekey.delay_count
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_DELAY_COUNT);
	cfg->scalekey.delay_count = json_toint(key->i_val, key->i_def, key->c_flag);

	// scalekey.load_on_startup
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_LOAD);
	cfg->scalekey.load_on_startup = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// scalekey.unload_on_exit
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_UNLOAD);
	cfg->scalekey.unload_on_exit = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// scalekey.opm_channel
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_OPM_CHANNEL);
	cfg->scalekey.opm_channel = json_tochannel(key->c_val, key->i_def, key->c_flag);

	// scalekey.polyphony
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_POLYPHONY);
	cfg->scalekey.polyphony = json_toint(key->i_val, key->i_def, key->c_flag);

	// scalekey.unison
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_UNISON);
	cfg->scalekey.unison = json_toint(key->i_val, key->i_def, key->c_flag);

	// scalekey.midi_channel_filter
	key = get_keyval(JSON_HIERVAL_SCALEKEY, JSON_KEY_SCALEKEY_CHANNEL_FILTER);
	// This key is the one place where c_flag is not tested: its table
	// holds both words ("off"/"any") and digit strings ("0".."16"), so
	// c_val is parsed as-is. An unrecognised value falls back to i_def,
	// which is the same outcome the other keys reach through c_flag.
	val = parse_channel_filter_value(key->c_val);
	cfg->scalekey.midi_channel_filter = (val == JSON_IGNORED_VALUE) ? key->i_def : val;

	// file_io.opm_channel.default
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_DEFAULT);
	cfg->opm_channel.def = json_tochannel(key->c_val, key->i_def, key->c_flag);

	// (no file_io.opm_channel.oed: see CONFIG_OPM_CHANNEL in json.h)

	// file_io.opm_channel.mml
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_MML);
	cfg->opm_channel.mml = json_tochannel(key->c_val, cfg->opm_channel.def, key->c_flag);

	// file_io.opm_channel.snd
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_SND);
	cfg->opm_channel.snd = json_tochannel(key->c_val, cfg->opm_channel.def, key->c_flag);

	// file_io.opm_channel.mdx
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_MDX);
	cfg->opm_channel.mdx = json_tochannel(key->c_val, cfg->opm_channel.def, key->c_flag);

	// file_io.opm_channel.opm
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_OPM);
	cfg->opm_channel.opm = json_tochannel(key->c_val, cfg->opm_channel.def, key->c_flag);

	// file_io.opm_channel.zms
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_ZMS);
	cfg->opm_channel.zms = json_tochannel(key->c_val, cfg->opm_channel.def, key->c_flag);

	// file_io.auto_select.oed
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_AUTO_SELECT, JSON_KEY_AUTO_SELECT_OED);
	cfg->auto_select.oed = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// file_io.auto_select.mml
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_AUTO_SELECT, JSON_KEY_AUTO_SELECT_MML);
	cfg->auto_select.mml = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// file_io.auto_select.snd
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_AUTO_SELECT, JSON_KEY_AUTO_SELECT_SND);
	cfg->auto_select.snd = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// file_io.auto_select.mdx
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_AUTO_SELECT, JSON_KEY_AUTO_SELECT_MDX);
	cfg->auto_select.mdx = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// file_io.auto_select.opm
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_AUTO_SELECT, JSON_KEY_AUTO_SELECT_OPM);
	cfg->auto_select.opm = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

	// file_io.auto_select.zms
	key = get_keyval((JSON_HIERVAL_FILE_IO<<HIERVAL_BITWIDTH)|JSON_HIERVAL_AUTO_SELECT, JSON_KEY_AUTO_SELECT_ZMS);
	cfg->auto_select.zms = json_tovalue(key->i_val, key->c_val, key->i_def, key->c_flag);

}

void print_config(const CONFIG *cfg)
{
	printf( "\n" );

	printf( "%s = %s\n", JSON_KEY_AUTHOR, cfg->author);
	printf( "%s = %s\n", JSON_KEY_EXEC_COMMAND, cfg->exec_command);
	printf( "%s = %d\n", JSON_KEY_AUTO_READONLY, cfg->auto_readonly);
	printf( "%s = %d\n", JSON_KEY_MOUSEOVER_HELP, cfg->mouseover_help);
	printf( "%s = %d\n", JSON_KEY_MOUSE_REPEAT_SPEED, cfg->mouse_repeat_speed);

	printf( "%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_EXPORT_TONE_NUMBER, cfg->offset_export_tone_number);
	printf( "%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_EXPORT_TEMPLATE_NUMBER, cfg->offset_export_template_number);

	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_OFFSET, cfg->scalekey.offset);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_OCTKEY, cfg->scalekey.octkey);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_LOAD, cfg->scalekey.load_on_startup);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_UNLOAD, cfg->scalekey.unload_on_exit);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_POLICY, cfg->scalekey.channel_assign_policy);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_PRIORITY, cfg->scalekey.channel_assign_priority);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_START_CHANNEL, cfg->scalekey.start_scan_channel);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_DELAY_COUNT, cfg->scalekey.delay_count);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_OPM_CHANNEL, cfg->scalekey.opm_channel);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_POLYPHONY, cfg->scalekey.polyphony);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_UNISON, cfg->scalekey.unison);
	printf( "%s.%s = %d\n", JSON_KEY_SCALEKEY, JSON_KEY_SCALEKEY_CHANNEL_FILTER, cfg->scalekey.midi_channel_filter);

	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_DEFAULT, cfg->opm_channel.def);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_MML, cfg->opm_channel.mml);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_SND, cfg->opm_channel.snd);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_MDX, cfg->opm_channel.mdx);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_OPM, cfg->opm_channel.opm);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_OPM_CHANNEL, JSON_KEY_OPM_CHANNEL_ZMS, cfg->opm_channel.zms);

	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_AUTO_SELECT, JSON_KEY_AUTO_SELECT_OED, cfg->auto_select.oed);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_AUTO_SELECT, JSON_KEY_AUTO_SELECT_MML, cfg->auto_select.mml);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_AUTO_SELECT, JSON_KEY_AUTO_SELECT_SND, cfg->auto_select.snd);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_AUTO_SELECT, JSON_KEY_AUTO_SELECT_MDX, cfg->auto_select.mdx);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_AUTO_SELECT, JSON_KEY_AUTO_SELECT_OPM, cfg->auto_select.opm);
	printf( "%s.%s.%s = %d\n", JSON_KEY_FILE_IO, JSON_KEY_AUTO_SELECT, JSON_KEY_AUTO_SELECT_ZMS, cfg->auto_select.zms);
}

// json_para_check.def
#include "json_para_check.def"

//
// correct parameter sub
//
static void para_sub(int *para, int min, int max)
{
	if( *para < min ) *para = min;
	else if( *para > max ) *para = max;
}

//
// correct json parameter
//
void correct_json_para(CONFIG *cfg)
{
	int i;
	int num_entries = (int)(sizeof(para_check_table)/sizeof(PARA_CHECK));

	for(i=0; i<num_entries; i++) {
		int *val_ptr = (int *)((char *)cfg + para_check_table[i].offset);

		para_sub(val_ptr, para_check_table[i].min, para_check_table[i].max);
	}
}
