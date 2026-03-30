# Functional Separation

Tone Editor separates editing and performance
into independent systems.

Editing and sound playback are handled by different components.

---

## Core Principle

> separate editing and performance

---

## System Structure

Tone Editor focuses on tone editing.

Performance functionality is provided by a separate system ("scalekey").

Each system operates independently.

---

## Editing System

Tone Editor is responsible for:

- parameter editing
- tone management
- data structure handling

---

## Performance System

The performance system ("scalekey") is responsible for:

- note input handling
- key assignment
- channel allocation
- real-time playback control

---

## Input Handling

The performance system accepts input from:

- PC keyboard
- MIDI devices

These inputs are processed independently from the editing system.

---

## Relation to Resource Philosophy

Separating functionality into independent modules
allows each component to operate within its own scope.

This enables:

- simpler debugging
- easier extension of functionality
- optimization at the module level

---

## System Relationship

The relationship between the editor and the performance system
is structured as follows:

- editor (master)
- performance system (slave)

The editor controls tone data and editing operations.

The performance system receives input and produces sound.

---

## Interface Independence

The performance system is designed to operate
with a defined interface.

This allows the editor component to be replaced
without changing the performance system.

For example:

- Tone Editor (editor) + scalekey (performance)
- other editor (editor) + scalekey (performance)

The roles remain consistent,
while the controlling component may vary.

---

## Relation to UI Responsibility Model

UI responsibility is separated within the editor.

Functional separation extends this concept
to the system level.

- editing responsibilities → Tone Editor
- performance responsibilities → scalekey

---

## Implication

Each system can be:

- developed independently
- extended independently
- optimized for its specific role

---

## Summary

Tone Editor separates editing and performance
into independent systems.

Each system is designed to handle its own role
without overlapping responsibilities.