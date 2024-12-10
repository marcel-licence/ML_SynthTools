/*
 * Copyright (c) 2024 Marcel Licence
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 * veröffentlichten Version, weiter verteilen und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
 * OHNE JEDE GEWÄHR,; sogar ohne die implizite
 * Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Einzelheiten.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
 */

/**
* @file audio_module.ino
* @author Marcel Licence
* @date 16.12.2021
*
* @brief  this file provides a general audio interface to make it easier working with different platforms
*/


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#if (defined SAMPLE_BUFFER_SIZE)


#define RP2350_USE_I2S_ML_LIB

#ifdef ML_SYNTH_INLINE_DECLARATION


#include <ml_types.h>


void Audio_Setup(void);
void Audio_Output(const float *left, const float *right);
void Audio_OutputMono(const int32_t *samples);
void Audio_Output(const int16_t *left, const int16_t *right);
void Audio_Output(const Q1_14 *left, const Q1_14 *right);
void Audio_Input(float *left, float *right);
void Audio_Input(Q1_14 *left, Q1_14 *right);
void Audio_Input(int16_t *left, int16_t *right);
void DaisySeed_Setup(void);
void Teensy_Setup();
void SAMD21_Synth_Init();

#ifdef AUDIO_PRINT_STATS
void Audio_PrintStats();
#endif


#ifdef ARDUINO_DISCO_F407VG
extern "C" {
    void STM32F407_AudioInit();
    void STM32F407G_AudioWriteS16(const int16_t *fl_sample, const int16_t *fr_sample);
    void STM32F407G_AudioWrite(const float *fl_sample, const float *fr_sample);
}
#endif


#endif // #ifdef ML_SYNTH_INLINE_DECLARATION


#ifdef ML_SYNTH_INLINE_DEFINITION


#ifdef SAMPLE_RATE
#include <ml_types.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <I2S.h>
#include <i2s_reg.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#endif

#ifdef TEENSYDUINO
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#endif

#if (defined ARDUINO_DAISY_SEED) || (defined STM32H7xx)
#include "DaisyDuino.h" /* requires the DaisyDuino library: https://github.com/electro-smith/DaisyDuino */
#endif

#ifdef RP2040_AUDIO_PWM
#include "RP2040_AudioPwm.h"
#ifndef RP2040_AUDIO_PWM_PIN
#define RP2040_AUDIO_PWM_PIN 0
#endif

uint32_t WavPwmDataBuff[SAMPLE_BUFFER_SIZE];
uint32_t WavPwmDataBuff2[SAMPLE_BUFFER_SIZE];
#endif

#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
#include <I2S.h>
#endif


#ifdef PICO_AUDIO_I2S
#ifdef RP2350_USE_I2S_ML_LIB
#include <librp2350_i2s_audio.h>
#else
#include <I2S.h>
// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);
#endif /* #endif RP2350_USE_I2S_ML_LIB */
#endif

#ifndef I2S_OVERSAMPLE
#define I2S_OVERSAMPLE 1
#endif

#ifdef OUTPUT_SAW_TEST
static float saw_left[SAMPLE_BUFFER_SIZE];
static float saw_right[SAMPLE_BUFFER_SIZE];
static int32_t saw_i32[SAMPLE_BUFFER_SIZE];
#endif
#ifdef OUTPUT_SINE_TEST
static float sin_left[SAMPLE_BUFFER_SIZE];
static float sin_right[SAMPLE_BUFFER_SIZE];
static int32_t sine_i32[SAMPLE_BUFFER_SIZE];
#endif

void Audio_Setup(void)
{
#if (defined ESP8266) || (defined ESP32)
    WiFi.mode(WIFI_OFF);
#endif

#ifdef OUTPUT_SAW_TEST
    /*
     * base frequency: SAMPLE_FREQ / SAMPLE_BUFFER_SIZE
     * for example: Fs : 44100Hz, Lsb = 48 -> Freq: 918.75 Hz
     */
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        float saw = ((float)i * 2.0f) / ((float)SAMPLE_BUFFER_SIZE);
        saw -= 1.0f;
        saw_left[i] = saw;
        saw_right[i] = saw;
        saw *= 1073741824;
        saw_i32[i] = saw;
    }
