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
 * @file i2s_module.ino
 * @author Marcel Licence
 * @date 21.11.2021
 *
 * @brief   Here you will find a simple implementation of the described mechnism of
 *          the "Audio Hacking on the ESP8266" by Jan Ostman published on March 22, 2019
 *
 * @see     https://www.hackster.io/janost/audio-hacking-on-the-esp8266-fa9464#toc-a-simple-909-drum-synth-0
 */

#ifdef ML_SYNTH_INLINE_DECLARATION

#ifdef I2S_NODAC
#ifdef ESP8266
void I2S_init(void);
void writeDAC(uint16_t DAC);
void I2S_Wait(void);
bool I2S_isNotFull(void);
#endif
#endif

#endif


#ifdef ML_SYNTH_INLINE_DEFINITION


#ifdef I2S_NODAC
#ifdef ESP8266


#ifdef ESP8266
//#include <driver/i2s.h>
#include <I2S.h>
#include <i2s_reg.h>
#endif

void I2S_init(void)
{
    i2s_begin(); //Start the i2s DMA engine
    i2s_set_rate(44100); //Set sample rate todo use audiorate
}


void writeDAC(uint16_t DAC)
{
    static uint32_t i2sACC = 0;
    static uint16_t err = 0;
    for (uint8_t i = 0; i < 32; i++)
    {
        i2sACC = i2sACC << 1;
        if (DAC >= err)
        {
            i2sACC |= 1;
            err += 0xFFFF - DAC;
        }
        else
        {
            err -= DAC;
        }
    }
    bool flag __attribute__((unused)) = i2s_write_sample(i2sACC);
}


void I2S_Wait(void)
{
    while (i2s_is_full())
    {
        ;
    }
}

bool I2S_isNotFull(void)
{
    return !(i2s_is_full());
}

#endif
#endif

#endif /* ML_SYNTH_INLINE_DEFINITION */
