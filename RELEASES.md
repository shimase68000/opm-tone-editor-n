# OPM Tone Editor 'Ｎ' version 1.20

English | [日本語](RELEASES.ja.md)

**Updated:** August 22, 2026  
**Author:** UG.

---

## Overview

In Tone Editor v1.20, integration with the performance module **scalekey**
lets you **play with unison, delay, and detune** from the keyboard or MIDI input.

Alongside this, mouse operations, the lock system, and file I/O were all
reviewed, with numerous bug fixes and reliability improvements.

---

## 1. New Features and Enhancements

### Performance (scalekey integration)

Working with the performance module scalekey, you can audition the tone you are
editing on the spot.

- Added **unison playback (UNISON 0–8 voices [0: Mute])**.
  Together with it, **delay**, which staggers the key-on timing, and **detune**,
  which offsets the pitch, were added.
- The **assign start channel**, the base channel used when assigning to OPM
  channels, can be specified.
  When the number of OPM channels is increased or decreased, the polyphony
  (POLY count) follows automatically.
- The channel assignment method can be switched between
  **Hold (first-come priority) / Over (last-come priority)**,
  for each of **Sequential (SEQ) / RoundRobin (RDR)**.
- Added **Slotmask** support.
  You can specify which operators are keyed on, per tone.

### Editor Operations

Mouse operations were extended.

- Value changes by click and SHIFT + click

  | Target | Click | SHIFT + Click |
  |------|------|------|
  | Detune value | ±1/64 semitone | ±semitone |
  | Delay / Volume value | ±1 | ±10 |
  | Note offset value | ±1 | ±12 |

- Value reset by CTRL + click
  - Detune / Delay value … reset to 0 with **CTRL + Click**
  - Volume value … set to the default (127 / 0) with **CTRL + Left / Right Click**
  - Note offset value … reset to the initial value with **CTRL + Click**
- **OPM Copy UNDO**
  A copy between the OPM parameter tables (△△△ / ▽▽▽) can be undone one step.
  The **[UNDO]** button that appears after copying restores the state before the copy.
- **Force Lock (CTRL + !)**
  A tone or file can be force-locked with **CTRL + !** even after it has been edited.
  Force-locked targets are shown with a `!` in yellow.

### Display

- **Main / Sub cursor display**
  For the OPM parameter tables, the current tone on the unfocused (sub) side is
  indicated by reversing the right edge of the tone name.
- **Tone lock mark display**
  - Normal lock … `!` in cyan
  - Force lock (edited + locked) … `!` in yellow
- **EXEC menu items added**
  - **DISP Sett** … shows the current values of the various settings
  - **DISP SKEY** … shows the version of the scalekey in use
- **EXEC menu enhancement**
  The following items can be run with a right click, which lets you edit the
  argument or file name first.
  - OED / SND / MML / OPM, EXEC1–4, DISP Fsts
- **Algorithm notation (OTG)**
  - op1 is shown in bold when it has self-feedback.

### File I/O and Settings

- When loading SND / MDX / MML / OPM / ZMS files,
  the **file name + (tone name) + tone number** is imported into the tone memo.
  The source information becomes the initial value of the tone memo.
- Added a setting to **automatically mark imported tones as export-selected**
  (`auto_select_imported_tones`). It can be configured per format in `oe.jsn`.
- The **mouseover help** (which shows a description when you hover over each UI
  element) can now be enabled or disabled in `oe.jsn` (default: enabled).
- Specifying a **jsn file** as a startup argument loads that settings file.
  If not specified, the jsn file in the same location as the executable is loaded.
- Added settings to `oe.jsn`.
  - `scalekey.polyphony` … initial POLY count
  - `scalekey.delay_count` … initial delay count
  - `scalekey.channel_assign_priority` … first-come / last-come priority for channel assignment
  - `scalekey.start_scan_channel` … assign start channel
  - `offset_export_tone_number` / `offset_export_template_number` … tone number offset on export
- `oe.jsn` now accepts more flexible notation.
  Values such as `true` / `false`, `"enable"` / `"disable"`, `"on"` / `"off"`,
  and `"yes"` / `"no"` can be used as booleans.

---

## 2. Behavior Changes

Points where behavior changed in v1.20.

- **Separated file lock and tone lock.**
  In v1.10 the two were linked; in v1.20 they operate independently.
- **Revised the conditions for setting the edit mark `*`** during tone editing.
  See the bundled manual (OE120.TXT) for details.
- **The way some setting values are specified in `oe.jsn` has changed.**
  If you are using a jsn file created with an earlier version, edit and use the
  `oe.jsn` bundled in the release archive, or check how the settings are written.

> The lock / edit states and the detailed file operation specifications are
> described in the bundled manual (OE120.TXT).

---

## 3. Bug Fixes and Reliability Improvements

Numerous bug fixes and improvements to file I/O reliability were made.\
The main ones are as follows.

- The file save and load paths were reworked throughout, improving reliability.
- When file loading fails, the **tone data being worked on is no longer lost**.
- Abnormal or corrupted files are now detected and rejected more accurately.
- Fixed a bug in OPM format export where the AMD value was written into the
  AMS field.
- The note display was aligned with the X68000 OPM clock (4.00MHz), and the
  default note offset was corrected from **+48 to +45**.

---

## 4. Requirements and Limitations

- **scalekey v1.10 or later** is required to use the performance features.
  scalekey is released separately — see
  [scalekey Releases](https://github.com/shimase68000/scalekey/releases).
- A single scalekey cannot be used from more than one Tone Editor.
  (In the second and later Tone Editors, the performance features are unavailable
  even though scalekey is resident.)
- Loading a template (TED) by itself is not supported.
- Verified on an X68000 emulator environment.

---

<footer>
<p align="center">Copyright (c) 2026 UG. All rights reserved.</p>
</footer>
