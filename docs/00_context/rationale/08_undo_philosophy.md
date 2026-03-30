# Undo Philosophy

Tone Editor does not implement a full undo/redo system.

This is a deliberate design decision.

---

## Limited Undo Model

Undo is supported only for a specific operation:

    copying between OPM parameter tables

The system uses a single temporary buffer.

Only the most recent copy operation can be undone.

---

## Principle

> provide undo only where mistakes are costly

Not all operations require undo.

Tone Editor focuses on protecting high-impact operations.

---

## Why Full Undo Is Not Used

A full undo/redo system would require:

- history stacks
- large memory buffers
- complex state management

This would introduce:

- higher memory usage
- increased implementation complexity
- potential instability

These costs are not acceptable for the Human68k / X68000 environment.

---

## Design Strategy

Instead of a general undo system, Tone Editor uses:

- LOCK        → prevents accidental modification
- UNDO        → recovers from table copy mistakes
- EDIT state  → tracks runtime changes

These mechanisms work together to provide sufficient safety.

---

## Context-Sensitive Undo

Undo is valid only within a specific context:

- source table
- destination table
- tone slots involved

If this context changes, the undo buffer is discarded.

## Principle

> undo is tied to context, not history

---

## Why Copy Needs Undo

Copying between parameter tables is a high-impact operation.

- it overwrites existing data
- mistakes are easy to make
- recovery is otherwise difficult

Therefore, undo is provided specifically for this case.

---

## Why Other Operations Do Not

Other operations are:

- incremental
- reversible by further editing
- low risk

Examples:

- parameter adjustment
- volume change
- selection changes

These do not require undo.

---

## Relation to Direct Edit Model

Tone Editor uses a direct-edit model.

There is no intermediate buffer.

This increases the importance of safety mechanisms.

Undo complements:

- direct editing
- real-time updates

---

## Design Summary

The undo system is designed to be:

- minimal
- targeted
- context-aware

---

## Core Principle

> avoid general mechanisms when a focused solution is sufficient
