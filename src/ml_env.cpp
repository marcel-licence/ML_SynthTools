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
 * @file ml_env.cpp
 * @author Marcel Licence
 * @date 16.01.2022
 *
 * @brief This file contains the implementation of a very simple envelope mechanism.
 * A modification might appear in the future for more complex enveleopes.
 *
 * @see First time used here: https://youtu.be/WJGOIgaY-1s
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#include "ml_env.h"


/*
 * very bad and simple implementation of ADSR
 * - but it works for the start
 */
bool ADSR_Process(const struct adsrT *ctrl, struct adsr_ctrl_t *adsr)
{
    switch (adsr->phase)
    {
    case attack:
        adsr->ctrl += ctrl->a;
        if (adsr->ctrl > 1.0f)
        {
            adsr->ctrl = 1.0f;
            adsr->phase = decay;
        }
        break;
    case decay:
        adsr->ctrl -= ctrl->d;
        if (adsr->ctrl < ctrl->s)
        {
            adsr->ctrl = ctrl->s;
            adsr->phase = sustain;
        }
        break;
    case sustain:
        break;
    case release:
        adsr->ctrl -= ctrl->r;
        if (adsr->ctrl < 0.0f)
        {
            adsr->ctrl = 0.0f;
            //voice->active = false;
            return false;
        }
    }
    return true;
}

void ADSR_Start(const struct adsrT *ctrl, struct adsr_ctrl_t *adsr)
{
    adsr->ctrl = ctrl->a;
    if (ctrl->a == 1.0f) /* not sure if that check is necessary */
    {
        adsr->phase = decay;
    }
    else
    {
        adsr->phase = attack;
    }
}

bool ASRM_Process(const struct adsrT *ctrl, struct adsr_ctrl_t *asr)
{
    switch (asr->phase)
    {
    case attack:
        asr->ctrl += ctrl->a;
        if (asr->ctrl > 1.0f)
        {
            asr->ctrl = 1.0f;
            asr->phase = decay;
        }
        break;
    case decay:
        asr->ctrl -= ctrl->d;
        if (asr->ctrl < 0)
        {
            asr->ctrl = 0;
            asr->phase = sustain;
        }
        break;
    case sustain:
        break;
    case release:
        asr->ctrl -= ctrl->r;
        if (asr->ctrl < 0.0f)
        {
            asr->ctrl = 0.0f;
            //voice->active = false;
            return false;
        }
    }
    return true;
}

