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
 * @file ml_pitch_shifter.h
 * @author Marcel Licence
 * @date 01.09.2023
 *
 * @brief   PitchShifter declarations
 *
 * @see little demo: https://youtu.be/hqK_U22Jha8
 */


#ifndef SRC_ML_PITCH_SHIFTER_H_
#define SRC_ML_PITCH_SHIFTER_H_


#include <stdint.h>
#include <math.h>


#include <ml_types.h>
#include <inttypes.h>


#define PITCH_SHIFTER_BUFFER_SIZE   (1024*8)


class ML_PitchShifter
{
public:
    ML_PitchShifter(float sample_rate);
    ~ML_PitchShifter() {};
    void Process(const float *in, float *out, uint32_t count);
    void ProcessHQ(const float *in, float *out, uint32_t count);
    void setDepth(float depth);
    void setSpeed(float speed);
    void setMix(float mix);
    void setFeedback(float feedback);

private:
    float sample_rate;
    float depth;
    float buffer[PITCH_SHIFTER_BUFFER_SIZE];
    int32_t inCnt;
    float outCnt;
    float speed;
    float dryV;
    float wetV;
    float feedback;
};


void ML_PitchShifter_ProcessHQ(const float *in, float *buffer, float *out, uint32_t count, float speed, int32_t &inCnt, float &outCnt, float wetV, float dryV, float feedback);


#endif /* SRC_ML_PITCH_SHIFTER_H_ */

