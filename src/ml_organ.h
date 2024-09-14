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




#include <stdint.h>



#define CTRL_PERC_SWITCH    0
#define CTRL_PERC_SPEED     1
#define CTRL_PERC_NOTE      2
#define CTRL_PERC_LOUD      3
#define CTRL_PERC_POLY      4
#define CTRL_INTR_FEEDBACK  5
#define CTRL_ROTARY_ACTIVE  6


#ifdef ARDUINO_ARCH_SAMD
#define Organ_Setup(...)    OrganSAMD21_Setup(__VA_ARGS__)
#define Organ_Process(...)    OrganSAMD21_Process(__VA_ARGS__)
#define Organ_Process_Buf(...)    OrganSAMD21_Process_Buf(__VA_ARGS__)
#define Organ_NoteOn    OrganSAMD21_NoteOn
#define Organ_NoteOff    OrganSAMD21_NoteOff
#define Organ_PercussionSet(...)    OrganSAMD21_PercussionSet(__VA_ARGS__)
#define Organ_SetLeslCtrl(...)    OrganSAMD21_SetLeslCtrl(__VA_ARGS__)
#define Organ_SetDrawbar(...)    OrganSAMD21_SetDrawbar(__VA_ARGS__)
#endif

#ifdef ARDUINO_ARCH_RP2040
#ifndef __ARM_FEATURE_DSP
#define Organ_Setup(...)    OrganRP2040_Setup(__VA_ARGS__)
#define Organ_Process(...)    OrganRP2040_Process(__VA_ARGS__)
#define Organ_Process_Buf(...)    OrganRP2040_Process_Buf(__VA_ARGS__)
#define Organ_NoteOn    OrganRP2040_NoteOn
#define Organ_NoteOff    OrganRP2040_NoteOff
#define Organ_PercussionSet(...)    OrganRP2040_PercussionSet(__VA_ARGS__)
#define Organ_SetLeslCtrl(...)    OrganRP2040_SetLeslCtrl(__VA_ARGS__)
#define Organ_SetDrawbar(...)    OrganRP2040_SetDrawbar(__VA_ARGS__)
#else
#define Organ_Setup(...)    OrganRP2350_Setup(__VA_ARGS__)
#define Organ_Process(...)    OrganRP2350_Process(__VA_ARGS__)
#define Organ_Process_Buf(...)    OrganRP2350_Process_Buf(__VA_ARGS__)
#define Organ_NoteOn    OrganRP2350_NoteOn
#define Organ_NoteOff    OrganRP2350_NoteOff
#define Organ_PercussionSet(...)    OrganRP2350_PercussionSet(__VA_ARGS__)
#define Organ_SetLeslCtrl(...)    OrganRP2350_SetLeslCtrl(__VA_ARGS__)
#define Organ_SetDrawbar(...)    OrganRP2350_SetDrawbar(__VA_ARGS__)
#endif
#endif

/**
 * call this first before using the module to initialize it
 */
void Organ_Setup(float sample_rate);

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

