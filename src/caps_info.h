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
 * @file caps_info.h
 * @author Marcel Licence
 * @date 24.02.2024
 *
 * @brief Contains function(s) to print capability information
 */


/* @see components/soc/esp32/include/soc/soc_caps.h */


#include <Arduino.h>


inline
void CapsPrintInfo(void)
{
#ifdef SOC_CPU_CORES_NUM
    Serial.printf("Cores num: %d\n", SOC_CPU_CORES_NUM);
#endif
#ifdef SOC_DAC_CHAN_NUM
    Serial.printf("Dac channel num: %d\n", SOC_DAC_CHAN_NUM);
#endif
#ifdef SOC_GPIO_PIN_COUNT
    Serial.printf("GPIO pin count: %d\n", SOC_GPIO_PIN_COUNT);
#endif
#ifdef SOC_I2C_NUM
    Serial.printf("I2C num: %d\n", SOC_I2C_NUM);
#endif
#ifdef SOC_I2S_NUM
    Serial.printf("I2S num: %d\n", SOC_I2S_NUM);
#endif
#ifdef SOC_I2S_SUPPORTS_ADC
    Serial.printf("I2S supports ADC\n");
#endif
#ifdef SOC_I2S_SUPPORTS_DAC
    Serial.printf("I2S supports DAC\n");
#endif
#ifdef SOC_CPU_HAS_FPU
    Serial.printf("Cpu has FPU\n");
#endif
}
