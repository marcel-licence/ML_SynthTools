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
 * @file ml_midi_monitor.ino
 * @author Marcel Licence
 * @date 14.12.2024
 *
 * @brief   This is simple testproject to test out the MIDI connection
 *          Received short MIDI messages will be displayed and decoded
 */

#define MIDI_MONITOR_ENABLED
//#define MIDI_DUMP_SERIAL2_TO_SERIAL /*!< use this to see raw data, incoming bytes */


/*
 * The following configuration is for the RP2040, RP2350
 */
#ifdef ARDUINO_ARCH_RP2040

#define MIDI_TX2_PIN  4
#define MIDI_RX2_PIN  5

#define MIDI_TX1_PIN  0
#define MIDI_RX1_PIN  1

#ifdef LED_BUILTIN
#define BLINK_LED_PIN LED_BUILTIN
#endif

#endif

/*
 * the following configuration is for ESP32, ESP32S2, ...
 */
#ifdef ESP32
#if (!defined CONFIG_IDF_TARGET_ESP32S2) && (!defined CONFIG_IDF_TARGET_ESP32C3)
#define MIDI_TX2_PIN  17
#define MIDI_RX2_PIN  16
#endif
#endif

/*
 * the following configuration is for Teensy
 */
#ifdef TEENSYDUINO // CORE_TEENSY

#define BLINK_LED_PIN 13 /* led pin on teensy 4.1 */

#define MIDI_TX1_PIN  1
#define MIDI_RX1_PIN  0
#endif

/*
 * Configuration for
 * Board: "LOLIN(WEMOS) D1 R2 & mini 2 or similar
 */
#ifdef ESP8266

#define SWAP_SERIAL
#define LED_PIN     LED_BUILTIN

#define MIDI_PORT_ACTIVE

#ifndef SWAP_SERIAL
#define RXD2 13 /* U2RRXD, D7 */
#define TXD2 15 /* U2RRXD, D0 */
#include <SoftwareSerial.h>
SoftwareSerial Serial2(RXD2, TXD2);
#define MIDI_PORT2_ACTIVE
#endif

#endif


/*
 * Configuration for
 * Board: "Generic STM32H7 Series"
 * Board part number: "Daisy Seed"
 */
#ifdef ARDUINO_DAISY_SEED

#define LED_PIN LED_BUILTIN
#define MIDI_PORT2_ACTIVE

#endif /* ARDUINO_DAISY_SEED */


/*
 * Configuration for
 * Board: "Seeeduino XIAO"
 */
#if (defined ARDUINO_SEEED_XIAO_M0) || (defined SEEED_XIAO_M0)

#define BLINK_LED_PIN LED_BUILTIN
#define MIDI_PORT1_ACTIVE

#endif /* (defined ARDUINO_SEEED_XIAO_M0) || (defined SEEED_XIAO_M0) */


#define ML_SYNTH_INLINE_DECLARATION
#define ML_SYNTH_INLINE_DEFINITION
#include <blink.h>
#include <midi_interface.h>

#ifdef USE_TINYUSB
#include <Adafruit_TinyUSB.h>
#endif

struct midiMapping_s midiMapping =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
};

void setup(void)
{
#ifdef BLINK_LED_PIN
    Blink_Setup();
#endif

    Serial.begin(115200);
    //delay(1500);
    while (!Serial)
    {
        delay(250);
#ifdef BLINK_LED_PIN
        Blink_Process();
#endif
    }

    Serial.println("ml_midi_monitor");

    Midi_Setup();
}

void loop_1hz(void)
{
    uint8_t msg[] = {0x90, 0x01, 0x02};
#if (defined MIDI_TX1_PIN) || (defined MIDI_TX2_PIN)
    Midi_SendRaw(msg);
#else
    Serial1.write(msg, sizeof(msg));
#endif
#if 0 /* show sent data */
    Serial.printf("Tx: 90 01 02\n");
#endif
#ifdef BLINK_LED_PIN
    Blink_Process();
#endif
}

void loop(void)
{
    uint32_t newCnt = millis();

    static uint32_t lastCnt = 0;
    static uint32_t cnt = 0;
    cnt += newCnt - lastCnt;
    lastCnt = newCnt;
    if (cnt >= 1000)
    {
        cnt -= 1000;
        loop_1hz();
    }

    Midi_Process();
    delayMicroseconds(100);
}
