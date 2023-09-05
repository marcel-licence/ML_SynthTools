/*
 * Copyright (c) 2023 Marcel Licence
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
 * @file ml_chorus.h
 * @author Marcel Licence
 * @date 20.12.2022
 *
 * @brief This file contains the declarations of a simple stereo chorus effect
 *
 * @see little demo: https://youtu.be/ZIiSp7yM6o8
 */


#ifndef SRC_ML_CHORUS_H_
#define SRC_ML_CHORUS_H_


#include <Arduino.h>


#include <ml_types.h>


void Chorus_Init(int16_t *buffer, uint32_t len);
void Chorus_Init2(int16_t *left, int16_t *right, uint32_t len);
void Chorus_Reset(void);
void Chorus_Process_Buff(float *signal_l, int buffLen);
void Chorus_Process_Buff(int16_t *signal_l, int buffLen);
void Chorus_Process_Buff(float *in, float *left, float *right, int buffLen);
void Chorus_Process_Buff(int16_t *in, int16_t *left, int16_t *right, int buffLen);
void Chorus_Process_Buff(Q1_14 *in, Q1_14 *left, Q1_14 *right, int buffLen);
void Chorus_Process_Buff2(float *signal_l, float *signal_r, int buffLen);
void Chorus_SetupDefaultPreset(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetInputLevel(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetThrough(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetDelay(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetPhaseShift(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetDepth(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetOutputLevel(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetLength(uint8_t unused __attribute__((unused)), float value);
void Chorus_SetSpeed(uint8_t unused __attribute__((unused)), float value);


void ChorusQ_Init(int16_t *buffer, uint32_t len);
void ChorusQ_Init2(int16_t *left, int16_t *right, uint32_t len);
void ChorusQ_Process_Buff(Q1_14 *in, Q1_14 *left, Q1_14 *right, int buffLen);
void ChorusQ_Reset(void);
void ChorusQ_SetDelay(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetDepth(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetInputLevel(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetLength(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetOutputLevel(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetPhaseShift(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetSpeed(uint8_t unused __attribute__((unused)), float value);
void ChorusQ_SetThrough(uint8_t unused __attribute__((unused)), float value);


#endif /* SRC_ML_CHORUS_H_ */

