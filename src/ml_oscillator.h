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
 * @file ml_oscillator.h
 * @author Marcel Licence
 * @date 26.11.2021
 *
 * @brief   This shows you all available functions to use the oscillator library
 *          The library is still in progress and will be updated in future
 *          Supported platforms: Teensy 4.1, ESP32 (DaisySeed gets stuck)
 *          The library might work on other platforms. You can test it
 */


#ifndef SRC_ML_OSCILLATOR_H_
#define SRC_ML_OSCILLATOR_H_


#include "Arduino.h"
#include <stdint.h>

/* controllable parameters, some of them might not work, but added in future */
#define SYNTH_PWM_OSC_PARAM_PULSE_WIDTH     17 /* offset of the pulse width */
#define SYNTH_PWM_OSC_PARAM_PULSE_LEVEL     18
#define SYNTH_PWM_OSC_PARAM_PULSE_MUL       19
#define SYNTH_PWM_OSC_PARAM_PULSE_MOD_DEPTH 20 /* intensity of the automated modulation */
#define SYNTH_PWM_OSC_PARAM_PULSE_MOD_SPEED 21 /* speed of the automated modulation */
#define SYNTH_PWM_OSC_PARAM_PULSE_OFFSET    22


class ML_Oscillator
{
public:
    ML_Oscillator();

    /*
     * required to setup all oscillators
     */
    static void Setup(Stream *ser, float sample_rate);

    void Start(uint32_t add);
    void Start(float *left, float *right, uint32_t add); /* Start oscillator with linked output */
    void Stop(void);
    void SetPitchMultiplier(float multiplier);

    void ProcessBuffer(float *left, float *right, int len);
    void ProcessBuffer(int len);

    bool isFree(void);
    bool isCon(void);
    bool isCon(float *sig);

    /*
     * set parameter for all oscillators
     */
    static void SetParam(uint8_t osc_param, float value);

private:

    uint32_t samplePos;
    uint32_t samplePosSub;
    uint32_t addVal;

    uint32_t lfo;

    float *dest[2];

    bool active;
};

#endif /* SRC_ML_OSCILLATOR_H_ */

