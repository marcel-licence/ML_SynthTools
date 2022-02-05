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
 * @file ml_filter.cpp
 * @author Marcel Licence
 * @date 16.01.2022
 *
 * @brief This file contains the implementation of a filter processing.
 * At the moment it can only calculate a low pass filter and apply it to an audio signal.
 *
 * @see First time used here: https://youtu.be/WJGOIgaY-1s
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#include "ml_filter.h"
#include "ml_waveform.h"


/* will be removed in future */
extern float sine[WAVEFORM_CNT];

void Filter_Proc_Init(struct filterProcT *const filterP)
{
    filterP->w[0] = 0.0f;
    filterP->w[1] = 0.0f;
    filterP->w[2] = 0.0f;
}

void Filter_Coeff_Init(struct filterCoeffT *const filterC)
{
    filterC->aNorm[0] = 0.0f;
    filterC->aNorm[1] = 0.0f;
    filterC->bNorm[0] = 1.0f;
    filterC->bNorm[1] = 0.0f;
    filterC->bNorm[2] = 0.0f;
}

void Filter_Reset(struct filterProcT *filter)
{
    filter->w[0] = 0.0f;
    filter->w[1] = 0.0f;
    filter->w[2] = 0.0f;
}

void Filter_Process(float *const signal, struct filterProcT *const filterP)
{
    const float out = filterP->filterCoeff->bNorm[0] * (*signal) + filterP->w[0];
    filterP->w[0] = filterP->filterCoeff->bNorm[1] * (*signal) - filterP->filterCoeff->aNorm[0] * out + filterP->w[1];
    filterP->w[1] = filterP->filterCoeff->bNorm[2] * (*signal) - filterP->filterCoeff->aNorm[1] * out;
    *signal = out;
}

void Filter_Process_Buffer(float *const signal, struct filterProcT *const filterP, uint32_t len)
{
    for (int n = 0; n < len; n++)
    {
        const float out = filterP->filterCoeff->bNorm[0] * signal[n] + filterP->w[0];
        filterP->w[0] = filterP->filterCoeff->bNorm[1] * signal[n] - filterP->filterCoeff->aNorm[0] * out + filterP->w[1];
        filterP->w[1] = filterP->filterCoeff->bNorm[2] * signal[n] - filterP->filterCoeff->aNorm[1] * out;
        signal[n] = out;
    }
}

/*
 * calculate coefficients of the 2nd order IIR filter
 */
void Filter_Calculate(float c, float reso, struct filterCoeffT *const filterC)
{
    float *aNorm = filterC->aNorm;
    float *bNorm = filterC->bNorm;

    float Q = reso;
    float  cosOmega, omega, sinOmega, alpha, a[3], b[3];

    /*
     * change curve of cutoff a bit
     * maybe also log or exp function could be used
     */
    c = c * c * c;

    if (c >= 1.0f)
    {
        omega = 1.0f;
    }
    else if (c < 0.0025f)
    {
        omega = 0.0025f;
    }
    else
    {
        omega = c;
    }

    /*
     * use lookup here to get quicker results
     */
    cosOmega = sine[WAVEFORM_I((uint32_t)((float)((1ULL << 31) - 1) * omega + (float)((1ULL << 30) - 1)))];
    sinOmega = sine[WAVEFORM_I((uint32_t)((float)((1ULL << 31) - 1) * omega))];

    alpha = sinOmega / (2.0 * Q);
    b[0] = (1 - cosOmega) / 2;
    b[1] = 1 - cosOmega;
    b[2] = b[0];
    a[0] = 1 + alpha;
    a[1] = -2 * cosOmega;
    a[2] = 1 - alpha;

    // Normalize filter coefficients
    float factor = 1.0f / a[0];

    aNorm[0] = a[1] * factor;
    aNorm[1] = a[2] * factor;

    bNorm[0] = b[0] * factor;
    bNorm[1] = b[1] * factor;
    bNorm[2] = b[2] * factor;
}
