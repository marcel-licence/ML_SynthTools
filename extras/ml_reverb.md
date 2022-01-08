<h1 align="center">Reverb</h1>
<h3 align="center">A simple reverb effect</h3>  

This simple reverb can be used to a little reverb to your project.

The following include is required:
    #include <ml_reverb.h>
	
To initialize the reverb from static memory use the following code:
    static float revBuffer[REV_BUFF_SIZE];
    Reverb_Setup(revBuffer);

In case you want to use the heap memory instead you can also use:
    static float *revBuffer = malloc(sizeof(float) * REV_BUFF_SIZE);
    Reverb_Setup(revBuffer);

To get reverb into you signal you can use:
	float sample[SAMPLE_BUFFER_SIZE];
	...
    Reverb_Process(sample, SAMPLE_BUFFER_SIZE);

To control the amount of reverb use:
	void Reverb_SetLevel(uint8_t not_used, float value);
	
Accepted values are between 0.0f and 1.0f:
	Reverb_SetLevel(0, 0.0f); // no reverb
	
	Reverb_SetLevel(0, 1.0f); // max amount of reverb