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
 * @file    ml_fm.cpp
 * @author  Marcel Licence
 * @date    15.09.2024
 *
 * @brief This is a simple implementation of an FM Synthesizer module
 *
 * - similar behavior as the YM2612 (known from Sega Mega Drive / Sega Genesis)
 * - polyphonic playback of voices with 4 operators
 * - AR DR1 D2L DR2 RR (adsr) envelope
 * - feedback (not completely like the original YM2612)
 * - level adjustable
 * - feedback
 * - length adjustable
 *
 *
 * @see https://youtu.be/rGTw05GKwvU
 */

#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#include <ml_utils.h>
#include <ml_status.h>


#include <stdio.h>
#include <stdint.h>
#include <math.h>
#ifndef GLOBAL_SINE
#include <stdlib.h>
#endif

static float sample_rate = 0;

#ifdef GLOBAL_SINE
extern float *sine;
#endif
#ifdef ARDUINO_RUNNING_CORE
#define SINE_BIT    11UL /* lower resolution required due to the fact that heap blocks are smaller */
#else
#define SINE_BIT    12UL
#endif

#define SINE_CNT    (1<<SINE_BIT)
#define SINE_MSK    ((1<<SINE_BIT)-1)
#define SINE_I(i)   ((i) >> (32 - SINE_BIT)) /* & SINE_MSK */

#ifndef GLOBAL_SINE
static float *sine = NULL;

static void Sine_Init(void)
{
    uint32_t memSize = sizeof(float) * SINE_CNT;
    sine = (float *)malloc(memSize);
    if (sine == NULL)
    {
        Status_LogMessage("not enough heap memory for sine buffer!\n");
    }
    for (int i = 0; i < SINE_CNT; i++)
    {
        sine[i] = (float)sin(i * 2.0 * M_PI / SINE_CNT);
    }
}
#endif

static float SineNorm(float alpha_div2pi)
{
    uint32_t index = ((uint32_t)(alpha_div2pi * ((float)SINE_CNT))) % SINE_CNT;
    return sine[index];
}

static float SineNormU32(uint32_t pos)
{
    return sine[SINE_I(pos)];
}

#ifndef ARDUINO
float millis()
{
    return 0.0f;
}
#endif

#define OP1 3
#define OP2 2
#define OP3 1
#define OP4 0

#define MIDI_CH_CNT 16

#define FM_VOICE_CNT    6

#define fabs(a) (a>0.0)?(a):(-a)
#define fmax(a,b) (a>b)?(a):(b)

enum ssg_eg_e
{
    ssgeg_off,
    ssgeg_repeat,
    ssgeg_once,
    ssgeg_mirror,
    ssgeg_once_high,
    ssgeg_loop_rev,
    ssgeg_once_inv,
    ssgeg_mirro_inv,
    ssgeg_once_high_inv
};

struct op_properties_s
{
    float mw; /* mod wheel  to amplitude */
    float am; /* amplitude modulation toggle, from lfo-ams */

    uint32_t ar; /* attack */
    uint32_t d1r; /* decay until d2l */
    uint32_t d2r; /* */
    float d2l; /* decay level */
    uint32_t rr;
    uint32_t rs; /* key rate dings, make envelope faster or slower */

    float ssgeg; /* software controlled sound generator - envelope behavior */
    float mul;

    float mul_fine;
    float mul_coarse;

    float fixed;
    float dt; /* detune */
    float vel; /* hm for what is this good for? */
    float tl; /* total level: envelope amplitude */

    float vel_to_tl; /* addon to push MIDI vel to tl */
};

struct custom_properties_s
{
    float feedback; /* op1 feedback */

    float lfo_enable;
    float lfo_speed;
    float fms; /* pitch modulation sensitivity */
    float fmsmw;
    float ams; /* amplitude modulation sensitivity */

    float legato_retrig;
    float pitchbend_range;
    float volume;
    float alg;
};

typedef uint8_t envState_t;

#define ENV_ATTACK  (envState_t)0
#define ENV_DECAY1  (envState_t)1
#define ENV_DECAY2  (envState_t)2
#define ENV_RELEASE (envState_t)3
#define ENV_OFF     (envState_t)4

struct synthTone_s
{
    float pitch;
    float pitchCalc;

    float out;
    float in;

    uint32_t pos;
    float sine_preoout;

    float lvl_env;
    float lvl_add;
    envState_t state;
    int stateLen;

    float vel;

    struct op_properties_s *op_prop;
};

struct synthVoice_s
{
    struct synthTone_s op[4];
    bool active;

    uint8_t midiNote;
    uint8_t midiCh;
    float out;
    float outSlow;

    struct channelSettingParam_s *settings;
};

struct synthVoice_s fmVoice[FM_VOICE_CNT];

struct channelSettingParam_s
{
    struct custom_properties_s props;
    struct op_properties_s op_prop[4];

