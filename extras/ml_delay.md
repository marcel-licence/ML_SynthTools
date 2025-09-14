# Delay
### A very simple delay module

This module provides a **mono** or **stereo delay effect**.  
It supports different audio formats (`int16_t` and `float`) and multiple input/output modes.

---

## Initialization

The delay module requires a buffer (or two for stereo). The buffer length determines the **maximum delay length**.

- For **mono**: `void Delay_Init(int16_t *buffer, uint32_t len);`

- For **stereo**: `void Delay_Init2(int16_t *left, int16_t *right, uint32_t len);`

- `buffer`, `left`, `right`: pointers to delay line buffers.  
- `len`: number of samples per buffer. This defines the maximum delay length.  
- Maximum delay in time = `len / SAMPLE_RATE` seconds.  

To reset the delay state:
```c
void Delay_Reset(void);
```

---

## Processing

The delay supports multiple processing functions depending on the input/output format and mode.  

### Mono
```c
// Mono in/out
void Delay_Process_Buff(float *signal_l, int buffLen);
void Delay_Process_Buff(int16_t *signal_l, int buffLen);
```

### Mono → Stereo
```c
// Mono input, stereo output
void Delay_Process_Buff(float *in, float *left, float *right, int buffLen);
void Delay_Process_Buff(int16_t *in, int16_t *left, int16_t *right, int buffLen);
```

### Stereo
```c
// Stereo input/output
void Delay_Process_Buff(float *in_l, float *in_r, float *out_l, float *out_r, int buffLen);
void Delay_Process_Buff(int16_t *in_l, int16_t *in_r, int16_t *out_l, int16_t *out_r, int buffLen);
void Delay_Process_Buff2(float *signal_l, float *signal_r, int buffLen); // in/out inplace
```

- `buffLen`: number of samples to process per call.  
- Use the function variant matching your signal format (`float` or `int16_t`).  

---

## Parameters

The module provides several parameter setters.  

### Input Level
```c
void Delay_SetInputLevel(uint8_t unused, float value);
void Delay_SetInputLevel(uint8_t unused, uint8_t value);
```

- `float`: range **0.0 … 1.0** (linear gain).  
- `uint8_t`: range **0 … 127** (MIDI-style scaling).  

### Feedback
```c
void Delay_SetFeedback(uint8_t unused, float value);
void Delay_SetFeedback(uint8_t unused, uint8_t value);
```

- Controls how much delayed signal is fed back into the input.  
- Same ranges as above.

### Output Level
```c
void Delay_SetOutputLevel(uint8_t unused, float value);
void Delay_SetOutputLevel(uint8_t unused, uint8_t value);
```

### Delay Length
```c
void Delay_SetLength(uint8_t unused, float value);
void Delay_SetLength(uint8_t unused, uint32_t value);
```

- `float`: **0.0 … 1.0** → percentage of max delay (`len`).  
- `uint32_t`: absolute delay length in samples.  

### Stereo Shift
```c
void Delay_SetShift(uint8_t unused, float value);
```

- Used only in **stereo mode**.  
- Shifts one channel relative to the other.  
- Range: **0.0 … 1.0**.

---

## Examples

### Mono Delay
```c
#define BUFFER_LEN 48000
static int16_t delayBuffer[BUFFER_LEN];

Delay_Init(delayBuffer, BUFFER_LEN);
Delay_SetFeedback(0, 0.5f);       // 50% feedback
Delay_SetLength(0, 0.25f);        // 25% of buffer length
Delay_SetOutputLevel(0, 1.0f);    // full output

Delay_Process_Buff(inputBuffer, 256);
```

### Stereo Delay
```c
#define BUFFER_LEN 48000
static int16_t delayBufferL[BUFFER_LEN];
static int16_t delayBufferR[BUFFER_LEN];

Delay_Init2(delayBufferL, delayBufferR, BUFFER_LEN);
Delay_SetFeedback(0, 64);         // MIDI style (64/127 ~ 0.5)
Delay_SetLength(0, 24000);        // 24000 samples delay
Delay_SetShift(0, 0.1f);          // slight stereo shift

Delay_Process_Buff(inputL, inputR, outputL, outputR, 256);
```
