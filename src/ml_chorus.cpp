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
 * @file ml_chorus.cpp
 * @author Marcel Licence
 * @date 20.12.2022
 *
 * @brief This file contains an implementation of a simple stereo chorus effect
 *
 * @see little demo: https://youtu.be/ZIiSp7yM6o8
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#if (!defined ARDUINO_RASPBERRY_PI_PICO) && (!defined ARDUINO_GENERIC_RP2040)


#include <ml_chorus.h>
#include <ml_alg.h>
#include <ml_status.h>

#include <math.h>
#include <stdio.h>


/*
 * module variables
 */
static int16_t *chorusLine_l;

static float chorusToMix = 0;
static float chorusInLvl = 1.0f;
static float chorusDepth = 0;
static float chorusSpeed = 2.0f / 3.0f;
static float chorusThrough = 1.0f;
static float chorusPhaseShift = 0.5f;
static uint32_t chorusLenMax = 0;
static uint32_t chorusLen = 1098;
static uint32_t chorusDelay = 0;
static uint32_t chorusIn = 0;
static uint32_t chorusOut = 0;
static uint32_t chorusOut2 = 0;


#define SINE_BIT    11
#define SINE_CNT    (1<<SINE_BIT)
#define SINE_CNT_HALF   (1<<(SINE_BIT-1))
#define SINE_MASK_BIT   (32-SINE_BIT)
#define SINE_MASK   (SINE_CNT-1)

float sineLookup[SINE_CNT];

struct sineL_s
{
    uint32_t rad;
    uint32_t add;
    float f;
    float a;
    float b;
};

struct sineL_s sinM;
struct sineL_s sinS;

static inline
void Chorus_InitializeSineLookup()
{
    for (int n = 0; n < SINE_CNT; n++)
    {
        float rad = n;
        rad *= 2.0f * M_PI;
        rad /= SINE_CNT;
        sineLookup[n] = sin(rad);
    }
}

static inline
void CSineSetFrequency(struct sineL_s *sine, float frequency, float sample_rate)
{
    sine->f = frequency;
    uint32_t add = 0x80000000;
    add /= sample_rate;
    add *= frequency * 2;
    sine->add = add;
}

static inline
void CSineCalc(struct sineL_s *sine)
{
    sine->rad += sine->add;
    sine->a = sineLookup[(sine->rad >> SINE_MASK_BIT) & SINE_MASK];
}

static inline
void CSineSetPhaseShift(struct sineL_s *sine, float shift, struct sineL_s *sine2)
{
    sine2->f = sine->f;
    sine2->add = sine->add;
    sine2->rad = sine->rad;

    uint32_t shiftAdd = 0x80000000;
    shiftAdd *= shift * 2;
    sine2->rad += shiftAdd;
}

static inline
void CSineInit(struct sineL_s *sine)
{
    sine->rad = 0;
    sine->add = 0;
    sine->f = 0;
    CSineCalc(sine);
}

void Chorus_Init(int16_t *buffer, uint32_t len)
{
    chorusLine_l = buffer;
    chorusLenMax = len - 2;

    if (chorusLine_l == NULL)
    {
        printf("Not enough memory available for mono chorus line!\n");
    }

    Chorus_Reset();
}

void Chorus_Reset(void)
{
    for (uint32_t i = 0; i < chorusLenMax; i++)
    {
        chorusLine_l[i] = 0;
    }

    Chorus_InitializeSineLookup();
    CSineInit(&sinM);
    CSineInit(&sinS);
}

void Chorus_Process(float *signal_l, float *signal_r __attribute__((unused)))
{
    chorusLine_l[chorusIn] = (((float)0x8000) * *signal_l * chorusInLvl);

    chorusOut = chorusIn + (1 + chorusLenMax - chorusLen);

    if (chorusOut >= chorusLenMax)
    {
        chorusOut -= chorusLenMax;
    }

    *signal_l += ((float)chorusLine_l[chorusOut]) * chorusToMix / ((float)0x8000);

    chorusLine_l[chorusIn] += (((float)chorusLine_l[chorusOut]) * chorusDepth);

    chorusIn ++;

    if (chorusIn >= chorusLenMax)
    {
        chorusIn = 0;
    }
}