    float fmFeedback;

    int algo;

    bool mono;
    bool legato;

    uint8_t notes[16];
    uint8_t noteStackCnt;
};

static uint32_t milliCnt = 0;

static float modulationDepth = 0.0f;
static float modulationSpeed = 5.0f;
static float modulationPitch = 1.0f;
static float pitchBendValue = 0.0f;
#ifdef PRESSURE_SENSOR_ENABLED
static float pressureValue = 0.0f;
static float pressureValueFilt = 0.0f;
#endif
static float pitchMultiplier = 1.0f;

static bool initChannelSetting = false;
static uint32_t initChannelSettingCnt = 0;

static struct channelSettingParam_s channelSettings[MIDI_CH_CNT];

static struct channelSettingParam_s *currentChSetting = &channelSettings[0];

static uint8_t selectedOp = OP4;

static struct op_properties_s op_props_silent;

static float multiplierPitchToAddValue = 0; // ((float)(1ULL << 32ULL) / ((float)sample_rate));


static void FmSynth_ProcessOperator(struct synthTone_s *osc);
static void FmSynth_EnvStateProcess(struct synthTone_s *osc);
static void FmSynth_AlgMixProcess(float *out, struct synthVoice_s *voice);


void FmSynth_ToneEnvUpdate(struct synthTone_s *tone)
{
    uint32_t attack = tone->op_prop->ar * tone->op_prop->rs;
    attack = attack > 0 ? attack : 1;

    uint32_t decay1 = tone->op_prop->d1r * tone->op_prop->rs;
    decay1 = decay1 > 0 ? decay1 : 1;

    uint32_t decay2 = tone->op_prop->d2r * tone->op_prop->rs;
    decay2 = decay2 > 0 ? decay2 : 1;

    uint32_t release = tone->op_prop->rr * tone->op_prop->rs;
    release = release > 0 ? release : 1;

    switch (tone->state)
    {
    case ENV_ATTACK:
        tone->lvl_add = 1.0f / (((float)attack));
        tone->stateLen = attack;
        break;
    case ENV_DECAY1:
        tone->lvl_add = (tone->op_prop->d2l - tone->lvl_env) / (((float)decay1));
        tone->stateLen = decay1;
        break;
    case ENV_DECAY2:
        tone->lvl_add = (0.0f - tone->lvl_env) / (((float)decay2));
        tone->stateLen = decay2;
        break;
    case ENV_RELEASE:
        tone->lvl_add = (0.0f - tone->lvl_env) / (((float)release));
        tone->stateLen = release;
        break;
    case ENV_OFF:
        tone->lvl_add = 0;
        tone->stateLen = 0;

        tone->lvl_env = 0.0f; /* ensure output is quiet */
        break;

    default:
        tone->lvl_add = 0;
        tone->stateLen = 1;
        tone->state = ENV_OFF;
        break;
    }
}

void FmSynth_ToneInit(struct synthTone_s *tone, struct op_properties_s *op_props)
{
    tone->pos = 0;
    tone->sine_preoout = 0.0f;

    tone->in = 0.0f;
    tone->out = 0.0f;

    tone->vel = 1.0f;

    tone->lvl_env = 0.0f;
    tone->lvl_add = 0.0f;

    tone->state = ENV_ATTACK;
    tone->op_prop = op_props;

    FmSynth_ToneEnvUpdate(tone);
}

void FmSynth_InitOpProps(struct op_properties_s *op_props)
{
    op_props->ar = 1;
    op_props->d1r = 32767;
    op_props->d2l = 1;
    op_props->d2r = 32767;
    op_props->rr = 1;
    op_props->rs = 50.0f;

    op_props->am = 0;

    op_props->mul_coarse = 1.0f;
    op_props->mul_fine = 0.0f;
    op_props->mul = 1.0f;

    op_props->dt = 0.0f;

    op_props->vel = 0;
    op_props->tl = 0;

    op_props->vel_to_tl = 0.0f;
}

void FmSynth_IntiVoice(struct synthVoice_s *voice)
{
    voice->out = 0.0f;
    voice->midiCh = 0xff;
    voice->midiNote = 0xff;
    voice->settings = currentChSetting;

    for (int i = 0; i < 4; i++)
    {
        FmSynth_ToneInit(&voice->op[i], &op_props_silent);
    }
}

void FmSynth_InitChannelSettings(struct channelSettingParam_s *channelSettings)
{

    channelSettings->fmFeedback = 0;
    channelSettings->algo = 0;
    channelSettings->mono = false;
    channelSettings->legato = false;
    channelSettings->noteStackCnt = 0;

    for (int i = 0; i < 4; i++)
    {
        FmSynth_InitOpProps(&channelSettings->op_prop[i]);
    }
    channelSettings->op_prop[OP4].tl = 1.0f;
}

