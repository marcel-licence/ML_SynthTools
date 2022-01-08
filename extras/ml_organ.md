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
