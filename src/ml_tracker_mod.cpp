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
 * @file ml_tracker_mod.cpp
 * @author Marcel Licence
 * @data 15.02.2024
 *
 * @brief   Implementation of the mod tracker module
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif

/*
 * includes
 */
#include "ml_tracker_mod.h"
#include "fs/fs_access.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include <ml_utils.h>
#include <ml_status.h>

#include <Arduino.h>


/*
 * defines (private)
 */
#define CHANNEL_COUNT   4

#define EFFECT_TONE_PORTAMENTO   0x3
#define EFFECT_VOLUME_SLIDE   0xA
#define EFFECT_POSITION_JUMP   0xB
#define EFFECT_SET_VOLUME   0xC
#define EFFECT_PATTERN_BREAK   0xD
#define EFFECT_SET_SPEED   0xF

#define SAMPLE_COUNT    31
#define PATTERNLEN      128

#ifdef ARDUINO_ARCH_RP2040
#define PATTERNCOUNT    15
#define SAMPLEDATALEN     (128*1024)
#endif
#ifdef ESP8266
#define PATTERNCOUNT    12
#define SAMPLEDATALEN     (28*1024)
#endif
#ifdef ESP32
#define PATTERNCOUNT    32
#define SAMPLEDATALEN     (600*1024)
#endif

#define printf(...) Serial.printf(__VA_ARGS__)

/*
 * data types (private)
 */
struct sample_s
{
    char samplename[22];
    uint16_t samplelength;
    uint8_t finetune;
    uint8_t volume;
    uint16_t repeatpoint;
    uint16_t repeatlength;
};

struct channel_s
{
    uint8_t data[4];
};

struct row_s
{
    struct channel_s data[CHANNEL_COUNT];
};

struct pattern_s
{
    struct row_s row[1024 / 4 / 4];
};

union track_s
{
    struct
    {
        char songname[20];
        struct sample_s sample[SAMPLE_COUNT];
        uint8_t songlength; /* Song length in patterns */
        uint8_t magicbyte; /* restart. Not used. Set to 127 in old trackers */
        uint8_t patterns[PATTERNLEN]; /* @ 438 */
        char letters[4]; /* @ 438 */
        pattern_s pattern[PATTERNCOUNT];
        int8_t sampledata[SAMPLEDATALEN];
    };
    struct
    {
        uint8_t data[20 + (22 + 2 + 1 + 1 + 2 + 2) * SAMPLE_COUNT + 1 + 1 + PATTERNLEN + 4];
        uint8_t data_pattern[(1024 * PATTERNCOUNT)];
        uint8_t data_samples[SAMPLEDATALEN];
    };
};

struct track_hdr_s
{
    char songname[20];
    struct sample_s sample[SAMPLE_COUNT];
    uint8_t songlength; /* Song length in patterns */
    uint8_t magicbyte; /* restart. Not used. Set to 127 in old trackers */
    uint8_t patterns[PATTERNLEN]; /* @ 438 */
    char letters[4]; /* @ 438 */
};

struct track_ptr_s
{
    struct
    {
        union
        {
            struct track_hdr_s *hdr;
            const uint8_t *hdrData;
        };
        union
        {
            pattern_s *pattern;
            const uint8_t *patternData;
        };
        union
        {
            int8_t *sample;
            const uint8_t *sampleData;
        };
    };
};

struct note_period_lk_s
{
    uint16_t period;
    uint8_t note;
};

struct sample_info_s
{
    int8_t *sampleStart;
    uint16_t sampleLen;
    bool loop;
    uint16_t loopStart;
    uint16_t loopEnd;
    uint8_t volume;
    uint8_t finetune;
};

struct channel_player_s
{
    struct sample_info_s *sampleInfo;
    bool active;
    uint16_t noteperiod;
    uint32_t subpos;
    uint32_t pos;
    uint8_t volume;
    uint8_t sampleId;
    uint8_t disp;
};

struct tracker_playback_s
{
    bool active;
    uint8_t patternIdx;
    uint8_t jumpIdx;
    uint8_t currentPattern;
    uint8_t currentRow;
    uint64_t timeOver;
    uint64_t timeOver2;
    uint64_t tempo;
    uint64_t speed;
    uint64_t rate;
    struct sample_info_s sampleInfo[SAMPLE_COUNT];
    struct channel_player_s channel[CHANNEL_COUNT];
    struct channel_player_s channelMIDI[16];

