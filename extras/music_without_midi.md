# Playing Notes Without a Hardware MIDI Interface

This document explains how to play notes in a library without relying on an external MIDI interface or receiving MIDI notes via a serial interface. All projects/modules are designed to function without additional hardware and use an interface defined in `z_config.ino`.

## Default Setup
Each project typically uses an interface that is configured in `z_config.ino`. The configuration looks like this:

```
struct midiMapping_s midiMapping =
{
    NULL,
    Organ_NoteOn,
    Organ_NoteOff,
    [...]
};
```

or 

```
struct midiMapping_s midiMapping =
{
    NULL,
    Sampler_NoteOn,
    Sampler_NoteOff,
    [...]
};
```

These lines indicate that the `Organ_NoteOn` and `Organ_NoteOff` functions are used to handle note-on and note-off events for an organ. Similarly, `Sampler_NoteOn` and `Sampler_NoteOff` are used for a sampler module.

## Manual Function Calls
To manually call these functions, you can find their declarations in the specific header files. For example:

### Organ
```
void Organ_NoteOn(uint8_t ch __attribute__((unused)), uint8_t note, uint8_t vel __attribute__((unused)));
void Organ_NoteOff(uint8_t ch __attribute__((unused)), uint8_t note);
```

### Sampler
```
void Sampler_NoteOn(uint8_t ch, uint8_t note, uint8_t vel);
void Sampler_NoteOff(uint8_t ch, uint8_t note, uint8_t vel __attribute__((unused)));
```

You can then play notes programmatically by calling these functions. For example:

### Playing Notes
To turn a note on:
```
Organ_NoteOn(0, 64, 127);
```

To turn the note off:
```
Organ_NoteOff(0, 64);
```

In this example:
- **Channel:** `0` (corresponds to Channel 1 in MIDI terms).
- **Note:** `64` (E4; see the [MIDI Note Numbers Reference](https://inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies)).
- **Velocity:** `127` (maximum volume).

For some instruments (e.g., an organ), the velocity (`vel`) may be ignored. The `__attribute__((unused))` annotation indicates that a parameter is not used by the function in such cases.

## Timing Limitations
### Avoiding `delay(...)`
Using `delay(...)` in the `loop` function is not recommended, as it will interfere with sound processing and cause distortion. Instead, implement precise timing mechanisms.

### Timing Calculations
To manage timing, calculate the elapsed time per loop cycle using this formula:

```
Elapsed Time = SAMPLE_BUFFER_SIZE / SAMPLE_RATE
```

If a precise loop delay is required (e.g., for 1 Hz timing), consider using a counter to handle timing instead of relying on `delay(...)`.

---

This guide should help you effectively use the library to play notes programmatically without a hardware MIDI interface. Let me know if further clarification is needed!
