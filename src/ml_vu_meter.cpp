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
 * @file ml_vu_meter.cpp
 * @author Marcel Licence
 * @date 01.01.2021
 *
 * @brief   This is a little helper for a simple vu meter
 */


#include <ml_vu_meter.h>


#define VU_METER_DECREASE_MULTIPLIER 0.98f /* this controls how fast the vu meter falls over time */
#define VU_METER_COUNT  2 /* we support two channels */

/* 2 storages to allow double buffering */
static float _vuMeterValueInBf[VU_METER_COUNT];
static float _vuMeterValueDisp[VU_METER_COUNT];

static float _vuVal[VU_METER_COUNT];

void VuMeter_Init(void)
{
    VuMeter_Process();
}

inline float ValToDb(float in)
{
    float out = 0.0f;
    if (in > 1.0f)
    {
        out = 8.0f;
    }
    else if (in > 0.0f)   /* log would crash if you put in zero */
    {
        out = 8 + (3.32f * log10(in));
    }

    if (out < 0)
    {
        out = 0;
    }

    return out;
}

void VuMeter_Process(void)
{
    memcpy(_vuMeterValueDisp, _vuMeterValueInBf, sizeof(_vuMeterValueDisp));
    for (int i = 0; i < 2; i++)
    {
        _vuMeterValueInBf[i] *= VU_METER_DECREASE_MULTIPLIER;
        _vuMeterValueDisp[i] = ValToDb(_vuMeterValueInBf[i]);
        _vuVal[i] = _vuMeterValueDisp[i] * 0.125f;
    }
}

#define ABS_F(a) ((a>0)?(a):(-a))

void VuMeter_PutSamples(float *left, float *right, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (ABS_F(left[i]) > _vuMeterValueInBf[0])
        {
            _vuMeterValueInBf[0] = ABS_F(left[i]);
        }
        if (ABS_F(right[i]) > _vuMeterValueInBf[1])
        {
            _vuMeterValueInBf[1] = ABS_F(right[i]);
        }
    }
}

float getVuMeterVal(uint8_t idx)
{
    return _vuVal[idx];
}

