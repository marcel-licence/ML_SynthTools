/*
 * Copyright (c) 2022 Marcel Licence
 * Modified for WM8978 support on LilyGO TTGO T9 by David Agnér
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
 * @file board_ttgo_t9_rgb_led_wm8978.h
 * @author Marcel Licence / David Agnér
 * @date 02.05.2023
 *
 * @brief This file contains init stuff for LilyGO TTGO T9 (v1.5) module
 * @see https://github.com/LilyGO/TTGO-TAudio
 * @see Schematic: https://github.com/LilyGO/TTGO-TAudio/blob/master/schematic.pdf
 */
 
 
 /* ---------  Pin mapping -----------------
 
 PIN	FUNCTION			Note
  0		I2S_MCLK_PIN		DAC WM8978 /HEADER
  1		TX UART0		CP210x USB 
  2		SPI_MISO		SD-card /HEADER
  3		RX UART0		CP210x USB
  4 		FREE			HEADER
  5		I2C_SCL_2		HEADER
 12		FREE			HEADER
 13		SPI_CS			SD-card /HEADER
 14		SPI_SCK			SD-card
 15		SPI_MOSI		SD-card /HEADER
 18		I2C_SCL			Control WM8978 
 19		I2C_SDA			Control WM8978
 21		BLINK_LED_PIN		Connector
 22 		LED_STRIP_PIN		WS2812b LED strip /Connector
 23		I2C_SDA_2		HEADER
 25		I2S_WCLK_PIN		DAC WM8978
 26		I2S_DOUT_PIN		DAC WM8978
 27		I2S_DIN_PIN		ADC WM9878
 32		FREE			HEADER
 33		I2S_BCLK_PIN		DAC WM8978
 34		PIN_KEY_4		Button 4 /HEADER
 35		MIDI_RX2		MIDI in /HEADER
 36		PIN_KEY_3		Button 3
 39		PIN_KEY_2		Button 2
 

 */
#ifdef __CDT_PARSER__
#include <cdt.h>
#endif

#ifndef BOARDS_BOARD_TTGO_T9_RGB_LED_WM8978_H_
#define BOARDS_BOARD_TTGO_T9_RGB_LED_WM8978_H_

#include <Wire.h>
#include <WM8978.h> // https://github.com/CelliesProjects/wm8978-esp32
#include <FastLED.h> // https://github.com/FastLED/FastLED

#define MIDI_PORT2_ACTIVE
#define MIDI_RX2_PIN 35 /* Free on HEADER */


/* on board led, green */
//#define BLINK_LED_PIN     22
#define BLINK_LED_PIN     21 /* use external free pin on connector */


/* For WS2812B leds */
#define LED_STRIP_PIN 22 /* onboard LEDs */
#define NUM_LEDS      19
#define BRIGHTNESS    64
#define LED_TYPE      WS2812B
#define COLOR_ORDER   RGB

/* The WM9878 DAC needs I2C for setup but it is not on the HEADER pins, use second I2C bus for (wire2) for OLED and ADS1115 for pots ? */
#define I2C_SDA_2 23		// For OLED later on ?
#define I2C_SCL_2 5		  // For OLED later on ?


/* WM8978 */
#define I2C_SDA 19		/* To control WM8978 */
#define I2C_SCL 18		/* To control WM8978 */
#define I2S_MCLK_PIN 0 /* 24 MHz */
#define I2S_BCLK_PIN 33
#define I2S_WCLK_PIN 25
#define I2S_DOUT_PIN 26 /* DAC */
#define I2S_DIN_PIN 27 /* ADC */

#define SAMPLE_SIZE_16BIT
//#define SAMPLE_SIZE_24BIT  // WM8978 supported, not tested

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100 
//#define SAMPLE_RATE 44100 // WM8978 supported, not tested

#endif
#define CHANNEL_COUNT   2
#define WORD_SIZE   16
#define I2S1CLK (512*SAMPLE_RATE)
#define BCLK    (SAMPLE_RATE*CHANNEL_COUNT*WORD_SIZE)
#define LRCK    (SAMPLE_RATE*CHANNEL_COUNT)


/* Need help on geting buttons to work in your code defines updated*/
#define BUTTON_DEBUG_MSG

//#define PIN_LED4    (22) // ? 
//#define PIN_LED5    (19) // ?


#define PIN_KEY_2                   (39)
#define PIN_KEY_3                   (36)
#define PIN_KEY_4                   (34)

