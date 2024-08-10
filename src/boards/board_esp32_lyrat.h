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
 * @file board_esp32_lyrat.h
 * @author Marcel Licence
 * @date 05.12.2021
 *
 * @brief Board description for the ESP32-LyraT
 * @see https://docs.espressif.com/projects/esp-adf/en/latest/design-guide/board-esp32-lyrat-v4.3.html
 */


#ifndef BOARDS_BOARD_ESP32_LYRAT_H_
#define BOARDS_BOARD_ESP32_LYRAT_H_


#define BLINK_LED_PIN   22 /* Green LED indicator */

#define ES8388_PIN_DOUT 35
#define ES8388_PIN_DIN  26
#define ES8388_PIN_LRCK 25
#define ES8388_PIN_SCLK 5
#define ES8388_PIN_MCLK 0

#define ES8388_PIN_SDA  18
#define ES8388_PIN_SCL  23

#define MIDI_RX_PIN 22


#endif /* BOARDS_BOARD_ESP32_LYRAT_H_ */

