# Resource Philosophy

Tone Editor is designed to minimize unnecessary resource usage.

The system avoids using the graphics plane
when it is not required for tone editing.

---

## Core Principle

> avoid unnecessary resource usage

---

## Text-Based Interface

The interface is constructed using the text plane only.

Bitmap graphics are not used unless required.

---

## Design Approach

UI elements are expressed using simple structures:

- text
- layout
- color attributes

This provides a consistent and lightweight interface.

---

## Why This Approach

Tone editing requires:

- precise parameter control
- continuous interaction
- stable and responsive behavior

A lightweight interface supports these requirements
by reducing unnecessary system overhead.

---

## Resource Usage

The system avoids:

- complex rendering
- unnecessary visual effects
- processes that waste resources
- unnecessary interrupts (especially timer interrupts)

Only the elements required for editing are implemented.

---

## Relation to Interface

The interface is structured to maximize usable space.

More screen area is allocated to:

- tone data
- parameter tables
- tone lists

This supports editing tasks directly.

---

## Relation to Editing Model

A simplified interface reduces:

- visual noise
- interaction overhead

This allows the user to focus on:

- parameter changes
- sound feedback

---

## Implication

The system maintains:

- predictable performance
- low overhead
- consistent interaction

---

## Summary

Tone Editor is designed to avoid unnecessary use of resources.

The interface includes only what is necessary
to support tone editing.