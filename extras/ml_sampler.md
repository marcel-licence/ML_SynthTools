# Sampler
This synthesizer module is made to playback samples from RAM or FLASH memory.
It allows to manipulate the playback by changing the pitch, volume and tweak parameters like ADSR (attack, decay, sustain, release).
In addition to that it is possible to loop samples.

To use the **sampler** you need to include ml_sampler.h into your project:
```cpp
#include <ml_sampler.h>

```

The sampler requires an initialization by calling its init routine for example from the setup():
```cpp
#include <ml_sampler.h>

#define SAMPLE_RATE 44100

void setup(void)
{
  Sampler_Init(SAMPLE_RATE);
  /*
   * some other setup routines might be called here
   */
}
```


## playing samples from FLASH
### preparing samples
Samples can be played directly from FLASH when they are compiled into the program code.
It is possible to convert wav-files into c files into a data array.
For example prepare a 16bit mono wav-file and store it as test.wav.
[Audacity](https://www.audacity.de/) can be used for this conversation
![image](https://github.com/user-attachments/assets/7cbfe52e-24f7-45c9-a8a6-a54b0d1aa272)

Then you can use [Bin2c](https://www.segger.com/free-utilities/bin2c/) to convert it to a c-file.
You will find an array in the c-file:
```cpp
static const unsigned char _actest[55164UL + 1] = {
```
which can be used as a sample in the sampler by loading it:
```cpp
#include <ml_sampler.h>

#define SAMPLE_RATE 44100

void setup(void)
{
  Sampler_Init(SAMPLE_RATE);
  Sampler_NewSampleStatic(_actest, sizeof(_actest));
  /*
   * some other setup routines might be called here
   */
}
```
Now the sampler is ready to playback the loaded sample.
