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
 * @file fs_teensy.cpp
 * @author Marcel Licence
 * @data 15.11.2024
 *
 * @brief   This file contains Teensy specific file system implementation
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef TEENSYDUINO

#ifdef ML_SYNTH_INLINE_DEFINITION


/*
 * include files
 */
#include <fs/fs_access.h>
#include <ml_status.h>
#include <string_utils.h>

#include <SD.h>

/*
 * definitions
 */
#define FST fs::FS

/*
 * static function declarations
 */
static void printDirectory(File dir, int numTabs);

/*
 * local variables
 */
static File f;
static File *g_file = NULL;
static File *t_file = NULL;
const int SD_CS = BUILTIN_SDCARD;

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
    File dir = SD.open("/", "r");
    char filename_clean[64];

    bool result = getFileFromIdx(dir, 0, idx, filename_clean, filter) == 0 ? true : false;
    sprintf(filename, "/%s", filename_clean);
    dir.close();
    return result;
}

/*
 * function definitions
 */
void FS_Setup(void)
{
    if (!SD.begin(SD_CS))
    {
        Serial.println("SD init failed!");
    }
    else
    {
        Serial.println("SD init passed.");
        File root;
        root = SD.open("/");
        while (true)
        {
            File entry = root.openNextFile();
            if (!entry)
            {
                break;
            }
            else
            {
                Serial.println(entry.name());
            }
        }
    }
}

bool FS_OpenFile(int fs, const char *filename, const char *mode)
{
    f = SD.open(filename, mode);
    if (f)
    {
        g_file = &f;
        return true;
    }
    else
    {
        Serial.printf("Error opening file: %s\n", filename);
        return false;
    }
}

void FS_LittleFS_CloseFile(void)
{
    g_file->close();
    g_file = NULL;
}

void FS_CloseFile(void)
{
    g_file->close();
    g_file = NULL;
}

void FS_UseTempFile(void)
{
    g_file = t_file;
}

uint32_t readBytes(uint8_t *buffer, uint32_t len)
{
    return g_file->read(buffer, len);
}

uint32_t writeBytes(uint8_t *buffer, uint32_t len)
{
    return g_file->write(buffer, len);
}

uint32_t availableBytes(void)
{
    return g_file->available();
}

void fileSeekTo(uint32_t pos)
{
    g_file->seek(0, SeekSet);
    g_file->seek(pos, SeekSet);
}

uint32_t getStaticPos(void)
{
    return g_file->position();
}

uint32_t getCurrentOffset(void)
{
    return g_file->position();
}

uint32_t readBytesFromAddr(uint8_t *buffer, uint32_t addr, uint32_t len)
{
    fileSeekTo(addr);
    return readBytes(buffer, len);
}

#endif  /* #ifdef ML_SYNTH_INLINE_DEFINITION */

#endif /* TEENSYDUINO */

