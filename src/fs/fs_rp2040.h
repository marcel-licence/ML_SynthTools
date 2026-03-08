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
 * @file fs_rp2040.cpp
 * @author Marcel Licence
 * @data 27.06.2023
 *
 * @brief   This file contains RP2040 sepecific file system imeplementation
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ARDUINO_ARCH_RP2040

#ifdef ML_SYNTH_INLINE_DEFINITION


/*
 * include files
 */
#include <fs/fs_access.h>
#include <ml_status.h>
#include <string_utils.h>


#include <FS.h>
#include <LittleFS.h>


/*
 * definitions
 */
#define FST fs::FS

#define FILENAME_MAX_LEN    64

/*
 * static function declarations
 */
static void printDirectory(File dir, int numTabs);

/*
 * static function definitions
 */
static void printDirectory(File dir, int numTabs)
{
    while (true)
    {

        File entry = dir.openNextFile();
        if (! entry)
        {
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

/*
 * extern function definitions
 */
void WavToKeyboard(fs_id_t id, const char *dirname, void(*fileInd)(const char *filename, int offset, uint8_t note), int depth, int max_depth, uint8_t note)
{
#ifdef ESP32
    File root = FsFromId(id).open(dirname);
#else
    File root = FsFromId(id).open(dirname, "r");
#endif
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    Serial.printf("WavToKeyboard root:%s\n", root.name());

    File file = root.openNextFile();
    if (!file)
    {
        Serial.printf("no files in root!\n");
    }
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.printf("Dir Found: %s (ignoreD)\n", file.name());
            if (max_depth != 0)
            {
            }
        }
        else
        {
            Serial.printf("File Found: %s\n", file.name());
            t_file = & file;
            fileInd(file.name(), depth, note++);
        }
        file = root.openNextFile();
    }
}

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
                    strncpy(filename, entry.name(), FILENAME_MAX_LEN - 1);
                    filename[FILENAME_MAX_LEN - 1] = '\0';
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
    char filename_clean[FILENAME_MAX_LEN - 1];

    bool found = (getFileFromIdx(dir, 0, idx, filename_clean, filter) == 0);

    if (found)
    {
        snprintf(filename, FILENAME_MAX_LEN, "/%s", filename_clean);
    }

    dir.close();
    return found;
}

static uint32_t getFileCountRecursive(File dir, char *filter)
{
    uint32_t count = 0;

    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }

        if (entry.isDirectory())
        {
            count += getFileCountRecursive(entry, filter);
        }
        else
        {
            if (str_ends_with_ul(entry.name(), filter))
            {
                count++;
            }
        }

        entry.close();
    }

    return count;
}

uint32_t getFileCount(char *filter)
{
    File dir = LittleFS.open("/", "r");
    if (!dir)
    {
        return 0;
    }

    uint32_t count = getFileCountRecursive(dir, filter);
    dir.close();
    return count;
}

/*
 * function definitions
 */
void FS_Setup(bool listFiles)
{
    if (!LittleFS.begin())
    {
        Serial.printf("LittleFS Mount Failed");
        return ;
    }

    Serial.printf("LittleFS Mount Successful!\n");

    if (listFiles)
    {
        Serial.printf("List files ...\n\n");
        File dir = LittleFS.open("/", "r");
        printDirectory(dir, 0);
        dir.close();
    }
}

void FS_Setup(void)
{
    FS_Setup(true);
}


#endif  /* #ifdef ML_SYNTH_INLINE_DEFINITION */

#endif /* ARDUINO_ARCH_RP2040 */