void FmSynth_Init(float sample_rate_in)
{
#ifndef GLOBAL_SINE
    Sine_Init();
#endif

    sample_rate = sample_rate_in;
    multiplierPitchToAddValue = ((float)(1ULL << 32ULL) / ((float)sample_rate));

    FmSynth_InitOpProps(&op_props_silent);
    op_props_silent.tl = 0.0f;
    op_props_silent.d1r = 1;
    op_props_silent.d2r = 1;

    for (int ch = 0; ch < MIDI_CH_CNT; ch++)
    {
        FmSynth_InitChannelSettings(&channelSettings[ch]);
    }

    for (int j = 0; j < FM_VOICE_CNT; j++)
    {
        FmSynth_IntiVoice(&fmVoice[j]);
    }

    struct channelSettingParam_s *setting = &channelSettings[0];

    setting->algo = 0;
    setting->fmFeedback = 0;
    setting->op_prop[0].ar = 1.855923;
    setting->op_prop[0].d1r = 1.855923;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767.000000;
    setting->op_prop[0].rr = 1.855923;
    setting->op_prop[0].rs = 50.000000;
    setting->op_prop[0].tl = 1.000000;
    setting->op_prop[0].mul = 2.500000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1.855923;
    setting->op_prop[1].d1r = 1.855923;
    setting->op_prop[1].d2l = 0.346456;
    setting->op_prop[1].d2r = 32767.000000;
    setting->op_prop[1].rr = 1.855923;
    setting->op_prop[1].rs = 50.000000;
    setting->op_prop[1].tl = 0.322834;
    setting->op_prop[1].mul = 3.500000;
    setting->op_prop[1].vel_to_tl = 0.614172;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1.855923;
    setting->op_prop[2].d1r = 1.855923;
    setting->op_prop[2].d2l = 0.401574;
    setting->op_prop[2].d2r = 32767.000000;
    setting->op_prop[2].rr = 1.855923;
    setting->op_prop[2].rs = 50.000000;
    setting->op_prop[2].tl = 0.047244;
    setting->op_prop[2].mul = 11.000000;
    setting->op_prop[2].vel_to_tl = 0.999998;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1.855923;
    setting->op_prop[3].d1r = 1.855923;
    setting->op_prop[3].d2l = 1.000000;
    setting->op_prop[3].d2r = 32767.000000;
    setting->op_prop[3].rr = 1.855923;
    setting->op_prop[3].rs = 50.000000;
    setting->op_prop[3].tl = 0.000000;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    setting = &channelSettings[1];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 1;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1023;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 1.000000;
    setting->op_prop[0].mul = 2.500000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 1;
    setting->op_prop[1].d2l = 0.346456;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1023;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.322834;
    setting->op_prop[1].mul = 3.500000;
    setting->op_prop[1].vel_to_tl = 0.614172;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 1;
    setting->op_prop[2].d2l = 0.401574;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1023;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.047244;
    setting->op_prop[2].mul = 11.000000;
    setting->op_prop[2].vel_to_tl = 0.999998;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 1;
    setting->op_prop[3].d2l = 1.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.000000;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * like a guitar
     */
    setting = &channelSettings[2];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 1.000000;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 147;
    setting->op_prop[1].d2l = 0.637794;
    setting->op_prop[1].d2r = 2386;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.354330;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 1239;
    setting->op_prop[2].d2l = 0.999998;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.181102;
    setting->op_prop[2].mul = 1.250000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 26;
    setting->op_prop[3].d2l = 0.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.291338;
    setting->op_prop[3].mul = 0.630000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * some hard voice
     */
    setting = &channelSettings[3];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.118110;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 32767;
    setting->op_prop[2].d2l = 1.000000;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.094488;
    setting->op_prop[2].mul = 3.250000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 1460;
    setting->op_prop[3].d2l = 0.307086;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.999998;
    setting->op_prop[3].mul = 0.750000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * bassy base
     */
    setting = &channelSettings[4];

    setting->algo = 0;
    setting->fmFeedback = 0.999998;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 1;
    setting->op_prop[0].d2l = 0.999998;
    setting->op_prop[0].d2r = 3050;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 1.000000;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 1345;
    setting->op_prop[1].d2l = 0.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.251968;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 32767;
    setting->op_prop[2].d2l = 1.000000;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.039370;
    setting->op_prop[2].mul = 1.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 115;
    setting->op_prop[3].d2l = 0.622046;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.094488;
    setting->op_prop[3].mul = 0.500000;
    setting->op_prop[3].vel_to_tl = 0.141732;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * organ
     */
    setting = &channelSettings[5];
    setting->algo = 7;
    setting->fmFeedback = 0.267716;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.881888;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 135;
    setting->op_prop[1].d2l = 0.448818;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.653542;
    setting->op_prop[1].mul = 3.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 14;
    setting->op_prop[2].d2l = 0.267716;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.401574;
    setting->op_prop[2].mul = 8.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 5;
    setting->op_prop[3].d2l = 0.496062;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.236220;
    setting->op_prop[3].mul = 4.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * some bass
     */
    setting = &channelSettings[6];

    setting->algo = 3;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.047244;
    setting->op_prop[1].mul = 0.500000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 427;
    setting->op_prop[2].d2l = 0.000000;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.196850;
    setting->op_prop[2].mul = 1.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 50;
    setting->op_prop[3].d2l = 0.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.062992;
    setting->op_prop[3].mul = 2.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * schnatter bass
     */
    setting = &channelSettings[7];

    setting->algo = 2;
    setting->fmFeedback = 0.055118;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 363;
    setting->op_prop[0].d2l = 0.614172;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.937006;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 204;
    setting->op_prop[1].d2l = 0.196850;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.314960;
    setting->op_prop[1].mul = 2.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 90;
    setting->op_prop[2].d2l = 0.543306;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.716534;
    setting->op_prop[2].mul = 2.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 10;
    setting->op_prop[3].d2l = 0.299212;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.724408;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * harpischord?
     */
    setting = &channelSettings[8];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 1.000000;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.102362;
    setting->op_prop[1].mul = 3.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 1239;
    setting->op_prop[2].d2l = 0.385826;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.094488;
    setting->op_prop[2].mul = 1.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 28;
    setting->op_prop[3].d2l = 0.283464;
    setting->op_prop[3].d2r = 4593;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.173228;
    setting->op_prop[3].mul = 15.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /*
     * some harsh
     */
    setting = &channelSettings[9];

    setting->algo = 3;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 2.800000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.236220;
    setting->op_prop[1].mul = 2.200000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 32767;
    setting->op_prop[2].d2l = 1.000000;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.173228;
    setting->op_prop[2].mul = 2.400000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 393;
    setting->op_prop[3].d2l = 0.307086;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.157480;
    setting->op_prop[3].mul = 2.400000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /* string bass */
    setting = &channelSettings[10];

    setting->algo = 3;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 0.500000;
    setting->op_prop[0].vel_to_tl = 0.078740;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.385826;
    setting->op_prop[1].mul = 0.500000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 334;
    setting->op_prop[2].d2l = 0.251968;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.330708;
    setting->op_prop[2].mul = 1.010000;
    setting->op_prop[2].vel_to_tl = 0.803148;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 27818;
    setting->op_prop[3].d2l = 0.181102;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.700786;
    setting->op_prop[3].mul = 0.500000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;


    /*
     * kick bass
     */
    setting = &channelSettings[11];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 2.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.078740;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 46;
    setting->op_prop[2].d2l = 0.078740;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.094488;
    setting->op_prop[2].mul = 11.000000;
    setting->op_prop[2].vel_to_tl = 0.425196;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 32767;
    setting->op_prop[3].d2l = 1.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.000000;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /* wood bass */
    setting = &channelSettings[12];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 2.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.133858;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 46;
    setting->op_prop[2].d2l = 0.078740;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.236220;
    setting->op_prop[2].mul = 3.000000;
    setting->op_prop[2].vel_to_tl = 0.425196;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 32767;
    setting->op_prop[3].d2l = 1.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.000000;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /* saw bass */
    setting = &channelSettings[13];


    setting->algo = 5;
    setting->fmFeedback = 0.503936;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.251968;
    setting->op_prop[1].mul = 1.010000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 546;
    setting->op_prop[2].d2l = 0.000000;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.330708;
    setting->op_prop[2].mul = 2.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 308;
    setting->op_prop[3].d2l = 0.251968;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.141732;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /* plug sound */
    setting = &channelSettings[14];

    setting->algo = 2;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 1;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 0.999998;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 76;
    setting->op_prop[1].d2l = 0.267716;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.212598;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 1;
    setting->op_prop[2].d1r = 36;
    setting->op_prop[2].d2l = 0.393700;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.110236;
    setting->op_prop[2].mul = 4.000000;
    setting->op_prop[2].vel_to_tl = 0.527558;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 1;
    setting->op_prop[3].d1r = 32767;
    setting->op_prop[3].d2l = 1.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.055118;
    setting->op_prop[3].mul = 1.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;

    /* fady stuff */
    setting = &channelSettings[15];

    setting->algo = 0;
    setting->fmFeedback = 0.000000;
    setting->op_prop[0].ar = 70;
    setting->op_prop[0].d1r = 32767;
    setting->op_prop[0].d2l = 1.000000;
    setting->op_prop[0].d2r = 32767;
    setting->op_prop[0].rr = 1;
    setting->op_prop[0].rs = 50;
    setting->op_prop[0].tl = 1.000000;
    setting->op_prop[0].mul = 1.000000;
    setting->op_prop[0].vel_to_tl = 0.000000;
    setting->op_prop[0].am = 0.000000;
    setting->op_prop[0].mw = 0.000000;
    setting->op_prop[0].vel = 0.000000;
    setting->op_prop[1].ar = 1;
    setting->op_prop[1].d1r = 32767;
    setting->op_prop[1].d2l = 1.000000;
    setting->op_prop[1].d2r = 32767;
    setting->op_prop[1].rr = 1;
    setting->op_prop[1].rs = 50;
    setting->op_prop[1].tl = 0.023622;
    setting->op_prop[1].mul = 1.000000;
    setting->op_prop[1].vel_to_tl = 0.000000;
    setting->op_prop[1].am = 0.000000;
    setting->op_prop[1].mw = 0.000000;
    setting->op_prop[1].vel = 0.000000;
    setting->op_prop[2].ar = 2198;
    setting->op_prop[2].d1r = 32767;
    setting->op_prop[2].d2l = 1.000000;
    setting->op_prop[2].d2r = 32767;
    setting->op_prop[2].rr = 1;
    setting->op_prop[2].rs = 50;
    setting->op_prop[2].tl = 0.496062;
    setting->op_prop[2].mul = 2.000000;
    setting->op_prop[2].vel_to_tl = 0.000000;
    setting->op_prop[2].am = 0.000000;
    setting->op_prop[2].mw = 0.000000;
    setting->op_prop[2].vel = 0.000000;
    setting->op_prop[3].ar = 30191;
    setting->op_prop[3].d1r = 32767;
    setting->op_prop[3].d2l = 1.000000;
    setting->op_prop[3].d2r = 32767;
    setting->op_prop[3].rr = 1;
    setting->op_prop[3].rs = 50;
    setting->op_prop[3].tl = 0.000000;
    setting->op_prop[3].mul = 4.000000;
    setting->op_prop[3].vel_to_tl = 0.000000;
    setting->op_prop[3].am = 0.000000;
    setting->op_prop[3].mw = 0.000000;
    setting->op_prop[3].vel = 0.000000;
}

