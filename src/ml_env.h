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
 * @file ml_env.h
 * @author Marcel Licence
 * @date 16.01.2022
 *
 * @brief This file contains the declarations and structures of the envelope module
 *
 * @see First time used here: https://youtu.be/WJGOIgaY-1s
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#ifndef ML_ENV_H_
#define ML_ENV_H_


#ifdef ARDUINO
#include <Arduino.h>
#endif


struct adsrT
{
    float a;
    float d;
    float s;
    float r;
    float w;
};

typedef enum
{
    attack, decay, sustain, release
} adsr_phaseT;

struct adsr_ctrl_t
{
    float ctrl;
    adsr_phaseT phase;
};

struct asmT
{
    float a;
    float d;
    float s;
    float r;
};


bool ADSR_Process(const struct adsrT *ctrl, struct adsr_ctrl_t *adsr);
void ADSR_Start(const struct adsrT *ctrl, struct adsr_ctrl_t *adsr);
bool ASRM_Process(const struct adsrT *ctrl, struct adsr_ctrl_t *asr);
void ASRM_Start(const struct adsrT *ctrl, struct adsr_ctrl_t *asr);


#endif /* ML_ENV_H_ */

