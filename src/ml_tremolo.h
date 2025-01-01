/*
 * Copyright (c) 2025 Marcel Licence
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
 * @file ml_tremolo.h
 * @author Marcel Licence
 * @date 08.12.2022
 *
 * @brief   Tremolo stereo implementation
 *
 * @see little demo: https://youtu.be/zu2xtRKlNVU
 */


#ifndef SRC_ML_TREMOLO_H_
#define SRC_ML_TREMOLO_H_


#include <stdint.h>
#include <math.h>


#include <ml_types.h>
#include <stdint.h>


class ML_Tremolo
{
public:
    ML_Tremolo(float sample_rate);
    ~ML_Tremolo() {};
    void Process(const float *in_l, const float *in_r, const float mod_in, float *out_l, float *out_r, uint32_t count);
    void Process(const float *in_l, const float *in_r, const float *mod_in, float *out_l, float *out_r, uint32_t count);
    void process(float *left, float *right, int32_t len);
    void setSpeed(float speed);
    void setPhaseShift(float shift);
    void setDepth(float new_depth);

private:
    float sample_rate;
    float speed;
    float phase_shift;
    float value;
    float depth;
    float depthInv;

    uint32_t speedU32;
    uint32_t valueU32;
};


class ML_TremoloQ
{
public:
    ML_TremoloQ() {};
    ~ML_TremoloQ() {};
    void init(float sample_rate);
    void process(Q1_14 *left, Q1_14 *right, uint32_t len) __attribute__((deprecated));
    void setSpeed(float speed);
    void setPhaseShift(float shift);
    void updatePhaseShift();
    void setDepth(float new_depth);
};


#endif /* SRC_ML_TREMOLO_H_ */
