/*
 * Copyright (c) 2025 Marcel Licence
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
 * @file ml_phaser.cpp
 * @author Marcel Licence
 * @date 20.12.2022
 *
 * @brief This is a simple implementation of a stereo s16 phaser line
 * - level adjustable
 * - feedback
 * - length adjustable
 * @see first peak: https://youtu.be/Kac9AB02BcQ
 * @see little demo: https://youtu.be/hqK_U22Jha8
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_phaser.h>
#include <ml_phaser_p.h> /* required library: ML SynthTools Libraries from https://github.com/marcel-licence/ML_SynthTools_Lib */
#include <ml_alg.h>
#include <ml_status.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/*
 * module variables
 */
static int16_t *phaserLine_l;
static int16_t *phaserLine_r;

static float phaserToMix = 0;
static float phaserInLvl = 1.0f;
static float phaserAuto = 1.0f;
static float phaserDepth = 1.0f;
static uint32_t phaserLenMax = 0;
static uint32_t phaserLen = 11098;


static struct allpass_s ap0 =
{
    NULL,
    0,
    0.7f,
    (int)(0)
};

static uint32_t AllpassInit(float *buffer, int i, struct allpass_s *ap, int len)
{
    ap->buf = &buffer[i];
    ap->p = 0.0f;
    ap->g = 0.00025f;
    ap->lim = len;
    return len;
}

static inline void Do_AllpassPhase(struct allpass_s *ap, const float *inSample, const float *len, float *outSample, int buffLen)
{
    struct allpass_s ap2;
    memcpy(&ap2, ap, sizeof(ap2));

    for (int n = 0; n < buffLen; n++)
    {
        float readback = ap2.buf[(uint32_t)ap2.p];
        readback += (-ap2.g) * inSample[n];
        const float newV = readback * ap2.g + inSample[n];
        ap2.buf[(uint32_t)ap2.p] = newV;
        ap2.p++;
        if (ap2.p >= len[n])
        {
            ap2.p -= len[n];
        }
        outSample[n] = readback;
    }
    memcpy(ap, &ap2, sizeof(ap2));
}

void Phaser_Init(float *buffer, uint32_t len)
{
    if (buffer == NULL)
    {
        printf("No memory to initialize Phaser!\n");
        return;
    }
    else
    {
        memset(buffer, 0, sizeof(float) * PHASER_BUFFER_SIZE);
    }
    uint32_t i = 0;


    i += AllpassInit(buffer, i, &ap0, PHASER_AP0);

    if (i == len)
    {
        printf("Phaser Init ok\n");
    }

}

void Phaser_Reset(void)
{
    for (uint32_t i = 0; i < phaserLenMax; i++)
    {
        phaserLine_l[i] = 0;
    }
    if (phaserLine_r != NULL)
    {
        for (uint32_t i = 0; i < phaserLenMax; i++)
        {
            phaserLine_r[i] = 0;
        }
    }
}

void Phaser_Process(const float *in, const float *lfo_in, float *out, int buffLen)
{
    float newsample[buffLen];
    float len1[buffLen];
    float phaserDepth_c = phaserDepth;
    float phaserMod_c = 1.0f;

    for (int n = 0; n < buffLen; n++)
    {
        /* causing errors when lfo_in == 0 */
        len1[n] = 3 + (1 + lfo_in[n]) * phaserMod_c * 48.0f;
    }

    memcpy(newsample, in, sizeof(newsample));

    Do_AllpassPhase(&ap0, newsample, len1, newsample, buffLen);

    for (int n = 0; n < buffLen; n++)
    {
        out[n] = in[n] - newsample[n] * phaserDepth_c;
    }
}

void Phaser_ProcessHQ(const float *in, const float *lfo_in, float *out, int buffLen)
{
    Phaser_ProcessHQ(in, lfo_in, out, buffLen, phaserDepth, & ap0);
}

void Phaser_SetInputLevel(uint8_t unused __attribute__((unused)), float value)
{
    phaserInLvl = value;
    Status_ValueChangedFloat("Phaser", "InputLevel", value);
}

void Phaser_SetDepth(uint8_t unused __attribute__((unused)), float value)
{
    phaserDepth = value;
    Status_ValueChangedFloat("Phaser", "Depth", value);
}

void Phaser_SetDepth(uint8_t unused __attribute__((unused)), uint8_t value_u8)
{
    float value = (1.0f / 127.0f) * (float)value_u8;
    phaserDepth = value;
    Status_ValueChangedFloat("Phaser", "Depth", value);
}

void Phaser_SetAuto(uint8_t unused __attribute__((unused)), float value)
{
    phaserAuto = value;
}

void Phaser_SetOutputLevel(uint8_t unused __attribute__((unused)), float value)
{
    phaserToMix = value;
    Status_ValueChangedFloat("Phaser", "Level", value);
}

void Phaser_SetG(uint8_t unused __attribute__((unused)), float value)
{
    ap0.g = value;
    Status_ValueChangedFloat("Phaser", "G", value);
}

void Phaser_SetG(uint8_t unused __attribute__((unused)), uint8_t value_u8)
{
    float value = (1.0f / 127.0f) * (float)value_u8;
    ap0.g = value;
    Status_ValueChangedFloat("Phaser", "G", value);
}

void Phaser_SetLength(uint8_t unused __attribute__((unused)), float value)
{
    phaserLen = (uint32_t)(((float)phaserLenMax - 1.0f) * value);
}