    uint32_t tempoMultiplier;
    uint32_t pitchMultiplier;
};

/*
 * static variable (private)
 */
static union track_s *trk = NULL;
static struct track_ptr_s trkP = {NULL, NULL, NULL};
static struct note_period_lk_s notePeriod[12 * 3];
static struct tracker_playback_s tply;
static bool chActive[4] = {true, true, true, true};
static bool nextStep = false;

/*
 * static functions (private)
 */
static uint16_t swapU16(uint16_t value)
{
    return ((value & 0xFF00) >> 8) + ((value & 0x00FF) << 8);
}

/*
 * extern functions (public)
 */
uint32_t TrackerGetMemSize()
{
    return sizeof(union track_s);
}

void TrackerSetBuffer(void *buffer)
{
    trk = (union track_s *)buffer;
}

void TrackerUseStaticBuffer(void)
{
    static union track_s staticTrk;
    trk = &staticTrk;
}

void printNote(uint16_t period)
{
    if (period != 0)
    {
        printf("\033[33;1m");
        float p = period;
        p /= 856;
        p = log(p) / log(2.0f) * -12.0f;
        p += 0.5f;
        uint16_t note = p;
        const char *noteLk[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};
        printf("%s", noteLk[note % 12]);
        printf("%u", (note / 12) + 4);
    }
    else
    {
        printf("\033[30;1m");
        printf("...");
    }
}

void PrintChannelInfo(struct channel_s *ch)
{
    uint16_t noteperiod;
    uint8_t effectNum;
    uint8_t effectParam;
    uint8_t sampleNum;

    noteperiod = ((ch->data[0] & 0x0F) << 8UL) + (ch->data[1]);
    effectNum = (ch->data[2] & 0x0F);
    effectParam = (ch->data[3]);
    sampleNum = (ch->data[0] & 0xF0) + (ch->data[2] >> 4);


    printf("\033[30;1m| ");

#ifdef PRINT_NOTEPERIOD
    printf("%04x", noteperiod);
#endif

    printNote(noteperiod);
    printf("\033[30m");
    if (sampleNum > 0)
    {
        printf("\033[31;1m");
        printf(" %02u", sampleNum);
        printf("\033[30m");
    }
    else
    {
        printf(" ..");
    }
    if (effectNum == EFFECT_SET_VOLUME)
    {
        printf("\033[36;1m");
        if (effectParam < 100)
        {
            printf(" %02u", effectParam);
        }
        else
        {
            printf(" %02u", effectParam / 10);
        }
        printf("\033[30;1m");
        printf(" .00");
    }
    else
    {
        printf(" ..");

        if (effectNum == 0x0)
        {
            printf(" .00");
        }
        else
        {
            printf("\033[34;1m");
            const char *effectLk[] = {"0", "1", "2", "G", "4", "5", "6", "7", "8", "9", "D", "b", "c", "C", "S", "A"};
            printf(" %s", effectLk[effectNum]);
            printf("%02X", effectParam);
            printf("\033[30;1m");
        }
    }
    printf("\033[0m ");
}

void TrackerLoadData(const uint8_t *data)
{
    trkP.hdrData = (uint8_t *)&data[0];
    trkP.patternData = (uint8_t *)&data[sizeof(struct track_hdr_s)];

    printf("SongName: %s\n", trkP.hdr->songname);

    printf("Pattern:");
    uint8_t patternCnt = 0;
    for (uint8_t n = 0; n < 128; n++)
    {
        if (trkP.hdr->patterns[n] > patternCnt)
        {
            patternCnt = trkP.hdr->patterns[n];
        }
        printf(" %02x", trkP.hdr->patterns[n]);
    }
    patternCnt++;
    printf("\n");

    printf("patternCnt: %d\n", patternCnt);

    trkP.sampleData = (uint8_t *)&data[sizeof(struct track_hdr_s) + patternCnt * sizeof(pattern_s)];

    for (int n = 0; n < SAMPLE_COUNT; n++)
    {
        struct sample_s *sample = &trkP.hdr->sample[n];

        printf("samplename: %s\n", sample->samplename);
        printf("samplelength: %u\n", swapU16(sample->samplelength) * 2);
        printf("finetune: %u\n", sample->finetune);
        printf("volume: %u\n", sample->volume);
        printf("repeatpoint: %u\n", swapU16(sample->repeatpoint) * 2);
        printf("repeatlength: %u\n", swapU16(sample->repeatlength) * 2);
    }

    uint64_t samplLen = 0;
    for (int n = 0; n < SAMPLE_COUNT; n++)
    {
        struct sample_s *sample = &trkP.hdr->sample[n];
        samplLen += swapU16(sample->samplelength) * 2;
    }
    printf("sampleLen: %llu\n", samplLen);
}

