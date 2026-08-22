# OPM Tone Editor 'Ｎ'

[English](README.md) | [日本語](README.ja.md)

OPM Tone Editor 'Ｎ' is a tone editing tool for the YM2151 (OPM) that runs on
Human68k on the X68000.
It is designed to make FM tone editing intuitive, responsive, and efficient.

---

## Overview

With OPM Tone Editor 'Ｎ', integration with the performance module **scalekey**
lets you edit a tone in real time while actually hearing it.
In v1.20 the performance features were extended, so you can now edit tones
under conditions closer to actual playing, using **unison, delay, and detune**.

![OPM Tone Editor 'Ｎ' Screenshot](docs/images/toneeditor120_screenshot.png)

---

## Key Features

- **Edit YM2151 (OPM) parameters directly**
- **Edit two files and two tone data sets at the same time**
- **Handle up to 200 tones without switching contexts**
- **Template slots (T01–T08)**
- **Preview the sound while editing**
- **Play and audition through scalekey** (keyboard and MIDI input)
- **Unison / delay / detune**
- **Protect tone data with edit lock**
- **Manage export targets with selection markers**

---

## Editing Experience

Parameter changes are applied immediately.
Because you can hear the result as you edit,\
it is easy to grasp how each parameter affects the sound.

Parameters are entered from the keyboard; targets are selected with the mouse.

---

## System Structure

OPM Tone Editor 'Ｎ' focuses on editing, while sound playback is handled by a
separate module (**scalekey**).

This separation allows:

- stable real-time playback
- responsive editing
- flexible extension of the performance features

> scalekey v1.10 or later is required to use the performance features.
> scalekey is distributed separately ([scalekey](https://github.com/shimase68000/scalekey)).

---

## Supported Formats

Input:

- OED, MML, OPM, SND, MDX, ZMS

Output:

- OED, MML, OPM, SND

---

## Installation

Download the distribution archive from Releases and extract it to any location.
Place the settings file `oe.jsn` in the same location as the executable, then run it.

- [Releases](../../releases)

To use the performance features, also obtain **scalekey** and place it somewhere
on your `PATH`. scalekey is released separately.

- [scalekey](https://github.com/shimase68000/scalekey) / [scalekey Releases](https://github.com/shimase68000/scalekey/releases)

Tone Editor v1.20 works with scalekey v1.10 or later.

---

## Documentation

Design rationale and specifications are available in:

- `docs/` — reference and design documents
- `docs/rationale/` — how the system is structured and why (for users)
- `docs/00_context/rationale/` — how the system is structured and why (for AI and implementers)

---

## Source Code

The `proj/` directory contains the main source code (`inc/`, `src/`, `Makefile`).
It is published as a reference; a detailed build environment guide is not
currently provided.
For normal use, please use the distribution archive from Releases.

The `oe.jsn` in the same directory also serves as a sample settings file.

---

## Summary

OPM Tone Editor 'Ｎ' provides an environment for editing tone data efficiently,
through:

- real-time editing while listening to the sound
- an efficient workflow that reuses existing tones
- an operation scheme with nothing wasted

---

## License

This project is released under the MIT License.

Copyright (c) 1994, 2023-2026 UG.