#endif
#ifdef OUTPUT_SINE_TEST
    /*
     * create sinewave with f and 2*f
     */
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        float w = i;
        w *= 1.0f / ((float)SAMPLE_BUFFER_SIZE);
        w *= 2.0f * M_PI;
        float sine = sin(w);
        sin_left[i] = sine;
        sin_right[i] = sin(w * 2.0f);
        sine *= 1073741824;
        sine_i32[i] = sine;
    }
#endif


#ifdef ESP32_AUDIO_KIT
#ifdef ES8388_ENABLED
    ES8388_Setup();
    ES8388_SetIn2OoutVOL(0, 0);
#else
    ac101_setup();
#endif
#endif


#ifdef WM8978_ENABLED
    WM8978_Setup();
#endif

#ifdef WM8960_ENABLED
    WM8960_Setup();
#endif

#ifdef ESP32
    setup_i2s();
#endif

#ifdef ESP8266
    if (!I2S.begin(I2S_PHILIPS_MODE, SAMPLE_RATE * 2, 16))
    {
        Serial.println("Failed to initialize I2S!");
        while (1)
            ; // do nothing
    }
#endif

#ifdef PICO_AUDIO_I2S
#ifndef RP2350_USE_I2S_ML_LIB
    i2s.setBCLK(PICO_AUDIO_I2S_CLOCK_PIN_BASE);
    i2s.setDATA(PICO_AUDIO_I2S_DATA_PIN);
    i2s.setBitsPerSample(16);

    // start I2S at the sample rate with 16-bits per sample
    if (!i2s.begin(SAMPLE_RATE))
    {
        Serial.println("Failed to initialize I2S!");
        while (1); // do nothing
    }
#else /* #ifndef RP2350_USE_I2S_ML_LIB */
    {
        int data_pin = 26;
        int clock_pin_base = 27;
        rp2350_i2s_init(data_pin, clock_pin_base);
        Serial.printf("rp2350_i2s_init\n\tdata_pin: %d\n\tclock_pin_base: %d\n\twclk_pin: %d\n", data_pin, clock_pin_base, clock_pin_base + 1);
    }
#endif /* #endif RP2350_USE_I2S_ML_LIB */
#endif


#ifdef TEENSYDUINO
    AudioMemory(4);
#endif

#ifdef ARDUINO_SEEED_XIAO_M0
    SAMD21_Synth_Init();
    pinMode(DAC0, OUTPUT);
#endif

#ifdef RP2040_AUDIO_PWM
    uint8_t pwmPinNumber = RP2040_AUDIO_PWM_PIN;
    Serial.printf("Initialize pwm audio used without DAC pin %d + pin %d:\n", pwmPinNumber, pwmPinNumber + 1);
    Serial.printf("    sample rate: %d\n", SAMPLE_RATE);
    Serial.printf("    buffer size: %d\n", SAMPLE_BUFFER_SIZE);
    RP2040_Audio_Pwm_Init(pwmPinNumber, SAMPLE_RATE, WavPwmDataBuff, WavPwmDataBuff2, SAMPLE_BUFFER_SIZE);
#endif

#if 0
#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
#ifdef RP2040_AUDIO_PWM
#else
    if (!I2S.begin(SAMPLE_RATE))
    {
        Serial.println("Failed to initialize I2S!");
        while (1); // do nothing
    }
#endif
#endif
#endif

#ifdef ARDUINO_DISCO_F407VG
    STM32F407_AudioInit();
#endif
#ifdef ARDUINO_GENERIC_F407VGTX
    STM32_AudioInit();
#endif
}

#ifdef TEENSYDUINO

#ifdef LED_PIN
const int ledPin = LED_PIN; /* pin configured in config.h */
#endif

