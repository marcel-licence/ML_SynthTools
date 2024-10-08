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
 * @file midi_via_usb.ino
 * @author Marcel Licence
 * @date 24.11.2022
 *
 * @brief This module is used to add USB support (tested with RP2040)
 *
 * @see https://youtu.be/awurJEY8X10
 * @see https://github.com/lathoub/Arduino-BLE-MIDI
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DECLARATION


#ifdef MIDI_USB_ENABLED
void Midi_Usb_Setup();
void Midi_Usb_Loop();


#ifdef TEENSYDUINO // CORE_TEENSY

void UsbMidiSetup();
void UsbMidiLoop();

#endif
#endif


#endif /* ML_SYNTH_INLINE_DECLARATION */


#ifdef ML_SYNTH_INLINE_DEFINITION

#ifdef MIDI_USB_ENABLED

#include <Arduino.h>
#ifndef TEENSYDUINO
#include <Adafruit_TinyUSB.h> /* Using library Adafruit TinyUSB Library at version 1.14.4 from https://github.com/adafruit/Adafruit_TinyUSB_Arduino */
#endif
#include <MIDI.h> /* Using library MIDI Library at version 5.0.2 from https://github.com/FortySevenEffects/arduino_midi_library */

Adafruit_USBD_MIDI usb_midi; /* create instance */

/* Create a new instance of the Arduino MIDI Library, */
/* and attach usb_midi as the transport. */
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);


void Midi_Usb_Setup()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    /* Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040 */
    TinyUSB_Device_Init(0);
#endif

    usb_midi.setStringDescriptor(shortName);

    MIDI.begin(MIDI_CHANNEL_OMNI);

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
    MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity __attribute__((unused)))
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

    Serial.printf("Wait for USB device to be mounted...\n");
    while (!TinyUSBDevice.mounted())
    {
        delay(1);
    }
    Serial.printf("mounted!\n");
}

