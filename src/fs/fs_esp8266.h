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
 * @file fs_esp8266.h
 * @author Marcel Licence
 * @data 27.06.2023
 *
 * @brief   This file contains ESP8266 sepecific file system imeplementation
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ESP8266

#ifdef ML_SYNTH_INLINE_DEFINITION


/*
 * include files
 */
#include <fs/fs_access.h>
#include <string_utils.h>


#include <FS.h>

#include <LittleFS.h>


/*
 * extern function definitions
 */
uint32_t getFileFromIdx(File dir, int numTabs, uint32_t idx, char *filename, char *filter)
{
    while (true)
    {

        File entry = dir.openNextFile();
        if (! entry)
        {
            break;
        }

        if (entry.isDirectory())
        {
            idx = getFileFromIdx(entry, numTabs + 1, idx, filename, filter);
        }
        else
        {
            if (str_ends_with_ul(entry.name(), filter))
            {
                if (idx == 0)
                {
                    strcpy(filename, entry.name());
                    return idx;
                }
                idx -= 1;
            }

        }
        entry.close();
    }
    return idx;
}

bool getFileFromIdx(uint32_t idx, char *filename, char *filter)
{
    File dir = LittleFS.open("/", "r");
    char filename_clean[64];

    bool result = getFileFromIdx(dir, 0, idx, filename_clean, filter) == 0 ? true : false;
    sprintf(filename, "/%s", filename_clean);
    dir.close();
    return result;
}

void FS_Setup(void)
{

    if (!LittleFS.begin())
    {
        Serial.printf("Error: LittleFS Mount Failed\n");
        return ;
    }


    Serial.printf("LittleFS Mount Successful!\n");

}


#endif /* #ifdef ML_SYNTH_INLINE_DEFINITION */

#endif /* ESP8266 */