bool TrackerLoadFile(void)
{
    for (uint16_t n = 0; n < 12 * 3; n++)
    {
        notePeriod[n].period = 856 * pow(2, - n / 12.0f);
        float p = notePeriod[n].period;
        p /= 856;
        p = log(p) / log(2.0f) * -12.0f;
    }

    {
        uint32_t rb;
        rb = readBytes(trk->data, sizeof(trk->data));
        if (rb != sizeof(trk->data))
        {
            printf("error reading file!\n");
            return false;
        }
        trkP.hdrData = trk->data;

        uint8_t patternCnt = 0;
        for (uint8_t n = 0; n < 128; n++)
        {
            if (trk->patterns[n] > patternCnt)
            {
                patternCnt = trk->patterns[n];
            }
        }
        patternCnt++;

        if (patternCnt * 1024 > sizeof(trk->data_pattern))
        {
            printf("pattern too big!\n");
            return false;
        }

        rb = readBytes(&trk->data[sizeof(struct track_hdr_s)], patternCnt * sizeof(pattern_s));

        if (rb != patternCnt * 1024)
        {
            printf("error reading file!\n");
            return false;
        }
        trkP.patternData = &trk->data[sizeof(struct track_hdr_s)];

        printf("SongName: %s\n", trk->songname);


        uint64_t samplLen = 0;
        for (int n = 0; n < SAMPLE_COUNT; n++)
        {
            struct sample_s *sample = &trk->sample[n];
            samplLen += swapU16(sample->samplelength) * 2;
        }
        printf("sampleLen: %llu\n", samplLen);

        if (samplLen > sizeof(trk->data_samples))
        {
            printf("sample data too big!\n");
            return false;
        }

        rb = readBytes(&trk->data[sizeof(struct track_hdr_s) + patternCnt * sizeof(pattern_s)], samplLen);
        if (rb != samplLen)
        {
            printf("error reading file!\n");
            return false;
        }
        trkP.sampleData = &trk->data[sizeof(struct track_hdr_s) + patternCnt * sizeof(pattern_s)];

        printf("songlength: %u\n", trk->songlength);
        printf("magicbyte: %u\n", trk->magicbyte);

        printf("pattern:");
        for (int n = 0; (n < PATTERNLEN) && (n < trk->songlength); n++)
        {
            printf(" %02x", trk->patterns[n]);
        }
        printf("\n");

        printf("letters:");
        for (int n = 0; n < 4; n++)
        {
            printf(" %02x", trk->letters[n]);
        }
        printf("\n");

        printf("letters: %c%c%c%c\n", trk->letters[0], trk->letters[1], trk->letters[2], trk->letters[3]);

        char mk[] = "M.K.";
        if (memcmp(trk->letters, mk, 4) != 0)
        {
            printf("letters in header no matching M.K.\nformat not supported yet!\n");
            return false;
        }

    }

    return true;
}

void PrintRowPattern(uint8_t patternIdx, uint8_t currentRow)
{
    for (int i = 0; i < CHANNEL_COUNT; i++)
    {

        PrintChannelInfo(&trkP.pattern[patternIdx].row[currentRow].data[i]);
    }
    printf("\n");
}

void TrackerBreakPattern(void)
{
    tply.currentRow = 63;
}

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t res = r;
    res <<= 8;
    res += g;
    res <<= 8;
    res += b;
    return res;
}

