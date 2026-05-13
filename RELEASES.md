# OPM Tone Editor 'N' v1.20 Release Notes

---

[English](RELEASES.md) | [日本語](RELEASES.ja.md)

## New Features

- **Slotmask support**
  - The **SM** (Slotmask) column in the OPM parameter table is now active,\
  allowing you to specify which operators are keyed on per tone.

- **OPM Copy UNDO**
  - The copy between OPM parameter tables via △△△ / ▽▽▽ can now be undone one step.
  - Click the **[UNDO]** button that appears after copying to restore the previous state.

- **Force Lock (CTRL + !)**
  - You can now lock a file or tone with **CTRL + !** even if it has been edited.
  - Force-locked tones are displayed with a `!` mark in yellow.

- **Auto-select on import (auto_select_imported_tones)**
  - Tones loaded from MML / OPM / ZMS / SND / MDX files can be automatically selected.
  - Configurable per format in `oe.jsn`.

- **Mouseover help (mouseover_help)**
  - Displays descriptive text when hovering over a UI element.\
  Can be enabled or disabled in `oe.jsn`. (default: true)

## Bug Fixes

- **OPM format file export fix**
  - Fixed a bug where the AMD value was incorrectly written in the AMS field.

- **Note offset default value fix**
  - Corrected the default note offset from **+48 to +45** to match the X68000 OPM clock (4.00MHz).

## Improvements

- **POLY count auto-correction improved**
  - POLY count is now correctly adjusted when the number of OPM channels is increased or decreased.

- **Algorithm symbol (OTG notation) improved**
  - Only op1, which has self-feedback, is now shown in **bold**.

- **Lock / select behavior fix**
  - Lock / unlock operations no longer set the tone-edited mark `*`.
  - Select / unselect operations no longer set the tone-edited mark `*`.
  - Both operations above will set the file-edited mark `*`.

- **oe.jsn extensions**
  - `scalekey.polyphony`: Sets the initial POLY count.
  - `auto_select_imported_tones`: Configures auto-selection per format on import.
  - `mouseover_help`: Enables or disables mouseover help display.
  - `true` / `false`, `"enable"` / `"disable"` and similar values are now accepted as booleans.

## Requirements

- scalekey **v1.01 or later** is required for playback functionality.

---

<footer>
<p align="center">Copyright (c) 2026 UG. All rights reserved.</p>
</footer>
