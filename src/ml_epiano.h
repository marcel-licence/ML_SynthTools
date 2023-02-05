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
 * @file ml_epiano.h
 * @author Marcel Licence
 * @date 02.11.2022
 *
 * @brief   This file contains all declarations to use the e-piano module
 *          It is pre-compiled and only linking is required (done by Arduino)
 *
 *          A short demonstration can be found here:
 * @see     https://youtu.be/m12w1Xtm5Ts
 */

#ifndef EPIANO_H_
#define EPIANO_H_


#include <stdint.h>


#include <ml_types.h>


class ML_EPiano
{
public:
    ML_EPiano();
    ~ML_EPiano() {};

    void Init(float sample_rate);

    void setSampleRate(float sample_rate);

    void Process(Q1_14 *samples, int sampleLen);
    void Process(float *samples, int sampleLen);

    void NoteOn(uint8_t ch, uint8_t note, float vel);
    void NoteOff(uint8_t ch, uint8_t note);
    void Sustain(float sustain); /*! sustain amount, you can also use 1.0 and 0.0 */

    void SetEffectSpeed(float speed); /*! speed for tremolo and modulation in Hz */
    void PitchBend(float bend); /*!< pitch bend -> -1.0 to 1.0 for -2 semitones to 2 semitones */
    void SetCurve(float value); /*!< this is one of two sound modifiers, it will take a short time to recalcute the sound-engine */
    void SetCurve2(float value); /*!< this is one of two sound modifiers, it will take a short time to recalcute the sound-engine */
    void CalcCurvePreset1(void); /*!< using a default preset */

    void SetCenterTuneA(float frequency); /*!< global tuning, use 220Hz or 880Hz to go one octave down or up */

    void SetQuickDamping(float value);
    void SetQuickDampingThr(float value);

    void SetModulationDepth(float value);
};


#endif /* EPIANO_H_ */