static void FmSynth_ProcessOperator(struct synthTone_s *osc)
{
    float pitch = osc->pitchCalc;

    int32_t phaseShift = 100000 * (int32_t)((float)(osc->in) * (float)multiplierPitchToAddValue);
    osc->in = 0;

    int32_t add = (int32_t)(pitch * multiplierPitchToAddValue);
    osc->pos += add ;

#if 0
    osc->pos += phaseShift;
    osc->sine_preoout = SineNormU32(osc->pos) ;
#else
    osc->sine_preoout = SineNormU32(osc->pos + phaseShift) ;
#endif
    osc->lvl_env += osc->lvl_add;
}

static void FmSynth_EnvStateProcess(struct synthTone_s *osc)
{
    osc->stateLen --;
    if (osc->stateLen <= 0)
    {
        if (osc->state != ENV_OFF)
        {
            osc->state++;
            FmSynth_ToneEnvUpdate(osc);
        }
    }
}

static void FmSynth_AlgMixProcess(float *out, struct synthVoice_s *voice)
{
    voice->out = 0.0f;

    voice->op[OP1].in += (voice->settings->fmFeedback * voice->op[OP1].out);

    switch (voice->settings->algo)
    {
    /*
     * alg 1:
     * 1->2
     * 2->3
     * 3->4
     * 4->O
     * eg. Distortion guitar, "high hat chopper" (?) bass
    */
    case 0:
        voice->out += voice->op[OP4].out;
        voice->op[OP4].in += voice->op[OP3].out;
        voice->op[OP3].in += voice->op[OP2].out;
        voice->op[OP2].in += voice->op[OP1].out;
        break;
    /*
    * alg 2:
    * 1->3
    * 2->3
    * 3->4
    * 4->O
    * eg. Harp, PSG (programmable sound generator) sound
    */
    case 1:
        voice->out += voice->op[OP4].out;
        voice->op[OP4].in += voice->op[OP3].out;
        voice->op[OP3].in += voice->op[OP2].out;
        voice->op[OP3].in += voice->op[OP1].out;
        break;
    /*
    * alg 3:
    * 1->3
    * 2->4
    * 3->4
    * 4->O
    * eg. Bass, electric guitar, brass, piano, woods
    */
    case 2:
        voice->out += voice->op[OP4].out;
        voice->op[OP4].in += voice->op[OP3].out;
        voice->op[OP4].in += voice->op[OP2].out;
        voice->op[OP3].in += voice->op[OP1].out;
        break;
    /*
    * alg 4:
    * 1->2
    * 2->4
    * 3->4
    * 4->O
    * eg. Strings, folk guitar, chimes
    */
    case 3:
        voice->out += voice->op[OP4].out;
        voice->op[OP4].in += voice->op[OP3].out;
        voice->op[OP4].in += voice->op[OP2].out;
        voice->op[OP2].in += voice->op[OP1].out;
        break;
    /*
    * alg 5:
    * 1->2
    * 2->O
    * 3->4
    * 4->O
    * eg. Flute, bells, chorus, bass drum, snare drum, tom-tom
    */
    case 4:
        voice->out += voice->op[OP4].out;
        voice->op[OP4].in += voice->op[OP3].out;
        voice->out += voice->op[OP2].out;
        voice->op[OP2].in += voice->op[OP1].out;
        break;
    /*
    * alg 6:
    * 1->2
    * 1->3
    * 1->4
    * eg. Brass, organ
    */
    case 5:
        voice->out += voice->op[OP4].out;
        voice->out += voice->op[OP3].out;
        voice->out += voice->op[OP2].out;
        voice->op[OP4].in += voice->op[OP1].out;
        voice->op[OP3].in += voice->op[OP1].out;
        voice->op[OP2].in += voice->op[OP1].out;
        break;
    /*
     * alg 7:
    * 1->2
    * 2->O
    * 3->O
    * 4->O
    * eg. Xylophone, tom-tom, organ, vibraphone, snare drum, base drum
     */
    case 6:
        voice->out += voice->op[OP4].out;
        voice->out += voice->op[OP3].out;
        voice->out += voice->op[OP2].out;
        voice->op[OP2].in += voice->op[OP1].out;
        break;
    /*
    * alg 8:
    * 1-4->O
    * eg. Pipe organ
     */
    case 7:
        voice->out += voice->op[OP4].out;
        voice->out += voice->op[OP3].out;
        voice->out += voice->op[OP2].out;
        voice->out += voice->op[OP1].out;
        break;
    }

    *out += voice->out;

    voice->outSlow = fmax(fabs(voice->out), voice->outSlow);
}

