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
 * @file ml_osc.h
 * @author Marcel Licence
 * @date 16.01.2022
 *
 * @brief This file contains the declarations and structures of the oscillator module
 *
 * @see First time used here: https://youtu.be/WJGOIgaY-1s
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#ifndef ML_OSC_H_
#define ML_OSC_H_


#include <inttypes.h>


struct synth_osc_cfg_s
{
    uint8_t pitchOctave; /* multiplier to go to higher octaves */
    float *pitchMultiplier;
    uint8_t waveform_idx;
    float volume;
    float pitch;
    float pitchModulationMix;
    float *selectedWaveForm;
    float *morphWaveForm;

    float *morph; /* points source of morph value */
};

struct oscillatorT
{
    float **waveForm;
    float *dest[2];
    uint32_t samplePos;
    uint32_t addVal;
    float pan_l;
    float pan_r;
    float *pitchMod;
    struct synth_osc_cfg_s *cfg;
};


void OscProcess(oscillatorT *osc, int cnt, uint32_t len);


#endif /* ML_OSC_H_ */