AudioPlayQueue queue1;
AudioPlayQueue queue2;
AudioOutputI2S i2s1;
AudioConnection patchCord1(queue1, 0, i2s1, 0); /* left channel */
AudioConnection patchCord2(queue2, 0, i2s1, 1); /* right channel */


static int16_t sampleBuffer[AUDIO_BLOCK_SAMPLES];
static int16_t sampleBuffer2[AUDIO_BLOCK_SAMPLES];
static int16_t *queueTransmitBuffer;
static int16_t *queueTransmitBuffer2;

void Teensy_Setup()
{
    Serial.printf("Teensy Setup\n");
    Serial.printf("\tBCK: 21\n");
    Serial.printf("\tLRCK: 20\n");
    Serial.printf("\tSDOUT: 7\n");
    Serial.printf("AudoBlockSamples: %d\n", AUDIO_BLOCK_SAMPLES);
    Midi_Setup();
}

#endif /* TEENSYDUINO */

#if (defined ARDUINO_DAISY_SEED) || (defined STM32H7xx)

static DaisyHardware hw;
static size_t num_channels;

volatile static bool dataReady = false;
static float out_temp[2][48];
static float *outCh[2] = {out_temp[0], out_temp[1]};

void MyCallback(float **in, float **out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {

        out[0][i] = out_temp[0][i];
        out[1][i] = out_temp[1][i];
        out_temp[0][i] = in[0][i];
        out_temp[1][i] = in[1][i];
    }
    dataReady = true;
}

void DaisySeed_Setup(void)
{
    float sample_rate;
    hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    num_channels = hw.num_channels;
    sample_rate = DAISY.get_samplerate();

    DAISY.begin(MyCallback);
}
#endif /* ARDUINO_DAISY_SEED */

#ifdef ARDUINO_SEEED_XIAO_M0

static int32_t u32buf[SAMPLE_BUFFER_SIZE];

union ts
{
    int16_t i16;
    uint16_t u16;
};
union ts testSineU[SAMPLE_BUFFER_SIZE] = {0};

inline
void ProcessAudio(uint16_t *buff, size_t len)
{
    /* convert from u16 to u10 */
    for (size_t i = 0; i < len; i++)
    {
        int32_t var = u32buf[i];
        var >>= 16;
        union ts varU;
        varU.i16 = var;
        varU.u16 /= 64;
        varU.u16 += 512;
        buff[i] = varU.u16;;
    }
}

#endif

#ifdef AUDIO_PRINT_STATS
#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
static uint32_t cc[4] = {0, 0, 0, 0};
static uint32_t cc2[4] = {0, 0, 0, 0};

void Audio_PrintStats()
{
    uint32_t d1 = cc2[2] - cc2[1];
    uint32_t d2 = cc2[3] - cc2[2];
    float l1 = d1;
    float l2 = d2;
    float l3 = d1 + d2;
    l1 *= (SAMPLE_RATE / SAMPLE_BUFFER_SIZE) / ((float)F_CPU);
    l2 *= (SAMPLE_RATE / SAMPLE_BUFFER_SIZE) / ((float)F_CPU);
    l3 *= (SAMPLE_RATE / SAMPLE_BUFFER_SIZE) / ((float)F_CPU);
    Serial.printf("f: %0.3f, %0.3f, %0.3f (%" PRIu32 ")\n", l1, l2, l3, d1 + d2);
}
#endif
#endif

void Audio_OutputMono(const int32_t *samples)
{
#ifdef OUTPUT_SAW_TEST
    samples = saw_i32;
#endif
#ifdef OUTPUT_SINE_TEST
    samples = sine_i32;
#endif

#ifdef ESP8266
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        int32_t sig = samples[i];
        static uint16_t sig16 = 0;
        sig *= 4;
        sig16 = sig;
        I2S.write(0x8000 + sig16);
    }
#endif

#ifdef PICO_AUDIO_I2S
#ifndef RP2350_USE_I2S_ML_LIB
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        int32_t sig = samples[i];
        static uint16_t sig16 = 0;
        sig >>= 2;
        sig16 = sig;
        i2s.write(sig16);
    }
