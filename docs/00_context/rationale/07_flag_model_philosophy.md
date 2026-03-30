# Flag Model Philosophy

Tone Editor uses a multi-layer flag system.

This system is designed to separate responsibilities clearly
between persistent state and runtime state.

---

## Two-Layer Model

The flag system consists of two distinct layers:

Persistent layer:

    Tone[tan][ton].mark

Runtime layer:

    toneflag[tan][ton]

These layers must never be unified.

---

## Responsibility Separation

Each flag has a clearly defined ownership.

LOCK
    persistent only

SELECT
    persistent only

EDIT
    runtime only

## Principle

> each state must belong to exactly one layer

---

## Persistence Control

Persistent state is stored in OED files.

Runtime state is never saved.

This ensures:

- reproducible state from file
- no leakage of temporary state
- clear distinction between saved data and session state

---

## Why Separation Matters

Mixing persistent and runtime state leads to:

- unpredictable behavior
- hidden side effects
- difficult debugging

By enforcing separation:

- behavior becomes deterministic
- state transitions become traceable

---

## Special Case: File Load

OED file load restores persistent state directly.

This is not treated as an edit operation.

## Principle

> restoration is not modification

---

## Direct Modification Rule

Flags should not be modified directly.

Instead, helper functions must be used.

Examples:

    flag_select_set()
    flag_lock_set_main()
    flag_edit_set_sub()

## Principle

> state transitions must be controlled

---

## Relation to Editing Model

The flag system compensates for the lack of full undo/redo.

Tone Editor relies on:

- LOCK for protection
- EDIT for tracking
- limited UNDO for recovery

This creates a lightweight but robust safety model.

---

## Design Summary

The flag system is designed to:

- separate persistent and runtime state
- assign clear ownership to each flag
- avoid hidden coupling
- maintain predictable behavior

---

## Core Principle

> do not merge what is conceptually separate
