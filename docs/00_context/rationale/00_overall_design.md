# Overall Design

OPM Tone Editor 'N' is designed as a system for editing FM tones
through continuous interaction with sound and data.

The system integrates editing, data handling,
and real-time feedback into a unified workflow.

---

## Core Concept

OPM Tone Editor 'N' is based on the following definition:

- tone data is treated as reusable assets
- editing is performed as iterative refinement
- interaction is continuous and immediate

---

## Editing Model

Editing is performed directly on tone data.

There is no intermediate buffer.

Parameter changes are applied immediately
and reflected in sound output.

---

## Data Model

Tone data is treated as a library.

All tones are reusable and can serve as templates
for further refinement.

Multiple tone sources can be accessed
and used within the same session.

---

## Context Model

Editing is performed across multiple contexts.

The system provides:

- dual tone files (dual tone lists + template slots)
- dual parameter tables

These contexts can be used simultaneously
to support comparison and refinement.

---

## Interaction Model

Interaction is structured by assigning roles
to interface elements.

- parameter editing → keyboard
- selection and control → mouse

Each operation is mapped to an appropriate input method.

---

## Interface Model

The interface is constructed using functional regions.

Each region has a defined purpose:

- tone selection
- parameter editing
- status display
- control operations

The layout prioritizes editing tasks.

---

## Resource Model

The system avoids unnecessary use of resources.

The interface is constructed using the text plane,
and the graphics plane is not used for normal operation.

Only the elements necessary for editing are implemented.

---

## Safety Model

Editing is protected through multiple mechanisms:

- lock → prevents unintended modification
- limited undo → recovers specific operations
- backup → protects against system-level failures

Each mechanism addresses a different type of risk.

---

## System Structure

Editing and performance are handled separately.

- Tone Editor → editing system
- scalekey → performance system

Each system operates independently
with defined responsibilities.

---

## Design Approach

The system is defined by a set of structural decisions:

- responsibilities are separated
- unnecessary operations are avoided
- interaction flow is preserved
- components are kept independent

These decisions are reflected across all parts of the system.

---

## Summary

OPM Tone Editor 'N' is designed as a structured editing system
based on reusable tone data,
continuous interaction,
and clearly separated responsibilities.