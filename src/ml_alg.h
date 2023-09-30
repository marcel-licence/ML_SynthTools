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
 * @file ml_alg.h
 * @author Marcel Licence
 * @date 23.12.2022
 *
 * @brief This file contains some useful algorithms
 */


#ifndef SRC_ML_ALG_H_
#define SRC_ML_ALG_H_


#include <math.h>




static int16_t mul(int16_t a, float b);
static float mul_f(int16_t a, float b);



static inline
int16_t mul(int16_t a, float b)
{
    float c = a;
    c *= b;
    return (int16_t)c;
}


static inline
float mul_f(int16_t a, float b)
{
    float c = a;
    c *= b;
    c /= (float)0x4000;
    return c;
}

static inline
float lerpOut(int16_t *buffer, float idx, uint32_t len_max)
{
    uint32_t idxFloor = round(idx);

    if (idxFloor >= len_max)
    {
        idxFloor -= len_max;
    }

    return buffer[idxFloor];
}






#endif /* SRC_ML_ALG_H_ */

