/*
 * Copyright (c) 2023 Marcel Licence
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
 * @file ml_delay.cpp
 * @author Marcel Licence
 * @date 06.01.2022
 *
 * @brief This is a simple implementation of a stereo s16 delay line
 * - level adjustable
 * - feedback
 * - length adjustable
 * @see https://youtu.be/Kac9AB02BcQ
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_delay.h>
#include <ml_alg.h>
#include <ml_status.h>


#ifndef ARDUINO
#include <stdio.h>
#endif


/*
 * module variables
 */
static int16_t *delayLine_l;
static int16_t *delayLine_r;

static float delayToMix = 0;
static float delayInLvl = 1.0f;
static float delayFeedback = 0;
static float delayShift = 2.0f / 3.0f;
static uint32_t delayLenMax = 0;
static uint32_t delayLen = 11098;
static uint32_t delayIn = 0;
static uint32_t delayOut = 0;
static uint32_t delayOut2 = 0;
static uint32_t delayOut3 = 0;


void Delay_Init(int16_t *buffer, uint32_t len)
{
    delayLine_l = buffer;
    delayLenMax = len;

    if (delayLine_l == NULL)
    {
        printf("Not enough memory available for mono delay line!\n");
    }

    Delay_Reset();
}

void Delay_Init2(int16_t *left, int16_t *right, uint32_t len)
{
    delayLine_l = left;
    delayLine_r = right;
    delayLenMax = len;

    if ((delayLine_l == NULL) || (delayLine_r == NULL))
    {
        printf("Not enough memory available for stereo delay line!!\n");
    }

    Delay_Reset();
}

void Delay_Reset(void)
{
    for (uint32_t i = 0; i < delayLenMax; i++)
    {
        delayLine_l[i] = 0;
    }
    if (delayLine_r != NULL)
    {
        for (uint32_t i = 0; i < delayLenMax; i++)
        {
            delayLine_r[i] = 0;
        }
    }
}

void Delay_Process(float *signal_l, float *signal_r __attribute__((unused)))
{

    delayLine_l[delayIn] = (((float)0x8000) * *signal_l * delayInLvl);


    delayOut = delayIn + (1 + delayLenMax - delayLen);

    if (delayOut >= delayLenMax)
    {
        delayOut -= delayLenMax;
    }

    *signal_l += ((float)delayLine_l[delayOut]) * delayToMix / ((float)0x8000);

    delayLine_l[delayIn] += (((float)delayLine_l[delayOut]) * delayFeedback);


    delayIn ++;

    if (delayIn >= delayLenMax)
    {
        delayIn = 0;
    }
}

void Delay_Process_Buff(float *signal_l, int buffLen)
{

    for (int n = 0; n < buffLen; n++)
    {
        delayLine_l[delayIn] = (((float)0x4000) * signal_l[n] * delayInLvl);

        delayOut = delayIn + (1 + delayLenMax - delayLen);

        if (delayOut >= delayLenMax)
        {
            delayOut -= delayLenMax;
        }

        signal_l[n] += ((float)delayLine_l[delayOut]) * delayToMix / ((float)0x4000);

        delayLine_l[delayIn] += (((float)delayLine_l[delayOut]) * delayFeedback);

        delayIn ++;

        if (delayIn >= delayLenMax)
        {
            delayIn = 0;
        }
    }
}

void Delay_Process_Buff(int16_t *signal_l, int buffLen)
{

    uint16_t delayInLvl_u = delayInLvl * 32768;
    uint16_t delayToMix_u = delayToMix * 32768;
    uint16_t delayFeedback_u = delayFeedback * 32768;

    for (int n = 0; n < buffLen; n++)
    {
        int32_t sigIn = (int32_t)signal_l[n] * (uint32_t)delayInLvl_u;
        sigIn >>= 15;
        delayLine_l[delayIn] = sigIn;

        delayOut = delayIn + (1 + delayLenMax - delayLen);

        if (delayOut >= delayLenMax)
        {
            delayOut -= delayLenMax;
        }

        int32_t sigMix = (int32_t)delayLine_l[delayOut] * (uint32_t)delayToMix_u;
        sigMix >>= 15;
        signal_l[n] += sigMix;

        int32_t sigFb = (int32_t)delayLine_l[delayOut] * (uint32_t)delayFeedback_u;
        sigFb >>= 15;
        delayLine_l[delayIn] += sigFb;

        delayIn ++;

        if (delayIn >= delayLenMax)
        {
            delayIn = 0;
        }
    }
}

