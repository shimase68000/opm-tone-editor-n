# OPM Tone Editor 'Ｎ' version 1.21

English | [日本語](RELEASES.ja.md)

**Updated:** August 26, 2026\
**Author:** UG.

---

## Overview

Added support for running alongside an external sound driver that supports\
MIDI output.

---

## 1. New Features and Enhancements

- Added a **mode in which the MIDI interface (YM3802) is left untouched**.

  When editing tones with a sound driver that supports MIDI output kept\
  resident, setting `scalekey.midi_enable` to `false` in `oe.jsn` stops\
  scalekey from initialising the MIDI interface.\
  The driver's own MIDI setup is left as it is.

  In this mode, playing via MIDI input is not available (keyboard performance\
  still works). The MIDI field shows `[MIDI:Off ]`.

  To make scalekey resident in this mode by hand, use the `-n` switch.

---

## 2. Requirements and Limitations

- **scalekey v1.10 or later** is required to use the performance features.\
  scalekey is released separately — see
  [scalekey Releases](https://github.com/shimase68000/scalekey/releases).
- **The `scalekey.midi_enable` setting requires scalekey v1.11 or later.**\
  If an older scalekey is resident, a warning is shown at startup.
- A single scalekey cannot be used from more than one Tone Editor.\
  (In the second and later Tone Editors, the performance features are unavailable\
  even though scalekey is resident.)
- Loading a template (TED) by itself is not supported.
- Verified on an X68000 emulator environment.

---

<footer>
<p align="center">Copyright (c) 2026 UG. All rights reserved.</p>
</footer>
