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
 * @file librp2350_i2s_audio.h
 * @author Marcel Licence
 * @date 28.09.2024
 *
 * @brief This file includes the prototypes of the librp2350_i2s_audio.a library
 * It is precompiled because some headers are not available withing the Arduino IDE to compile
 * DIN->DOUT: 26
 * BCK->BCK: 27
 * WCK->LCK: 28
 */

void rp2350_i2s_init(int data_pin, int clock_base_pin);
void rp2350_i2s_write_stereo_samples_buff(const float *in_l, const float *in_r, uint32_t sample_count);
void rp2350_i2s_write_stereo_samples_buff(const int16_t *in_l, const int16_t *in_r, uint32_t sample_count);
