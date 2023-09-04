/*
 * Copyright (c) 2023 Marcel Licence
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
 * @file midi_via_ble.ino
 * @author Marcel Licence
 * @date 24.02.2022
 *
 * @brief This module is used to add BLE support
 *
 * @see https://youtu.be/awurJEY8X10
 * @see https://github.com/lathoub/Arduino-BLE-MIDI
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DECLARATION

#endif /* ML_SYNTH_INLINE_DECLARATION */


#ifdef ML_SYNTH_INLINE_DEFINITION

#ifdef MIDI_BLE_ENABLED

#include <BLEMIDI_Transport.h> /* Using library Arduino-BLE-MIDI at version 2.2 from https://github.com/lathoub/Arduino-BLE-MIDI */

#ifdef MIDI_BLE_CLIENT

#include <hardware/BLEMIDI_Client_ESP32.h>
//#include <hardware/BLEMIDI_ESP32_NimBLE.h>
//#include <hardware/BLEMIDI_ESP32.h>
//#include <hardware/BLEMIDI_nRF52.h>
//#include <hardware/BLEMIDI_ArduinoBLE.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE(); /* Connect to first server found */
#ifdef OTHER_POSSIBLE_DEFINES
BLEMIDI_CREATE_INSTANCE("", MIDI) //Connect to the first server found
BLEMIDI_CREATE_INSTANCE("01:23:45:ab:cd:ef", MIDI) //Connect to a specific BLE address server
BLEMIDI_CREATE_INSTANCE("MyBLEserver", MIDI) //Connect to a specific name server
#endif
#else

#include <hardware/BLEMIDI_ESP32.h>
//#include <hardware/BLEMIDI_ESP32_NimBLE.h>
//#include <hardware/BLEMIDI_nRF52.h>
//#include <hardware/BLEMIDI_ArduinoBLE.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE() /* create instance as server */

#endif


/*
 * The test-mode does send a NoteOn and NoteOff event
 * for each key of a 64 keys keyboard
 *
 * Starting with the lowest c going up to the highest c
 * It will repeat over and over again
 *
 * Between NoteOn and NoteOff there is a delay of 250ms
 */
//#define MIDI_BLE_SEND_TEST_ENABLED


#ifdef MIDI_BLE_SEND_TEST_ENABLED
bool isConnected = false;
#endif

extern struct midiMapping_s midiMapping; /* definition in z_config.ino */


/* -----------------------------------------------------------------------------
 * When BLE connected, LED will turn on (indication that connection was successful)
 * When receiving a NoteOn, LED will go out, on NoteOff, light comes back on.
 * This is an easy and conveniant way to show that the connection is alive and working.
 * ----------------------------------------------------------------------------- */
