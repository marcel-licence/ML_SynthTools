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
 * @file    ml_multi_saw.h
 * @author  Marcel Licence
 * @date    24.03.2023
 *
 * @brief   You will find all current available functions to control the multi saw module below
 *
 * @see     https://youtu.be/kcf597op8o4
 */


#ifndef SYNTH_SUPER_SAW_H_
#define SYNTH_SUPER_SAW_H_


#include <ml_types.h>


void MultiSawSynth_Init(float sample_rate);
void MultiSawSynth_SetDetune(uint8_t value);
void MultiSawSynth_SetCount(uint8_t value);
void MultiSawSynth_Process(Q1_14 *ch_l, Q1_14 *ch_r, int sampleLen);
void MultiSawSynth_NoteOn(uint8_t ch, uint8_t note, uint8_t vel);
void MultiSawSynth_NoteOff(uint8_t ch, uint8_t note);
void MultiSawSynth_NoteSetPitch(uint8_t ch, uint8_t note, uint32_t pitch);
void MultiSawSynth_NoteSetVolume(uint8_t ch, uint8_t note, uint16_t volume);
void MultiSawSynth_PitchBend(uint8_t ch, uint16_t bend);
void MultiSawSynth_SetProfile(uint8_t value);
void MultiSawSynth_SetCurrentCh(uint8_t value);


#endif /* SYNTH_SUPER_SAW_H_ */

