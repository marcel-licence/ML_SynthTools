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
 * @file ml_utils.cpp
 * @author Marcel Licence
 * @date 02.09.2023
 *
 * @brief Some useful functions
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include <ml_utils.h>

#include <math.h>
#include <inttypes.h>


float maxAbsValueFromSampleBuffer(float *samples, uint16_t sample_count)
{
    float absMax = 0;

    /* find max value */
    for (int n = 0; n < sample_count; n++)
    {
        absMax = samples[n] > absMax ? samples[n] : absMax;
        absMax = -samples[n] > absMax ? -samples[n] : absMax;
    }

    return absMax;
}

float maxValueFromSampleBuffer(float *samples, uint16_t sample_count)
{
    float maxValue = 0;

    /* find max value */
    for (int n = 0; n < sample_count; n++)
    {
        maxValue = samples[n] > maxValue ? samples[n] : maxValue;
    }

    return maxValue;
}

float minValueFromSampleBuffer(float *samples, uint16_t sample_count)
{
    float minValue = 0;

    /* find max value */
    for (int n = 0; n < sample_count; n++)
    {
        minValue = samples[n] < minValue ? samples[n] : minValue;
    }

    return minValue;
}

/* convert a value between 0..1 to log scale with min at 16 bit 1/0 */
float log16bit(float f)
{
    if (f < (1.0 / 65535.0f))
    {
        return 0;
    }
    else if (f > 1.0f)
    {
        return 1.0f;
    }
    else
    {
        float range = -((log(1) / log(10) * 20) - (log(0xFFFF) / log(10) * 20));
        f *= 65535.0f;
        f = (range + log(f / 65535.0f) / log(10) * 20) / range;
        return f;
    }
}

float floatFromU7(uint8_t value)
{
    float f = value;
    f *= (1.0f / 127.0f);
    return f;
}

float log2fromU7(uint8_t value, float minExp, float maxExp)
{
    float f = floatFromU7(value);
    float minV = minExp;
    float maxV = maxExp;
    f *= maxV - minV;
    f += minV;
    return pow(2, f);
}

float log10fromU7(uint8_t value, float minExp, float maxExp)
{
    float f = floatFromU7(value);
    float minV = minExp;
    float maxV = maxExp;
    f *= maxV - minV;
    f += minV;
    return pow(10, f);
}

float log(float base, float value)
{
    return log(value) / log(base);
}

float log10fromU7val(uint8_t value, float minVal, float maxVal)
{
    float f = floatFromU7(value);
    float minV = log(10, minVal);
    float maxV = log(10, maxVal);
    f *= maxV - minV;
    f += minV;
    return pow(10, f);
}

