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
 * @file ml_as5600_inline.h
 * @author Marcel Licence
 * @date 20.10.2024
 *
 * @brief  Implementation to use the AS5600 for scratching via I2C
 * @see https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf
 * @see https://www.youtube.com/watch?v=Ml6VrlV3hvk
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef AS5600_ENABLED


#ifdef ML_SYNTH_INLINE_DECLARATION


void AS5600_Setup(void);
void AS5600_Loop(void);


#endif /* ML_SYNTH_INLINE_DECLARATION */


#ifdef ML_SYNTH_INLINE_DEFINITION


#include <Wire.h>


//#define AS5600_DEBUG_ENABLED /* use this to print out some debug messages */

#define AS5600_ADDR 0x36

#define REG_CONF        0x07
#define REG_RAW_ANGLE   0x0C



static float pitchOffset = -100.0f;
static int sumCh = 0;


inline
void AS5600_WriteReg(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

inline
void AS5600_WriteRegU16(uint8_t reg, uint16_t value)
{
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(reg);
    Wire.write((value >> 8) & 0xFF);
    Wire.write(value & 0xFF);
    Wire.endTransmission();
}

inline
uint8_t AS5600_ReadReg(uint8_t reg)
{
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(AS5600_ADDR, 1);
    return Wire.read();
}

inline
uint16_t AS5600_ReadReg_u16(uint8_t reg)
{
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(AS5600_ADDR, 2);
    return (((uint16_t)Wire.read()) << 8) + (uint16_t)Wire.read();
}

void AS5600_Setup(void)
{
#ifdef AS5600_DEBUG_ENABLED
    Serial.printf("CONF: 0x%08x\n", AS5600_ReadReg_u16(REG_CONF));
#endif
    AS5600_WriteRegU16(REG_CONF, 0x08); // hyst to 2 LSB to avoid static noise
#ifdef AS5600_DEBUG_ENABLED
    Serial.printf("CONF: 0x%08x\n", AS5600_ReadReg_u16(REG_CONF));
#endif
}

void AS5600_Loop(void)
{
    static uint16_t lastVal = 0;
    uint16_t newVal = AS5600_ReadReg_u16(REG_RAW_ANGLE);
    bool valUpdate = false;

    if (newVal > lastVal)
    {
        if (newVal - lastVal > 0)
        {
            valUpdate = true;
        }
    }
    else
    {
        if (lastVal - newVal > 0)
        {
            valUpdate = true;
        }
    }
    if (valUpdate)
    {
        int ch = ((int)newVal) - ((int)lastVal);
        if (ch > 2048)
        {
            ch -= 4096;
        }
        if (ch < -2048)
        {
            ch += 4096;
        }
#ifdef AS5600_DEBUG_ENABLED
        Serial.printf("RAW_ANGLE: %d, %d\n", newVal, ch);
#endif
        lastVal = newVal;
        sumCh += ch;
    }
}

float AS5600_GetPitch(uint8_t oversample)
{
    static float lastPitch = 0;
    float pitch = sumCh;

    sumCh = 0;
    pitch *= 0.25f;
    pitch /= oversample;
    if (pitch > 50)
    {
        pitch = 50;
    }
    if (pitch < -50)
    {
        pitch = -50;
    }

    for (int i = 0; i < oversample; i++)
    {
        lastPitch = pitch * 0.05 + lastPitch * 0.95;
    }
    return max(lastPitch, pitchOffset);
}

void AS5600_SetPitchOffset(float offset)
{
    pitchOffset = offset;
}

#endif /* ML_SYNTH_INLINE_DEFINITION */

#endif  /* AS5600_ENABLED */
