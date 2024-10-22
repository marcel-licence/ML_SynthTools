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
 * @file ml_phaser.h
 * @author Marcel Licence
 * @date 20.12.2022
 *
 * @brief This file contains an implementation of a simple stereo phaser effect
 *
 * @see first peak: https://youtu.be/Kac9AB02BcQ
 * @see little demo: https://youtu.be/hqK_U22Jha8
 */


#ifndef SRC_ML_PHASER_H_
#define SRC_ML_PHASER_H_


#include <stdint.h>


#define PHASER_AP0 150

#define PHASER_BUFFER_SIZE   (PHASER_AP0)


struct allpass_s
{
    float *buf;
    uint32_t p;
    float g;
    uint32_t lim;
};


void Phaser_Init(float *buffer, uint32_t len);
void Phaser_Process(const float *in, const float *lfo_in, float *out, int buffLen);
void Phaser_ProcessHQ(const float *in, const float *lfo_in, float *out, int buffLen);
void Phaser_SetInputLevel(uint8_t unused __attribute__((unused)), float value);
void Phaser_SetDepth(uint8_t unused __attribute__((unused)), float value);
void Phaser_SetOutputLevel(uint8_t unused __attribute__((unused)), float value);
void Phaser_SetLength(uint8_t unused __attribute__((unused)), float value);
void Phaser_SetG(uint8_t unused __attribute__((unused)), float value);
void Phaser_SetAuto(uint8_t unused __attribute__((unused)), float value);
void Phaser_SetDepth(uint8_t unused __attribute__((unused)), uint8_t value_u8);
void Phaser_SetG(uint8_t unused __attribute__((unused)), uint8_t value_u8);


#endif /* SRC_ML_PHASER_H_ */

