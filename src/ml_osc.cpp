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
 * @file ml_osc.cpp
 * @author Marcel Licence
 * @date 16.01.2022
 *
 * @brief This file contains the implementation of an oscillator.
 * The implementation is not cleaned up in the moment but working and tested.
 *
 * @see First time used here: https://youtu.be/WJGOIgaY-1s
 */


#include "ml_osc.h"
#include "ml_waveform.h"


static void OscProcessSingle(oscillatorT *osc, uint32_t len);


/* will be removed in future */
extern float sine[WAVEFORM_CNT];
extern float saw[WAVEFORM_CNT];

static void OscProcessSingle(oscillatorT *osc, uint32_t len)
{
    for (uint32_t n = 0U; n < len; n++)
    {
        osc->samplePos += (uint32_t)((*osc->cfg->pitchMultiplier) * ((float)osc->addVal) * osc->cfg->pitchOctave * osc->cfg->pitch * *osc->pitchMod);
        uint32_t samplePos = osc->samplePos;
#if 1
        float morphMod = /*(*osc->cfg->morph) * */ osc->cfg->morphWaveForm[WAVEFORM_I(osc->samplePos)];
        morphMod *= ((float)89478480);
        morphMod *= (*osc->cfg->morph) * 64;
        samplePos += morphMod;

#endif
        float sig = osc->cfg->selectedWaveForm[WAVEFORM_I(samplePos)];
#if 0
        float sig = 0;
        if (saw == NULL)
        {
            Serial.printf("saw not ready!\n");
        }
        else
        {
            sig = sine[WAVEFORM_I(osc->samplePos)];
            sig *= osc->cfg->volume;
        }
#endif
        sig *= osc->cfg->volume;

#if 0
        osc->dest[0][n] += osc->pan_l * sig;
        osc->dest[1][n] += osc->pan_r * sig;
#else
        osc->dest[0][n] += sig;
        osc->dest[1][n] += sig;
#endif
    }
}

void OscProcess(oscillatorT *osc, int cnt, uint32_t len)
{
    for (int i = 0; i < cnt; i++)
    {
        OscProcessSingle(&osc[i], len);
    }
}
