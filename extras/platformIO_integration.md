# Integrating ML_SynthTools Library into PlatformIO for ESP32

To use the `ML_SynthTools` library in your ESP32 project with PlatformIO, follow these steps to configure your `platformio.ini` file.

## Steps to Integrate

1. **Open your `platformio.ini` file** located in the root directory of your PlatformIO project.

2. **Add the following configuration** under the `[env:your_env]` section, or create a new environment section if it doesn't exist:

```ini
   [env:esp32]
   platform = espressif32
   board = esp32dev
   framework = arduino
   lib_deps = 
     https://github.com/marcel-licence/ML_SynthTools
   build_flags = 
     -D ESP32
     -L $BUILD_DIR/../../libdeps/$BOARD/ML_SynthTools/src/esp32/
```

3. **Integrate inline sources** open your main.cpp or create a new file and integrate the following code:

```cpp
/* include function declarations from ML_SynthTools */
#define ML_SYNTH_INLINE_DECLARATION
#include <ml_inline.h>
#undef ML_SYNTH_INLINE_DECLARATION

/* include the inline definitions from ML_SynthTools */
#define ML_SYNTH_INLINE_DEFINITION
#include <ml_inline.h>
#undef ML_SYNTH_INLINE_DEFINITION
```