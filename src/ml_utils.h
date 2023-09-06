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
 * @file ml_utils.h
 * @author Marcel Licence
 * @date 02.09.2023
 *
 * @brief Some useful functions
 */


#ifndef SRC_ML_UTILS_H_
#define SRC_ML_UTILS_H_


#include <inttypes.h>


float maxAbsValueFromSampleBuffer(float *samples, uint16_t sample_count);
float maxValueFromSampleBuffer(float *samples, uint16_t sample_count);
float minValueFromSampleBuffer(float *samples, uint16_t sample_count);

float log16bit(float f);
float floatFromU7(uint8_t value);
float log2fromU7(uint8_t value, float minExp, float maxExp);
float log10fromU7(uint8_t value, float minExp, float maxExp);


#include <ml_types.h>


static inline
void mul(const float *in, float gain, float *out, uint32_t len)
{
    for (uint32_t n = 0; n < len; n++)
    {
        out[n] = gain * in[n];
    }
}

static inline
void mul(const Q1_14 *in, float gain, Q1_14 *out, uint32_t len)
{
    for (uint32_t n = 0; n < len; n++)
    {
        float val = (float)in[n].s16 * gain;
        out[n].s16 = val;
    }
}

static inline
void mixStereoToMono(const Q1_14 *in_l, const Q1_14 *in_r, float *mono_out, uint32_t len)
{
    for (uint32_t n = 0; n < len; n++)
    {
        mono_out[n] = (in_l[n].s16 * (0.5f / 16384.0f)) + (in_r[n].s16 * (0.5f / 16384.0f));
    }
}


#endif /* SRC_ML_UTILS_H_ */

