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
 * @file string_utils.h
 * @author Marcel Licence
 * @date 16.03.2024
 *
 * @brief Some little functions for string calculations
 */


#ifndef SRC_STRING_UTILS_H_
#define SRC_STRING_UTILS_H_


#include <stdio.h>
#include <string.h>
#include <ctype.h>


static inline
bool str_ends_with(const char *str, const char *end)
{
    if (strlen(str) < strlen(end))
    {
        return false;
    }
    if (strcmp(end, &str[strlen(str) - strlen(end)]) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static inline
void convert_to_lowercase(char *input_string)
{
    for (int i = 0; input_string[i] != '\0'; i++)
    {
        input_string[i] = tolower(input_string[i]);
    }
}

static inline
bool str_ends_with_ul(const char *str, char *end)
{
    if (strlen(str) < strlen(end))
    {
        return false;
    }

    char str1[128];
    strcpy(str1, str);

    convert_to_lowercase(str1);
    convert_to_lowercase(end);

    if (strcmp(end, &str1[strlen(str1) - strlen(end)]) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


#endif /* SRC_STRING_UTILS_H_ */
