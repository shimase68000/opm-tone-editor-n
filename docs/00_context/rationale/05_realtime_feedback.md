# Real-Time Feedback

All parameter changes in Tone Editor are applied immediately
to the YM2151 (OPM).

---

## Execution Flow

input → parameter update → opmset() → sound change

There is no delayed update or batch processing.

---

## Principle

> editing produces sound immediately

---

## Direct Mapping

Each numeric change directly affects the audible result.

- changing a value changes the sound instantly
- no intermediate confirmation step exists

---

## Why This Matters

FM synthesis is not inherently intuitive.

- parameter meanings are complex
- interactions are non-linear

Understanding is built through:

- direct manipulation
- immediate feedback
- repetition

---

## Design Goal

Tone Editor treats sound as part of the UI.

The user does not:

- edit first, then play

Instead:

- editing is playing

---

## No Mode Separation

There is no separation between:

- editing mode
- audition mode

Both are unified.

---

## Implication

- faster experimentation
- stronger intuition building
- continuous workflow

---

## Relation to Other Systems

This behavior is tightly coupled with:

- direct edit model
- numeric input model
- flag safety system

---

## Summary

Sound is treated as immediate feedback,
not as a delayed result.
