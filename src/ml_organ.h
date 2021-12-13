/*
 * Copyright (c) 2021 Marcel Licence
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
 * @file ml_organ.h
 * @author Marcel Licence
 * @date 26.11.2021
 *
 * @brief   This shows you all available functions to use the organ library
 *          The library is still in progress and will be updated in future
 *          You can integrate this lib at the moment in your ESP32 and ESP8266 projects
 */


#ifndef SRC_ML_ORGAN_H_
#define SRC_ML_ORGAN_H_


#include "Arduino.h"
#include <stdint.h>

/**
 * call this first before using the module to initialize it
 */
void Organ_Setup(Stream *ser);

/**
 * when initialized you will get your audio samples from this function
 */
int16_t Organ_Process(void);
void Organ_Process_Buf(int32_t *buf, uint8_t len); /* not supported by ESP32, ESP8266 */

/**
 * this functions are used to feed in the note on/off events
 */
void Organ_NoteOn(uint8_t ch __attribute__((unused)), uint8_t note, uint8_t vel __attribute__((unused)));
void Organ_NoteOff(uint8_t ch __attribute__((unused)), uint8_t note);

/**
 * finally you can control some settings with the following functions
 */
void Organ_PercussionSet(uint8_t setting);
void Organ_SetLeslCtrl(uint8_t val); /* not supported by SAMD21 */
void Organ_SetDrawbar(uint8_t idx, uint8_t value);


#endif /* SRC_ML_ORGAN_H_ */
