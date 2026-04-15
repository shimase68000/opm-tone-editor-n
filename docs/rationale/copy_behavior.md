## Copy Behavior

Tone Editor provides two types of copy operations.

The first is a standard copy using mouse operations.

The user selects a source tone with CTRL + click,\
and then selects a destination tone with click.

This performs a full copy of the tone data.

---

The second is copying between parameter tables.

This is done by clicking the arrows displayed between the tables:

* △△△
* ▽▽▽

In this case, only OPM parameters are copied.

Other elements such as:

* tone name
* tone memo
* mark
* flags

are not copied and remain unchanged in the original data

---

The two tables represent different tones,\
each treated as a separate entity.

Copying between tables is not intended to duplicate tone data,\
but to apply the OPM parameters of one tone to another.

---

Undo behavior differs between the two operations.

Undo is available for parameter table copy,\
but not for mouse-based full copy (in the current implementation).

This is because undo support is prioritized based on the difference in meaning\
between the two operations.