#define PIN_PLAY                    PIN_KEY_2
#define PIN_VOL_UP                  PIN_KEY_4
#define PIN_VOL_DOWN                PIN_KEY_3

#define OUTPUT_PIN 0 // ?
#define MCLK_CH 0
#define PWM_BIT 1

/* SPI SD-card reader + header */
#define SPI_SCK     14
#define SPI_MISO    2
#define SPI_MOSI    15
#define SPI_SS      13 			// SD-card CS



CRGB ledstrip[NUM_LEDS];

void WS2812b_Blink_Process(void)
{
    static bool ledOn = true;
    if (ledOn)
    {  
      ledstrip[12] = CRGB::Green;  // Led at USB connector
      FastLED.show();
    }
    else
    {
      ledstrip[12] = CRGB::Black; // Led at USB connector
      FastLED.show();
    }
    ledOn = !ledOn;
}

void WS2812b_setup() {
  FastLED.addLeds<LED_TYPE, LED_STRIP_PIN, COLOR_ORDER>(ledstrip, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {
    ledstrip[i] = CRGB::Black;
    FastLED.show();
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    ledstrip[i] = CRGB::Green;
    FastLED.show();
    delay(40);
    ledstrip[i] = CRGB::Black;
    FastLED.show();
    delay(20);
  }
    for (int i = NUM_LEDS -1; i >= 0 ; i--) {
    ledstrip[i] = CRGB::Green;
    FastLED.show();
    delay(40);
    ledstrip[i] = CRGB::Black;
    FastLED.show();
    delay(20);
  }
  ledstrip[12] = CRGB::Green;
  FastLED.show();
}

static WM8978 ac;

void WM8978_setup()
{
    Serial.printf("Connect to WS8978 codec... ");
    while (not ac.begin(I2C_SDA, I2C_SCL))
    {
        Serial.printf("Failed!\n");
        delay(1000);
    }
    Serial.printf("OK\n");

	// Volume control.
	ac.setSPKvol(40); // Change volume here for board speaker output (Max 63).
	ac.setHPvol(50, 50); // Change volume here for headphone jack left, right channel.
/*
#if 1
    ac.SetLineSource();
#else
    ac.SetMicSource(); // handle with care: mic is very sensitive and might cause feedback using amp!!! 
#endif

#if 0
    ac.DumpRegisters();
#endif

    // Enable amplifier
#if 0 // amplifier only required when speakers attached? 
    pinMode(GPIO_PA_EN, OUTPUT);
    digitalWrite(GPIO_PA_EN, HIGH);
#endif
*/
}

/*
 * pullup required to enable reading the buttons (buttons will connect them to ground if pressed)
 */
/*void button_setup()
{
    // Configure keys on LilyGO TTGO T9 
    pinMode(PIN_PLAY, INPUT_PULLUP);
    pinMode(PIN_VOL_UP, INPUT_PULLUP);
    pinMode(PIN_VOL_DOWN, INPUT_PULLUP);
}
*/
//#ifdef AC101_ENABLED
/*
 * selects the microphone as audio source
 * handle with care: mic is very sensitive and might cause feedback using amp!!!
 */
 /*
void ac101_setSourceMic(void)
{
    ac.SetMicSource();
}
*/
/*
 * selects the line in as input
 */
 /*
void ac101_setSourceLine(void)
{
    ac.SetLineSource();
}
#endif / #ifdef AC101_ENABLED 
*/


 //* very bad implementation checking the button state
 //* there is some work required for a better functionality
/*
void button_loop()
{

    if (digitalRead(PIN_PLAY) == LOW)
    {
        Serial.println("PIN_PLAY pressed");
        if (buttonMapping.key4_pressed != NULL)
        {
            buttonMapping.key4_pressed();
        }
    }
    if (digitalRead(PIN_VOL_UP) == LOW)
    {
        Serial.println("PIN_VOL_UP pressed");
        if (buttonMapping.key5_pressed != NULL)
        {
            buttonMapping.key5_pressed();
        }
    }
    if (digitalRead(PIN_VOL_DOWN) == LOW)
    {
        Serial.println("PIN_VOL_DOWN pressed");
        if (buttonMapping.key6_pressed != NULL)
        {
            buttonMapping.key6_pressed();
        }
    }

}
*/

#endif /* BOARDS_BOARD_TTGO_T9_RGB_LED_WM8978_H_ */
