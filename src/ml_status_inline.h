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
 * @file    ml_status_inline.h
 * @author  Marcel Licence
 * @date    06.01.2022
 *
 * @brief   This file contains the prototypes of functions for status messages generation
 */


#ifdef ML_SYNTH_INLINE_DEFINITION


#ifdef STATUS_SIMPLE


static uint32_t status_cnt = 0;
static uint32_t status_elapsed_time = 0;

#ifndef F_CPU
#define F_CPU 240000000UL
#endif

#define STATUS_DISPLAY_TIME (5UL*F_CPU)


/**
 * @brief This function is a stuf for easy replacement for another status module
 */
void Status_Setup(void)
{
    /* nothing to do */
}

/**
 * @brief This function is a stuf for easy replacement for another status module
 */
void Status_Process(void)
{
    /* nothing to do */
}

/**
 * @brief This function is a stuf for easy replacement for another status module
 */
void Status_Process_Sample(uint32_t count __attribute__((unused)))
{
    /* nothing to do */
}

/**
 * @brief
 *
 * @param descr
 * @param value
 */
void Status_ValueChangedFloat(const char *descr, float value)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s: %0.3f\n", descr, value);
}

void Status_ValueChangedFloat(const char *group, const char *descr, float value)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s - %s: %0.3f\n", group, descr, value);
}

/*
 * update top line message including a float value
 */
void Status_ValueChangedFloatArr(const char *descr, float value, int index)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s[%d]: %0.3f", descr, index, value);
}

/**
 * @brief
 *
 * @param descr
 * @param value
 */
void Status_ValueChangedInt(const char *descr, int value)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s: %d\n", descr, value);
}

void Status_ValueChangedInt(const char *group, const char *descr, int value)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s - %s: %d\n", group, descr, value);
}

void Status_ValueChangedStr(const char *descr, const char *value)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s: %s\n", descr, value);
}

void Status_ValueChangedStr(const char *group, const char *descr, const char *value)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s - %s: %s\n", group, descr, value);
}

void Status_LogMessage(const char *msg)
{
    status_cnt = STATUS_DISPLAY_TIME;
    Serial.printf("%s\n", msg);
}

void Status_Loop(uint32_t elapsed_time)
{
    status_elapsed_time += elapsed_time;
}

void Status_LoopMain()
{
    if (status_cnt > 0)
    {
        if (status_cnt > status_elapsed_time)
        {
            status_cnt -= status_elapsed_time;
        }
        else
        {
            status_cnt = 0;
        }
        status_elapsed_time = 0;

        if (status_cnt == 0)
        {
            Serial.printf("\n");
        }
    }
}


#endif

#endif

