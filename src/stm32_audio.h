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
 * @file stm32_audio.ino
 * @author Marcel Licence
 * @date 21.12.2021
 *
 * @brief  This file provides an implementation to get the sound running on the stm32 discovery board
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DECLARATION


void STM32_AudioInit();
void STM32_AudioWriteS16(const int32_t *samples);
void STM32_AudioWrite(const float *fl_sample, const float *fr_sample);


#endif  /* ML_SYNTH_INLINE_DECLARATION */


#ifdef ML_SYNTH_INLINE_DEFINITION


#if (defined ARDUINO_DISCO_F407VG) || (defined ARDUINO_BLACK_F407VE) || (defined ARDUINO_BLUEPILL_F103C8) || (defined ARDUINO_BLUE_F103VE)

#ifdef ARDUINO_DISCO_F407VG
#define SETUP_CS43L22_CODEC
#endif

#include <I2S.h> /* ensure that library from board lib is used and not and external one */

#ifdef SETUP_CS43L22_CODEC

#include <Wire.h>


#define CODEC_ADDR 0x4A


void codec_writeReg(unsigned char reg, unsigned char data)
{
    uint8_t error;
    Wire.beginTransmission(CODEC_ADDR);
    Wire.write(reg);
    Wire.write(data);
    error = Wire.endTransmission();
    if (error != 0)
    {
        Serial.printf("Wire transmission error: %d\n", error);
    }
}

uint8_t I2C_ReadReg(uint8_t dev, uint8_t reg)
{
    Wire.beginTransmission(dev);
    Wire.write(reg); // set memory pointer to reg address

    Wire.requestFrom(dev, 1U); // request one byte of data from codec
    uint8_t result = Wire.read(); // store the incoming byte into "inputs"
    Wire.endTransmission();
    return result;
}

void I2C_WriteReg(uint8_t dev, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(dev);
    Wire.write(reg); // IODIRA register
    Wire.write(value);
    Wire.endTransmission();
}

void register_write(uint8_t reg, uint8_t value)
{
    I2C_WriteReg(0x4A, reg, value);
}

// Reads internal codec register over I2C
void register_read(uint8_t reg, uint8_t &value)
{
    value = I2C_ReadReg(0x4A, reg);
}

#endif /* SETUP_CS43L22_CODEC */

/*
 * object to access I2S interface
 */
I2SClass I2S(I2S_I2SN, I2S_SDIN, I2S_LRCK, I2S_SCLK, I2S_MCLK);

void STM32_AudioInit()
{
#ifdef SETUP_CS43L22_CODEC
    pinMode(DAC_RESET, OUTPUT);
    digitalWrite(DAC_RESET, LOW); /* turn the codec off */
#endif

    I2S.begin(I2S_PHILIPS_MODE, SAMPLE_RATE, 16);

#ifdef SETUP_CS43L22_CODEC
    digitalWrite(DAC_RESET, HIGH); /* turn the codec on */


    Wire.begin();

    /* @see https://www.mouser.de/datasheet/2/76/CS43L22_F2-1142121.pdf */
    Serial.printf("Dev 0x%02x: 0x01 - 0x%02x\n", 0x1A, I2C_ReadReg(0x1A, 0x01)); /* not sure what this is - gyro? */
    Serial.printf("Dev 0x%02x: 0x01 - 0x%02x\n", 0x4A, I2C_ReadReg(0x4A, 0x01)); /* this should return Chip I.D. | Chip Revision e.g.: 0xe3 */

    /*
     * now it would be perfect to setup I2S and the line output of the DAC
     * it is just copy & paste of a working configuration
     * @see https://github.com/danieleff/STM32GENERIC/blob/master/STM32/libraries/BoardExamples/examples/Discovery407VG/STM32F4_Discovery_CS43L22_AudioCodecBasicTest/STM32F4_Discovery_CS43L22_AudioCodecBasicTest.ino
     */
    codec_writeReg(0x02, 0x01); // power save registers -> all on
    codec_writeReg(0x00, 0x99);
    codec_writeReg(0x47, 0x80); //inits
    codec_writeReg(0x0d, 0x03); // playback ctrl
    codec_writeReg(0x32, (1 << 7)); // vol
    codec_writeReg(0x32, (0 << 7)); // vol
    codec_writeReg(0x00, 0x00); // inits
    codec_writeReg(0x04, 0xaf); // power ctrl
    codec_writeReg(0x0d, 0x70);
    codec_writeReg(0x05, 0x81); // clocking: auto speed is determined by the MCLK/LRCK ratio.
    codec_writeReg(0x06, 0x07); // DAC interface format, I2S 16 bit
    codec_writeReg(0x0a, 0x00);
    codec_writeReg(0x27, 0x00);
    codec_writeReg(0x80, 0x0a); // both channels on
    codec_writeReg(0x1f, 0x0f);
    codec_writeReg(0x02, 0x9e);

    /*
     * dump all registers
     */
    for (uint8_t i = 0; i < 15; i++)
    {
        uint8_t val;
        register_read(i, val);
        Serial.printf("0x%02x: 0x%02x\n", i, val);
    }
#endif /* SETUP_CS43L22_CODEC */
}

void STM32_AudioWriteS16(const int32_t *samples)
{
    int16_t u16int[2 * SAMPLE_BUFFER_SIZE];

    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        u16int[2 * i] = samples[i];
        u16int[(2 * i) + 1] = samples[i];
    }
    for (int i = 0; i < SAMPLE_BUFFER_SIZE * 2; i++)
    {
        I2S.write(u16int[i]);
    }
}

void STM32_AudioWrite(const float *fl_sample, const float *fr_sample)
{
    for (size_t i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        static union sampleTUNT
        {
            uint32_t sample;
            int16_t ch[2];
        } sampleDataU;

        sampleDataU.ch[0] = int16_t(fr_sample[i] * 16383.0f); /* some bits missing here */
        sampleDataU.ch[1] = int16_t(fl_sample[i] * 16383.0f);

        I2S.write(sampleDataU.ch[0]);
        I2S.write(sampleDataU.ch[1]);
    }
}

#endif

#endif /* ML_SYNTH_INLINE_DEFINITION */
