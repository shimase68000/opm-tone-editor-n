## Relationship Between OPM Tone Editor 'N' and OED

OED is the native storage format of OPM Tone Editor 'N'.\
However, OED was not designed first.

The initial goal was simply to build Tone Editor itself.

As the design of Tone Editor progressed,\
the OED format was defined as a way to store editing results.

During this process, the concept of OED gradually expanded,\
particularly as a tone library format.

As a result, there were cases where Tone Editor design\
was influenced by ideas originating from OED.

For example, header fields such as:

* author
* target device
* environment
* datetime

were incorporated into Tone Editor after being introduced in OED.

This means that Tone Editor and OED were not designed independently,\
but evolved through mutual interaction.

In other words:

* OED can be seen as a formalization of Tone Editor functionality
* Tone Editor can be seen as a UI representation of the OED format

These two perspectives coexist and define the relationship between them.
