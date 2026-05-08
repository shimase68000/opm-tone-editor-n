# Rationale — OPM Tone Editor 'N'

## Starting Point

The motivation was simple:

> build a practical FM tone editor for OPM.

Not a demonstration tool. Not a reference implementation.\
Something that could actually be used for editing tones —\
efficiently, without friction, in the way an experienced user would work.

---

## Designing for OPM Users

OPM Tone Editor is designed for the user who already understands OPM.

This is a deliberate choice, not a limitation.

Parameter values in Tone Editor correspond directly to OPM hardware register values.\
There is no abstraction layer that translates values into "friendlier" representations.

The reason is simple:\
a user who understands OPM does not need the values reinterpreted.\
Abstraction would only add indirection between the user's intention\
and what the hardware actually does.

> The tool should stay out of the way of the user's understanding.

---

## Beyond Single-Tone Editing

Most tone editors focus on one tone at a time.\
The workflow is: edit a tone, listen, save, move on.

Tone Editor is designed differently.

The editing environment includes awareness of how tones are actually used:

- which OPM channel the tone is assigned to
- what volume it plays at relative to other channels
- how it sounds alongside other tones in the same session

This means the editor reflects the context in which tones exist —\
not just their internal parameters in isolation.

Editing two tones in parallel, comparing them directly,\
copying parameters between them, hearing both together:\
these are not added features. They follow from the question\
of what tone editing actually requires.

### OPM Register Responsibility

This context-awareness is grounded in a clear division of hardware responsibility.

OPM Tone Editor and scalekey each own distinct regions of the OPM register space:

- **OPM Tone Editor** controls tone parameters —\
  operator settings, algorithm, feedback, LFO, pan.
- **scalekey** controls note and playback —\
  key on/off and note/octave.

Neither process writes to the other's register domain.\
This is not a protocol or lock — it is a design agreement.

The result is that tone parameters can be edited freely\
while notes are playing, without conflict.\
The hardware boundary makes independent development possible.

---

## An Editor for Tones

One influence on the design was the way text editors handle their subject matter.

In an editor like vim, characters, words, and lines\
are all treated with a consistent set of operations.\
Copy, delete, protect, navigate — the same concepts apply at every level.

The same idea can apply to tone data.

A tone editor is an editor.\
The operations that make text editors powerful —\
copy, lock, mark, annotate, template —\
should be equally available for tones.

This is why:

- tones can be locked to prevent accidental modification
- tones can be marked for selective export
- every tone has a memo field, not as an optional feature
  but as a first-class part of the data
- template slots exist as a reusable reference layer

These are not tone-specific inventions.\
They are editor concepts applied to tone data.

---

## Input Responsibility

Parameter editing is handled by keyboard.\
Selection, channel assignment, and volume control are handled by mouse.

This division exists for a specific reason:

> when editing parameters, the cursor never needs to leave the parameter table.

A user editing a parameter with the keyboard does not need to reach for the mouse.\
A user selecting a tone with the mouse does not disturb the keyboard editing state.

The result is that during parameter editing,\
attention stays on the parameter table —\
and on the sound being produced.

There is no need to hunt for a cursor.\
There is no interruption between adjusting a value and hearing the result.

---

## Toward an FM Tone Workstation

OPM Tone Editor began as an editor.

It was intended to be a practical, efficient environment\
for editing OPM tones — better than what existed,\
shaped by the actual requirements of the task.

Over time, as features were added in response to real usage,\
something changed.

The tool now handles:

- simultaneous editing of two files
- real-time playback via an independent module
- MIDI input
- channel assignment policies
- template tone management
- format conversion across multiple standards

At some point, "editor" stopped being the most accurate description.

The term that fits better is **an FM tone workstation** —\
an environment for FM sound design as a complete activity,\
not just a tool for adjusting parameters in isolation.

This was not planned.\
It emerged from taking the question seriously:\
*what does a tone editor actually need to do?*

---

## The Ecosystem

OPM Tone Editor is one part of a larger structure.

As the design evolved, it became clear that\
the responsibilities of tone editing, tone storage,\
topology description, and real-time playback\
belong in separate, independent components:

- **Tone Editor** — editing environment (master)
- **OED** — tone library format
- **OTG** — operator topology notation
- **scalekey** — OPM playback engine (slave)

scalekey is designed as a common slave engine.\
The master can change — Tone Editor today,\
a Motif Editor tomorrow, other tools in the future —\
and scalekey remains the shared interface to OPM hardware.

This was not a concrete plan when scalekey was separated from Tone Editor.\
It was a decision not to close off options\
that a future master might need.

When phrase recording is added to scalekey,\
a new format will be needed for that data.\
Its role in the ecosystem mirrors OED's role for tone data:

- **OED** treats tone data as a primary resource
- **MED (Motif Editor Data)** treats phrase data as a primary resource

The same philosophy, applied one level up.

---

## Thirty Years of Accumulated Thought

Version 0.07 was shelved in 1996.

But the ideas did not stop.

Through the years that followed,\
possibilities accumulated —\
features that would have made sense,\
interactions that would have been useful,\
concepts that were almost right but not yet fully formed.

When development resumed in 2023,\
those accumulated ideas shaped what became version 1.00.

The gap was not empty time.\
It was the interval in which the design matured\
beyond what could have been built in 1996.

---

## Summary

OPM Tone Editor is built on a small number of principles:

- match the user's understanding of OPM, not replace it
- treat tones with the same seriousness that text editors treat text
- eliminate unnecessary interruption from the editing flow
- reflect the context in which tones actually exist

What it has become is larger than what it was designed to be.

That is not a problem to be solved.\
It is a sign that the original question was worth asking.

The design does not predict the future.\
It avoids closing off options that the future might need.\
That instinct — present from the very first version —\
is what allowed a shelved prototype from 1994\
to grow into an FM tone workstation in 2025.

---

## On the X68000 Phase

When development resumed in 2023,\
the goal was not simply to finish what had been left incomplete in 1996.

The X68000 version was conceived from the start as a test case —\
a working implementation on constrained hardware\
that would validate the design before it moved to a wider platform.

The X68000 imposes real constraints:\
limited memory, a single-tasking OS, direct hardware access,\
assembly language where performance matters.\
Building a complete FM tone workstation under these conditions\
is not a compromise. It is a proof.

If the ideas hold up on the X68000,\
they will hold up anywhere.

Version 1.20 is intended as the close of this phase.\
Not because the X68000 no longer matters,\
but because what needed to be validated on this platform\
will have been validated.

The ecosystem — OPM Tone Editor, scalekey, OED, OTG —\
will continue on Windows,\
carrying the same philosophy\
that was proven on the platform where it began.
