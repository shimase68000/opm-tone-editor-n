# UI Responsibility Model

Tone Editor assigns user interface elements
based on the role of each editing operation.

Each editing function is mapped
to the interface element most suitable for it.

---

## Core Principle

> map each editing operation to an appropriate interface element

---

## Input Device Roles

Input devices have clearly defined responsibilities:

- keyboard:
    parameter editing

- mouse:
    selection and control operations

    - tone selection
    - channel selection
    - volume control

These roles do not overlap.

---

## Parameter Editing

Parameter editing is performed through the keyboard.

The editing focus remains within the parameter table.

---

## Selection and Control

Selection and control operations are performed through the mouse.

These operations do not interfere with parameter editing.

---

## Editing Focus Stability

Because input roles do not overlap,
the cursor does not leave the parameter table during editing.

This eliminates the need to relocate the cursor
and allows continuous focus on parameter editing.

---

## Table Responsibility

Each parameter table represents an editing context.

- table 0
- table 1

Each table is bound to a tone slot.

The active table is selected explicitly.

---

## Context Switching

Switching between parameter tables is performed explicitly.

This maintains a clear editing target at all times.

---

## Interface Regions

The interface is divided into functional regions:

- tone lists (left / right)
- parameter tables (upper / lower)
- template slots
- system message area
- control/menu area

Each region has a defined purpose.

---

## Layout Strategy

Screen space is allocated based on function.

More space is assigned to:

- tone data
- parameter editing

Less space is assigned to:

- control elements
- auxiliary information

---

## Relation to Resource Philosophy

Responsibility separation reduces unnecessary operations.

Each interaction is directed to a specific function,
avoiding redundant input paths.

---

## Relation to Editing Model

The editing flow is structured:

- select (mouse)
- edit (keyboard)
- hear result (real-time feedback)

Each step is handled by a different part of the interface.

---

## Implication

The interface maintains:

- stable focus
- predictable interaction
- reduced cognitive load

---

## Summary

Tone Editor assigns interface elements
based on the role of each editing operation.

This structure maintains a consistent editing flow
and allows uninterrupted focus on parameter editing.