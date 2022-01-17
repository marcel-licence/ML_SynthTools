<h1 align="center">ML_SynthTools</h1>
<h3 align="center">An Arduino synthesizer library</h3>  
<p align="center"> 
  <img src="extras/splash.jpg" alt="project picture" width="480px" height="270px"><br>
  <a href="https://youtu.be/9AHQ4mQrjE8">link to the video</a>
</p>

This library supports different platforms:
- ESP32
- ESP8266
- Seeedstudio XIAO (samd21 - cortex-m0plus)
- Teensy 4.1 (imxrt1062)
- Daisy Seed (cortex-m7)
- Raspberry Pi Pico (rp2040)
- STM32F407 (cortex-m4)

The organ example can be found here: https://github.com/marcel-licence/ml_synth_organ_example

All platforms are tested. Actually the sound quality might be a bit limited.
The organ supports full polyphony (you can play all 64 notes of the upper manual at the same time).
You can modify the sound using 9 drawbars.
Percussion is supported (2nd, 3rd).
A simple rotary implementation creates a simple (noisy) leslie like sound.

Demo using ESP32, ESP8266: https://youtu.be/c7TL8jcrnzs
Demo using the XIAO: https://youtu.be/2wT8nByoUNw
Demo using the Teensy4.1: https://youtu.be/H-NDCQnSDV0

More information will be available in future

# Modules of this library

The library contains the following modules:
- arpeggiator <a href="extras/ml_arp.md">more details</a>
- board pinout definitions <a href="extras/ml_boards.md">more details</a>
- a simple delay <a href="extras/ml_delay.md">more details</a>
- a simple reverb [more details](extras/ml_reverb.md)
- organ sound generator <a href="extras/ml_organ.md">more details</a>
- saw/square pulse width modulated oscillator <a href="extras/ml_oscillator.md">more details</a>
- vu meter (helper) <a href="extras/ml_vu_meter.md">more details</a>
- oled scope <a href="extras/ml_scope.md">more details</a>

# Compiling note

In some cases the following error might occur: 
	
	The plaform does not support 'compiler.libraries.ldflags' for precompiled libraries.
	
To fix this you can look for platform.txt

Add the following line: 

	compiler.libraries.ldflags=
	
If you get a lot of "undefined reference to" errors the library couldn't be found by Arduino.

To support new platforms I need the information in which folder Arduino was looking for the library. 
Example:

	Compiling libraries...
	Compiling library "ML_SynthTools"
	Library ML_SynthTools has been declared precompiled:
	Precompiled library in "C:\Users\...\Documents\Arduino\libraries\ML_SynthTools\src\cortex-m4\fpv4-sp-d16-hard" not found
	Precompiled library in "C:\Users\...\Documents\Arduino\libraries\ML_SynthTools\src\cortex-m4" not found

---

Please feel free to share your experience and ideas. 