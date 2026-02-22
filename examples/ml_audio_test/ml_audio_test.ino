/*
 * Copyright (c) 2026 Marcel Licence
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
 * @file ml_audio_test.ino
 * @author Marcel Licence
 * @date 14.01.2025
 *
 * @brief   This is simple testproject to check the audio output using a DAC
 *          connected via I2S
 */
#define OUTPUT_SAW_TEST
//#define OUTPUT_SINE_TEST


#ifdef LED_BUILTIN
#define BLINK_LED_PIN LED_BUILTIN
#endif


/*
 * The following configuration is for the RP2040, RP2350
 */
#ifdef ARDUINO_ARCH_RP2040

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48

#endif

/*
 * the following configuration is for ESP32, ESP32S2, ..
 */
#if (defined ESP32) && (!defined ARDUINO_LOLIN_S2_MINI) && (!defined ARDUINO_ESP32S2_DEV)

//#define BOARD_ML_V1 /* activate this when using the ML PCB V1 */
#define BOARD_ML_SYNTH_V2 /* activate this when using the ML PCB V1 */
//#define BOARD_ESP32_AUDIO_KIT_AC101 /* activate this when using the ESP32 Audio Kit v2.2 with the AC101 codec */
//#define BOARD_ESP32_AUDIO_KIT_ES8388 /* activate this when using the ESP32 Audio Kit v2.2 with the ES8388 codec */
//#define BOARD_ESP32_DOIT /* activate this when using the DOIT ESP32 DEVKIT V1 board */
//#define BOARD_WEMOS_D1_MINI_ESP32

/*
 * include the board configuration
 * there you will find the most hardware depending pin settings
 */
#include <ml_boards.h> /* requires the ML_Synth library:  https://github.com/marcel-licence/ML_SynthTools */

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48

#endif /* (defined ESP32) && (!defined ARDUINO_LOLIN_S2_MINI) */


#if (defined ARDUINO_LOLIN_S2_MINI) || (defined ARDUINO_ESP32S2_DEV)

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48

#define I2S_BCLK_PIN -1
#define I2S_WCLK_PIN -1
#define I2S_DOUT_PIN 17
#define I2S_DIRECT_OUT
#define AUDIO_MONO_DOWNMIX
#define AUDIO_OUT_MONO

#endif


/*
 * the following configuration is for Teensy
 */
#ifdef TEENSYDUINO // CORE_TEENSY

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48

#endif

/*
 * Configuration for
 * Board: "LOLIN(WEMOS) D1 R2 & mini 2 or similar
 */
#ifdef ESP8266

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48
#define AUDIO_OUT_MONO

#endif


/*
 * Configuration for
 * Board: "Generic STM32H7 Series"
 * Board part number: "Daisy Seed"
 */
#ifdef ARDUINO_DAISY_SEED

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48

#endif /* ARDUINO_DAISY_SEED */


/*
 * Configuration for
 * Board: "Seeeduino XIAO"
 */
#if (defined ARDUINO_SEEED_XIAO_M0) || (defined SEEED_XIAO_M0)

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE_16BIT
#define SAMPLE_BUFFER_SIZE  48
#define AUDIO_OUT_MONO

#endif /* (defined ARDUINO_SEEED_XIAO_M0) || (defined SEEED_XIAO_M0) */


#define ML_SYNTH_INLINE_DECLARATION
#include <ml_inline.h>
#undef ML_SYNTH_INLINE_DECLARATION

#define ML_SYNTH_INLINE_DEFINITION
#include <ml_inline.h>
#undef ML_SYNTH_INLINE_DEFINITION

void setup()
{
    /*
     * this code runs once
     */
#ifdef BLINK_LED_PIN
    Blink_Setup();
    Blink_Fast(1);
#endif

    Serial.begin(115200);

    Serial.printf("Initialize Audio Interface\n");
    Audio_Setup();
}

void loop()
{
    static int loop_cnt_1hz = 0; /*!< counter to allow 1Hz loop cycle */

#ifdef SAMPLE_BUFFER_SIZE
    loop_cnt_1hz += SAMPLE_BUFFER_SIZE;
#else
    loop_cnt_1hz += 1; /* in case only one sample will be processed per loop cycle */
#endif
    if (loop_cnt_1hz >= SAMPLE_RATE)
    {
        loop_cnt_1hz -= SAMPLE_RATE;
#ifdef BLINK_LED_PIN
        Blink_Process();
#endif
    }

#ifndef AUDIO_OUT_MONO
    float left[SAMPLE_BUFFER_SIZE], right[SAMPLE_BUFFER_SIZE];
    /*
     * Output the audio
     */
    Audio_Output(left, right);
#else
    int32_t mono[SAMPLE_BUFFER_SIZE];
    /*
     * Output the audio
     */
    Audio_OutputMono(mono);
#endif
}
