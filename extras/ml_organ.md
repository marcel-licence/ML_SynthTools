<h1 align="center">Organ</h1>
<h3 align="center">A full polyphone organ sound generator</h3>  

This provides a full polyphone (64 notes) organ sound generation
with a simple (noisy) leslie effect.

An example can be found [here](https://github.com/marcel-licence/ml_synth_organ_example)

If you want to listen to it look [here](https://youtu.be/9AHQ4mQrjE8)

The following include is required:

	#include <ml_organ.h>
	
To initialize the organ use the following code in your setup routine:

	Organ_Setup(&Serial, SAMPLE_RATE);

You can add the following code to generate a note on startup:

	OrganPro_NoteOn(0, 60, 127);
    OrganPro_SetLeslCtrl(127);

The sample can be generated with the following code:

	int32_t mono[SAMPLE_BUFFER_SIZE];
    Organ_Process_Buf(mono, SAMPLE_BUFFER_SIZE);

To control the drawbars use:

	Organ_SetDrawbar(id, value); // id in range from 0 to 8, value in range from 0.0f to 1.0f

# Controlable parameters

## Drawbars

Using the following function allows to control the level of the different drawbars:

	void Organ_SetDrawbar(uint8_t idx, uint8_t value);
	
The idx can set to a number 0 .. 8 referencing the drawbar 1 .. 9.
The value accepts values from 0 .. 127 setting min. until max. volume per drawbar.

## Leslie

The simulated leslie speed can set with the following function:

	void Organ_SetLeslCtrl(uint8_t val);

The value can set to 0 for stop until 127 for maximum speed. Values in between are also accepted.

## Other switches

There are also functions to switch on/off or between two different values.
For that you can call:

	void Organ_PercussionSet(uint8_t setting);

Using CTRL_PERC_SWITCH for setting will switch the percussion on/off. Please note that it might not work with the NoteOn call active in the setup function.

Using CTRL_PERC_SPEED allows to switch between short and long percussion.

Using CTRL_PERC_NOTE switches between 2nd and 3rd percussion.

CTRL_PERC_LOUD switches between soft and normal percussion.

CTRL_PERC_POLY turns on/off the polyphonic percussion. It might sound odd because it will trigger the percussion for all notes each time a new note will be played.

CTRL_INTR_FEEDBACK has no function at the moment.

CTRL_ROTARY_ACTIVE turns the leslie on/off. Turning the leslie off will bypass it to allow to get a cleaner sound and for example use it with external effects. You might call this twice the first time to get the values initialized. I will change that in future.

