# Dual-Context Editing

Tone Editor supports editing across multiple contexts simultaneously.

The editor provides multiple views into tone data
and allows them to be used in parallel within a single session.

---

## Core Principle

> editing is performed with reference to multiple contexts

---

## Dual Context Model

Tone Editor provides two independent contexts:

- two tone files (left / right)
- two parameter tables (table 0 / table 1)

Each parameter table is bound to a tone slot in either of the two tone files.

Both tables can be active within the same session.

---

## Editing Behavior

Editing is performed while referencing other tones.

Typical operations include:

- comparing two tones
- adjusting parameters with reference to another tone
- transferring parameters between contexts

---

## Parameter Interaction

FM synthesis involves non-linear parameter interactions.

Small parameter changes can result in large differences in sound.

As a result, editing is often guided by relative comparison
rather than absolute parameter values.

---

## Relation to Library Model

This model extends the library-oriented approach.

Tone data is treated as a set of reusable assets,
and multiple assets can be accessed and referenced simultaneously.

---

## Relation to Interface

This model is reflected in the interface:

- left/right file panels
- dual parameter tables
- table focus switching

These elements provide access to multiple contexts
within a single workspace.

---

## Implication

The editor maintains multiple active references
during editing.

This enables:

- continuous comparison
- parameter adjustment with context awareness
- movement between tones without leaving the editing flow

---

## Summary

Tone Editor supports editing with multiple simultaneous contexts.

This structure allows tones to be compared,
referenced, and refined within a unified workspace.