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
 * @file ml_scope_oled.cpp
 * @author Marcel Licence
 * @date 03.02.2023
 *
 * @brief This file contains the oled specific implementation
 */


#if (!defined ARDUINO_RASPBERRY_PI_PICO) && (!defined ARDUINO_GENERIC_RP2040)


#include <ml_scope.h>


#include <Wire.h>
#include <Adafruit_GFX.h> /* requires library Adafruit-GFX-Library from https://github.com/adafruit/Adafruit-GFX-Library */
#include <Adafruit_SSD1306.h> /* requires library  Adafruit_SSD1306 from https://github.com/adafruit/Adafruit_SSD1306 */


#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS2 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
static Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


struct disp_s
{
    Adafruit_SSD1306 *oled;
};


static struct disp_s d[2];


static void display_set_oled(Adafruit_SSD1306 *oled1, Adafruit_SSD1306 *oled2);


void ScopeOled_Setup(void)
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

    if (!display2.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS2))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

    display_set_oled(&display2, &display);

    display.display();
    delay(250); // Pause for 2 seconds

    display.clearDisplay();
    display.display();

    display2.display();
    delay(250); // Pause for 2 seconds

    display2.clearDisplay();
    display2.display();
}

static void display_set_oled(Adafruit_SSD1306 *oled1, Adafruit_SSD1306 *oled2)
{
    d[0].oled = oled1;
    d[1].oled = oled2;
}

void ScopeOled_DrawData(const float *dispData, uint8_t idx)
{
    static uint8_t drawData[128];
    bool proc = false;
    d[idx].oled->clearDisplay();

    float maxVal = 0.01f;

    for (int n = 0; n < SCREEN_WIDTH; n++)
    {
        if (dispData[n] > maxVal)
        {
            maxVal = dispData[n];
        }
        if ((-dispData[n]) > maxVal)
        {
            maxVal = -dispData[n];
        }
    }

    float multiplier = 1.0f; // 2.0f
    {
        multiplier =  1.0f / maxVal;
    }

    /* determin if the display should draw the waveform or not */
    for (int n = 0; n < SCREEN_WIDTH; n++)
    {
        drawData[n] = 32 - (dispData[n] * 32.0f * multiplier);
        if (drawData[n] != 32)
        {
            proc = true;
        }
    }

    /* draw waveform */
    if (proc)
    {
        for (int n = 0; n < 127; n++)
        {
            d[idx].oled->drawLine(n, drawData[n], n + 1, drawData[n + 1], SSD1306_WHITE);
        }
    }
    d[idx].oled->display();
}

#endif

