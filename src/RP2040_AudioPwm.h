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
 * @file RP2040_AudioPwm.h
 * @author Marcel Licence
 * @date 01.12.2022
 *
 * @brief   This is a PWM based PWM audio driver for the RP2040
 *          It supports stereo sound and requires a passive filter for DC offset removal
 *
 * @see Short demonstration: https://youtu.be/b_vXfD0tSy4
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#ifndef __WAV_PWM_AUDIO_H
#define __WAV_PWM_AUDIO_H


#if (defined ARDUINO_ARCH_RP2040)
/* ignore */
#else
#error not supported by this platform!
#endif


#include <stdint.h>


void RP2040_Audio_Pwm_Init(uint8_t gpio_pin_number, float sample_rate, uint32_t *audio_buffer_a, uint32_t *audio_buffer_b, uint8_t len);
bool RP2040_Audio_Pwm_BufferReady();
uint32_t *RP2040_Audio_Pwm_getFreeBuff();


#endif /* ARDUINO_ARCH_RP2040 */