inline
float FmSynth_GetModulationPitchMultiplier(void)
{
    float modSpeed = modulationSpeed;
    return modulationDepth * modulationPitch * (SineNorm((modSpeed * ((float)milliCnt) / 1000.0f)));
}

void FmSynth_Process(const float *in __attribute__((unused)), float *out, int bufLen)
{
    milliCnt += (bufLen * 1000) / sample_rate;

    float pitchVar = pitchBendValue + FmSynth_GetModulationPitchMultiplier();
    pitchMultiplier = pow(2.0f, pitchVar / 12.0f);

    for (int n = 0; n < bufLen; n++)
    {
#ifdef PRESSURE_SENSOR_ENABLED
        pressureValueFilt = pressureValueFilt * 0.99f + pressureValue * 0.01f;
#endif

        float sample = 0.0f;

        for (int j = 0; j < FM_VOICE_CNT; j++)
        {
            struct synthVoice_s *voice = &fmVoice[j];

            for (int i = 0; i < 4; i++)
            {
                struct synthTone_s *osc = &voice->op[i];
                osc->pitchCalc = osc->op_prop->mul * osc->pitch * pitchMultiplier;
                FmSynth_ProcessOperator(osc);
                {
                    osc->out = osc->sine_preoout * osc->op_prop->tl * osc->lvl_env * osc->vel;
#ifdef PRESSURE_SENSOR_ENABLED
                    osc->out *= pressureValueFilt;
#endif
                }

                FmSynth_EnvStateProcess(osc);
            }


            FmSynth_AlgMixProcess(&sample, voice);
        }

        out[n] = sample / 8;
    }

    /*
     * let the max gain fade away
     */
    for (int j = 0; j < FM_VOICE_CNT; j++)
    {
        fmVoice[j].outSlow *= 0.99f;
    }

    if (initChannelSetting)
    {
        initChannelSettingCnt++;
        if (initChannelSettingCnt > 3 * ((float)sample_rate / (float)bufLen))
        {
            initChannelSetting = false;
            initChannelSettingCnt = 0;
            FmSynth_InitChannelSettings(currentChSetting);
        }
    }
    else
    {
        initChannelSettingCnt = 0;
    }
}

