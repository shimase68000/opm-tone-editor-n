# OPM Tone Editor 'Ｎ' version 1.22

English | [日本語](RELEASES.ja.md)

**Updated:** November 14, 2026\
**Author:** UG.

---

## Overview

Added support for reading the tone bank format of NOTE.X.

---

## 1. New Features and Enhancements

- Added **BIN** to the input formats.

  BIN is the tone bank file that NOTE.X writes with `-t` switch (save tone data). Selecting\
  one loads the tones it holds into the editor.

  A BIN file can hold up to 256 tones. Tone Editor handles 200, so the tones\
  it holds are read from the beginning and the rest are discarded. Undefined\
  entries are skipped, and the tones that follow are packed up without leaving\
  gaps.

  The original number in the BIN file is kept as the tone name, in the form\
  `@n`, so a tone can still be traced after being packed up.

  BIN is read but not written. Imported tones are written out in one of the\
  output formats.

  The OPM channel used for auditioning and whether imported tones are marked\
  as selected follow the MDX settings in `oe.jsn`\
  (`default_opm_channel.mdx`, `auto_select_imported_tones.mdx`).

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
