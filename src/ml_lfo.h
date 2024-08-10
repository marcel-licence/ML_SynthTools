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
 * @file ml_lfo.h
 * @author Marcel Licence
 * @date 01.09.2023
 *
 * @brief   LFO declarations
 *
 * @see little demo: https://youtu.be/hqK_U22Jha8 (more details coming soon)
 */


#ifndef SRC_ML_LFO_H_
#define SRC_ML_LFO_H_


#include <inttypes.h>


class ML_LFO
{
public:
    ML_LFO(float sample_rate, float *buffer, uint32_t sample_count);
    ~ML_LFO() {};
    void Process(const float *frequency_in, float *buffer, uint32_t len);
    void Process(const float *frequency_in, uint32_t len);
    void Process(uint32_t len);
    void setFrequency(float speed);
    void setPhase(float phase);

private:
    float sample_rate;
    float frequency;
    float phase;
    float *buffer;
    float bufferSize;
};


#endif /* SRC_ML_LFO_H_ */

