/*
 * Copyright (c) 2022 Marcel Licence
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
 * @file ml_delay.h
 * @author Marcel Licence
 * @date 06.01.2022
 *
 * @brief This file contains an implementation of a simple reverb effect
 */


#ifndef SRC_ML_DELAY_H_
#define SRC_ML_DELAY_H_


#include <Arduino.h>


void Delay_Init(int16_t *buffer, uint32_t len);
void Delay_Init2(int16_t *left, int16_t *right, uint32_t len);
void Delay_Reset(void);
//void Delay_Process(float *signal_l, float *signal_r);
void Delay_Process_Buff(float *signal_l, int buffLen);
void Delay_Process_Buff2(float *signal_l, float *signal_r, int buffLen);
void Delay_SetInputLevel(uint8_t unused, float value);
void Delay_SetFeedback(uint8_t unused, float value);
void Delay_SetOutputLevel(uint8_t unused, float value);
void Delay_SetLength(uint8_t unused, float value);


#endif /* SRC_ML_DELAY_H_ */
