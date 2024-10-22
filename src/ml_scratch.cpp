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
 * @file ml_scratch.cpp
 * @author Marcel Licence
 * @date 20.10.2024
 *
 * @brief  Implementation of simple sample playback functions used for scratching
 * @see https://www.youtube.com/watch?v=Ml6VrlV3hvk
 */


#include <stdint.h>
#include <ml_types.h>
#include "ml_scratch.h"


#include <stdint.h>
#include <string.h>


#include <ml_status.h>
#include <ml_alg.h>
#include <ml_wavfile.h>


#define SCRATCH_SAMPLE_COUNT    16
#define min(a,b) a>b?b:a


struct scratch_sample_s
{
    union
    {
        uint8_t *data;
        int16_t *samples;
    };

    uint32_t size;

    uint32_t sample_count;
    uint8_t note;
    uint32_t pos = 0;

    float pitch;
    float pos_f;
};

struct scratch_s
{
    struct scratch_sample_s samples[SCRATCH_SAMPLE_COUNT];
    uint8_t sample_count;
};


static struct scratch_s scratch;


void Scratch_Init(float sample_rate)
{
    scratch.sample_count = 0;
}

bool Scratch_AddSampleStatic(const uint8_t *data, uint32_t size, uint8_t idx)
{
    if (scratch.sample_count >= SCRATCH_SAMPLE_COUNT)
    {
        return false;
    }


    uint8_t *wav_data = NULL;

    union wavHeader *wavHdr = (union wavHeader *)data;
    int bytesRead = min(size, sizeof(wavHdr->wavHdr));
    data = &data[bytesRead];
    size -= bytesRead;

    if (bytesRead == sizeof(wavHdr->wavHdr))
    {
    }
    else
    {
        Status_LogMessage("error reading wave header!\n");
    }

    if (wavHdr->lengthOfData > 16)
    {
        data = &data[wavHdr->lengthOfData - 16];
    }

    while (memcmp(wavHdr->nextTag.tag_name, "data", 4) != 0)
    {
        char tagStr[5] = {0};
        memcpy(tagStr, wavHdr->nextTag.tag_name, 4);
        data = &data[wavHdr->nextTag.tag_data_size];

        int bytesRead = min(size, sizeof(wavHdr->wavHdr));
        if (bytesRead != sizeof(wavHdr->nextTag.wavHdr))
        {
            Status_LogMessage("error reading tag\n");
            return false;
        }
    }


    if (memcmp(wavHdr->nextTag.tag_name, "data", 4) == 0)
    {
        wav_data = (uint8_t *)&wavHdr->nextTag.tag_data_size + 4UL;
    }
    else
    {
        Status_LogMessage("Error: No data tag found!\n");
        return false;
    }

    struct scratch_sample_s *newSample = &scratch.samples[scratch.sample_count];

    newSample->data = wav_data;
    newSample->size = (wavHdr->nextTag.tag_data_size);
    newSample->sample_count = (wavHdr->nextTag.tag_data_size / wavHdr->bytesPerSample);

    scratch.sample_count++;

    return true;
}

void Scratch_SetPitchAbs(float pitch, uint8_t idx)
{
    if (idx < SCRATCH_SAMPLE_COUNT)
    {
        struct scratch_sample_s *sample = &scratch.samples[idx];

        sample->pitch = pitch;
    }
}

void Scratch_ProcessSample(Q1_14 *samples_l, Q1_14 *samples_r, uint32_t len, struct scratch_sample_s *sample)
{
    for (uint32_t n = 0 ; n < len; n++)
    {
        sample->pos = sample->pos_f;

        samples_l[n].s16 += sample->samples[sample->pos] / 4;
        samples_r[n].s16 += sample->samples[sample->pos] / 4;

        sample->pos_f += sample->pitch;
        if (sample->pos_f >= sample->sample_count)
        {
            sample->pos_f -= sample->sample_count;
        }
        if (sample->pos_f < 0)
        {
            sample->pos_f += sample->sample_count;
        }
    }
}

void Scratch_Process(Q1_14 *samples_l, Q1_14 *samples_r, uint32_t len)
{
    for (uint8_t n = 0; n<scratch.sample_count; n++)
    {
    Scratch_ProcessSample(samples_l, samples_r, len, &scratch.samples[n]);
    }
}
