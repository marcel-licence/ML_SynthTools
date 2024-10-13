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
 * @file    ml_slicer.h
 * @author  Marcel Licence
 * @date    24.03.2023
 *
 * @brief   A simple slicer effect (work still on progress)
 *
 * @see     https://youtu.be/kcf597op8o4
 */


#ifndef ML_SLICER_H_
#define ML_SLICER_H_


#include <ml_types.h>


void Slicer_Process(Q1_14 *ch_l, Q1_14 *ch_r, uint32_t sample_count);
uint32_t Slicer_GetLastMultiplier(void);
void Slicer_SetSpeed(uint8_t unused __attribute__((unused)), uint8_t value);
void Slicer_SetDepth(uint8_t unused __attribute__((unused)), uint8_t value);
void Slicer_SetType(uint8_t unused __attribute__((unused)), uint8_t value);
void Slicer_SetAttack(uint8_t unused __attribute__((unused)), uint8_t value);
void Slicer_SetDecay(uint8_t unused __attribute__((unused)), uint8_t value);


#endif /* ML_SLICER_H_ */
