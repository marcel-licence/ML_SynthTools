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
 * @file fs_common.h
 * @author Marcel Licence
 * @data 13.03.2024
 *
 * @brief   This file contains the platform independent implementation of file access functions
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DEFINITION


/*
 * include files
 */
#include "fs/fs_access.h"
#include "string_utils.h"


#include <FS.h>
#if (defined ARDUINO_RUNNING_CORE) || (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_ARCH_RP2040) || (defined ESP8266) /* tested with arduino esp32 core version 2.0.2 */
#include <LittleFS.h> /* Using library LittleFS at version 2.0.0 from https://github.com/espressif/arduino-esp32 */
#else
#include <LITTLEFS.h> /* Using library LittleFS_esp32 at version 1.0.6 from https://github.com/lorol/LITTLEFS */
#define LittleFS LITTLEFS
#endif

#ifdef ESP32
#include <SD_MMC.h>
#endif

/*
 * datatypes
 */


/*
 * definitions
 */
#define FST fs::FS

/*
 * function declarations
 */
static bool FS_OpenFile(FST &fs, const char *filename);

/*
 * local variables
 */
static File f;
static File *g_file = NULL;
static File *t_file = NULL;


/*
 * static function definitions
 */
static FST &FsFromId(fs_id_t id)
{
    switch (id)
    {
    case FS_ID_LITTLEFS:
        return LittleFS;
#ifdef ESP32
#ifdef SD_MMC_ENABLED
    case FS_ID_SD_MMC:
        return SD_MMC;
#endif
#endif
    default:
        Serial.printf("Error: Unknown FS!\n");
    }
    return LittleFS;
}

bool FS_OpenFile(fs_id_t id, const char *filename)
{
    return FS_OpenFile(FsFromId(id), filename);
}

static bool FS_OpenFile(FST &fs, const char *filename)
{
#ifdef ESP32
    f = fs.open(filename);
#else
    f = fs.open(filename, "r");
#endif
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

/*
 * extern function definitions
 */
void FS_CloseFile(void)
{
    g_file->close();
    g_file = NULL;
}

#ifdef ESP32
#else
#endif

void FS_UseTempFile(void)
{
    g_file = t_file;
}

uint32_t readBytes(uint8_t *buffer, uint32_t len)
{
    return g_file->read(buffer, len);
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

#ifdef ESP32
#else
#endif


#endif /* #ifdef ML_SYNTH_INLINE_DEFINITION */

