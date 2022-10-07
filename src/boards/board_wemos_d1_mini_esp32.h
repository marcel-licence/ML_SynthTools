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
* @file board_wemos_d1_mini_esp32.h
* @author Marcel Licence
* @date 13.12.2021
*
* @brief Board description for a circuit using the Wemos D1 Mini ESP32 board
*        This board is equipped with the ESP-WROOM-32
*
* @see https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf
*/


#ifndef BOARDS_BOARD_WEMOS_D1_MINI_ESP32_H_
#define BOARDS_BOARD_WEMOS_D1_MINI_ESP32_H_


/* on board led */
#define BLINK_LED_PIN     2

#define DAC_1 25
#define DAC_2 26

/*
 * U1RXD 16 with Serial1 works, U2RXD 4 with Serial2 works
 * U1RXD 4 with Serial1 works, U2RXD 16 with Serial2 does not work
 */

#define U1RXD 9
#define U1TXD 10

#define U2RXD 16
#define U2TXD 17

#define ADC2_CH7 27
#define ADC2_CH6 14
#define ADC2_CH5 12
#define ADC2_CH4 13
#define ADC2_CH3 15

#define ADC1_CH0 4
#define ADC1_CH3 5
#define ADC1_CH6 6
#define ADC1_CH7 7

/*
 * Define and connect your PINS to DAC here
 */

#ifdef I2S_NODAC
#define I2S_NODAC_OUT_L_PIN   25  /* internal DAC */
#define I2S_NODAC_OUT_R_PIN   26  /* internal DAC */
#else
/*
 * pins to connect a real DAC like PCM5201
 */
#define I2S_BCLK_PIN    25
#define I2S_WCLK_PIN    27
#define I2S_DOUT_PIN    26
/* I2S_DIN_PIN is not used here */
#endif

#ifdef ADC_TO_MIDI_ENABLED
#define ADC_INPUTS  8
#define ADC_MUL_S0_PIN  33
#define ADC_MUL_S1_PIN  32
#define ADC_MUL_S2_PIN  13
#define ADC_MUL_SIG_PIN 12
#endif

/*
 * Pins defined by board
 */
#define RXD2 16
#define TXD2 17

#endif /* BOARDS_BOARD_WEMOS_D1_MINI_ESP32_H_ */