void midi_ble_setup()
{
    MIDI.begin(MIDI_CHANNEL_OMNI);

#ifdef LED_BLE_STATUS_PIN
    pinMode(LED_BLE_STATUS_PIN, OUTPUT);
    digitalWrite(LED_BLE_STATUS_PIN, LOW);
#endif

    BLEMIDI.setHandleConnected([]()
    {
        Serial.println("BLE MIDI connected");
#ifdef MIDI_BLE_SEND_TEST_ENABLED
        isConnected = true;
#endif
#ifdef LED_BLE_STATUS_PIN
        digitalWrite(LED_BLE_STATUS_PIN, HIGH);
#endif
    });

    BLEMIDI.setHandleDisconnected([]()
    {
        Serial.println("BLE MIDI disconnected");
#ifdef MIDI_BLE_SEND_TEST_ENABLED
        isConnected = false;
#endif
#ifdef LED_BLE_STATUS_PIN
        digitalWrite(LED_BLE_STATUS_PIN, LOW);
#endif
    });

    MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity)
    {
        channel -= 1;

        if (midiMapping.noteOn != NULL)
        {
#ifdef MIDI_FMT_INT
            midiMapping.noteOn(channel, note, velocity);
#else
            midiMapping.noteOn(channel, note, pow(2, ((velocity * NORM127MUL) - 1.0f) * 6));
#endif
        }

#ifdef LED_BLE_STATUS_PIN
        digitalWrite(LED_BLE_STATUS_PIN, LOW);
#endif
#ifdef MIDI_BLE_DEBUG_ENABLED
        Serial.print("NoteOn(rx): CH: ");
        Serial.print(channel);
        Serial.print(" | ");
        Serial.print(note);
        Serial.print(", ");
        Serial.println(velocity);
#endif
    });
    MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity)
    {
        channel -= 1;

        if (midiMapping.noteOff != NULL)
        {
            midiMapping.noteOff(channel, note);
        }

#ifdef LED_BLE_STATUS_PIN
        digitalWrite(LED_BLE_STATUS_PIN, HIGH);
#endif
#ifdef MIDI_BLE_DEBUG_ENABLED
        Serial.print("NoteOff(rx): CH: ");
        Serial.print(channel);
        Serial.print(" | ");
        Serial.println(note);
#endif
    });
    MIDI.setHandleControlChange([](byte channel, byte number, byte value)
    {
        channel -= 1;

        Midi_CC_Map(channel, number, value, midiMapping.controlMapping, midiMapping.mapSize);
#ifdef MIDI_MAP_FLEX_ENABLED
        Midi_CC_Map(channel, number, value, midiMapping.controlMapping_flex, midiMapping.mapSize_flex);
#endif

        if (number == 1)
        {
            if (midiMapping.modWheel != NULL)
            {
#ifdef MIDI_FMT_INT
                midiMapping.modWheel(channel, value);
#else
                midiMapping.modWheel(channel, (float)value * NORM127MUL);
#endif
            }
        }

#ifdef MIDI_BLE_DEBUG_ENABLED
        Serial.print("ControlChange(rx): CH: ");
        Serial.print(channel);
        Serial.print(" | number: ");
        Serial.print(number);
        Serial.print(", value: ");
        Serial.println(value);
#endif
    });
    MIDI.setHandlePitchBend([](unsigned char channel, int bend)
    {
        channel -= 1;

        union
        {
            uint16_t bendU;
            int16_t bendI;
        } bender;
        bender.bendI = bend;
        bender.bendI += 8192;

        float value = ((float)bender.bendU - 8192.0f) * (1.0f / 8192.0f);
        if (midiMapping.pitchBend != NULL)
        {
            midiMapping.pitchBend(channel, value);
        }
        Serial.print("PitchBend(rx): CH: ");
        Serial.print(channel);
        Serial.print(", bend: ");
        Serial.println(bend);
    });

    Serial.printf("BLE MIDI setup done\n");
}

void midi_ble_loop()
{
    MIDI.read();

#ifdef MIDI_BLE_SEND_TEST_ENABLED
    static unsigned long t0 = millis();
    static uint8_t i = 36; /* low c on 64 key keyboard */
    static bool noteOn = false;

    if (isConnected && (millis() - t0) > 250) /* next step after 250 ms */
    {
        t0 = millis();

        if (noteOn)
        {
            MIDI.sendNoteOff(i++, 0, 1); // note <i>, velocity 0 on channel 1
            Serial.print("NoteOff(tx): CH: ");
            Serial.print(1);
            Serial.print(" | ");
            Serial.print(i);
            Serial.print(", ");
            Serial.println(0);
        }
        else
        {
            MIDI.sendNoteOn(i, 100, 1); // note <i>, velocity 100 on channel 1
            Serial.print("NoteOn(tx): CH: ");
            Serial.print(1);
            Serial.print(" | ");
            Serial.print(i);
            Serial.print(", ");
            Serial.println(100);
        }
        noteOn = !noteOn;

        if (i > 96) /* high c on 64 key keyboard */
        {
            i = 63; /* low c on 64 key keyboard */
        }
    }
#endif
}

/*
 *
 */

void Ble_rawMsg(uint8_t *msg)
{

}

void Ble_NoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
    MIDI.sendNoteOn(note, vel, ch + 1);
    Serial.print("NoteOn(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(note);
    Serial.print(", ");
    Serial.println(vel);
}

void Ble_NoteOff(uint8_t ch, uint8_t note)
{
    MIDI.sendNoteOff(note, 0, ch + 1);
    Serial.print("NoteOff(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.println(note);
}

void Ble_ControlChange(uint8_t ch, uint8_t number, uint8_t value)
{
    MIDI.sendControlChange(number, value, ch + 1);
    Serial.print("ControlChange(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(number);
    Serial.print(", ");
    Serial.println(value);
}

void Ble_PitchBend(uint8_t ch, uint16_t bend)
{
    union
    {
        uint16_t bendU;
        int16_t bendI;
    } bender;
    bender.bendU = bend;
    bender.bendI -= 8192;
    MIDI.sendPitchBend(bender.bendI, ch + 1);
    Serial.print("PitchBend(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.println(bender.bendI);
}

void Ble_RttMsg(uint8_t msg)
{
#ifdef EXAMPLE_CODE
    MIDI.sendRealTime((kMIDIType)msg);
#endif
}

void Ble_SongPos(uint16_t pos)
{
    MIDI.sendSongPosition(pos);
}

#endif /* #ifdef MIDI_BLE_ENABLED */


#endif /* ML_SYNTH_INLINE_DEFINITION */
