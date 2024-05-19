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
 * @file h
 * @author Marcel Licence
 * @date 01.01.2022
 *
 * @brief This file contains an implementation of a little fast scope which
 * is optimized to create steady images
 *
 * In this version it uses the default Wire connection to talk to an OLED display
 * It is written to be used with up to two 128x64 OLED displays
 * The left channel will be output on i2c addr 0x3C and the right on i2c addr 0x3D
 */


#ifndef SRC_ML_SCOPE_H_
#define SRC_ML_SCOPE_H_


/*
 * includes
 */
#include <stdint.h>
#include <Wire.h>

#define ML_SCOPE_OLED

/*
 * function declarations
 */
void ScopeOled_Setup(void);
#ifdef ESP32
void ScopeOled_Setup(TwoWire *twi);
#endif
void ScopeOled_Process(void);
void ScopeOled_AddSamples(float *left, float *right, uint32_t len);
void ScopeOled_DrawData(const float *dispData, uint8_t idx);


#endif /* SRC_ML_SCOPE_H_ */

