# OPM Tone Editor 'Ｎ' version 1.22

[English](RELEASES.md) | 日本語

**更新日:** 2026年11月14日\
**作成者:** UG.

---

## 概要

NOTE.X の音色バンク形式の読み込みに対応しました。

---

## 1. 新機能・機能拡張

- 入力フォーマットに **BIN** を追加しました。

  BIN は NOTE.X が `-t` スイッチ (save tone data) で出力する音色バンクファイルです。\
  読み込むと、その中の音色がエディタに展開されます。

  BIN ファイルは最大 256 音色を保持します。Tone Editor が扱えるのは 200 音色\
  のため、先頭から順に読み込み、超えた分は捨てます。未定義の音色は読み飛ばし、\
  以降の音色を歯抜けのないよう詰めて配置します。

  BIN ファイル上の音色番号は、`@n` の形で音色名に残します。\
  詰めて配置したあとでも元の番号をたどれます。

  BIN は読み込みのみ対応しています。取り込んだ音色は、出力フォーマットの\
  いずれかで書き出します。

  試聴に使う OPM チャンネルと、読み込んだ音色を選択状態にするかどうかは、\
  `oe.jsn` の MDX 用の設定（`default_opm_channel.mdx`、\
  `auto_select_imported_tones.mdx`）に従います。

---

## 2. 制限事項・動作条件

- 演奏機能を利用するには、**scalekey v1.10 以降**が必要です。\
  scalekey は別途リリースしています（[scalekey Releases](https://github.com/shimase68000/scalekey/releases) から入手してください）。
- **`scalekey.midi_enable` の設定を使う場合は、scalekey v1.11 以降が必要です。**\
  旧バージョンの scalekey が常駐している場合は、起動時に警告を表示します。
- 1 つの scalekey を複数の Tone Editor から使用することはできません。\
  （2 つめ以降の Tone Editor では scalekey が常駐していても演奏機能が使えません）
- テンプレート（TED）単体の読み込みには対応していません。
- 動作確認は X68000 エミュレータ環境で行っています。

---

<footer>
<p align="center">Copyright (c) 2026 UG. All rights reserved.</p>
</footer>
