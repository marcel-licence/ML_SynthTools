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
 * @file board_ml_synth_v2.h
 * @author Marcel Licence
 * @date 23.06.2023
 *
 * @brief Board description for my custom PCB
 */


#ifndef BOARDS_BOARD_ML_SYNTH_V2_H_
#define BOARDS_BOARD_ML_SYNTH_V2_H_


/*
 * MIDI In/Out
 */
#define MIDI_PORT2_ACTIVE
#define MIDI_RX2_PIN 34
#define MIDI_TX2_PIN 33

/*
 * SD/MMC
 */
#define SD_MMC_ENABLED

/*
 * I2S Audio In/Out
 */
#define I2S_BCLK_PIN    32
#define I2S_WCLK_PIN    25
#define I2S_DOUT_PIN    26 /* connected to SDIN of audio codec */
#define I2S_DIN_PIN     35 /* connected to SDOUT of audio codec */

/*
 * I2C
 */
#define I2C_ENABLED
#define I2C_SDA 21
#define I2C_SCL 22


#endif /* BOARDS_BOARD_ML_SYNTH_V2_H_ */

