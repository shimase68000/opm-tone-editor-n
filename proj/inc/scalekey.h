//
// scalekey.h
//

// scalekey version
#define OLD_SCALEKEY_VERSION100       "1.00"   // scalekey v1.00 はバージョン情報を返さないため特別扱い
#define OLD_SCALEKEY_VERSION101       "1.01"   // scalekey v1.01 はバージョン情報を返さないため特別扱い

#define REQUIRED_SCALEKEY_VERSION     "1.10"   // これ以降であれば起動を許可する (v1.10 を含む)
                                               // 2文字目の '.' はサフィックスが無いことを表す
                                               // ex) v1.20a の場合は "1a20" となる
                                               //
                                               // v1.20: UNSUPPORTED_SCALEKEY_VERSION から改名した。
                                               //   保持しているのは「必要な最小バージョン」であり、
                                               //   旧名では境界がどちら側か毎回考える必要があった。

// scalekey v1.00/v1.01 identification value
//
//   v1.00/v1.01 はバージョン取得 API を持たない。機能番号 1 を呼ぶと、v1.00/v1.01 の
//   ディスパッチャが残した値がそのまま d0 に返る。すなわち proc_table の
//   機能 1 のエントリ (routine1 - proc_table) = 0x34/0x38 である。
//   v1.00/v1.01 のアセンブルリストで確認済み。
//
//     move.w  proc_table(pc,d0.w),d0    ; d0.w <- 0x0034
//     jsr     proc_table(pc,d0.w)       ; routine1 は d0 に触れない
//     rte                               ; trap7_entry も d0 を復元しない
//
//   設計された署名ではなく実装の副産物だが、v1.00/v1.01 は凍結されたバイナリで
//   あり、この値が変わることはない。
//
//   下位ワードのみを比較すること。v1.00/v1.01 は move.w で d0 を設定するため、
//   上位ワードは呼び出し側の値がそのまま残る。現在の _trap7 ラッパは
//   move.l 8(a6),d0 で int 引数をそのまま渡すので上位ワードは 0 になるが、
//   マスクしておけばラッパの実装に依存しなくなる。
//
//   この判別は表示上の親切ではなく安全要件である。v1.00/v1.01 のディスパッチャ
//   には機能番号の範囲チェックが無く (v1.20 の cmp.w #PROC_NUM,d0 に相当
//   するものが存在しない)、proc_table は 0..12 の 13 本しか持たない。
//   v1.00/v1.01 を available と誤判定して v1.20 の API 番号を投げると、テーブル
//   外を読んで任意アドレスへ jsr する。
#define SCALEKEY_V100_SIGNATURE       0x34
#define SCALEKEY_V101_SIGNATURE       0x38

// exec command
#define COMMAND_EXEC_LOAD_SCALEKEY    "scalekey -s"
#define COMMAND_EXEC_UNLOAD_SCALEKEY  "scalekey -r -s"

// notify message status
//   v1.20: MESSTAT_SCALEKEY_UNKNOWN を追加。
//   mes_scalekey_table[] の並びと対応するため、必ず末尾に足すこと。
enum SCALEKEY_MESSAGE {
	MESSTAT_SCALEKEY_NOT_EXIST = 0,
	MESSTAT_SCALEKEY_UNSUPPORTED,
	MESSTAT_SCALEKEY_ALREADY_IN_USE,
	MESSTAT_SCALEKEY_UNKNOWN
};

// notify message
//   *0:at start up, *1:upon returning
#define MES_SCALEKEY_NOT_EXIST0      "scalekey が見つかりません。演奏機能なしで起動しますか？[Y/N]"
#define MES_SCALEKEY_NOT_EXIST1      "scalekey が見つかりません。演奏機能は使用できません。[ENTER]"

#define MES_SCALEKEY_UNSUPPORTED0    "scalekey のバージョンがサポート外です。演奏機能なしで起動しますか？[Y/N]"
#define MES_SCALEKEY_UNSUPPORTED1    "scalekey のバージョンがサポート外です。演奏機能は使用できません。[ENTER]"

#define MES_SCALEKEY_ALREADY_IN_USE0 "他のプロセスが scalekey を使用中です。演奏機能なしで起動しますか？（前回の異常終了の場合は scalekey を再常駐させてください）[Y/N]"
#define MES_SCALEKEY_ALREADY_IN_USE1 "他のプロセスが scalekey を使用中です。演奏機能は使用できません。[ENTER]"

#define MES_SCALEKEY_UNKNOWN0        "TRAP #7 は使用中ですが scalekey として応答しません。演奏機能なしで起動しますか？[Y/N]"
#define MES_SCALEKEY_UNKNOWN1        "TRAP #7 は使用中ですが scalekey として応答しません。演奏機能は使用できません。[ENTER]"