void Midi_Usb_Loop()
{
    MIDI.read();

#ifdef MIDI_USB_SEND_TEST_ENABLED
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

void Midi_Usb_rawMsg(uint8_t *msg __attribute__((unused)))
{

}

void Midi_Usb_NoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
    MIDI.sendNoteOn(note, vel, ch + 1);
    Serial.print("NoteOn(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(note);
    Serial.print(", ");
    Serial.println(vel);
}

void Midi_Usb_NoteOff(uint8_t ch, uint8_t note)
{
    MIDI.sendNoteOff(note, 0, ch + 1);
    Serial.print("NoteOff(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.println(note);
}

void Midi_Usb_ControlChange(uint8_t ch, uint8_t number, uint8_t value)
{
    MIDI.sendControlChange(number, value, ch + 1);
    Serial.print("ControlChange(tx): CH: ");
    Serial.print(ch);
    Serial.print(" | ");
    Serial.print(number);
    Serial.print(", ");
    Serial.println(value);
}

void Midi_Usb_PitchBend(uint8_t ch, uint16_t bend)
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

void Midi_Usb_RttMsg(uint8_t msg __attribute__((unused)))
{
#ifdef EXAMPLE_CODE
    MIDI.sendRealTime((kMIDIType)msg);
#endif
}

void Midi_Usb_SongPos(uint16_t pos)
{
    MIDI.sendSongPosition(pos);
}

#ifdef TEENSYDUINO // CORE_TEENSY


void processMIDI(void);


void myNoteOn(byte channel, byte note, byte velocity)
{
    Serial.print("NoteON(usb): CH: ");
    Serial.print(channel);
    Serial.print(" | ");
    Serial.print(note);
    Serial.print(", ");
    Serial.println(velocity);

    Midi_NoteOn(channel, note, velocity);
}

void myNoteOff(byte channel, byte note, byte velocity)
{
    Serial.print("NoteOFF(usb): CH: ");
    Serial.print(channel);
    Serial.print(" | ");
    Serial.println(note);
    Serial.print(", ");
    Serial.println(velocity);

    Midi_NoteOff(channel, note);
}

void UsbMidiSetup(void)
{
    Serial.println("UsbMidiSetup");
    usbMIDI.setHandleNoteOff(myNoteOff);
    usbMIDI.setHandleNoteOn(myNoteOn);
    Serial.println("UsbMidiSetup done");
#ifdef EXAMPLE_CODE
    usbMIDI.setHandleAfterTouchPoly(myAfterTouchPoly)
    usbMIDI.setHandleControlChange(myControlChange)
    usbMIDI.setHandleProgramChange(myProgramChange)
    usbMIDI.setHandleAfterTouch(myAfterTouch)
    usbMIDI.setHandlePitchChange(myPitchChange)
    usbMIDI.setHandleSystemExclusive(mySystemExclusiveChunk);
    usbMIDI.setHandleTimeCodeQuarterFrame(myTimeCodeQuarterFrame);
    usbMIDI.setHandleSongPosition(mySongPosition);
    usbMIDI.setHandleSongSelect(mySongSelect);
    usbMIDI.setHandleTuneRequest(myTuneRequest);
    usbMIDI.setHandleClock(myClock);
    usbMIDI.setHandleStart(myStart);
    usbMIDI.setHandleContinue(myContinue);
    usbMIDI.setHandleStop(myStop);
    usbMIDI.setHandleActiveSensing(myActiveSensing);
    usbMIDI.setHandleSystemReset(mySystemReset);
    usbMIDI.setHandleRealTimeSystem(myRealTimeSystem);
#endif
}


void UsbMidiTick(void)
{
    static bool notePlaying = false;

    if (notePlaying)
    {
        usbMIDI.sendNoteOff(60, 0, 0); // 60 = C4
    }
    else
    {
        usbMIDI.sendNoteOn(60, 64, 0); // 60 = C4
    }

    notePlaying = !notePlaying;
}

void UsbMidiLoop(void)
{
    if (usbMIDI.read())
    {

        processMIDI();
    }
}

void processMIDI(void)
{
    byte type, channel, data1, data2, cable;

    /* fetch the MIDI message, defined by these 5 numbers (except SysEX) */
    type = usbMIDI.getType(); // which MIDI message, 128-255
    channel = usbMIDI.getChannel(); // which MIDI channel, 1-16
    data1 = usbMIDI.getData1(); // first data byte of message, 0-127
    data2 = usbMIDI.getData2(); // second data byte of message, 0-127
    cable = usbMIDI.getCable(); // which virtual cable with MIDIx8, 0-7

    /* uncomment if using multiple virtual cables */
    Serial.print("cable ");
    Serial.print(cable, DEC);
    Serial.print(": ");

    /* print info about the message */
    switch (type)
    {
    case usbMIDI.NoteOff: // 0x80
        Serial.print("Note Off, ch=");
        Serial.print(channel, DEC);
        Serial.print(", note=");
        Serial.print(data1, DEC);
        Serial.print(", velocity=");
        Serial.println(data2, DEC);
        break;

    case usbMIDI.NoteOn: // 0x90
        Serial.print("Note On, ch=");
        Serial.print(channel, DEC);
        Serial.print(", note=");
        Serial.print(data1, DEC);
        Serial.print(", velocity=");
        Serial.println(data2, DEC);
        break;

    case usbMIDI.AfterTouchPoly: // 0xA0
        Serial.print("AfterTouch Change, ch=");
        Serial.print(channel, DEC);
        Serial.print(", note=");
        Serial.print(data1, DEC);
        Serial.print(", velocity=");
        Serial.println(data2, DEC);
        break;

    case usbMIDI.ControlChange: // 0xB0
        Serial.print("Control Change, ch=");
        Serial.print(channel, DEC);
        Serial.print(", control=");
        Serial.print(data1, DEC);
        Serial.print(", value=");
        Serial.println(data2, DEC);
        break;

    case usbMIDI.ProgramChange: // 0xC0
        Serial.print("Program Change, ch=");
        Serial.print(channel, DEC);
        Serial.print(", program=");
        Serial.println(data1, DEC);
        break;

    case usbMIDI.AfterTouchChannel: // 0xD0
        Serial.print("After Touch, ch=");
        Serial.print(channel, DEC);
        Serial.print(", pressure=");
        Serial.println(data1, DEC);
        break;

    case usbMIDI.PitchBend: // 0xE0
        Serial.print("Pitch Change, ch=");
        Serial.print(channel, DEC);
        Serial.print(", pitch=");
        Serial.println(data1 + data2 * 128, DEC);
        break;

    case usbMIDI.SystemExclusive: // 0xF0
        /*
         * Messages larger than usbMIDI's internal buffer are truncated.
         * To receive large messages, you *must* use the 3-input function
         * handler.  See InputFunctionsComplete for details.
         */
        Serial.print("SysEx Message: ");
        printBytes(usbMIDI.getSysExArray(), data1 + data2 * 256);
        Serial.println();
        break;

    case usbMIDI.TimeCodeQuarterFrame: // 0xF1
        Serial.print("TimeCode, index=");
        Serial.print(data1 >> 4, DEC);
        Serial.print(", digit=");
        Serial.println(data1 & 15, DEC);
        break;

    case usbMIDI.SongPosition: // 0xF2
        Serial.print("Song Position, beat=");
        Serial.println(data1 + data2 * 128);
        break;

    case usbMIDI.SongSelect: // 0xF3
        Serial.print("Sond Select, song=");
        Serial.println(data1, DEC);
        break;

    case usbMIDI.TuneRequest: // 0xF6
        Serial.println("Tune Request");
        break;

    case usbMIDI.Clock: // 0xF8
        Serial.println("Clock");
        break;

    case usbMIDI.Start: // 0xFA
        Serial.println("Start");
        break;

    case usbMIDI.Continue: // 0xFB
        Serial.println("Continue");
        break;

    case usbMIDI.Stop: // 0xFC
        Serial.println("Stop");
        break;

    case usbMIDI.ActiveSensing: // 0xFE
        Serial.println("Actvice Sensing");
        break;

    case usbMIDI.SystemReset: // 0xFF
        Serial.println("System Reset");
        break;

    default:
        Serial.println("Opps, an unknown MIDI message type!");
    }
}


void printBytes(const byte *data, unsigned int size)
{
    while (size > 0)
    {
        byte b = *data++;
        if (b < 16)
        {
            Serial.print('0');
        }
        Serial.print(b, HEX);
        if (size > 1)
        {
            Serial.print(' ');
        }
        size = size - 1;
    }
}

#endif // #ifdef TEENSYDUINO // CORE_TEENSY


#endif /* #ifdef MIDI_USB_ENABLED */

#endif /* ML_SYNTH_INLINE_DEFINITION */

