<h1 align="center">Connection of PCM5102a</h1>
<h3 align="center">ESP32</h3>  

The PCM5102a might have the following PINs which should be all connected to ensure that it works as expected:
<table>
<tr><th>PIN</th><th>Description</th><th>Connection</th></tr>
<tr><td>VCC</td><td>Power Supply</td><td>3V3 output of ESP32</td></tr>
<tr><td>3.3V</td><td>Power output</td><td>can be left open / connected to XMT</td></tr>
<tr><td>GND</td><td>Power Supply</td><td>GND of ESP32</td></tr>
<tr><td>FLT</td><td>Filter select : Normal latency (Low) / Low latency (High)</td><td>can be connected to GND</td></tr>
<tr><td>DMP</td><td>e-emphasis control for 44.1-kHz sampling rate(1): Off (Low) / On (High)</td><td>can be connected to GND</td></tr>
<tr><td>SCL</td><td>System clock input</td><td>can be connected to GND</td></tr>
<tr><td>BCK</td><td>Audio data bit clock input</td><td>Connect to pin number defined by I2S_BCLK_PIN</td></tr>
<tr><td>DIN</td><td>Audio data input</td><td>Connect to pin number defined by I2S_DOUT_PIN</td></tr>
<tr><td>LCK</td><td>Audio data word clock input</td><td>Connect to pin number defined by I2S_WCLK_PIN</td></tr>
<tr><td>FMT</td><td>Audio format selection : I2S (Low) / Left-justified (High)</td><td>Should be connected to GND</td></tr>
<tr><td>XMT</td><td>Soft mute control(1): Soft mute (Low) / soft un-mute (High)</td><td>Should be to connected 3.3V</td></tr>
</table>

Definitions of I2S_BCLK_PIN, I2S_DOUT_PIN, I2S_WCLK_PIN can be found in the selected board or could be placed in the config.h.
Please ensure that all defined PINs are capable being used as output. Some PINs of the ESP32 can be used as input only.
I2S_DOUT_PIN refers to the digital output of the ESP32 which shall be connected to DIN of the codec (like tx and rx).

In case ml_boards.h is included you would find there some define depended specific board include which might contain the defines of the I2S PINs.

To drive speakers or headphones an additional amplifier would be recommended.

To test the connection of the DAC I would recommend to enable the audio testmode which can be activated by defining: OUTPUT_SAW_TEST