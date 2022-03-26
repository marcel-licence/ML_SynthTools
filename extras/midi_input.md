<h1 align="center">MIDI input schematics</h1>

There are different possibilities to make a little circuit for your MIDI input.
The circuit requires an opto-coupler and some resistors.

The input side of the opto-coupler connects to an internal LED. To limit the current a serial resistance of 220Ohms is necessary.
It can be achieved by one 220Ohm resistor or two resistors: 100Ohm + 120Ohm. I found different designs.
The diode is used for EMI protection. In case of a high reverse voltage it will flow through the external diode instead the internal diode of the opto-coupler.

The output is in most cases an open-collector circuit. To amplify the signal an internal transistor is used.
If a low current flows from base to emitter the transistor opens and allows a high current flow from collector to emitter.
An external pull-up resistor is used to keep the signal normally high (you can pull to 5V or 3.3V, it depends on your needs).
When the transistor opens the signal will be pulled to GND.

In some cases you can use the internal pull-up of your microcontroller. In that case you do not net the external resistor.

Some opto-coupler require an operating voltage of 5V and may also work with 3.3V. 

Please ensure that your opto-coupler does not drive 5V at the output to your 3.3V microcontroller. If your microcontroller does not have a 5V tolerant input it may destroy your RX pin.

Here are some examples:
<img src="extras/MIDI_IN_H11L1.png" /><br>
<img src="extras/MIDI_IN_6N138.png" /><br>
<img src="extras/MIDI_IN_6N137.png" /><br>

Please feel free to use the discussion area for questions, ideas etc.
https://github.com/marcel-licence/ML_SynthTools/discussions/12