#else /* #ifndef RP2350_USE_I2S_ML_LIB */
    int16_t mono_u16[SAMPLE_BUFFER_SIZE];
    for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
    {
        mono_u16[n] = samples[n] >> 16;
    }
    rp2350_i2s_write_stereo_samples_buff(mono_u16, mono_u16, SAMPLE_BUFFER_SIZE);
#endif /* #endif RP2350_USE_I2S_ML_LIB */
#endif

#ifdef ESP32
    float mono[SAMPLE_BUFFER_SIZE];
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        float sigf = samples[i];
        sigf /= INT16_MAX;
        mono[i] = sigf;
    }

    i2s_write_stereo_samples_buff(mono, mono, SAMPLE_BUFFER_SIZE);
#endif /* ESP32 */

#ifdef TEENSYDUINO
    {
#ifdef CYCLE_MODULE_ENABLED
        calcCycleCountPre();
#endif
        queueTransmitBuffer = queue1.getBuffer(); /* blocking? */
        queueTransmitBuffer2 = queue2.getBuffer();
#ifdef CYCLE_MODULE_ENABLED
        calcCycleCount();
#endif
        if (queueTransmitBuffer)
        {
            {
                for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
                {
                    sampleBuffer[i] = (int16_t)((samples[i]));
                }

                memcpy(queueTransmitBuffer, sampleBuffer, AUDIO_BLOCK_SAMPLES * 2);
                memcpy(queueTransmitBuffer2, sampleBuffer, AUDIO_BLOCK_SAMPLES * 2);
            }
            queue1.playBuffer();
            queue2.playBuffer();
        }
    }
#endif /* TEENSYDUINO */

#if (defined ARDUINO_DAISY_SEED) || (defined STM32H7xx)

    float sig_f[SAMPLE_BUFFER_SIZE];

#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    while (!dataReady)
    {
        /* just do nothing */
    }
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif

    for (size_t i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        sig_f[i] = ((float)samples[i]) * (1.0f / ((float)INT16_MAX));
    }

    memcpy(out_temp[0], sig_f, sizeof(out_temp[0]));
    memcpy(out_temp[1], sig_f, sizeof(out_temp[1]));

    dataReady = false;
#endif /* ARDUINO_DAISY_SEED */

#ifdef ARDUINO_SEEED_XIAO_M0
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    memcpy(u32buf, samples, sizeof(int32_t)*SAMPLE_BUFFER_SIZE);
    while (!SAMD21_Synth_Process(ProcessAudio))
    {
        /* just do nothing */
    }
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif
#endif /* ARDUINO_SEEED_XIAO_M0 */

#ifdef ARDUINO_DISCO_F407VGxx
    int16_t mono_s16[SAMPLE_BUFFER_SIZE];

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        uint16_t val = samples[i] ; /* 21 with 32 bit input */
        mono_s16[i] = val;
    }

    STM32F407G_AudioWriteS16(mono_s16, mono_s16);
#endif /* ARDUINO_DISCO_F407VGxx */

#ifdef RP2040_AUDIO_PWM
    union sample
    {
        uint32_t buff;
        struct
        {
            uint16_t left;
            uint16_t right;
        };
    };

#ifdef AUDIO_PRINT_STATS
    cc[1] = cc[3];
    cc[2] = rp2040.getCycleCount();
#endif

    while (!RP2040_Audio_Pwm_BufferReady())
    {
        /* block! */
    }

#ifdef AUDIO_PRINT_STATS
    cc[3] = rp2040.getCycleCount();
    memcpy(cc2, cc, sizeof(cc2));
#endif

    union sample *audioBuff = (union sample *) RP2040_Audio_Pwm_getFreeBuff();

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        int32_t val32 = samples[i];
        val32 >>= 16;
        uint16_t val = (val32 + 0x8000) >> 5; /* 21 with 32 bit input */
        val += 361;

        audioBuff[i].left = val;
        audioBuff[i].right = val;
    }
#endif

