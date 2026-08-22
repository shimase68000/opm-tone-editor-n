//
// message.h
//

#include    "escseq.h"

// keyin ESC
#define MES_KEYIN_ESC        "エディタの終了は SHIFT+ESC です。"

// cancel for key_inputYN
#define MES_KEYIN_CANCEL     "キャンセルしました。"

// cancel for load file
#define MES_LOADFILE_CANCEL  "ファイルの読み込みはキャンセルされました。"

// startup: config file could not be read (warning / error)
#define MES_CONFIRM_START_ANYWAY "このまま %s%s を起動してもよろしいですか？ [Y/N]"
#define MES_CONFIG_READ_ERROR    "設定ファイル %s%s%s の読み込み中にエラーが発生しました。"

// check scalekey version
#define MES_CHECK_SCALEKEY_VERSION0 "scalekey version %s が検出されました。"
#define MES_CHECK_SCALEKEY_VERSION1 "version %s 以降をご使用ください。"
#define MES_CHECK_SCALEKEY_UNKNOWN  "TRAP #7 は使用中ですが、scalekey として応答しませんでした。"

// confirm reading template data
#define MES_READ_TEMPLATE_DATA "[%s] テンプレートデータも読み込みますか？ [Y/N]"

// confirm exit oe.x
#define MES_EXIT             "音色データに変更があります。本当にエディタを終了してもよろしいですか？ [Y/N]"

// confirm purge file data
#define MES_PURGE_FILEDATA   "ファイルは編集されていますが、データを破棄しますか？ [Y/N]"

// confirm save file
#define MES_CONF_SAVE_FILE   "ファイル「%s」を保存しますか？ [Y/N]"

// edit filename before save file
#define MES_SAVEFILE_EDIT    "ファイルを保存します: "

// cancel save file
#define MES_SAVEFILE_CANCEL  "キャンセルしました。"

// illigal filename
#define MES_ILLIGAL_FILENAME "ファイル名が不正です。"

// confirm overwrite
#define MES_FILE_OVERWRITE   "ファイル「%s」を上書きしますか？ [Y/N]"

// confirm save to same file
#define MES_SAMEFILE         "読み込んだ %s ファイルに上書きしようとしています。本当によろしいですか？ [Y/N]"

// filename is NULL
#define MES_FILENAME_IS_NULL "保存するにはファイル名の入力が必要です。"

// input filename
#define MES_LOADFILE         "ファイル名を入力してください: "

// tone copy
#define MES_TONECOPY0        "音色コピー中です。コピー先を選択してください。" C1 "ESC" C1 ":Cancel"

// file command
#define MES_FILEOP_COMMAND   "ファイル操作コマンドを入力してください。" C46 "S" C33 ":Save " C46 "L" C33 ":Load " C46 "R" C33 ":Rename " C46 "M" C33 ":FileMemo " C46 "!" C33 ":FileLock  " C46 "ESC" C33 ":Cancel"

// file locked
#define MES_FILE_LOCKED      "ファイルをロックしました。"

// file unlocked
#define MES_FILE_UNLOCKED    "ファイルのロックを解除しました。"

// file cannot lock for filelock
#define MES_FILE_CANNOT_LOCK "ファイルは編集済みのためロックできません。（CTRL+!:強制ロック）"

// confirm file unlock
#define MES_CONFIRM_FILEUNLOCK  "ファイルロックを解除してもよろしいですか？ [Y/N]"

// tone cannot lock for tonelock
#define MES_TONE_CANNOT_LOCK "音色データは編集済みのためロックできません。（CTRL+!:強制ロック）"

// tone is locked
#define MES_TONE_IS_LOCKED   "音色データはロックされているため編集できません。"

// tone is locked for tonecopy
#define MES_TONE_IS_LOCKED2  "音色データはロックされているためコピーできません。"

// file is locked
#define MES_FILE_IS_LOCKED   "ファイルはロックされているため編集できません。"
#define MES_FILE_IS_LOCKED2  "ファイルはロックされているため音色データのロック操作はできません。"

// file is readonly (file cannot write)
#define MES_FILE_IS_READONLY "ファイルが書き込み禁止のため、保存できませんでした。[%s]"