struct synthVoice_s *FmSynth_GetQuietestVoice(void)
{
    /*
     * if all voices at full velocity then going round
     */
    static uint8_t roundCnt = 0;
    roundCnt++;
    if (roundCnt >= FM_VOICE_CNT)
    {
        roundCnt = 0;
    }

    struct synthVoice_s *foundVoice = &fmVoice[roundCnt];

    float quietestVoice = 10.0f;

    for (int i = 0; i < FM_VOICE_CNT; i++)
    {
        struct synthVoice_s *voice = &fmVoice[i];

        if (voice->outSlow < quietestVoice)
        {
            quietestVoice = voice->outSlow;
            foundVoice = voice;
        }
    }

    return foundVoice;
}

void FmSynth_NoteOn(uint8_t ch, uint8_t note, float vel)
{
    if (ch < MIDI_CH_CNT)
    {
        currentChSetting = &channelSettings[ch];
    }

    bool tonesFound = false;

    struct synthVoice_s *newVoice = FmSynth_GetQuietestVoice();

    synthTone_s *tones[4];

    tonesFound = true;
    tones[0] = &newVoice->op[OP4];
    tones[1] = &newVoice->op[OP3];
    tones[2] = &newVoice->op[OP2];
    tones[3] = &newVoice->op[OP1];

    newVoice->midiCh = ch;
    newVoice->midiNote = note;
    newVoice->settings = currentChSetting;

    newVoice->outSlow = 5.0f; /* ensure it will not be killed by next MIDI note on */

    if (tonesFound)
    {
        if (currentChSetting->mono)
        {
            currentChSetting->notes[currentChSetting->noteStackCnt++] = note;
        }
        float tonePitch = ((pow(2.0f, (float)(note - 69) / 12.0f) * 440.0f));
        struct synthTone_s *tone;

        tone = tones[OP4];
        FmSynth_ToneInit(tone, &currentChSetting->op_prop[OP4]);
        tone->pitch = tonePitch;
        tone->vel *= (1.0f - tone->op_prop->vel_to_tl) + tone->op_prop->vel_to_tl * vel;

        tone = tones[OP3];
        FmSynth_ToneInit(tone, &currentChSetting->op_prop[OP3]);
        tone->pitch = tonePitch;

        tone->vel *= (1.0f - tone->op_prop->vel_to_tl) + tone->op_prop->vel_to_tl * vel;

        tone = tones[OP2];
        FmSynth_ToneInit(tone, &currentChSetting->op_prop[OP2]);
        tone->pitch = tonePitch;
        tone->vel *= (1.0f - tone->op_prop->vel_to_tl) + tone->op_prop->vel_to_tl * vel;

        tone = tones[OP1];
        FmSynth_ToneInit(tone, &currentChSetting->op_prop[OP1]);
        tone->pitch = tonePitch;
        tone->vel *= (1.0f - tone->op_prop->vel_to_tl) + tone->op_prop->vel_to_tl * vel;
    }
}

