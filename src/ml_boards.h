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
 * @file ml_boards.h
 * @author Marcel Licence
 * @date 02.01.2021
 *
 * @brief Board description main include file
 */


#ifdef BOARD_ML_V1
#include "./boards/board_ml_v1.h"
#elif (defined BOARD_ESP32_AUDIO_KIT_AC101)
#include "./boards/board_audio_kit_ac101.h"
#elif (defined BOARD_ESP32_AUDIO_KIT_ES8388)
#include "./boards/board_audio_kit_es8388.h"
#elif (defined BOARD_TTGO_T9_RGB_LED_WM8978)
#include "./boards/board_ttgo_t9_rgb_led_wm8978.h"
#elif (defined BOARD_ESP32_DOIT)
#include "./boards/board_esp32_doit.h"
#elif (defined ARDUINO_GENERIC_F407VGTX)
#include "boards/board_stm32f407g-disc1.h"
#elif (defined ARDUINO_DISCO_F407VG)
#include "boards/board_stm32f407g-disc1.h"
#elif (defined ARDUINO_GENERIC_RP2040)
#include "boards/board_tiny2040.h"
#else
/* no include here */
#endif
