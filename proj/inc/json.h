//
// json.h
//

#define JSON_FILE_EXT      ".jsn"

#define JSON_TRUE_VALUE    (1)
#define JSON_FALSE_VALUE   (0)
#define JSON_IGNORED_VALUE (0x80000000)

// number of OPM channels a channel-mask string can address
#define JSON_OPM_CHANNEL_NUM (8)

// true
#define JSON_STR_TRUE      "true"
#define JSON_STR_ENABLE    "enable"
#define JSON_STR_ON        "on"
#define JSON_STR_YES       "yes"

// false
#define JSON_STR_FALSE     "false"
#define JSON_STR_DISABLE   "disable"
#define JSON_STR_OFF       "off"
#define JSON_STR_NO        "no"

// mouse_repeat_speed
#define JSON_STR_REPEAT_FAST   "fast"
#define JSON_STR_REPEAT_MEDIUM "medium"
#define JSON_STR_REPEAT_SLOW   "slow"

// channel_assign_policy
#define JSON_STR_POLICY_SEQ    "sequential"
#define JSON_STR_POLICY_RDR    "round_robin"
#define JSON_VAL_POLICY_SEQ    (0)
#define JSON_VAL_POLICY_RDR    (1)

// channel_assign_priority
#define JSON_STR_PRIORITY_HOLD "hold"
#define JSON_STR_PRIORITY_OVER "over"
#define JSON_VAL_PRIORITY_HOLD (0)
#define JSON_VAL_PRIORITY_OVER (1)

// Combine priority and policy into the single value scalekey expects.
#define ASSIGN_POLICY(priority,policy)  ((((priority)&1)<<1) | ((policy)&1))

//
// hier value
//
#define HIERVAL_BITWIDTH   (4)

enum {
	JSON_HIERVAL_TOP = 0,
	JSON_HIERVAL_SCALEKEY,
	JSON_HIERVAL_OPM_CHANNEL,
	JSON_HIERVAL_AUTO_SELECT,
	JSON_HIERVAL_FILE_IO
};

// top key
#define JSON_KEY_AUTHOR                 "author"
#define JSON_KEY_EXEC_COMMAND           "exec_command"
#define JSON_KEY_AUTO_READONLY          "auto_readonly"
#define JSON_KEY_MOUSE_REPEAT_SPEED     "mouse_repeat_speed"
#define JSON_KEY_MOUSEOVER_HELP         "mouseover_help"

// layer scalekey
#define JSON_KEY_SCALEKEY                "scalekey"
#define JSON_KEY_SCALEKEY_OFFSET         "offset"
#define JSON_KEY_SCALEKEY_OCTKEY         "octkey"
#define JSON_KEY_SCALEKEY_LOAD           "load_on_startup"
#define JSON_KEY_SCALEKEY_UNLOAD         "unload_on_exit"
#define JSON_KEY_SCALEKEY_POLICY         "channel_assign_policy"
#define JSON_KEY_SCALEKEY_OPM_CHANNEL    "opm_channel"
#define JSON_KEY_SCALEKEY_POLYPHONY      "polyphony"
#define JSON_KEY_SCALEKEY_UNISON         "unison"
#define JSON_KEY_SCALEKEY_CHANNEL_FILTER "midi_channel_filter"
#define JSON_KEY_SCALEKEY_PRIORITY       "channel_assign_priority"
#define JSON_KEY_SCALEKEY_START_CHANNEL  "start_scan_channel"
#define JSON_KEY_SCALEKEY_DELAY_COUNT    "delay_count"

// layer default_opm_channel
//   No "oed" key: an OED file carries its own opm_channel setting.
#define JSON_KEY_OPM_CHANNEL            "default_opm_channel"
#define JSON_KEY_OPM_CHANNEL_DEFAULT    "default"
#define JSON_KEY_OPM_CHANNEL_MML        "mml"
#define JSON_KEY_OPM_CHANNEL_SND        "snd"
#define JSON_KEY_OPM_CHANNEL_MDX        "mdx"
#define JSON_KEY_OPM_CHANNEL_OPM        "opm"
#define JSON_KEY_OPM_CHANNEL_ZMS        "zms"

// layer file_io
#define JSON_KEY_FILE_IO                "file_io"

// top key: export tone number offset
#define JSON_KEY_EXPORT_TONE_NUMBER     "offset_export_tone_number"
#define JSON_KEY_EXPORT_TEMPLATE_NUMBER "offset_export_template_number"

// layer auto_select_imported_tones
#define JSON_KEY_AUTO_SELECT            "auto_select_imported_tones"
#define JSON_KEY_AUTO_SELECT_OED        "oed"
#define JSON_KEY_AUTO_SELECT_MML        "mml"
#define JSON_KEY_AUTO_SELECT_SND        "snd"
#define JSON_KEY_AUTO_SELECT_MDX        "mdx"
#define JSON_KEY_AUTO_SELECT_OPM        "opm"
#define JSON_KEY_AUTO_SELECT_ZMS        "zms"

