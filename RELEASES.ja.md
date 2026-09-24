# OPM Tone Editor 'Ｎ' version 1.21

[English](RELEASES.md) | 日本語

**更新日:** 2026年8月26日\
**作成者:** UG.

---

## 概要

MIDI 出力をサポートする外部サウンドドライバとの併用に対応しました。

---

## 1. 新機能・機能拡張

- **MIDI I/F（YM3802）に干渉しないモード** を追加しました。

  MIDI 出力をサポートするサウンドドライバを常駐させたまま音色編集を行う場合、\
  `oe.jsn` の `scalekey.midi_enable` を `false` に設定すると、\
  scalekey は MIDI I/F を初期化しません。\
  ドライバ側の MIDI 設定はそのまま保たれます。

  この場合、MIDI 入力による演奏はできません（本体キーボードからの演奏は可能です）。\
  MIDI 表示欄には `[MIDI:Off ]` と表示されます。

  このモードで scalekey を手動で常駐させる場合は、`-n` スイッチを指定します。

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
