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
 * @file    ml_status.h
 * @author  Marcel Licence
 * @date    06.01.2022
 *
 * @brief   This file contains the prototypes of functions for status messages generation
 */


#ifndef SRC_ML_STATUS_H_
#define SRC_ML_STATUS_H_


#include <inttypes.h>


void Status_Init(uint32_t sample_rate);
void Status_ValueChangedFloat(const char *group, const char *descr, float value);
void Status_ValueChangedFloat(const char *descr, float value);
void Status_ValueChangedFloatArr(const char *descr, float value, int index);
void Status_ValueChangedIntArr(const char *descr, int value, int index);
void Status_ValueChangedInt(const char *group, const char *descr, int value);
void Status_ValueChangedInt(const char *descr, int value);
void Status_ValueChangedStr(const char *descr, const char *value);
void Status_ValueChangedStr(const char *group, const char *descr, const char *value);
void Status_LogMessage(const char *text);


#ifdef STATUS_SIMPLE
void Status_Loop(uint32_t elapsed_time);
void Status_LoopMain();
#endif


#endif /* SRC_ML_STATUS_H_ */