void TrackerGetClr(uint32_t *clr)
{
    uint32_t patclr[] = {0x660000, 0x551100, 0x442200, 0x333300, 0x224400, 0x115500, 0x006600, 0x005511, 0x004422, 0x003333, 0x002244, 0x001155, 0x000066, 0x110055, 0x220044, 0x330033, 0x440022, 0x550011, 0x660000, 0x551100, 0x442200, 0x333300, 0x224400, 0x115500};

    for (int n = 0; n < 64; n++)
    {
        clr[n] = 0;
    }
    for (int n = 0; n < 4; n++)
    {
        if (tply.channel[n].active)
        {
            uint8_t r = tply.channel[n].sampleId * 3;
            uint8_t g = 31 - r;
            uint8_t b = 0;
            clr[n] = rgb(r, g, b);
            clr[n] = patclr[tply.channel[n].sampleId] * tply.channel[n].disp;
            clr[tply.channel[n].sampleId + 8] = rgb(r, g, b);
            clr[tply.channel[n].sampleId + 8] = patclr[tply.channel[n].sampleId] * tply.channel[n].disp;
            if (tply.channel[n].disp > 1)
            {
                tply.channel[n].disp --;
            }
        }


    }
}

void ProcessChannel(uint8_t patternIdx, uint8_t currentRow, uint8_t chId)
{
    struct channel_s *ch;
    uint16_t noteperiod;
    uint8_t effectNum;
    uint8_t effectParam;
    uint8_t sampleNum;

    ch = &trkP.pattern[patternIdx].row[currentRow].data[chId];
    noteperiod = ((ch->data[0] & 0x0F) << 8UL) + (ch->data[1]);
    effectNum = (ch->data[2] & 0x0F);
    effectParam = (ch->data[3]);
    sampleNum = (ch->data[0] & 0xF0) + (ch->data[2] >> 4);

    if ((noteperiod != 0) && (sampleNum > 0))
    {
        if (sampleNum >= SAMPLE_COUNT)
        {
            /* ignore, that would be an error */
        }
        else
        {
            tply.channel[chId].active = true;
            tply.channel[chId].volume = 64;
            tply.channel[chId].sampleInfo = &tply.sampleInfo[sampleNum - 1];
            tply.channel[chId].noteperiod = noteperiod;

            tply.channel[chId].pos = 0;
            tply.channel[chId].subpos = 0;

            tply.channel[chId].sampleId = sampleNum - 1;
            tply.channel[chId].disp = 2;
        }
    }

    if (effectNum == EFFECT_SET_VOLUME)
    {
        tply.channel[chId].volume = effectParam;
        if (effectParam == 0)
        {
            tply.channel[chId].active = false;
        }
    }

    /*
         Cmd A. Volumeslide [Speed:$0-$F/$0-$F]
         --------------------------------------
         Usage: $A + upspeed + downspeed
         Example: C-300A05  5 is the speed to
                        turn the volume down.
                  C-300A40  4 is the speed to
                                 slide it up.
         NOTE: The slide will be called as
         many times as the speed of the song.
         The slower the song, the more the
         volume will be changed on each note.
     */
    if (effectNum == EFFECT_VOLUME_SLIDE)
    {

    }

    /*
         Cmd B. Position-jump [Pos:$00-$7F]
           ----------------------------------
         Usage: $B + position to continue at
         Example: C-300B01  B is the command,
                         1 is the position to
                         restart the song at.
         This command will also perform a
         pattern-break (see 2 pages below).
         You can use this command instead of
         restart as on noisetracker, but you
         must enter the position in hex!
     */
    if (effectNum == EFFECT_POSITION_JUMP)
    {
        tply.jumpIdx = effectParam;
        TrackerBreakPattern();
    }

    if (effectNum == EFFECT_SET_SPEED)
    {
        tply.speed = effectParam;
    }

    if (effectNum == EFFECT_PATTERN_BREAK)
    {
        TrackerBreakPattern();
    }
}