void Delay_Process_Buff(float *in, float *left, float *right, int buffLen)
{

    for (int n = 0; n < buffLen; n++)
    {
        delayLine_l[delayIn] = (((float)0x4000) * in[n] * delayInLvl);

        delayOut = delayIn + (1 + delayLenMax - delayLen);
        delayOut2 = delayIn + (1 + delayLenMax - (delayLen - 1000));
        delayOut3 = delayIn + (1 + delayLenMax - (delayLen * delayShift));

        if (delayOut >= delayLenMax)
        {
            delayOut -= delayLenMax;
        }

        if (delayOut2 >= delayLenMax)
        {
            delayOut2 -= delayLenMax;
        }

        if (delayOut3 >= delayLenMax)
        {
            delayOut3 -= delayLenMax;
        }

        left[n] += mul_f(delayLine_l[delayOut], delayToMix);
        right[n] += mul_f(delayLine_l[delayOut2], delayToMix);
        left[n] += mul_f(delayLine_l[delayOut3], delayToMix);

        delayLine_l[delayOut2] += (((float)delayLine_l[delayOut]) * delayFeedback);
        delayLine_l[delayIn] += (((float)delayLine_l[delayOut3]) * delayFeedback);

        delayIn ++;

        if (delayIn >= delayLenMax)
        {
            delayIn = 0;
        }
    }
}

/*
 * this implementation is not complete
 */
void Delay_Process_Buff(float *in_l, float *in_r, float *left, float *right, int buffLen)
{
    for (int n = 0; n < buffLen; n++)
    {
        delayLine_l[delayIn] = (((float)0x4000) * in_l[n] * delayInLvl);
        delayLine_r[delayIn] = (((float)0x4000) * in_r[n] * delayInLvl);

        delayOut = delayIn + (1 + delayLenMax - delayLen);
        delayOut2 = delayIn + (1 + delayLenMax - (delayLen - 1000));
        delayOut3 = delayIn + (1 + delayLenMax - (delayLen * delayShift));

        if (delayOut >= delayLenMax)
        {
            delayOut -= delayLenMax;
        }

        if (delayOut2 >= delayLenMax)
        {
            delayOut2 -= delayLenMax;
        }

        if (delayOut3 >= delayLenMax)
        {
            delayOut3 -= delayLenMax;
        }

        left[n] += mul_f(delayLine_l[delayOut], delayToMix);
        right[n] += mul_f(delayLine_r[delayOut2], delayToMix);
        left[n] += mul_f(delayLine_l[delayOut3], delayToMix);

        delayLine_l[delayOut2] += (((float)delayLine_l[delayOut]) * delayFeedback);
        delayLine_l[delayIn] += (((float)delayLine_l[delayOut3]) * delayFeedback);

        delayLine_r[delayOut2] += (((float)delayLine_r[delayOut]) * delayFeedback);
        delayLine_r[delayIn] += (((float)delayLine_r[delayOut3]) * delayFeedback);

        delayIn ++;

        if (delayIn >= delayLenMax)
        {
            delayIn = 0;
        }
    }
}

