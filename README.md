# OPM Tone Editor 'N'

[English](README.md) | [日本語](README.ja.md)

OPM Tone Editor 'N' is an FM tone editing system
for YM2151 (OPM) that runs on Human68k on the X68000.

It is designed to make FM tone editing intuitive,
responsive, and efficient.

---

## Overview

With OPM Tone Editor 'N',
you can edit tones while listening to the sound in real time.

![OPM Tone Editor 'N' Screenshot](docs/images/ui_01_toneeditor_main_screenshot.png)

---

## Key Features

- **Edit YM2151 (OPM) parameters directly**
- **Simultaneous editing of two files and two tone data sets**
- **Access up to 200 tones without switching contexts**
- **Use template slots (T01–T08) for frequently used tones**
- **Preview sounds as you edit**
- **Protect tones with edit lock**
- **Mark tones for export using selection markers**

---

## Editing Experience

Parameter changes are applied immediately.

You can hear the result as you edit,\
making it easy to understand how each parameter affects the sound.

---

## Interface

Editing operations are mapped to the most suitable input method:

- parameter editing → keyboard  
- selection and control → mouse  

This allows you to edit continuously
without losing focus.

---

## System Structure

Tone Editor focuses on editing,\
while sound playback is handled by a separate module (**scalekey**).

This separation allows:

- stable real-time playback  
- responsive editing  
- flexible extension of performance features  

---

## Supported Formats

Input:

- OED, MML, OPM, SND, MDX, ZMS

Output:

- OED, MML, OPM, SND

---

## Documentation

Design rationale is available in:

- `docs/00_context/rationale/`

This explains how the system is structured.

---

## Status

Implementation and related documentation will be published\
after the v1.20 release.

---

## Summary

Tone Editor provides a focused environment\
for shaping FM tones through direct interaction,\
real-time feedback, and reusable tone data.
