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
 * @file ml_vibrato.h
 * @author Marcel Licence
 * @date 01.09.2023
 *
 * @brief   Vibrato declarations
 *
 * @see little demo: https://youtu.be/hqK_U22Jha8
 */


#ifndef SRC_ML_VIBRATO_H_
#define SRC_ML_VIBRATO_H_


#include <stdint.h>
#include <math.h>


#include <ml_types.h>
#include <inttypes.h>


#define VIBRATO_BUFFER_SIZE    1024


class ML_Vibrato
{
public:
    ML_Vibrato(float sample_rate);
    ~ML_Vibrato() {};
    void Process(const float *in, const float *mod_in, float *out, uint32_t count);
    void ProcessHQ(const float *in, const float *mod_in, float *out, uint32_t count);
    void setDepth(float depth);
    void setIntensity(float intensity);

private:
    void ModMultiplierUpdate();

    float sample_rate;
    float depth;
    float depthInv;
    float buffer[VIBRATO_BUFFER_SIZE];
    float mod_multiplier;
    float mod_multiplier_curr;
    int32_t inCnt;
};


#endif /* SRC_ML_VIBRATO_H_ */