void TrackerProcess(uint64_t passed)
{
    if (tply.active || nextStep)
    {
        tply.timeOver += passed * tply.tempo * 2;
        tply.timeOver2 += passed * ((uint64_t)tply.rate);
        uint64_t procTime = 0;

        uint64_t time_limit = tply.rate * tply.speed * 5;
        time_limit *= tply.tempoMultiplier;
        time_limit /= 0x10000;

        /* tempo of 125 matches 50 hz -> *2 left and *5 on the right to get a / 2.5 */
        if ((tply.timeOver > time_limit) || nextStep)
        {
            if (nextStep)
            {
                nextStep = false;
                tply.timeOver = 0;
            }
            else
            {
                tply.timeOver -= time_limit;
            }
            procTime += tply.rate;


            tply.currentRow ++;
            if (tply.currentRow >= 64)
            {
                tply.currentRow = 0;
                tply.patternIdx ++;
                tply.currentPattern = trkP.hdr->patterns[tply.patternIdx];

                if (tply.patternIdx >= trkP.hdr->songlength)
                {
                    tply.active = false;
                }
            }
        }

        static uint8_t lastRow = 255;
        if (tply.currentRow != lastRow)
        {
            for (int ch = 0; ch < CHANNEL_COUNT; ch++)
            {
                ProcessChannel(tply.currentPattern, tply.currentRow, ch);
            }
            lastRow = tply.currentRow;
        }

        uint16_t samplCnt = 0;

        while ((tply.timeOver2 > tply.rate) && (procTime > 0))
        {
            samplCnt++;

            tply.timeOver2 -= tply.rate;
            procTime -= tply.rate;
        }
    }
}

void TrackerStartPlayback(void)
{
    tply.patternIdx = 0;
    tply.jumpIdx = 0xFF;
    tply.currentPattern = trkP.hdr->patterns[0];
    tply.currentRow = 0;
    tply.timeOver = 0;
    tply.timeOver2 = 0;
    tply.tempo = 125;
    tply.speed = 6;

    tply.tempoMultiplier = 0x10000;
    tply.pitchMultiplier = 0x10000;


    uint32_t offset = 0;
    for (int n = 0; n < SAMPLE_COUNT; n++)
    {
        struct sample_s *sample = &trkP.hdr->sample[n];
        tply.sampleInfo[n].volume = sample->volume;
        tply.sampleInfo[n].finetune = sample->finetune;
        tply.sampleInfo[n].sampleStart = &trkP.sample[offset];
        tply.sampleInfo[n].sampleLen = swapU16(sample->samplelength) * 2;

        if (swapU16(sample->repeatlength) * 2 > 2)
        {
            tply.sampleInfo[n].loop = true;
            tply.sampleInfo[n].loopStart = swapU16(sample->repeatpoint) * 2;
            tply.sampleInfo[n].loopEnd = swapU16(sample->repeatpoint) * 2 + swapU16(sample->repeatlength) * 2 ;
        }
        else
        {
            tply.sampleInfo[n].loop = false;
        }

        offset += swapU16(sample->samplelength) * 2;
    }

    for (int n = 0; n < 4; n++)
    {
        tply.channel[n].active = false;
        tply.channel[n].sampleInfo = NULL;
        tply.channel[n].volume = 64;
        chActive[n] = true;
    }

    for (int n = 0; n < 16; n++)
    {
        tply.channelMIDI[n].active = false;
        tply.channelMIDI[n].sampleInfo = NULL;
        tply.channelMIDI[n].volume = 64;
    }

    tply.active = true;
}

void TrackerSetup(uint32_t sample_rate)
{
    tply.rate = sample_rate;
}


//#define FUNNY_SOUND

int16_t TrackerProcessChannelPlayer(struct channel_player_s *ch)
{
    int16_t newSample = 0;

    if (ch->active && (ch->sampleInfo != NULL))
    {
        newSample = (int16_t)ch->sampleInfo->sampleStart[ch->pos];
        newSample *= ch->volume;
        newSample /= 64;
        newSample *= ch->sampleInfo->volume;

        ch->subpos += 75;

        uint64_t limit = ch->noteperiod;
        limit *= tply.pitchMultiplier;
        limit /= 0x10000;

        while (ch->subpos > limit)
        {
            ch->subpos -= limit;
            ch->pos++;
        }
        if (ch->sampleInfo->loop)
        {
            if (ch->pos >= ch->sampleInfo->loopEnd)
            {
                ch->pos = ch->sampleInfo->loopStart;
            }
        }
        if (ch->pos >= ch->sampleInfo->sampleLen)
        {
            ch->active = false;
            ch->pos = 0;
            ch->subpos = 0;
        }
    }
    else
    {
        ch->pos = 0;
        ch->subpos = 0;
    }

    return newSample;
}

