/*
 * Copyright (c) 2021 Marcel Licence
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
 * @file ml_midi_ctrl.cpp
 * @author Marcel Licence
 * @date 02.01.2021
 *
 * @brief   This is a little helper to make a midi split point
 *          Transpose is also supported
 */


#include <ml_midi_ctrl.h>


struct
{
    uint8_t split;
    int transpose[2];
    bool listenAll;
} midiCtrlParam =
{
    60,
    {-12, -12},
    true,
};

void MidiCtrl_NoteOn(uint8_t ch, uint8_t note, float vel)
{
    if (midiCtrlParam.listenAll)
    {
        if (note < midiCtrlParam.split)
        {
            if ((note + midiCtrlParam.transpose[0] > 0) && (note + midiCtrlParam.transpose[0] < 128))
            {
                MidiCtrl_Cb_NoteOn(0, note + midiCtrlParam.transpose[0], vel);

            }
#if 0
            Status_ValueChangedInt("lower", note);
#endif
        }
        else
        {
            if ((note + midiCtrlParam.transpose[1] > 0) && (note + midiCtrlParam.transpose[1] < 128))
            {
                MidiCtrl_Cb_NoteOn(1, note + midiCtrlParam.transpose[1], vel);
            }
#if 0
            Status_ValueChangedInt("upper", note);
#endif
        }
    }
    else
    {
        /* otherwise bypass */
        MidiCtrl_Cb_NoteOn(ch, note, vel);
    }
}

void MidiCtrl_NoteOff(uint8_t ch, uint8_t note)
{
    if (midiCtrlParam.listenAll)
    {
        if (note < midiCtrlParam.split)
        {
            if ((note + midiCtrlParam.transpose[0] > 0) && (note + midiCtrlParam.transpose[0] < 128))
            {
                MidiCtrl_Cb_NoteOff(0, note + midiCtrlParam.transpose[0]);
            }
        }
        else
        {
            if ((note + midiCtrlParam.transpose[1] > 0) && (note + midiCtrlParam.transpose[1] < 128))
            {
                MidiCtrl_Cb_NoteOff(1, note + midiCtrlParam.transpose[1]);
            }
        }
    }
    else
    {
        /* otherwise bypass */
        MidiCtrl_Cb_NoteOff(ch, note);
    }
}

void MidiCtrl_TransposeUp(uint8_t ch, float value)
{
    if (value > 0)
    {
        midiCtrlParam.transpose[ch] ++;
        MidiCtrl_Status_ValueChangedIntArr("transpose", midiCtrlParam.transpose[ch], ch);
    }
}

void MidiCtrl_TransposeDown(uint8_t ch, float value)
{
    if (value > 0)
    {
        midiCtrlParam.transpose[ch] --;
        MidiCtrl_Status_ValueChangedIntArr("transpose", midiCtrlParam.transpose[ch], ch);
    }
}

void MidiCtrl_TransposeReset(uint8_t ch, float value)
{
    if (value > 0)
    {
        midiCtrlParam.transpose[ch] = 0;
        MidiCtrl_Status_ValueChangedIntArr("transpose", midiCtrlParam.transpose[ch], ch);
    }
}
