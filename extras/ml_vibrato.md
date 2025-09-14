# Vibrato
### A simple vibrato effect

The `ML_Vibrato` class implements a vibrato effect using a circular buffer and modulation input.  
It supports standard and high-quality processing modes.

---

## Constants

```c
#define VIBRATO_BUFFER_SIZE    1024
```

- Size of the internal circular buffer used for delay modulation.  
- Determines maximum vibrato depth (in samples).  

---

## Class Interface

```c++
class ML_Vibrato
{
public:
    ML_Vibrato(float sample_rate);
    ~ML_Vibrato() {};

    void Process(const float *in, const float *mod_in, float *out, uint32_t count);
    void ProcessHQ(const float *in, const float *mod_in, float *out, uint32_t count);

    void setDepth(float depth);
    void setIntensity(float intensity);

private:
    void ModMultiplierUpdate();

    float sample_rate;
    float depth;
    float depthInv;
    float buffer[VIBRATO_BUFFER_SIZE];
    float mod_multiplier;
    float mod_multiplier_curr;
    int32_t inCnt;
};
```

---

## Constructor & Destructor

```c++
ML_Vibrato(float sample_rate);
~ML_Vibrato();
```

- `sample_rate`: system sampling rate in Hz.  
- Destructor does nothing special.  

---

## Processing

### Standard Processing
```c++
void Process(const float *in, const float *mod_in, float *out, uint32_t count);
```

- `in`: pointer to input buffer.  
- `mod_in`: pointer to modulation buffer (typically LFO).  
- `out`: pointer to output buffer.  
- `count`: number of samples to process.  

### High-Quality Processing
```c++
void ProcessHQ(const float *in, const float *mod_in, float *out, uint32_t count);
```

- Similar to `Process()` but uses a higher-quality interpolation method for smoother vibrato.  

---

## Parameters

### Depth
```c++
void setDepth(float depth);
```
- Sets vibrato depth in **samples**.  
- Maximum is limited by `VIBRATO_BUFFER_SIZE`.  

### Intensity
```c++
void setIntensity(float intensity);
```
- Controls modulation intensity (scaling applied to `mod_in`).  

---

## Private Helper

```c++
void ModMultiplierUpdate();
```
- Updates modulation multiplier values internally.  

---

## Example Usage

```c++
#include "ML_Vibrato.h"

#define BLOCK_SIZE 256

ML_Vibrato vibrato(48000.0f);

float in[BLOCK_SIZE];
float mod[BLOCK_SIZE];  // typically filled with LFO samples
float out[BLOCK_SIZE];

void audioCallback()
{
    // set parameters
    vibrato.setDepth(64.0f);       // depth in samples
    vibrato.setIntensity(0.8f);    // modulation intensity

    // process audio
    vibrato.Process(in, mod, out, BLOCK_SIZE);
}
```
