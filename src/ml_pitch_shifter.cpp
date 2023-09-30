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
 * @file ml_pitch_shifter.cpp
 * @author Marcel Licence
 * @date 01.09.2023
 *
 * @brief   PitchShifter implementation
 *
 * @see little demo: https://youtu.be/hqK_U22Jha8
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_pitch_shifter.h>

#include <ml_status.h>


#include <string.h>
#include <math.h>


#define i32_abs(x) ((x)>0?(x):-(x))


ML_PitchShifter::ML_PitchShifter(float sample_rate)
{
    this->sample_rate = sample_rate;
    inCnt = 0;
    outCnt = 0;
    speed = 1;
    depth = 1.0f;
    wetV = 1.0f;
    dryV = 0.0f;
    memset(buffer, 0, sizeof(buffer));
    feedback = 0.125f;
}


uint32_t minDistance(int32_t pointerA, int32_t pointerB)
{
    uint32_t distance1 = i32_abs(pointerA - pointerB);
    uint32_t distance2 = PITCH_SHIFTER_BUFFER_SIZE - distance1;
    return (distance1 < distance2) ? distance1 : distance2;
}


void ML_PitchShifter::ProcessHQ(const float *in, float *out, uint32_t count)
{
    ML_PitchShifter_ProcessHQ(in, buffer, out, count, speed, inCnt, outCnt, wetV, dryV, feedback);
}

void ML_PitchShifter::Process(const float *in, float *out, uint32_t count)
{
    const float speed_c = speed;
    for (uint32_t n = 0; n < count; n++)
    {
        float outCnt2 = outCnt + (PITCH_SHIFTER_BUFFER_SIZE / 2);
        if (outCnt2 >= PITCH_SHIFTER_BUFFER_SIZE)
        {
            outCnt2 -= PITCH_SHIFTER_BUFFER_SIZE;
        }

        buffer[inCnt] = in[n];
        uint32_t outU = floor(outCnt);
        uint32_t outU2 = floor(outCnt2);
        uint32_t diffU = minDistance(inCnt, outU);
        float diff = diffU;
        diff *= 1.0f / (PITCH_SHIFTER_BUFFER_SIZE / 2);
        float diffI = 1.0f - diff;
        out[n] = (diff * buffer[outU] + diffI * buffer[outU2]) * wetV + in[n] * dryV;

        buffer[inCnt] += feedback * out[n];

        inCnt++;
        if (inCnt >= PITCH_SHIFTER_BUFFER_SIZE)
        {
            inCnt -= PITCH_SHIFTER_BUFFER_SIZE;
        }

        outCnt += speed_c;
        outCnt++;
        if (outCnt >= PITCH_SHIFTER_BUFFER_SIZE)
        {
            outCnt -= PITCH_SHIFTER_BUFFER_SIZE;
        }
        if (outCnt < 0)
        {
            outCnt += PITCH_SHIFTER_BUFFER_SIZE;
        }
    }
}

void ML_PitchShifter::setDepth(float depth)
{
    this->depth = depth;
    Status_ValueChangedFloat("PitchShifter", "Depth", depth);
}

void ML_PitchShifter::setSpeed(float speed)
{
    this->speed = speed;
    Status_ValueChangedFloat("PitchShifter", "Speed", speed);
}

void ML_PitchShifter::setFeedback(float feedback)
{
    this->feedback = feedback;
    Status_ValueChangedFloat("PitchShifter", "Feedback", feedback);
}

/**
 * Works like a cross fader.
 * Mix = 0.0 : dry = 1.0f, wet = 0.0f
 * Mix = 0.5 : dry = 1.0f, wet = 1.0f
 * Mix = 1.0 : dry = 0.0f, wet = 1.0f
 *
 * @param mix
 * @param dry
 * @param wet
 */
void MixToDryWet(float mix, float *dry, float *wet)
{
    if (mix >= 0.5f)
    {
        *dry = (1.0f - mix) * 2.0f;
    }
    else
    {
        *dry = 1.0f;
    }

    if (mix <= 0.5f)
    {
        *wet = (mix) * 2.0f;
    }
    else
    {
        *wet = 1.0f;
    }
}

void ML_PitchShifter::setMix(float mix)
{
    MixToDryWet(mix, &dryV, &wetV);
    Status_ValueChangedFloat("PitchShifter", "Mix", mix);
}