void Chorus_Process_Buff(float *in, float *left, float *right, int buffLen)
{
    const float mult = chorusDepth * 0.5f;

    for (int n = 0; n < buffLen; n++)
    {
        CSineCalc(&sinM);
        CSineCalc(&sinS);

        chorusLine_l[chorusIn] = (((float)0x4000) * in[n] * chorusInLvl);

        float sinV1 = mult * (1.0f - sinM.a);
        float sinV2 = mult * (1.0f - sinS.a);

        chorusOut = chorusIn + (0 + chorusLenMax - sinV1 - chorusDelay);
        chorusOut2 = chorusIn + (0 + chorusLenMax - sinV2 - chorusDelay);

        float chorusOutf = chorusIn + (0 + chorusLenMax - (uint32_t)sinV1 - chorusDelay);
        float chorusOutf2 = chorusIn + (0 + chorusLenMax - (uint32_t)sinV2 - chorusDelay);

        left[n] *= chorusThrough;
        right[n] *= chorusThrough;

        left[n] -= mul_f(lerpOut(chorusLine_l, chorusOutf, chorusLenMax), chorusToMix);
        right[n] -= mul_f(lerpOut(chorusLine_l, chorusOutf2, chorusLenMax), chorusToMix);


        chorusIn ++;
        chorusIn = chorusIn >= chorusLenMax ? 0 : chorusIn;
    }
}

void Chorus_SetupDefaultPreset(uint8_t unused __attribute__((unused)), float value)
{
    if (value > 0)
    {
        Chorus_SetInputLevel(0, 1.0f);
        Chorus_SetOutputLevel(0, 1.0f);
        Chorus_SetThrough(0, 1.0f);
        Chorus_SetDepth(0, 0.5f);
        Chorus_SetSpeed(0, 0.0f);
        Chorus_SetPhaseShift(0, 0.5f);
    }
}

void Chorus_SetInputLevel(uint8_t unused __attribute__((unused)), float value)
{
    chorusInLvl = value;

    Status_ValueChangedFloat("Chorus_SetInputLevel", value);
}

void Chorus_SetThrough(uint8_t unused __attribute__((unused)), float value)
{
    chorusThrough = value;

    Status_ValueChangedFloat("Chorus_SetThrough", value);
}

void Chorus_UpdatePhaseShift(void)
{
    CSineSetPhaseShift(&sinM, chorusPhaseShift, &sinS);
}

void Chorus_SetPhaseShift(uint8_t unused __attribute__((unused)), float value)
{
    chorusPhaseShift = value;
    Chorus_UpdatePhaseShift();

    Status_ValueChangedFloat("Chorus_SetPhaseShift", value);
}

void Chorus_SetDelay(uint8_t unused __attribute__((unused)), float value)
{
    chorusDelay = chorusLenMax * value;

    if (chorusDepth + chorusDelay >= chorusLenMax)
    {
        chorusDelay = chorusLenMax - chorusDepth;
    }

    Status_ValueChangedInt("Chorus_SetDelay", chorusDelay);
}

void Chorus_SetOutputLevel(uint8_t unused __attribute__((unused)), float value)
{
    chorusToMix = value;

    Status_ValueChangedFloat("Chorus_SetOutputLevel", value);
}

void Chorus_SetDepth(uint8_t unused __attribute__((unused)), float value)
{
    chorusDepth = chorusLenMax * value;

    if (chorusDepth + chorusDelay >= chorusLenMax)
    {
        chorusDepth = chorusLenMax - chorusDelay;
    }

    Status_ValueChangedInt("Chorus_SetDepth", chorusDepth);
}

void Chorus_SetSpeed(uint8_t unused __attribute__((unused)), float value)
{
    chorusSpeed = (0.05 + 7 * value);

    CSineSetFrequency(&sinM, chorusSpeed, 48000.0f);
    Chorus_UpdatePhaseShift();

    Status_ValueChangedFloat("Chorus_SetSpeed", chorusSpeed);
}

#endif /* #if (!defined ARDUINO_RASPBERRY_PI_PICO) && (!defined ARDUINO_GENERIC_RP2040) */

