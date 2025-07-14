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
 * @file ml_arp.cpp
 * @author Marcel Licence
 * @date 01.01.2021
 *
 * @brief   This is a little arpeggiator
 */


#include <ml_arp.h>


#define ARP_STEPS 16

enum arpState_e
{
    arp_idle,
    arp_acti,
    arp_rec,
};

static uint8_t arpSelected = 0; /*!< selected riff, 0 means no playback */

uint8_t arp[7][ARP_STEPS] =
{
    { 12 - 12, 12 - 12, 12, 12, 12 + 12, 12, 12, 12 + 15, 12 - 12, 12 - 12, 12, 12, 12 + 12, 12, 12, 12 + 15}, /* Drax Ltd. II - Amphetamine */
    {0, 12, 24, 0, 12, 24, 0, 12, 24, 0, 12, 24, 0, 12, 24, 36}, /* popcorn bass riff */
    {0, 0, 12, 0, 12, 12, 0, 12, 0, 0, 12, 0, 12, 12, 0, 12},
    {0, 0, 12, 0, 10, 12, 0, 0, 12, 0, 10, 12, 0, 13, 0, 10},
    {0, 0, 12, 7, 0, 0, 12, 7, 0, 0, 12, 7, 0, 0, 7, 12 },
    {12, 24, 12, 24, 12 - 5, 24 - 5, 12 - 2, 24 - 2, 12, 24, 12, 24, 12 - 5, 24 - 5, 12 - 2, 24 - 2}, /* I Feel Love */

    {0, 12, 12, 0, 12, 12, 0, 0, 12, 12, 0, 10, 0, 10, 11, 12}, /* my first own */
};

static uint32_t arp_sample_rate = 6;
static uint8_t arpNote = 0xFF;
static uint64_t arp_cnt = 0;
static uint64_t arp_act = 0;
static uint32_t arp_pos = 0;
static uint32_t arp_key = 0;

static struct
{
    arpState_e arpState;
    uint8_t activeCh;
    uint8_t activeNote;
    uint32_t tempo;
    uint32_t gate;
    float f_gate;
    uint8_t rxCh;
} arpModule =
{
    arp_idle,
    0xFF,
    0xFF,
    (uint32_t)(arp_sample_rate * (1.0f - 0.875f)),
    (uint32_t)(arp_sample_rate * (1.0f - 0.875f) * 0.5f),
    0.5f,
    0,
};

void Arp_Init(uint32_t sample_rate)
{
    arp_sample_rate = sample_rate;

    arpModule.tempo = (uint32_t)(arp_sample_rate * (1.0f - 0.875f));
    arpModule.gate = (uint32_t)(arp_sample_rate * (1.0f - 0.875f) * 0.5f);
    arpModule.arpState = arp_idle;

    Arp_Tempo(0, 0.5f);
}

void Arp_Process(uint64_t elapsed_ms)
{
    if (arpModule.arpState == arp_acti)
    {

        arp_cnt += elapsed_ms;

        /*
         * stop short notes
         */
        if (arp_cnt > arpModule.gate)
        {
            if (arpNote != 0xFF)
            {
                Arp_Cb_NoteOff(arpModule.activeCh, arpModule.activeNote);
            }
        }

        if (arp_cnt >= arpModule.tempo)
        {
            arp_cnt = 0;
            if (arpNote != 0xFF)
            {
                Arp_Cb_NoteOff(arpModule.activeCh, arpModule.activeNote);
                arpModule.activeNote = arpNote + arp[arpSelected][arp_pos];
                Arp_Cb_NoteOn(arpModule.activeCh, arpModule.activeNote, 1);
                arp_act = arp_pos;
                arp_pos ++;
                if (arp_pos >= ARP_STEPS)
                {
                    arp_pos = 0;
                }
                Arp_Cb_Step(arp_pos);
            }
        }
    }
}

void Arp_Reset(void)
{
    arp_pos = 0;
    arp_act = sizeof(arp) - 1;
    arp_cnt = arpModule.tempo;
}

