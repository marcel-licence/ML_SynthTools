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
 * @file ml_vibrato.cpp
 * @author Marcel Licence
 * @date 01.09.2023
 *
 * @brief   Vibrato implementation
 *
 * @see little demo: https://youtu.be/hqK_U22Jha8
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_vibrato.h>

#include <ml_status.h>

#include <string.h>


ML_Vibrato::ML_Vibrato(float sample_rate)
{
    this->sample_rate = sample_rate;
    inCnt = 0;
    depth = 1.0f;
    depthInv = 0.0f;
    mod_multiplier = 0.5f * (VIBRATO_BUFFER_SIZE - 2);
    mod_multiplier_curr = mod_multiplier;
    memset(buffer, 0, sizeof(buffer));
}

void ML_Vibrato::ModMultiplierUpdate()
{
    if (mod_multiplier_curr > mod_multiplier)
    {
        mod_multiplier_curr --;
    }
    if (mod_multiplier_curr < mod_multiplier)
    {
        mod_multiplier_curr ++;
    }
}

void ML_Vibrato::Process(const float *in, const float *mod_in, float *out, uint32_t count)
{
    ModMultiplierUpdate();
    for (uint32_t n = 0; n < count; n++)
    {
        float mod = (1.0f + mod_in[n]) * mod_multiplier_curr;
        int outCnt = inCnt - mod;
        if (outCnt < 0)
        {
            outCnt += VIBRATO_BUFFER_SIZE;
        }

        buffer[inCnt] = in[n];
        out[n] = depth * buffer[outCnt] + depthInv * in[n];

        inCnt++;
        if (inCnt >= VIBRATO_BUFFER_SIZE)
        {
            inCnt -= VIBRATO_BUFFER_SIZE;
        }
    }
}

void ML_Vibrato::ProcessHQ(const float *in, const float *mod_in, float *out, uint32_t count)
{
    ModMultiplierUpdate();
    ML_Vibrato_ProcessHQ(in, mod_in, out, count, mod_multiplier_curr, inCnt, buffer, depth, depthInv);
}

void ML_Vibrato::setDepth(float depth)
{
    this->depth = depth;
    this->depthInv = 1.0f - depth;
    Status_ValueChangedFloat("Vibrato", "Depth", depth);
}

void ML_Vibrato::setIntensity(float intensity)
{
    mod_multiplier = 0.5f * (VIBRATO_BUFFER_SIZE - 2) * intensity;
    Status_ValueChangedFloat("Vibrato", "Intensity", depth);
}

