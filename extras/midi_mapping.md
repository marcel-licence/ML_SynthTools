# MIDI Mapping

## Introduction

Most of my audio projects use a static MIDI map. Each project supports a wide range of features, and to control them, you need to assign MIDI controllers to specific functions. The defined structure for mapping is usually found in the `z_config.ino` file of the respective project.

A typical mapping might look like this:

```c
struct midiControllerMapping edirolMapping[] = {
    /* MIDI defaults */
    { ALL_ORGAN_CHANNELS, 7, "Volume", NULL, App_MainVolume, 0 },
    { 0x0, 91, "Reverb", NULL, Reverb_SetLevelInt, 8 },
    { 0x0, 93, "Chorus", NULL, Organ_SetLeslieSpeedNorm, 0 },
};
```

## Details of MIDI Mapping

To understand the mapping function, let’s take a closer look at the `struct midiControllerMapping`. This structure contains the following elements:

```c
struct midiControllerMapping {
    uint8_t channel;
    uint8_t data1;
    const char *desc;
    void (*callback_mid)(uint8_t ch, uint8_t data1, uint8_t data2);
#ifdef MIDI_FMT_INT
    void (*callback_val)(uint8_t userdata, uint8_t value);
#else
    void (*callback_val)(uint8_t userdata, float value);
#endif
    uint8_t user_data;
};
```

### MIDI channel number `channel`

This field selects the channel on which the message is received. It can be a number from 0 to 15 (for channels 1 to 16) or a predefined mask (with current limitations).

To link a function to all four channels, you can use a define like this:

```c
#define ALL_ORGAN_CHANNELS  (MIDI_CHANNEL_MASK | MIDI_CHANNEL_0 | MIDI_CHANNEL_1 | MIDI_CHANNEL_2)
```

### Controller number (`data1`)

The `data1` field refers to the controller number. Each knob or slider on your MIDI controller is assigned a specific controller number, which this field uses to identify the corresponding control.

Standard MIDI controller numbers include:
- 1: Modulation
- 4: Foot Pedal
- 7: Volume
- 64: Sustain Pedal
- 91: Reverb
- 93: Chorus

However, not all projects support the full General MIDI (GM) or General Standard (GS) specifications.

### Description `desc`

This field is for a text description to make identification easier. For example, you can include the names of the controllers. Currently, this is not actively used but serves as a reference.

### Callback Functions

Callback functions are invoked when the channel and controller number of a received message match the mapping. There are two types of callbacks:

#### Short callback `callback_mid`

This function receives the complete MIDI short message and is called when the channel and controller number match the received MIDI message.

#### Value callback `callback_val`

This function provides `user_data` (an identifier) and the value of the received control change message.

### Additional data `user_data`

The `user_data` field allows you to pass a custom identifier to the callback function. This is useful when using the same callback for different control change messages, as it helps differentiate between them. It also simplifies remapping because the `user_data` remains unchanged if you modify the `controller_number`.

## Examples

### Example 1

A mapping with a single entry:

```c
struct midiControllerMapping edirolMapping[] = {
    { 0x0, 0x40, "sustain", NULL, Organ_SetLeslieSpeedNorm, 0 },
};
```

When receiving a short message `0xC0 0x40 <value>`, the function `Organ_SetLeslieSpeedNorm` will be called as follows:

```c
Organ_SetLeslieSpeedNorm(0, <value>);
```

Values received on channel 0 for control change number `0x40` (64) will trigger this function.

### Example 2

Using a defined mask:

```c
#define ALL_ORGAN_CHANNELS  (MIDI_CHANNEL_MASK | MIDI_CHANNEL_0 | MIDI_CHANNEL_1 | MIDI_CHANNEL_2)

struct midiControllerMapping edirolMapping[] = {
    { ALL_ORGAN_CHANNELS, 0x01, "modulation", NULL, App_Modulation, 0 },
};
```

In this case, control number 1 received on channels 0 to 2 will call `App_Modulation`.

### Example 3

A mapping with multiple entries:

```c
struct midiControllerMapping edirolMapping[] = {
    { 0x00, 0x20, "C1", NULL, App_Param, 0 },
    { 0x00, 0x21, "C2", NULL, App_Param, 1 },
    { 0x00, 0x22, "C3", NULL, App_Param, 2 },
    { 0x00, 0x23, "C4", NULL, App_Param, 3 },
};
```

Here, `App_Param` will be called for control change numbers `0x20` to `0x23` on channel 0. The `user_data` distinguishes the messages:

- `user_data = 0` for `0x20`
- `user_data = 1` for `0x21`
- …

The `App_Param` function should be defined as:

```c
void App_Param(uint8_t userdata, float value);
```

## MIDI Float or Integer

The system can be configured to use either integer or float values via a define:

```c
#define MIDI_FMT_INT
```

If `MIDI_FMT_INT` is defined, the value is passed as a `uint8_t`, representing the raw MIDI value (0–127). If not defined, the value is converted to a float internally and passed as a normalized value (0.0 to 1.0).


## Setting Up Your Own MIDI Map

To set up your own MIDI map, follow these steps:

1. Add the following line to your `config.c` file:
    ```c
    #define MIDI_MONITOR_ENABLED
    ```

2. Upload the new firmware to your device. You should now see messages when your MIDI device sends control change messages. For example:

    **Knob 1:**
    ```
    2 | b1 5b 00 | Control Change   | Channel  1 | Number    91 | Value      0
    ```

    **Knob 2:**
    ```
    2 | b0 5d 11 | Control Change   | Channel  0 | Number    93 | Value     17
    ```

    **Knob 3:**
    ```
    2 | b0 05 02 | Control Change   | Channel  0 | Number     5 | Value      2
    ```

3. Use these messages to create your own mapping. Enter the channel (first value), the number (second value), and give it a name to connect it to a specific function. For example:

    ```c
    { 0x01, 0x5B, "Knob 1", NULL, Organ_SetDrawbarInv, 0 },
    { 0x00, 0x5D, "Knob 2", NULL, Organ_SetDrawbarInv, 1 },
    { 0x00, 0x05, "Knob 3", NULL, Organ_SetDrawbarInv, 2 },
    ```