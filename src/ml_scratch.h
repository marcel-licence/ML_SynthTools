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
 * @file ml_scratch.cpp
 * @author Marcel Licence
 * @date 20.10.2024
 *
 * @brief  Declarations of simple sample playback functions used for scratching
 * @see https://www.youtube.com/watch?v=Ml6VrlV3hvk
 */

#ifndef ML_SCRATCH_H_
#define ML_SCRATCH_H_


#include <stdint.h>
#include <ml_types.h>


void Scratch_Init(float sample_rate);
bool Scratch_AddSampleStatic(const uint8_t *data, uint32_t size, uint8_t idx);
void Scratch_Process(Q1_14 *samples_l, Q1_14 *samples_r, uint32_t len);
void Scratch_SetPitchAbs(float pitch, uint8_t idx);


#endif /* ML_SCRATCH_H_ */
