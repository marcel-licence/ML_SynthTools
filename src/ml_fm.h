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
 * @file    ml_fm.h
 * @author  Marcel Licence
 * @date    15.09.2024
 *
 * @brief This is a simple implementation of an FM Synthesizer module
 *
 * - similar behavior as the YM2612 (known from Sega Mega Drive / Sega Genesis)
 * - polyphonic playback of voices with 4 operators
 * - AR DR1 D2L DR2 RR (adsr) envelope
 * - feedback (not completely like the original YM2612)
 * - level adjustable
 * - feedback
 * - length adjustable
 *
 *
 * @see https://youtu.be/rGTw05GKwvU
 */


#ifndef SRC_ML_FM_H_
#define SRC_ML_FM_H_


#include <inttypes.h>


class SineSynth
{
public:
    SineSynth();

    void Progress(int16_t *out, int len);
    void Midi_NoteOn(uint8_t ch, uint8_t note, uint8_t vel);
    void Midi_NoteOff(uint8_t ch, uint8_t note);

private:
    float sinePos;
    float f;
};


void FmSynth_Init(float sample_rate_in);
void FmSynth_Process(const float *in, float *out, int bufLen);
void FmSynth_NoteOn(uint8_t ch, uint8_t note, float vel);
void FmSynth_NoteOff(uint8_t ch, uint8_t note);
void FmSynth_PitchBend(uint8_t ch, float bend);
void FmSynth_ModulationWheel(uint8_t ch, float value);
void FmSynth_MegaFM_CtrlChange(uint8_t ctrl, uint8_t value);

void FmSynth_ChannelSettingDump(uint8_t ch __attribute__((unused)), float value);
void FmSynth_ChannelSettingInit(uint8_t ch, float value);
void FmSynth_ToggleMono(uint8_t param __attribute__((unused)), float value);
void FmSynth_ToggleLegato(uint8_t param __attribute__((unused)), float value);
void FmSynth_SelectOp(uint8_t param, float value);
void FmSynth_SetAlgorithm(uint8_t param, float value);
void FmSynth_ChangeParam(uint8_t param, float value);
void FmSynth_VelToLev(uint8_t unused __attribute__((unused)), float value);
void FmSynth_LfoAM(uint8_t unused __attribute__((unused)), float value);
void FmSynth_LfoFM(uint8_t unused __attribute__((unused)), float value);
void FmSynth_Attack(uint8_t unused __attribute__((unused)), float value);
void FmSynth_Decay1(uint8_t unused __attribute__((unused)), float value);
void FmSynth_DecayL(uint8_t unused __attribute__((unused)), float value);
void FmSynth_Decay2(uint8_t unused __attribute__((unused)), float value);
void FmSynth_Release(uint8_t unused __attribute__((unused)), float value);
void FmSynth_Feedback(uint8_t unused __attribute__((unused)), float value);


#endif /* SRC_ML_FM_H_ */
