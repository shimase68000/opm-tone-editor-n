# Direct Edit Model

Tone Editor does not use intermediate edit buffers.

All parameter changes are applied directly to tone storage.

---

## Model

table[0] -> Tone[tan[0]][ton[0]]
table[1] -> Tone[tan[1]][ton[1]]

tnsw selects the active table.

active table (tnsw = 0 or 1):

    table[tnsw] -> Tone[tan[tnsw]][ton[tnsw]]

---

## Principle

> no separation between view and data

---

## View as Binding

Parameter tables do not own data.

They act as views bound to tone slots.

Editing through a table modifies the bound tone directly.

---

## No Edit Buffer

Traditional editors use:

- edit buffer
- commit step

Tone Editor does not.

There is no staging area.

---

## Implication

- no synchronization issues
- no copy between buffer and storage
- always consistent state

---

## Trade-off

Direct editing increases risk.

Accidental changes immediately affect tone data.

---

## Compensation Mechanisms

To manage this risk, Tone Editor uses:

- LOCK  → prevents modification
- limited UNDO → recovers copy mistakes
- EDIT flag → tracks modification state

---

## Why This Model

This model simplifies both:

- implementation
- mental model

The user always edits the real data.

---

## Summary

Editing is direct, not staged.

The parameter table is a view,
not a buffer.