void Arp_NoteOn(uint8_t ch, uint8_t note, float vel)
{
    if ((arpModule.arpState == arp_acti) && (ch == arpModule.rxCh))
    {
        arpNote = note;
        arpModule.activeCh = ch;
        if (arp_key == 0)
        {
            arp_pos = 0;
            arp_act = sizeof(arp) - 1;
            arp_cnt = arpModule.tempo; /* if to high then it will overflow in next arp process call */
        }
        arp_key ++;
    }
    else if (arpModule.arpState == arp_rec)
    {
        Arp_Cb_NoteOn(ch, note, vel);

        arp[arpSelected][arp_pos] = note;
        arp_pos++;
        if (arp_pos >= ARP_STEPS)
        {
            uint8_t arpMin = 0xFF;
            for (int i = 0; i < ARP_STEPS; i++)
            {
                if (arp[arpSelected][i] < arpMin)
                {
                    arpMin = arp[arpSelected][i];
                }
            }
            for (int i = 0; i < ARP_STEPS; i++)
            {
                arp[arpSelected][i] -= arpMin;
            }
            Arp_Active();
        }
    }
    else
    {
        Arp_Cb_NoteOn(ch, note, vel);
    }
}

void Arp_NoteOff(uint8_t ch, uint8_t note)
{
    if ((arpModule.arpState == arp_acti) && (arpModule.rxCh == ch))
    {
        if (arp_key == 1)
        {
            Arp_Cb_NoteOff(arpModule.activeCh, arpModule.activeNote);
            arpNote = 0xFF;
        }
        arp_key --;
    }
    else
    {
        Arp_Cb_NoteOff(ch, note);
    }
}

void Synth_ArpActive(float value)
{
    uint8_t val8 = (value * 5.0f);
    if (val8 == 0)
    {
        arpModule.arpState = arp_idle;
    }
    else
    {
        arpSelected = (val8 - 1) % 4;
        arpModule.arpState = arp_acti;
    }
}

void Arp_SelectSequence(uint8_t seq, float value)
{
    if (value > 0)
    {
        if (seq < (sizeof(arp) / sizeof(arp[0])))
        {
            arpSelected = seq;
            arpModule.arpState = arp_acti;
            Arp_Status_ValueChangedInt("ArpSeq", seq);
        }
        else
        {
            Arp_Idle();
        }
    }
}

void Arp_StartRecord(uint8_t seq __attribute__((unused)), float value __attribute__((unused)))
{
    Arp_Status_LogMessage("Arp Record");
    arpModule.arpState = arp_rec;
    arp_pos = 0; /* start with first note when doing a record */
    arp_key = 0;
}

void Arp_Idle()
{
    Arp_Status_LogMessage("Arp Idle");
    arpModule.arpState = arp_idle;
    if (arpNote != 0xFF)
    {
        Arp_Cb_NoteOff(arpModule.activeCh, arpModule.activeNote);
    }
    arpNote = 0xFF;
    arp_key = 0;
}

void Arp_Active()
{
    Arp_Status_LogMessage("Arp active");
    arpModule.arpState = arp_acti;
    arp_key = 0;
}

void Arp_Tempo(uint8_t unused __attribute__((unused)), float value)
{
    float f_tempo = ((float)arp_sample_rate);// - (value * ((float)(arp_sample_rate)));
    for (int i = 0; i < value * 8; i++)
    {
        f_tempo /= 2;
    }
    arpModule.tempo = f_tempo;
    arpModule.gate = ((float)arpModule.tempo) * arpModule.f_gate;
    Arp_Status_ValueChangedFloat("ArpTempo", value);
}

void Arp_GateTime(uint8_t unused __attribute__((unused)), float value)
{
    arpModule.f_gate = value;
    arpModule.gate = ((float)arpModule.tempo) * arpModule.f_gate;
    Arp_Status_ValueChangedFloat("ArpGate", arpModule.f_gate);
}

uint32_t Arp_GetPos(void)
{
    return arp_pos;
}

/* please implement the following functions in your application */
__attribute__((weak)) void Arp_Cb_NoteOn(uint8_t ch, uint8_t note, float vel) {};
__attribute__((weak)) void Arp_Cb_NoteOff(uint8_t ch, uint8_t note) {};
__attribute__((weak)) void Arp_Status_ValueChangedInt(const char *msg, int value) {};
__attribute__((weak)) void Arp_Status_LogMessage(const char *msg) {};
__attribute__((weak)) void Arp_Status_ValueChangedFloat(const char *msg, float value) {};
__attribute__((weak)) void Arp_Cb_Step(uint8_t step) {};
