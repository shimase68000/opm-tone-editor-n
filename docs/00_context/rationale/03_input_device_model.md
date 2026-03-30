# Input Device Model

Tone Editor uses both keyboard and mouse,
but their roles are strictly separated.

---

## Role Separation

- Keyboard:
    OPM parameter editing

- Mouse:
    selection and control operations

    - tone selection
    - OPM channel selection
    - volume control

---

## Non-Overlapping Design

Operations are not duplicated across input devices.

Each device has a clearly defined responsibility.

---

## Why Separation Matters

Mixing input roles leads to:

- loss of focus
- inconsistent interaction
- increased cognitive load

Tone Editor avoids this by assigning fixed roles.

---

## Focus Stability

Keyboard interaction is restricted to parameter editing.

This ensures:

- the cursor always stays within the parameter table
- the user does not need to search for focus
- editing flow remains uninterrupted

---

## Design Principle

> assign each input device a fixed role
> to preserve editing focus

---

## Editing Priority

OPM parameter editing is the primary operation.

All interaction design decisions prioritize:

- fast parameter access
- continuous editing
- minimal interruption

---

## Summary

Input devices are not interchangeable.

Each device has a defined purpose.

This separation improves:

- clarity
- speed
- usability
