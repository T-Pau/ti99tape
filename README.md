This is ti99tape, a utility for creating TZX files for TI 99/4A.

The resulting files can be used on a MaxDuino or similar tape recorder emulator to load images on a real TI 99/4a.

To connect the TI 99/4a to the MaxDuino, build a cable with a female DB-9 connector on the one and and whatever your MaxDuino uses on the other, with the following connections:

- DB-9 pin 1 to Remote
- DB-9 pin 8 to Audio Out
- DB-9 pins 2 and 9 to Ground

This tools is currently in early development. Features are still missing, usage will change, and what's there might not work as expected.

Currently, it can read audio data of a TI 99/4A tape from a PCM encoded WAV file and outputs the corresponding TZX file on standard output.

It is written in C++17.

See the [INSTALL.md](INSTALL.md) file for installation instructions and dependencies.

The latest version can always be found there.  The official repository is at [github](https://github.com/T-Pau/ti99tape/).

If you want to reach the authors in private, use <ti99tape@tpau.group>.