#if 0
#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
#ifdef RP2040_AUDIO_PWM
#else /* RP2040_AUDIO_PWM */
    /*
     * @see https://arduino-pico.readthedocs.io/en/latest/i2s.html
     * @see https://www.waveshare.com/pico-audio.htm for connections
     */
    int16_t u16int[2 * SAMPLE_BUFFER_SIZE];

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        u16int[2 * i] = samples[i];
        u16int[(2 * i) + 1] = samples[i];
    }
    for (int i = 0; i < SAMPLE_BUFFER_SIZE * 2; i++)
    {
        I2S.write(u16int[i]);
    }
#endif /* RP2040_AUDIO_PWM */
#endif /* ARDUINO_RASPBERRY_PI_PICO, ARDUINO_GENERIC_RP2040 */
#endif

#ifdef ARDUINO_GENERIC_F407VGTX
    /*
     * Todo Implementation for the STM32F407VGT6
     * Can be found on the ST Discovery Board
     */
#endif /* ARDUINO_GENERIC_F407VGTX */

#ifdef ARDUINO_DISCO_F407VG
    int16_t mono_s16[SAMPLE_BUFFER_SIZE];

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        uint16_t val = samples[i] ; /* 21 with 32 bit input */
        mono_s16[i] = val;
    }

    STM32F407G_AudioWriteS16(mono_s16, mono_s16);
#endif /* ARDUINO_DISCO_F407VG */
}

void Audio_Output(const Q1_14 *left, const Q1_14 *right)
{
    Audio_Output((const int16_t *)left, (const int16_t *)right);
}

#ifndef ARDUINO_SEEED_XIAO_M0
void Audio_Output(const int16_t *left, const int16_t *right)
{
#ifdef ESP8266
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        static uint16_t s16 = 0xAAAA;
        static int16_t err = 0;
        int16_t inSample = left[i];
        inSample >>= 5;

        for (int k = 0; k < 2 * 2; k++)
        {
            for (int n = 0; n < 16; n++)
            {
                if (err > 0)
                {
                    s16 >>= 1;
                    s16 += 0x8000;
                    err += inSample - 0x100;
                }
                else
                {
                    s16 >>= 1;
                    s16 += 0x0000;
                    err += inSample + 0x100;
                }
            }

            I2S.write(s16);
        }
    }
#endif

#ifdef ESP32
#ifdef I2S_DIRECT_OUT
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        static uint16_t s16 = 0xAAAA;
        static int16_t err = 0;
        int16_t inSample = left[i];
        inSample >>= 5;

        for (int k = 0; k < I2S_OVERSAMPLE; k++)
        {
            for (int n = 0; n < 16; n++)
            {
                if (err > 0)
                {
                    s16 >>= 1;
                    s16 += 0x8000;
                    err += inSample - 0x100;
                }
                else
                {
                    s16 >>= 1;
                    s16 += 0x0000;
                    err += inSample + 0x100;
                }
            }
            i2s_write_stereo_samples_i16((const int16_t *)&s16, (const int16_t *)&s16, 1);


        }
    }
#else
    i2s_write_stereo_samples_i16(left, right, SAMPLE_BUFFER_SIZE);
#endif
#endif /* ESP32 */

#ifdef ARDUINO_DISCO_F407VG
    STM32F407G_AudioWriteS16(left, right);
#endif /* ARDUINO_DISCO_F407VG */

#if (defined ARDUINO_DAISY_SEED) || (defined STM32H7xx)

    float sig_l[SAMPLE_BUFFER_SIZE];
    float sig_r[SAMPLE_BUFFER_SIZE];

#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    while (!dataReady)
    {
        /* just do nothing */
    }
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif

    for (size_t i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        sig_l[i] = ((float)left[i]) * (1.0f / ((float)INT16_MAX));
        sig_r[i] = ((float)right[i]) * (1.0f / ((float)INT16_MAX));
    }

    memcpy(out_temp[0], sig_l, sizeof(out_temp[0]));
    memcpy(out_temp[1], sig_r, sizeof(out_temp[1]));

    dataReady = false;
#endif /* ARDUINO_DAISY_SEED */

