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
 * @file ml_reverb.h
 * @author Marcel Licence
 * @date 04.01.2022
 *
 * @brief This file contains an implementation of a simple reverb effect
 */


#ifndef SRC_ML_REVERB_H_
#define SRC_ML_REVERB_H_


#include <Arduino.h>


#ifdef REVERB_DIV
#define REV_MUL(a)  (a/REVERB_DIV)
#else
#define REV_MUL(a)  (a)
#endif

#define l_CB0 REV_MUL(3460)
#define l_CB1 REV_MUL(2988)
#define l_CB2 REV_MUL(3882)
#define l_CB3 REV_MUL(4312)
#define l_AP0 REV_MUL(480)
#define l_AP1 REV_MUL(161)
#define l_AP2 REV_MUL(46)


#define REV_BUFF_SIZE   (l_CB0 + l_CB1 + l_CB2 + l_CB3 + l_AP0 + l_AP1 + l_AP2)


void Reverb_Process(float *signal_l, int buffLen);
void Reverb_Process(const float *signal_l, float * out, int buffLen);
void Reverb_Setup(float *buffer);
void Reverb_SetLevel(uint8_t not_used, float value);
void Reverb_SetLevelInt(uint8_t not_used, uint8_t value);


#endif /* SRC_ML_REVERB_H_ */
