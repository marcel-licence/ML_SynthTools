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
 * @file ml_lfo.cpp
 * @author Marcel Licence
 * @date 01.09.2023
 *
 * @brief   LFO implementation
 *
 * @see little demo: https://youtu.be/hqK_U22Jha8 (more details coming soon)
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_lfo.h>
#include <math.h>
#include <string.h>


ML_LFO::ML_LFO(float sample_rate, float *buffer, uint32_t sample_count)
{
    this->sample_rate = sample_rate;
    frequency = 1.0f;
    phase = 0.0f;
    this->buffer = buffer;
    bufferSize = sample_count;
    memset(buffer, 0, sizeof(float) * sample_count);
}

void ML_LFO::Process(const float *frequency_in, float *buffer, uint32_t len)
{
    for (uint32_t n = 0; n < len; n++)
    {
        const float omega = frequency_in[n] * 2.0f * M_PI / (sample_rate);

        phase += omega;
        if (phase >= 2.0f * M_PI)
        {
            phase -= 2.0f * M_PI;
        }
        buffer[n] = sinf(phase);
    }
}

void ML_LFO::Process(const float *frequency_in, uint32_t len)
{
    for (uint32_t n = 0; n < len; n++)
    {
        const float omega = frequency_in[n] * 2.0f * M_PI / (sample_rate);

        phase += omega;
        if (phase >= 2.0f * M_PI)
        {
            phase -= 2.0f * M_PI;
        }
        buffer[n] = sinf(phase);
    }
}

void ML_LFO::Process(uint32_t len)
{
    const float omega = frequency * 2.0f * M_PI / (sample_rate);

    for (uint32_t n = 0; n < len; n++)
    {
        phase += omega;
        if (phase >= 2.0f * M_PI)
        {
            phase -= 2.0f * M_PI;
        }
        buffer[n] = sinf(phase);
    }
}

void ML_LFO::setFrequency(float frequency)
{
    this->frequency = frequency;
}

void ML_LFO::setPhase(float phase)
{
    this->phase = phase;
}

