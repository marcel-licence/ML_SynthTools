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
 * @file wm8978.h
 * @author Marcel Licence
 * @date 02.05.2023
 *
 * @brief This module is used to initialize the WM8978 codec
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DECLARATION
#ifdef WM8978_ENABLED


void WM8978_Setup();


#endif // WM8978_ENABLED
#endif // ML_SYNTH_INLINE_DECLARATION


#ifdef ML_SYNTH_INLINE_DEFINITION
#ifdef WM8978_ENABLED


#include <WM8978.h> /* requires library wm8978-esp32 from https://github.com/CelliesProjects/wm8978-esp32 */

void WM8978_Setup()
{
    /* do something */
}


#endif // WM8978_ENABLED
#endif // ML_SYNTH_INLINE_DEFINITION
