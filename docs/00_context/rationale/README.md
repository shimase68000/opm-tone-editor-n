# OPM Tone Editor 'N' — Rationale

This document set defines the design structure of OPM Tone Editor 'N'.

It describes how the system is organized,
and how its components relate to each other.

---

## Overview

OPM Tone Editor 'N' is designed as a structured system for FM tone editing.

The system is defined by:

- reusable tone data
- iterative editing
- continuous interaction
- clearly separated responsibilities

These elements form a unified editing workflow.

---

## Repository Status

This repository currently contains design rationale only.

Implementation and related materials will be published
after the v1.20 release.

---

## Entry Point

The overall structure of the system is described here:

- [Overall Design](00_overall_design.md)

---

## Structure

The design is organized into the following aspects:

### Foundations

- [Documentation Philosophy](01_documentation_philosophy.md)
- [Interface Philosophy](02_interface_philosophy.md)

---

### Interaction

- [Input Device Model](03_input_device_model.md)
- [Numeric Input Model](04_numeric_input_model.md)
- [Real-Time Feedback](05_realtime_feedback.md)

---

### Editing Model

- [Direct Edit Model](06_direct_edit_model.md)
- [Flag Model Philosophy](07_flag_model_philosophy.md)

---

### Safety

- [Undo Philosophy](08_undo_philosophy.md)
- [Backup Philosophy](09_backup_philosophy.md)

---

### Data and Context

- [Library-Oriented Editing](11_library_model.md)
- [Dual-Context Editing](12_dual_context_editing.md)

---

### System Design

- [Resource Philosophy](13_resource_philosophy.md)
- [UI Responsibility Model](14_ui_responsibility_model.md)
- [Functional Separation](15_functional_separation.md)

---

## Reading Guide

The documents are independent but connected.

A typical reading order is:

1. Overall Design  
2. Data and Context  
3. Interaction  
4. Editing Model  
5. Safety  
6. System Design  

---

## Notes

These documents assume:

- familiarity with FM synthesis concepts  
- understanding of YM2151 (OPM) parameters  

---

## Summary

This document set defines the structure of Tone Editor.

It provides a consistent description of:

- how tone data is handled  
- how editing is performed  
- how the system is organized  

Each document describes one aspect of the system,
and together they form a complete design definition.