// json comment pattern
#define JSON_COMMENT_PATTERN1           "\"_comment_\""

//
// json default value
//
#define DEFAULT_CFG_AUTHOR                  ""
#define DEFAULT_CFG_EXEC_COMMAND            "OE"
#define DEFAULT_CFG_AUTO_READONLY           1            // enable
#define DEFAULT_CFG_MOUSE_REPEAT_SPEED      (MOUSE_REPEAT_SPEED_MEDIUM)    // oe.h
#define DEFAULT_CFG_DEFAULT_OPM_CHANNEL     (0b11111111) // all on
#define DEFAULT_CFG_MOUSEOVER_HELP          1            // enable
//
#define DEFAULT_CFG_EXPORT_TONE_NUMBER      0            // fileA/B  : @1 - @200
#define DEFAULT_CFG_EXPORT_TEMPLATE_NUMBER  200          // template : @201 - @208
//
#define DEFAULT_CFG_SCALEKEY_OFFSET         (48-CORRECT_OPM_NOTE_VALUE)
#define DEFAULT_CFG_SCALEKEY_OCTKEY         0
#define DEFAULT_CFG_SCALEKEY_LOAD           1
#define DEFAULT_CFG_SCALEKEY_UNLOAD         1
#define DEFAULT_CFG_SCALEKEY_POLICY         (JSON_VAL_POLICY_SEQ)
#define DEFAULT_CFG_SCALEKEY_PRIORITY       (JSON_VAL_PRIORITY_OVER)
#define DEFAULT_CFG_SCALEKEY_OPM_CHANNEL    (0b11111111)
#define DEFAULT_CFG_SCALEKEY_START_CHANNEL  0            // 0:Ch.A (0-7:normal scan, 8-15:reverse scan)
#define DEFAULT_CFG_SCALEKEY_POLYPHONY      8
#define DEFAULT_CFG_SCALEKEY_UNISON         1
#define DEFAULT_CFG_SCALEKEY_DELAY_COUNT    0            // 0:Delay OFF
#define DEFAULT_CFG_SCALEKEY_CHANNEL_FILTER 0            // -1:Off, 0:Any, 1-16:Ch.1-16
//
#define DEFAULT_CFG_AUTO_SELECT_OED         0
#define DEFAULT_CFG_AUTO_SELECT_MML         1
#define DEFAULT_CFG_AUTO_SELECT_SND         0
#define DEFAULT_CFG_AUTO_SELECT_MDX         1
#define DEFAULT_CFG_AUTO_SELECT_OPM         1
#define DEFAULT_CFG_AUTO_SELECT_ZMS         1

//
// struct
//
// string -> value tables.
//   JSON_BOOLEAN: boolean tokens (true/on/yes/...)
//   JSON_ENUM   : per-key keyword sets (mouse_repeat_speed, ...)
typedef struct {
	char *str;
	int  value;
} JSON_BOOLEAN;

typedef struct {
	char *str;
	int  value;
} JSON_ENUM;

typedef struct {
	char  *key;
	int   value;
} MIDI_CHANNEL_FILTER;

// OPM channel mask per import format.
//   No "oed" member: an OED file carries its own opm_channel setting.
//   Members follow the JSON_KEY_* order above; json_hier.def,
//   json_para_check.def, minmax.h and json.c use the same order.
typedef struct {
	int   def;
	int   mml;
	int   snd;
	int   mdx;
	int   opm;
	int   zms;
} CONFIG_OPM_CHANNEL;

// Auto-select imported tones, per format.
//   No "def" member: every format is stated individually.
typedef struct {
	int   oed;
	int   mml;
	int   snd;
	int   mdx;
	int   opm;
	int   zms;
} CONFIG_AUTO_SELECT;

typedef struct {
	int   offset;
	int   octkey;
	int   load_on_startup;
	int   unload_on_exit;
	int   channel_assign_policy;
	int   channel_assign_priority;
	int   opm_channel;
	int   start_scan_channel;
	int   polyphony;
	int   unison;
	int   delay_count;
	int   midi_channel_filter;
} CONFIG_SCALEKEY;

typedef struct {
	char *author;
	char *exec_command;
	int   auto_readonly;
	int   mouse_repeat_speed;
	int   mouseover_help;
	int   offset_export_tone_number;      // export offset for fileA/fileB tone number
	int   offset_export_template_number;  // export offset for template slot tone number
	CONFIG_SCALEKEY scalekey;
	CONFIG_OPM_CHANNEL opm_channel;
	CONFIG_AUTO_SELECT auto_select;
} CONFIG;