// success loading file
#define MES_SUCCESS_LOADING_FILE "ファイルを正常に読み込みました。[%s]"

// file saved
#define MES_FILE_SAVED       "ファイルを保存しました。[%s]"

// file saved, and locked file
#define MES_FILE_SAVED_AND_LOCKED  "ファイルを保存して書き込み禁止にしました。[%s]"

// check file status (file does not exist)
#define MES_FILESTATUS_NOT_EXIST   "ファイルは存在しません。"

// check file status (template data)
#define MES_FILESTATUS_TEMPDATA    "テンプレートデータです。"

// message for OPM mute
#define MES_OPM_MUTE         "OPM Mute しました。（ALL SLOT TL=127）"

// opm copy message
#define MES_MAP_OPMCOPY0     "CTRL+左クリックで下段から上段↑へコピーします。"
#define MES_MAP_OPMCOPY1     "CTRL+左クリックで上段から下段↓へコピーします。"
#define MES_MAP_OPMCOPY_UNDO "パラメータをコピーする前の状態に戻します。"

// message for MAP_CHANNEL0/1
#define MES_MAP_CHANNEL      "編集中の音色データを反映するOPMチャンネルを選択します。"

// message for scalekey offset
#define MES_MAP_SCALEKEY_KEYOFFSET "演奏モジュールのノート（半音）オフセットを設定します。（初期値:+45）"

// message for scalekey playmode
#define MES_MAP_SCALEKEY_UNISON     "演奏モジュールの演奏モード（UNISON数）を設定します。（UNI0=Mute）"
#define MES_MAP_SCALEKEY_POLYPHONIC "演奏モジュールの演奏モード（POLYPHONY数）を設定します。（POLY0=Mute）"

// message for MAP_SCALEKEY_ASSIGN_POLICY
#define MES_MAP_SCALEKEY_ASSIGN_POLICY "OPMチャンネルのアサインポリシーを選択します。（Sequential/RoundRobin, Hold/Over）"

// message for MAP SCALEKEY_START_CHANNEL
#define MES_MAP_SCALEKEY_START_CHANNEL "OPMへのアサイン開始チャンネルを設定します。"

// message for MAP_SCALEKEY_DELAY_COUNT
#define MES_MAP_SCALEKEY_DELAY_COUNT "UNISON演奏時のディレイ時間を設定します。"

// message for MAP_SCALEKEY_OPMCH
#define MES_MAP_SCALEKEY_OPMCH  "演奏モジュールで使用するOPMチャンネルを選択します。"

// message for MAP_SCALEKEY_MIDI_CHANNEL
#define MES_MAP_SCALEKEY_MIDI_CHANNEL "MIDIチャンネルフィルタ（OFF/Any/Ch.1〜16）"

// message for MAP_SCALEKEY_DETUNE
#define MES_MAP_SCALEKEY_DETUNE "OPMチャンネルごとのディチューンを設定します。"

// message for MAP_CHANNELVOL0/1
#define MES_MAP_CHANNELVOL   "OPMチャンネルごとのボリュームを設定します。"

// message for MAP_MASTERVOL0/1
#define MES_MAP_MASTERVOL    "編集中の音色のボリュームを設定します。"

// EXEC normal
#define MES_EXEC_NORMAL       "%s[%s] 正常終了しました。(%d)"

// EXEC error
#define MES_EXEC_ERROR        "%s[%s] 異常終了しました。(%d)"

// "author" is undefined
#define MES_UNDEFINED_AUTHOR  "使用者名 \"author\" が設定されていませんが、続けてもよろしいですか？ [Y/N]"
#define MES_UNDEFINED_AUTHOR2 "使用者名は %s%s%s ファイルで設定してください。"

// "exec_command" is undefined
#define MES_UNDEFINED_EXEC_COMMAND   "実行コマンド名 \"exec_command\" が設定されていませんが、続けてもよろしいですか？ [Y/N]"
#define MES_UNDEFINED_EXEC_COMMAND2  "実行コマンド名は %s%s%s ファイルで設定してください。"

// no tone data in mdx
#define MES_NO_TONE_DATA_MDXFILE     "音色データを持たないMDXファイルです。"

// tone data error
#define MES_CORRECT_ILLEGAL_TONEDATA "異常な音色データを修復しました。[%s]"
