# Numeric Input Model

Tone Editor uses a dual-layer numeric input system
designed for fast and precise parameter editing.

---

## Two Input Methods

- Direct numeric input
- Incremental input (ROLL UP / ROLL DOWN)

## Strategy

- Numeric input:
    fast access to target values

- Incremental input:
    fine adjustment and exploration

---

## Parameter Constraints

Each OPM parameter has its own valid range.

These limits are defined by register bit width
and vary per parameter.

Users are not expected to memorize these constraints.

---

## Design Approach

> accept input, then normalize

Invalid values are not rejected.

Instead, they are automatically corrected
into the valid range.

---

## Example

max = 7  
input "9" → 7  

max = 15  
input "9" → 9  
input "9" → 99 → 15  

---

## Input State Handling

Numeric input supports short sequences of digits.

Input behavior adapts based on:

- current value
- parameter limit

If further digit input would exceed the valid range,
the input state resets automatically.

---

## Design Principle

> do not interfere with user intent  
> but ensure valid results

---

## Why This Works

OPM parameters:

- have inconsistent ranges
- are frequently edited
- require rapid experimentation

Strict validation would:

- interrupt workflow
- increase cognitive load

Normalization removes this friction.

---

## Outcome

- no input errors
- smooth editing flow
- reduced need for memorization

---

## Summary

The numeric input system:

- absorbs invalid input
- enforces valid output
- preserves editing continuity
