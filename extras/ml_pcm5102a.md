# PCM5102A Breakout Board

This breakout board utilizes the PCM5102A DAC (Digital-to-Analog Converter) to deliver high-quality audio output from digital I2S input. It is particularly well-suited for projects requiring high-resolution audio, providing low-distortion and minimal phase noise. Below, you’ll find an overview of the PCM5102A's features, essential pin configurations, and connection guidelines for stable operation.

---

## Features of the PCM5102A

- **High-Performance Audio DAC**: 24-bit/192kHz PCM audio playback.
- **Low Distortion and High SNR**: Achieves a signal-to-noise ratio of 112 dB.
- **Advanced Filtering**: Built-in digital filters for minimizing audio distortion.
- **Flexible Audio Formats**: Supports I2S and other serial audio formats.
- **Pop-Noise Free Operation**: Integrated design reduces unwanted audio artifacts.

---

## Signal and Pin Overview

| Signal    | Function                                                                                      | Suggested Connection |
|-----------|-----------------------------------------------------------------------------------------------|----------------------|
| **VIN**   | Power input. Connect to 3.3V to power the PCM5102A.                                           | 3.3V                 |
| **GND**   | Ground connection                                                                             | Ground               |
| **LRCK**  | Left-Right Clock, also known as Word Clock (WCK). Synchronizes left and right audio channels. | Required for I2S     |
| **DIN**   | Digital Input, receives serial data                                                           | Required for I2S     |
| **BCK**   | Bit Clock, clocks each data bit                                                               | Required for I2S     |
| **SCK**   | System clock (if unused, connect to ground)                                                   | Ground or use bridge |
| **FLT**   | Filter setting                                                                                | Ground or bridge 1L  |
| **DEMP**  | De-emphasis control                                                                           | Ground or bridge 2L  |
| **XSMT**  | Mute control, high to enable audio output                                                     | High or bridge H3    |
| **FMT**   | Format selection, low for I2S audio format                                                    | Ground or bridge 4L  |

![grafik](https://github.com/user-attachments/assets/2399d381-b9fa-41b5-81d0-ce29f8ba2564)

![grafik](https://github.com/user-attachments/assets/e732754c-4a8e-4f71-9a3a-a6e7144a7ef8)

---

## Connecting the PCM5102A for Stable Operation

For a quick and stable setup of the PCM5102A breakout board:

1. **Power Supply (VIN and GND)**:  
   - Connect **VIN** to 3.3V to power the breakout board and **GND** to the ground of your power source.

2. **System Clock (SCK)**:  
   - This clock can either be connected to a low signal or, if unused, you can bridge the solder pads marked on the top of the board to ground SCK. This simplifies setup for cases where an external clock isn’t required.
   
3. **Connecting Control Pins**:
   - For the most stable operation, it’s recommended to ground **FLT**, **DEMP**, and **FMT**.  
   - **XSMT** should be held high to unmute the DAC and allow audio output.

4. **Bottom Solder Pads (H1L, H2L, H3L, H4L)**:  
   These pads allow flexible control by connecting the following pins directly to either high or low signals:
   - **H1L** - Controls **FLT**
   - **H2L** - Controls **DEMP**
   - **H3L** - Controls **XSMT**
   - **H4L** - Controls **FMT**

Refer to the picture below for these solder pad markings and pin locations on the board.

---

With these setup instructions, your PCM5102A DAC should be properly configured for high-quality audio playback in any project requiring precise and stable DAC operation.
