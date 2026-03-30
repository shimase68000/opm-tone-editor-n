# Backup Philosophy

Tone Editor includes multiple safety mechanisms,
but they do not protect against all kinds of failure.

---

## Limits of Interactive Safety

Mechanisms such as:

- LOCK
- UNDO
- EDIT state

protect against editing mistakes and accidental operations.

However, they do not protect against forced termination.

Examples:

- tool hangup
- system crash
- abnormal shutdown

These failures cannot be fully prevented
by improving the editor alone.

---

## Why Backup Is Needed

Because some failures are external to the editing model,
a separate recovery mechanism is required.

This is the role of auto backup.

## Principle

> editing safety and crash recovery are different problems

They must not be treated as the same thing.

---

## Role of Auto Backup

The purpose of auto backup is to recover from states where:

- the editor stops unexpectedly
- the system is forcibly terminated
- in-memory changes are lost

This is a different layer of protection from undo.

Undo protects against user mistakes.

Backup protects against system-level loss.

---

## BED Format

As preparation for this feature,
a dedicated backup format named BED is being designed.

Its purpose is to support automatic recovery
without changing the role of existing file formats.

---

## Design Summary

Tone Editor safety is layered:

- LOCK / UNDO / EDIT
    protects editing operations

- auto backup / BED
    protects against forced termination

These mechanisms complement each other.

---

## Core Principle

> when one safety mechanism cannot solve a class of failures,
> introduce a separate mechanism with a separate responsibility
