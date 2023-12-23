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

/*
 *  @file   ml_sampler.h
 *  @author Marcel Licence
 *  @date   03.06.2023
 *
 *  @brief  This file includes prototypes for a multiplatform sampler module
 */

#ifndef SRC_ML_SAMPLER_H_
#define SRC_ML_SAMPLER_H_


#include <stdint.h>
#include <ml_types.h>



#define SAMPLER_PARAM_INSTRUMENT_IDX    0
#define SAMPLER_PARAM_HOLD              1
#define SAMPLER_PARAM_RELEASE           2
#define SAMPLER_PARAM_LOOP_MODE         3


void Sampler_Init(uint32_t sample_rate);

void Sampler_SetSampleBuffer(uint8_t *buffer, uint32_t buffer_len);
void Sampler_UseStaticBuffer(Q1_14 *buffer, uint32_t count);

uint32_t Sampler_GetFreeSpace(void);
uint32_t Sampler_GetUsedSpace(void);
uint32_t Sampler_GetMaxSpace(void);

void Sampler_NoteOn(uint8_t ch, uint8_t note, uint8_t vel);
void Sampler_NoteOff(uint8_t ch, uint8_t note, uint8_t vel __attribute__((unused)));
void Sampler_NoteOff(uint8_t ch, uint8_t note);
void Sampler_AllNotesOff(void);
void Sampler_PitchBend(uint8_t ch, uint16_t bend);

void Sampler_ChangeParameter(uint8_t parameter, float value);
void Sampler_ChangeParameter(uint8_t parameter, uint8_t value);
void Sampler_ChangeParameterSample(uint8_t parameter, float value);
void Sampler_ChangeParameterSample(uint8_t parameter, uint8_t value);
void Sampler_ProgramChange(uint8_t channel, uint8_t program_number);

void Sampler_StartTransfer(void);
void Sampler_EndTransfer(void);
void Sampler_InstrumentDone(void);

bool Sampler_NewSample(void);
bool Sampler_AddSamples(Q1_14 *samples, uint32_t sample_count);
bool Sampler_AddSamplesU8(uint8_t *samples, uint32_t sample_count);
void Sampler_NewSampleSetRange(uint32_t start, uint32_t end);
void Sampler_NewSampleSetLoop(uint32_t start, uint32_t end);
void Sampler_SetLoopMode(uint8_t mode);
void Sampler_SetPitch(uint8_t originalPitch, uint32_t sampleRate, int8_t pitchCorrection);
void Sampler_SetKeyRange(uint8_t keyLowest, uint8_t keyHighest);
void Sampler_SetVelRange(uint8_t velLowest, uint8_t velHighest);
void Sampler_SetExclusiveClass(uint8_t ex_class);
void Sampler_SetRelease(uint32_t release);
void Sampler_SetHold(uint32_t hold);
void Sampler_FinishSample(void);

void Sampler_TuneCoarse(uint8_t unused, uint8_t value);
void Sampler_TuneFine(uint8_t unused, uint8_t value);
void Sampler_LoopEntireSample(uint8_t unused, uint8_t value);

void Sampler_ClearAllSamples(void);

void Sampler_DebugInt(const char *str, uint32_t num);
void Sampler_DebugFloat(const char *str, float num);

uint8_t Sampler_GetSampleCount(void);

void Sampler_DecSample(void);
void Sampler_IncSample(void);

void Sampler_Process(Q1_14 *left, Q1_14 *right, uint32_t count);


#endif /* SRC_ML_SAMPLER_H_ */