void FmSynth_NoteOff(uint8_t ch, uint8_t note)
{
    /*
     * find matching note and put into release
     */
    for (int i = 0; i < FM_VOICE_CNT ; i++)
    {
        if ((fmVoice[i].midiNote == note) && (fmVoice[i].midiCh == ch))
        {
            for (int j = 0; j < 4; j++)
            {
                fmVoice[i].op[j].state = ENV_RELEASE;
                FmSynth_ToneEnvUpdate(&fmVoice[i].op[j]);
            }
        }
    }
}

void FmSynth_ChannelSettingDump(uint8_t ch __attribute__((unused)), float value)
{
    if (value > 0)
    {
        printf("setting->algo = %d;\n", currentChSetting->algo);
        printf("setting->fmFeedback =  %0.6f;\n", currentChSetting->fmFeedback);
        for (int i = 0; i < 4; i++)
        {
            printf("setting->op_prop[%d].ar = %" PRIu32 ";\n", i, currentChSetting->op_prop[i].ar);
            printf("setting->op_prop[%d].d1r = %" PRIu32 ";\n", i, currentChSetting->op_prop[i].d1r);
            printf("setting->op_prop[%d].d2l = %0.6f;\n", i, currentChSetting->op_prop[i].d2l);
            printf("setting->op_prop[%d].d2r = %" PRIu32 ";\n", i, currentChSetting->op_prop[i].d2r);
            printf("setting->op_prop[%d].rr = %" PRIu32 ";\n", i, currentChSetting->op_prop[i].rr);
            printf("setting->op_prop[%d].rs = %" PRIu32 ";\n", i, currentChSetting->op_prop[i].rs);
            printf("setting->op_prop[%d].tl = %0.6f;\n", i, currentChSetting->op_prop[i].tl);
            printf("setting->op_prop[%d].mul = %0.6f;\n", i, currentChSetting->op_prop[i].mul);
            printf("setting->op_prop[%d].vel_to_tl = %0.6f;\n", i, currentChSetting->op_prop[i].vel_to_tl);
            printf("setting->op_prop[%d].am = %0.6f;\n", i, currentChSetting->op_prop[i].am);
            printf("setting->op_prop[%d].mw = %0.6f;\n", i, currentChSetting->op_prop[i].mw);
            printf("setting->op_prop[%d].vel = %0.6f;\n", i, currentChSetting->op_prop[i].vel);
        }
    }
}

void FmSynth_ChannelSettingInit(uint8_t ch __attribute__((unused)), float value)
{
    if (value > 0)
    {
        initChannelSetting = true;
    }
    else
    {
        initChannelSetting = false;
    }
}

