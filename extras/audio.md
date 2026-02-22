# Audio Codec Interface with Microcontroller

This project demonstrates how to set up and interface an audio codec with a microcontroller using I2S communication. It covers the necessary signal connections, configuration options, and a test signal mode for validation.

## Getting Started

### Overview of Audio Codec Signals

When interfacing an audio codec with a microcontroller, several key signals are involved:

- **BCLK (Bit Clock)**: This is the clock signal that synchronizes the data transfer between the microcontroller and the codec. It defines the speed at which bits are transferred.
- **WCLK (Word Clock) / LRCLK (Left-Right Clock)**: This signal specifies the sample rate and is used to separate left and right audio channels.
- **DIN (Data In)**: The data input pin on the codec, where audio data is received from the microcontroller.
- **DOUT (Data Out)**: The data output pin on the microcontroller, where audio data is sent to the codec.

The microcontroller must be configured to use I2S for these signals, ensuring the codec receives the audio data correctly.

### Configuration

The configuration options for this project are specified in `config.h`. Important parameters include:

- **`SAMPLE_RATE`**: Defines the audio sampling rate.
- **`SAMPLE_BUFFER_SIZE`**: Determines the buffer size for audio data. This impacts the generated test frequency if enabled.

To test your setup, you can enable a saw wave test signal by defining `OUTPUT_SAW_TEST` in `config.h`. This test signal will output a saw wave with a frequency calculated as:

```c
Frequency = SAMPLE_RATE / SAMPLE_BUFFER_SIZE
```

Example:
```c
#define OUTPUT_SAW_TEST  // Uncomment to enable the saw wave test signal
```

### Codec-Specific Considerations

Some codecs may require additional configuration:

- **I2S Configuration**: Most audio codecs can be configured over I2S. Ensure that your microcontroller's I2S settings (bit depth, sample rate, etc.) are compatible with the codec's specifications.
- **Additional Pins**: Certain codecs, such as the PCM5201a, may have additional pins like a *Mute* pin. Refer to the codec-specific documentation [PCM5102a](ml_pcm5102a.md) for details on these configurations.

### Running the Project

1. **Flash the Firmware**: Program the microcontroller with the provided firmware.
2. **Check Serial Monitor**: After starting the firmware, open the serial monitor. The monitor will display the pin settings for the I2S interface, allowing you to verify correct signal connections and configuration.

## Contributing and Support

For questions, feedback, or further assistance, feel free to start a discussion on the GitHub repository.

Happy coding!