#ifdef RP2040_AUDIO_PWM
    union sample
    {
        uint32_t buff;
        struct
        {
            uint16_t left;
            uint16_t right;
        };
    };

#ifdef AUDIO_PRINT_STATS
    cc[1] = cc[3];
    cc[2] = rp2040.getCycleCount();
#endif

    while (!RP2040_Audio_Pwm_BufferReady())
    {
        /* block! */
    }

#ifdef AUDIO_PRINT_STATS
    cc[3] = rp2040.getCycleCount();
    memcpy(cc2, cc, sizeof(cc2));
#endif

    union sample *audioBuff = (union sample *) RP2040_Audio_Pwm_getFreeBuff();

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        uint16_t val;
        val = (left[i] + 0x8000) >> 5; /* 21 with 32 bit input */
        val += 361;

        audioBuff[i].left = val;

        val = (right[i] + 0x8000) >> 5; /* 21 with 32 bit input */
        val += 361;
        audioBuff[i].right = val;
    }
#endif

#if 0
#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
#ifdef RP2040_AUDIO_PWM
#else /* RP2040_AUDIO_PWM */
    /*
     * @see https://arduino-pico.readthedocs.io/en/latest/i2s.html
     * @see https://www.waveshare.com/pico-audio.htm for connections
     */
    int16_t u16int[2 * SAMPLE_BUFFER_SIZE];

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        u16int[2 * i] = samples[i];
        u16int[(2 * i) + 1] = samples[i];
    }
    for (int i = 0; i < SAMPLE_BUFFER_SIZE * 2; i++)
    {
        I2S.write(u16int[i]);
    }
#endif /* RP2040_AUDIO_PWM */
#endif /* ARDUINO_RASPBERRY_PI_PICO, ARDUINO_GENERIC_RP2040 */
#endif

#ifdef PICO_AUDIO_I2S
#ifndef RP2350_USE_I2S_ML_LIB
    while (i2s.availableForWrite() == false)
    {

    }
    union
    {
        uint32_t u32;
        struct
        {
            int16_t l;
            int16_t r;
        };
    } bf[SAMPLE_BUFFER_SIZE];
    for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
    {
        bf[n].l = left[n];
        bf[n].r = right[n];
    }
    i2s.write((const uint8_t *)bf, SAMPLE_BUFFER_SIZE * 4);
#else
    rp2350_i2s_write_stereo_samples_buff(left, right, SAMPLE_BUFFER_SIZE);
#endif
#endif /* PICO_AUDIO_I2S */
}
#endif

#if (defined ESP32) || (defined TEENSYDUINO) || (defined ARDUINO_DAISY_SEED) || (defined ARDUINO_GENERIC_F407VGTX) || (defined ARDUINO_DISCO_F407VG) || (defined ARDUINO_BLACK_F407VE) || (defined ARDUINO_ARCH_RP2040) || (((defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)) && (defined RP2040_AUDIO_PWM))
#ifdef ESP32
void Audio_Input(float *left, float *right)
{
    i2s_read_stereo_samples_buff(left, right, SAMPLE_BUFFER_SIZE);
}

void Audio_Input(Q1_14 *left, Q1_14 *right)
{
    i2s_read_stereo_samples_buff((int16_t *)left, (int16_t *)right, SAMPLE_BUFFER_SIZE);
}

void Audio_Input(int16_t *left, int16_t *right)
{
    i2s_read_stereo_samples_buff(left, right, SAMPLE_BUFFER_SIZE);
}
#else
void Audio_Input(float *left __attribute__((__unused__)), float *right __attribute__((__unused__)))
{

}
#endif /* ESP32 */