void FmSynth_PitchBend(uint8_t ch __attribute__((unused)), float bend)
{
    pitchBendValue = bend;
}

void FmSynth_ModulationWheel(uint8_t ch __attribute__((unused)), float value)
{
    modulationDepth = value;
}

#ifdef PRESSURE_SENSOR_ENABLED
void FmSynth_Pressure(uint8_t unused __attribute__((unused)), float value)
{
    pressureValue = value;
}
#endif

void FmSynth_ToggleMono(uint8_t param __attribute__((unused)), float value)
{
    if (value > 0)
    {
        if (currentChSetting->mono)
        {
            currentChSetting->mono = false;
        }
        else
        {
            currentChSetting->mono = true;
        }
    }
}

void FmSynth_ToggleLegato(uint8_t param __attribute__((unused)), float value)
{
    if (value > 0)
    {
        if (currentChSetting->legato)
        {
            currentChSetting->legato = false;
        }
        else
        {
            currentChSetting->legato = true;
        }
    }
}

void FmSynth_SelectOp(uint8_t param, float value)
{
    if (value > 0)
    {
        if (param < 4)
        {
            selectedOp = param;
        }
        Status_ValueChangedInt("selectedOp", 4 - selectedOp);
    }
}

void FmSynth_SetAlgorithm(uint8_t param, float value)
{
    if (value > 0)
    {
        currentChSetting->algo = param;
        Status_ValueChangedInt("Algorithm", currentChSetting->algo);
    }
}

void FmSynth_ChangeParam(uint8_t param, float value)
{
    switch (param)
    {
    case 0:
        currentChSetting->op_prop[selectedOp].tl = value;
        Status_ValueChangedFloatArr("op_tl", currentChSetting->op_prop[selectedOp].tl, 4 - selectedOp);
        break;
    case 1:
        {
            uint32_t u32 = value * 31;
            float mul_c = 1;
            if (u32 > 0)
            {
                mul_c = u32;
            }
            else
            {
                mul_c = 0.5;
            }

            currentChSetting->op_prop[selectedOp].mul_coarse = mul_c;
            currentChSetting->op_prop[selectedOp].mul = currentChSetting->op_prop[selectedOp].mul_coarse + currentChSetting->op_prop[selectedOp].mul_fine;
            Status_ValueChangedFloatArr("op_mul", currentChSetting->op_prop[selectedOp].mul, 4 - selectedOp);
        }
        break;
    case 2:
        {
            uint32_t u32 = value * 100;

            currentChSetting->op_prop[selectedOp].mul_fine = (float)u32 * 0.01f;
            currentChSetting->op_prop[selectedOp].mul = currentChSetting->op_prop[selectedOp].mul_coarse + currentChSetting->op_prop[selectedOp].mul_fine;
            Status_ValueChangedFloatArr("op_mul", currentChSetting->op_prop[selectedOp].mul, 4 - selectedOp);
        }
        break;

    }
}

void FmSynth_VelToLev(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].vel_to_tl = value;
    Status_ValueChangedFloatArr("vel_to_tl", currentChSetting->op_prop[selectedOp].vel_to_tl, 4 - selectedOp);
}

void FmSynth_LfoAM(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].am = value;
    Status_ValueChangedFloatArr("op_lfo_am", currentChSetting->op_prop[selectedOp].am, 4 - selectedOp);
}

void FmSynth_LfoFM(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].mw = value;
    Status_ValueChangedFloatArr("op_lfo_mw", currentChSetting->op_prop[selectedOp].mw, 4 - selectedOp);
}

void FmSynth_Attack(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].ar = pow(2, value * 15);
    Status_ValueChangedFloatArr("op_attackRate", currentChSetting->op_prop[selectedOp].ar, 4 - selectedOp);
}

void FmSynth_Decay1(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].d1r = pow(2, value * 15);
    Status_ValueChangedFloatArr("op_decay1rate", currentChSetting->op_prop[selectedOp].d1r, 4 - selectedOp);
}

void FmSynth_DecayL(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].d2l = value;
    Status_ValueChangedFloatArr("op_decay2level", currentChSetting->op_prop[selectedOp].d2l, 4 - selectedOp);
}

void FmSynth_Decay2(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].d2r = pow(2, value * 15);
    Status_ValueChangedFloatArr("op_decay2rate", currentChSetting->op_prop[selectedOp].d2r, 4 - selectedOp);
}

void FmSynth_Release(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->op_prop[selectedOp].rr = pow(2, value * 12);
    Status_ValueChangedFloatArr("op_releaseRate", currentChSetting->op_prop[selectedOp].rr, 4 - selectedOp);
}

void FmSynth_Feedback(uint8_t unused __attribute__((unused)), float value)
{
    currentChSetting->fmFeedback = value;
    Status_ValueChangedFloat("feedback", currentChSetting->fmFeedback);
}
