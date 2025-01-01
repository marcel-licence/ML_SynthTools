/*
 * Copyright (c) 2025 Marcel Licence
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
 * @file board_ttgo_t9_rgb_led_wm8978.h
 * @author Marcel Licence
 * @date 20.12.2022
 *
 * @brief Board description for TTGO-T9-RGB_LED-WM8978
 * @see https://github.com/LilyGO/TTGO-TAudio
 */


#ifndef BOARDS_BOARD_TTGO_T9_RGB_LED_WM8978_H_
#define BOARDS_BOARD_TTGO_T9_RGB_LED_WM8978_H_


/* on board led, green */
#define BLINK_LED_PIN     22

#define MIDI_PORT2_ACTIVE
#define MIDI_RX2_PIN 34 /* use a free pin! */

#define LED_STRIP_PIN 22 /* onboard LEDs */


#define I2C_SDA 19
#define I2C_SCL 18


//#define I2C_SDA 23
//#define I2C_SCL 5


/* WM8978 */
#define I2S_MCLK_PIN 0 /* 24 MHz */
#define I2S_BCLK_PIN 33
#define I2S_WCLK_PIN 25
#define I2S_DOUT_PIN 26 /* DAC */
#define I2S_DIN_PIN 27 /* ADC */


#endif /* BOARDS_BOARD_TTGO_T9_RGB_LED_WM8978_H_ */

