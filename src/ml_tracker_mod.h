/*
 * Copyright (c) 2024 Marcel Licence
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
 * @file ml_tracker_mod.h
 * @author Marcel Licence
 * @data 17.02.2024
 *
 * @brief   Declarations of the mod tracker module
 */


#ifndef SRC_ML_TRACKER_MOD_H_
#define SRC_ML_TRACKER_MOD_H_


#include <stdint.h>
#include <ml_types.h>


uint32_t TrackerGetMemSize(void);
void TrackerSetBuffer(void *buffer);
void TrackerUseStaticBuffer(void);
bool Tracker_IsPlaying(void);
bool Tracker_HasTrackFinished(void);
bool TrackerLoadFile(void);
void TrackerSetup(uint32_t sample_rate);
void TrackerStartPlayback(void);
void TrackerProcess(uint64_t passed);
void TrackerProcessSamples(Q1_14 *chL, Q1_14 *chR, uint64_t count);
void TrackerProcessSamples(Q1_14 *ch1, Q1_14 *ch2, Q1_14 *ch3, Q1_14 *ch4, uint64_t count);
void TrackerProcessOutput(void);
void TrackerLoadData(const uint8_t *data);

void Tracker_NoteOn(uint8_t ch, uint8_t note, uint8_t vel);
void Tracker_NoteOff(uint8_t ch, uint8_t note, uint8_t vel __attribute__((unused)));
void Tracker_NoteOff(uint8_t ch, uint8_t note);

void Tracker_Restart(uint8_t param, uint8_t value);
void Tracker_Stop(uint8_t param, uint8_t value);
void Tracker_End(uint8_t param __attribute__((unused)), uint8_t value);
void Tracker_Start(uint8_t param, uint8_t value);
void Tracker_Step(uint8_t param, uint8_t value);

void Tracker_SetTempo(uint8_t param, uint8_t value);
void Tracker_SetPitch(uint8_t param, uint8_t value);
void Tracker_ToggleMute(uint8_t param, uint8_t value);

void TrackerGetClr(uint32_t *clr);


#endif /* SRC_ML_TRACKER_MOD_H_ */