void Audio_Output(const float *left, const float *right)
{
#ifdef OUTPUT_SAW_TEST
    left = saw_left;
    right = saw_right;
#endif
#ifdef OUTPUT_SINE_TEST
    left = sin_left;
    right = sin_right;
#endif

#ifdef ESP32
    i2s_write_stereo_samples_buff(left, right, SAMPLE_BUFFER_SIZE);
#endif /* ESP32 */

#ifdef PICO_AUDIO_I2S
#ifndef RP2350_USE_I2S_ML_LIB
    while (i2s.availableForWrite() == false)
    {

    }
    union
    {
        uint32_t u32;
        struct
        {
            int16_t l;
            int16_t r;
        };
    } bf[SAMPLE_BUFFER_SIZE];
    for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
    {
        bf[n].l = (int16_t)(left[n] * INT16_MAX);
        bf[n].r = (int16_t)(right[n] * INT16_MAX);
    }
    i2s.write((const uint8_t *)bf, SAMPLE_BUFFER_SIZE * 4);
#else /* #ifndef RP2350_USE_I2S_ML_LIB */
    rp2350_i2s_write_stereo_samples_buff(left, right, SAMPLE_BUFFER_SIZE);
#endif /* #ifndef,else RP2350_USE_I2S_ML_LIB */
#endif /* PICO_AUDIO_I2S */

#ifdef TEENSYDUINO
    {
#ifdef CYCLE_MODULE_ENABLED
        calcCycleCountPre();
#endif
        queueTransmitBuffer = queue1.getBuffer(); /* blocking? */
        queueTransmitBuffer2 = queue2.getBuffer();
#ifdef CYCLE_MODULE_ENABLED
        calcCycleCount();
#endif
        if (queueTransmitBuffer)
        {
            {
                for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
                {
                    sampleBuffer[i] = (int16_t)(left[i] * INT16_MAX);
                    sampleBuffer2[i] = (int16_t)(right[i] * INT16_MAX);
                }

                memcpy(queueTransmitBuffer, sampleBuffer, AUDIO_BLOCK_SAMPLES * 2);
                memcpy(queueTransmitBuffer2, sampleBuffer2, AUDIO_BLOCK_SAMPLES * 2);
            }

            queue1.playBuffer();
            queue2.playBuffer();
        }
    }
#endif /* TEENSYDUINO */

#if (defined ARDUINO_DAISY_SEED) || (defined STM32H7xx)

#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    while (!dataReady)
    {
        /* just do nothing */
    }
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif

    memcpy(out_temp[0], left, sizeof(out_temp[0]));
    memcpy(out_temp[1], right, sizeof(out_temp[1]));

    dataReady = false;

#endif /* ARDUINO_DAISY_SEED */

#ifdef ARDUINO_GENERIC_F407VGTX

    STM32_AudioWrite(left, right);

#endif /* ARDUINO_GENERIC_F407VGTX */

#ifdef ARDUINO_DISCO_F407VG

    STM32F407G_AudioWrite(left, right);

#endif /* ARDUINO_GENERIC_F407VGTX */

#ifdef RP2040_AUDIO_PWM
    union sample
    {
        uint32_t buff;
        struct
        {
            uint16_t left;
            uint16_t right;
        };
    };

#ifdef AUDIO_PRINT_STATS
    cc[1] = cc[3];
    cc[2] = rp2040.getCycleCount();
#endif

    while (!RP2040_Audio_Pwm_BufferReady())
    {
        /* block! */
    }

#ifdef AUDIO_PRINT_STATS
    cc[3] = rp2040.getCycleCount();
    memcpy(cc2, cc, sizeof(cc2));
#endif

    union sample *audioBuff = (union sample *)RP2040_Audio_Pwm_getFreeBuff();

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        float val;

        val = left[i];
        val += 0.5f;
        val *= (0x8000) >> 5;
        audioBuff[i].left = val;

        val = right[i];
        val += 0.5f;
        val *= (0x8000) >> 5;
        audioBuff[i].right = val;
    }
#endif
}
#endif /* (defined ESP32) || (defined TEENSYDUINO) || (defined ARDUINO_DAISY_SEED) || (defined ARDUINO_GENERIC_F407VGTX) || (defined ARDUINO_DISCO_F407VG) */

#endif

#endif // ML_SYNTH_INLINE_DEFINITION

#endif /* #if (defined SAMPLE_BUFFER_SIZE) */
