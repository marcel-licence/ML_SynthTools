/*
 * Copyright (c) 2022 Marcel Licence
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
 * @file ml_reverb.cpp
 * @author Marcel Licence
 * @date 04.01.2022
 *
 * @brief This file contains an implementation of a simple reverb effect
 *
 * This is a optimized version using the algorithm of the public code from YetAnotherElectronicsChannel
 * @see: https://github.com/YetAnotherElectronicsChannel/STM32_DSP_Reverb/blob/master/code/Src/main.c
 *
 * The explanation of the original module can be found here: https://youtu.be/nRLXNmLmHqM
 *
 * Changes to its original:
 * - ported into an arduino project
 * - optimized for buffer processing
 * - added interface to set the level
 * - reduced the code size
 * - put this into a library to allow using it in different projects
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_reverb.h>


//define wet 0.0 <-> 1.0
//float wet = 1.0f;
//define time delay 0.0 <-> 1.0 (max)
static float rev_time = 1.0f;
static float rev_level = 0.0f;

//define pointer limits = delay time

struct comb_s
{
    float *buf;
    int p;
    float g;
    int lim;
};

static struct comb_s cf0 =
{
    NULL,
    0,
    0.805f,
    (int)(rev_time * l_CB0),
};

static struct comb_s cf1 =
{
    NULL,
    0,
    0.827f,
    (int)(rev_time * l_CB1),
};

static struct comb_s cf2 =
{
    NULL,
    0,
    0.783f,
    (int)(rev_time * l_CB2),
};

static struct comb_s cf3 =
{
    NULL,
    0,
    0.764f,
    (int)(rev_time * l_CB3),
};

static void Do_Comb(struct comb_s *cf, const float *inSample, float *outSample, int buffLen)
{
    for (int n = 0; n < buffLen; n++)
    {
        float readback = cf->buf[cf->p];
        float newV = readback * cf->g + inSample[n];
        cf->buf[cf->p] = newV;
        cf->p++;
        if (cf->p >= cf->lim)
        {
            cf->p = 0;
        }
        outSample[n] += readback;
    }
}

struct allpass_s
{
    float *buf;
    int p;
    float g;
    int lim;
};

static struct allpass_s ap0 =
{
    NULL,
    0,
    0.7f,
    (int)(rev_time * l_AP0)
};

static struct allpass_s ap1 =
{
    NULL,
    0,
    0.7f,
    (int)(rev_time * l_AP1)
};

static struct allpass_s ap2 =
{
    NULL,
    0,
    0.7f,
    (int)(rev_time * l_AP2)
};

static void Do_Allpass(struct allpass_s *ap, float *inSample, int buffLen)
{
    for (int n = 0; n < buffLen; n++)
    {
        float readback = ap->buf[ap->p];
        readback += (-ap->g) * inSample[n];
        float newV = readback * ap->g + inSample[n];
        ap->buf[ap->p] = newV;
        ap->p++;
        if (ap->p >= ap->lim)
        {
            ap->p = 0;
        }
        inSample[n] = readback;
    }
}

void Reverb_Process(float *signal_l, int buffLen)
{
    float inSample[buffLen];
    for (int n = 0; n < buffLen; n++)
    {
        /* create mono sample */
        inSample[n] = signal_l[n]; /* it may cause unwanted audible effects */
    }
    float newsample[buffLen];
    memset(newsample, 0, sizeof(newsample));
    Do_Comb(&cf0, inSample, newsample, buffLen);
    Do_Comb(&cf1, inSample, newsample, buffLen);
    Do_Comb(&cf2, inSample, newsample, buffLen);
    Do_Comb(&cf3, inSample, newsample, buffLen);
    for (int n = 0; n < buffLen; n++)
    {
        newsample[n] *= 0.25f;
    }

    Do_Allpass(&ap0, newsample, buffLen);
    Do_Allpass(&ap1, newsample, buffLen);
    Do_Allpass(&ap2, newsample, buffLen);

    /* apply reverb level */
    for (int n = 0; n < buffLen; n++)
    {
        newsample[n] *= rev_level;
        signal_l[n] += newsample[n];
    }
}

static int CombInit(float *buffer, int i, struct comb_s *cf, int len)
{
    cf->buf = &buffer[i];
    cf->lim = (int)(rev_time * len);
    return len;
}

static int AllpassInit(float *buffer, int i, struct allpass_s *ap, int len)
{
    ap->buf = &buffer[i];
    ap->lim = (int)(rev_time * len);
    return len;
}

void Reverb_Setup(float *buffer)
{
    if (buffer == NULL)
    {
        Serial.printf("No memory to initialize Reverb!\n");
        return;
    }
    else
    {
        memset(buffer, 0, sizeof(float) * REV_BUFF_SIZE);
    }
    int i = 0;

    i += CombInit(buffer, i, &cf0, l_CB0);
    i += CombInit(buffer, i, &cf1, l_CB1);
    i += CombInit(buffer, i, &cf2, l_CB2);
    i += CombInit(buffer, i, &cf3, l_CB3);

    i += AllpassInit(buffer, i, &ap0, l_AP0);
    i += AllpassInit(buffer, i, &ap1, l_AP1);
    i += AllpassInit(buffer, i, &ap2, l_AP2);

#if 0
    cf0.buf = &buffer[i];
    cf0.lim = (int)(rev_time * l_CB0);
    i += l_CB0;

    cf1.buf = &buffer[i];
    cf1.lim = (int)(rev_time * l_CB1);
    i += l_CB1;

    cf2.buf = &buffer[i];
    cf2.lim = (int)(rev_time * l_CB2);
    i += l_CB2;

    cf3.buf = &buffer[i];
    cf3.lim = (int)(rev_time * l_CB3);
    i += l_CB3;

    ap0.buf = &buffer[i];
    ap0.lim = (int)(rev_time * l_AP0);
    i += l_AP0;

    ap1.buf = &buffer[i];
    ap1.lim = (int)(rev_time * l_AP1);
    i += l_AP1;

    ap2.buf = &buffer[i];
    ap2.lim = (int)(rev_time * l_AP2);
    i += l_AP2;
#endif
    Serial.printf("rev: %d, %d\n", i, REV_BUFF_SIZE);
    if (i != REV_BUFF_SIZE)
    {
        Serial.printf("Error during initialization of Reverb!\n");
    }
    else
    {
        Serial.printf("Reverb is ready!\n");
    }
}

void Reverb_SetLevel(uint8_t not_used __attribute__((unused)), float value)
{
    rev_level = value;
    //Status_ValueChangedFloat("ReverbLevel", rev_level);
}
