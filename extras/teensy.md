# ML_SynthTools Library with Teensy

This guide explains how to use the `ML_SynthTools` library with the Teensy board, including necessary modifications to ensure proper functionality. Follow the steps below to integrate and configure everything correctly.

## Required Changes to Teensy Setup

To use `ML_SynthTools` with the Teensy, there are a few modifications that need to be made to the Teensy core files. These changes adjust linker flags and audio settings for compatibility with your library.

### 1. Add Linker Flags to `platform.txt`

To configure the correct linker flags, you need to modify the `platform.txt` file for Teensy. This ensures that the necessary libraries are linked correctly.

- Locate the `platform.txt` file in the following directory:
  ```
  %appdata%\..\Local\Arduino15\packages\teensy\hardware\avr\1.59.0\platform.txt
  ```

- Open the file in a text editor, and add the following line under the section for `compiler.libraries.ldflags`:
  ```
  compiler.libraries.ldflags=
  ```

This will enable the necessary linker flags required by `ML_SynthTools`.

### 2. Modify Audio Stream Header for Block Size and Sample Rate

Next, you need to update some audio settings in the Teensy core to ensure compatibility with the `ML_SynthTools` library.

- Navigate to the following Teensy file:
  ```
  %appdata%\..\Local\Arduino15\packages\teensy\hardware\avr\1.59.0\cores\teensy4\AudioStream.h
  ```

- Open the file and locate the line:
  ``` c
  #define AUDIO_BLOCK_SAMPLES  128
  ```

- Change it to:
  ``` c
  #define AUDIO_BLOCK_SAMPLES  48
  ```

  This modification adjusts the number of audio samples in each audio block for better performance with the `ML_SynthTools` library.

- Next, find the line:
  ``` c
  #define AUDIO_SAMPLE_RATE_EXACT 44100.0f
  ```

- Change it to:
  ``` c
  #define AUDIO_SAMPLE_RATE_EXACT 48000.0f
  ```

  This sets the audio sample rate to 48 kHz, which is required for the correct operation of `ML_SynthTools`.

## Installation Steps

1. **Install Teensyduino:** If you haven't already, make sure to install [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html), which provides the necessary libraries and tools for programming the Teensy board.

2. **Install `ML_SynthTools` Library:**  
   - Download or clone the `ML_SynthTools` library to your Arduino libraries folder.
   - You can also install it via the Arduino Library Manager (if available).

3. **Configure the Teensy Board:**  
   - In the Arduino IDE, go to **Tools** > **Board** and select your Teensy model.
   - Set the correct COM port for the Teensy.

4. **Upload Your Code:**  
   - Once the above steps are completed, upload your code that uses the `ML_SynthTools` library to your Teensy board.

## Conclusion

After making these changes, you should be able to use the `ML_SynthTools` library with your Teensy board. If you encounter any issues, make sure all modifications are correctly applied and that the Teensy board is properly selected in the Arduino IDE.
