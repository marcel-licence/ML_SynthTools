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
 * @file ml_filter.h
 * @author Marcel Licence
 * @date 16.01.2022
 *
 * @brief This file contains the declarations and structures of the filter module
 *
 * @see First time used here: https://youtu.be/WJGOIgaY-1s
 *
 * Look here: https://www.musicdsp.org/en/latest/Filters/229-lpf-24db-oct.html
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#ifndef ML_FILTER_H_
#define ML_FILTER_H_


#include <ml_types.h>


#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#endif

struct filterCoeffT
{
    union
    {
        struct
        {
            float bNorm[3];
            float aNorm[2];
        };
        float coef[5];
    };
};

struct filterProcT
{
    struct filterCoeffT *filterCoeff;
    float w[3];
};

struct filterCoeffQ16T
{
    union
    {
        struct
        {
            Q1_14 bNorm[3];
            Q1_14 aNorm[2];
        };
        Q1_14 coef[5];
    };
};


struct filterProcQ16T
{
    struct filterCoeffQ16T *filterCoeff;
    Q1_14 w[3];
};


#define Filter_Calculate(...)   Filter_CalculateLowPass(__VA_ARGS__)


void Filter_Init(struct filterProcT *const filterP, struct filterCoeffT *const filterC);
void Filter_Coeff_Init(struct filterCoeffT *const filterC);
void Filter_Proc_Init(struct filterProcT *const filterP);

void Filter_Reset(struct filterProcT *filter);

void Filter_CalculateLowPass(float c, float reso, struct filterCoeffT *const filterC);
void Filter_CalculateLowPassSlow(float c, float reso, struct filterCoeffT *const filterC);
void Filter_CalculateNone(struct filterCoeffT *const filterC);
void Filter_CalculateNotch(float c, float reso, struct filterCoeffT *const filterC);

void Filter_Process(float *const signal, struct filterProcT *const filterP);
void Filter_Process_Buffer(float *const signal, struct filterProcT *const filterP, uint32_t len);


void Filter_Init(struct filterProcQ16T *const filterP, struct filterCoeffQ16T *const filterC);
void Filter_Coeff_Init(struct filterCoeffQ16T *const filterC);
void Filter_Proc_Init(struct filterProcQ16T *const filterP);

void Filter_Reset(struct filterProcQ16T *filter);

void Filter_CalculateLowPass(Q1_14 c, Q1_14 reso, struct filterCoeffQ16T *const filterC);
void Filter_CalculateLowPass(float c, float reso, struct filterCoeffQ16T *const filterC);
void Filter_CalculateNotch(Q1_14 c, Q1_14 reso, struct filterCoeffQ16T *const filterC);

void Filter_Process(Q1_14 *const signal, struct filterProcQ16T *const filterP);
void Filter_Process_Buffer(Q1_14 *const signal, struct filterProcQ16T *const filterP, uint32_t len);


#endif /* ML_FILTER_H_ */