void TrackerProcessSamples(Q1_14 *chL, Q1_14 *chrR, uint64_t count)
{
    for (uint64_t n = 0; n < count; n++)
    {
        TrackerProcess(1);

        Q1_14 sample;
        sample.s16 = 0;

        for (uint8_t chIdx = 0; chIdx < CHANNEL_COUNT; chIdx ++)
        {
            if (chActive[chIdx])
            {
                sample.s16 += TrackerProcessChannelPlayer(&tply.channel[chIdx]);
            }
        }

        for (uint8_t chIdx = 0; chIdx < 16; chIdx ++)
        {
            sample.s16 += TrackerProcessChannelPlayer(&tply.channelMIDI[chIdx]);
        }

        chL[n].s16 += sample.s16;
        chrR[n].s16 += sample.s16;
    }
}

void TrackerProcessOutput(void)
{
    static uint8_t lastPrintRow = 0xFF;
    static uint8_t lastPrintPattern = 0xFF;

    if (tply.currentPattern != lastPrintPattern || tply.currentRow != lastPrintRow)
    {
        lastPrintPattern = tply.currentPattern;
        lastPrintRow = tply.currentRow;
        printf(" %02x %02x ", tply.currentPattern, tply.currentRow);
        PrintRowPattern(tply.currentPattern, tply.currentRow);

    }
}

void Tracker_NoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
    float period = 428 * 16;
    float notef = note;
    note += 60;
    period *= pow(2.0f, -notef / 12.0f);
    tply.channelMIDI[ch].noteperiod = period;
    printf("noteon: %f, %f, %u\n", period, notef, tply.channelMIDI[ch].noteperiod);
    tply.channelMIDI[ch].active = true;
    tply.channelMIDI[ch].sampleInfo = &tply.sampleInfo[ch];
    tply.channelMIDI[ch].pos = 0;
    tply.channelMIDI[ch].subpos = 0;
    tply.channelMIDI[ch].volume = (vel / 2) + 1;
}

void Tracker_NoteOff(uint8_t ch, uint8_t note, uint8_t vel __attribute__((unused)))
{
    float period = 428 * 16;
    float notef = note;
    note += 60;
    period *= pow(2.0f, -notef / 12.0f);
    uint16_t periodU = period;
    if (tply.channelMIDI[ch].noteperiod == periodU)
    {
        tply.channelMIDI[ch].active = false;
    }
}

void Tracker_NoteOff(uint8_t ch, uint8_t note)
{
    Tracker_NoteOff(ch, note, 0);
}

void Tracker_Restart(uint8_t param __attribute__((unused)), uint8_t value)
{
    if (value > 0)
    {
        TrackerStartPlayback();
    }
}

void Tracker_Stop(uint8_t param __attribute__((unused)), uint8_t value)
{
    if (value > 0)
    {
        tply.active = false;
        for (int n = 0; n < CHANNEL_COUNT; n++)
        {
            tply.channel[n].active = false;
        }
    }
}

void Tracker_End(uint8_t param __attribute__((unused)), uint8_t value)
{
    if (value > 0)
    {
        if (trkP.hdr)
        {
            tply.patternIdx = trkP.hdr->songlength;
        }
    }
}

void Tracker_Start(uint8_t param __attribute__((unused)), uint8_t value)
{
    if (value > 0)
    {
        if (tply.active)
        {
            tply.active = false;
        }
        else
        {
            tply.active = true;
        }
    }
}

void Tracker_Step(uint8_t param __attribute__((unused)), uint8_t value)
{
    if (value > 0)
    {
        nextStep = true;
    }
}

void Tracker_SetTempo(uint8_t param __attribute__((unused)), uint8_t value)
{
    Status_ValueChangedInt("Tracker", "Tempo", value);
    float multiplier = log2fromU7(value, 2, -2);
    multiplier *= 65536;
    tply.tempoMultiplier = multiplier;
}

void Tracker_SetPitch(uint8_t param, uint8_t value)
{
    Status_ValueChangedInt("Tracker", "Pitch", value);
    float multiplier = log2fromU7(value, 2, -2);
    multiplier *= 65536;
    tply.pitchMultiplier = multiplier;
}


void Tracker_ToggleMute(uint8_t param, uint8_t value)
{
    if (value > 0)
    {
        if (param < 4)
        {
            chActive[param] = !chActive[param];
        }
    }
}

bool Tracker_IsPlaying(void)
{
    return tply.active;
}

bool Tracker_HasTrackFinished(void)
{
    if (trkP.hdr && (tply.active == false))
    {
        return (tply.patternIdx >= trkP.hdr->songlength);
    }
    else
    {
        return false;
    }
}
