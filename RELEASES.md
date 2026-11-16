# OPM Tone Editor 'Ｎ' version 1.23

English | [日本語](RELEASES.ja.md)

**Updated:** November 16, 2026\
**Author:** UG.

---

## Overview

Fixed a bug in reading MML files.

---

## 1. Bug Fixes

- Fixed MML loading taking lines other than tone definitions for tones.

  In MDX MML, a tone is defined as `@n = { ... }`. When loading an MML file,\
  Tone Editor also took the following for the start of a tone definition:

  - **A `@` inside a comment** (v1.20 to v1.22)\
    MML written by Tone Editor puts each tone name in a comment above its\
    definition, and tones imported from MML, MDX or BIN are named `@n`.\
    Reading such a file back shifted the parameters of every tone by one.\
    A message reporting that invalid tone data had been corrected was shown,\
    but the tones loaded were not the original ones.
  - **A `@` in track data**, such as `@16` or `@v124` (v1.20 to v1.22)
  - **Waveform definitions `@w`, tone map definitions `@k` and tone macro\
    definitions `@@`** (v1.10 to v1.22)

  Only `@n = {` outside comments is now read as a tone definition.

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