void Delay_Process_Buff(int16_t *in, int16_t *left, int16_t *right, int buffLen)
{

    for (int n = 0; n < buffLen; n++)
    {
        delayLine_l[delayIn] = mul(in[n], delayInLvl);

        delayOut = delayIn + (1 + delayLenMax - delayLen);
        delayOut2 = delayIn + (1 + delayLenMax - (delayLen - 1000));
        delayOut3 = delayIn + (1 + delayLenMax - (delayLen / 3 * 2));

        if (delayOut >= delayLenMax)
        {
            delayOut -= delayLenMax;
        }

        if (delayOut2 >= delayLenMax)
        {
            delayOut2 -= delayLenMax;
        }

        if (delayOut3 >= delayLenMax)
        {
            delayOut3 -= delayLenMax;
        }

        left[n] += mul(delayLine_l[delayOut], delayToMix);
        right[n] += mul(delayLine_l[delayOut2], delayToMix);
        left[n] += mul(delayLine_l[delayOut3], delayToMix);

        delayLine_l[delayOut2] += mul(delayLine_l[delayOut], delayFeedback);
        delayLine_l[delayIn] += mul(delayLine_l[delayOut3], delayFeedback);

        delayIn ++;

        if (delayIn >= delayLenMax)
        {
            delayIn = 0;
        }
    }
}


void Delay_Process_Buff2(float *signal_l, float *signal_r, int buffLen)
{

    for (int n = 0; n < buffLen; n++)
    {
        delayLine_l[delayIn] = (((float)0x8000) * signal_l[n] * delayInLvl);
        delayLine_r[delayIn] = (((float)0x8000) * signal_r[n] * delayInLvl);

        delayOut = delayIn + (1 + delayLenMax - delayLen);

        if (delayOut >= delayLenMax)
        {
            delayOut -= delayLenMax;
        }

        signal_l[n] += ((float)delayLine_l[delayOut]) * delayToMix / ((float)0x8000);
        signal_r[n] += ((float)delayLine_r[delayOut]) * delayToMix / ((float)0x8000);

        delayLine_l[delayIn] += (((float)delayLine_l[delayOut]) * delayFeedback);
        delayLine_r[delayIn] += (((float)delayLine_r[delayOut]) * delayFeedback);

        delayIn ++;

        if (delayIn >= delayLenMax)
        {
            delayIn = 0;
        }
    }
}

void Delay_SetInputLevel(uint8_t unused __attribute__((unused)), float value)
{
    delayInLvl = value;
    Status_ValueChangedFloat("Delay_SetInputLevel", value);
}

void Delay_SetFeedback(uint8_t unused __attribute__((unused)), float value)
{
    delayFeedback = value;
    Status_ValueChangedFloat("Delay_SetFeedback", value);
}

void Delay_SetFeedback(uint8_t unused __attribute__((unused)), uint8_t value)
{
    float value_f = value;
    value_f /= 127.0;
    delayFeedback = value_f;
    Status_ValueChangedFloat("Delay_SetFeedback", value);
}

void Delay_SetOutputLevel(uint8_t unused __attribute__((unused)), float value)
{
    delayToMix = value;
    Status_ValueChangedFloat("Delay_SetOutputLevel", value);
}

void Delay_SetOutputLevel(uint8_t unused __attribute__((unused)), uint8_t value)
{
    float value_f = value;
    value_f /= 127.0;
    delayToMix = value_f;
    Status_ValueChangedFloat("Delay_SetOutputLevel", value);
}

void Delay_SetLength(uint8_t unused __attribute__((unused)), float value)
{
    delayLen = (uint32_t)(((float)delayLenMax - 1.0f) * value);
    Status_ValueChangedFloat("Delay_SetLength", value);
}

void Delay_SetLength(uint8_t unused __attribute__((unused)), uint32_t value)
{
    if (value != delayLen)
    {
        delayLen = value < delayLenMax ? value : delayLenMax;
    }
    Status_ValueChangedInt("Delay_SetLength", value);
}

void Delay_SetShift(uint8_t unused __attribute__((unused)), float value)
{
    delayShift = value;
    Status_ValueChangedFloat("Delay_SetShift", value);
}
