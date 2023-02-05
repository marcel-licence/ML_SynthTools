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
 * @file ml_arp.h
 * @author Marcel Licence
 * @date 01.01.2021
 *
 * @brief   This is a little arpeggiator
 *
 * @see https://youtu.be/o-XjbrZHfWA
 */


#ifndef SRC_ML_ARP_H_
#define SRC_ML_ARP_H_


#include <Arduino.h>


void Arp_Init(uint32_t sample_rate);
void Arp_Process(uint64_t elapsed_ms);
void Arp_Reset(void);
void Arp_NoteOn(uint8_t ch, uint8_t note, float vel);
void Arp_NoteOff(uint8_t ch, uint8_t note);
void Synth_ArpActive(float value);
void Arp_SelectSequence(uint8_t seq, float value);
void Arp_StartRecord(uint8_t seq, float value);
void Arp_Idle(void);
void Arp_Active(void);
void Arp_Tempo(uint8_t unused, float value);
void Arp_GateTime(uint8_t unused, float value);
uint32_t Arp_GetPos(void);

/*
 * Following functions shall be implemented in your application
 * They will be called from the arp module
 */
void Arp_Cb_NoteOn(uint8_t ch, uint8_t note, float vel);
void Arp_Cb_NoteOff(uint8_t ch, uint8_t note);
void Arp_Status_ValueChangedInt(const char *msg, int value);
void Arp_Status_LogMessage(const char *msg);
void Arp_Status_ValueChangedFloat(const char *msg, float value);
void Arp_Cb_Step(uint8_t step);


#endif /* SRC_ML_ARP_H_ */
