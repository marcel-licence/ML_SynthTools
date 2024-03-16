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
 * @file fs_esp32.h
 * @author Marcel Licence
 * @data 27.06.2023
 *
 * @brief   This file contains ESP32 sepecific file system imeplementation
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ESP32

#ifdef ML_SYNTH_INLINE_DEFINITION


/*
 * include files
 */
#include <fs/fs_access.h>
#include <ml_status.h>
#include <string_utils.h>


#include <FS.h>
#ifdef ARDUINO_RUNNING_CORE /* tested with arduino esp32 core version 2.0.2 */
#include <LittleFS.h> /* Using library LittleFS at version 2.0.0 from https://github.com/espressif/arduino-esp32 */
#else
#include <LITTLEFS.h> /* Using library LittleFS_esp32 at version 1.0.6 from https://github.com/lorol/LITTLEFS */
#define LittleFS LITTLEFS
#endif
#include <SD_MMC.h>
#ifndef SD_MMC
#ifdef SD
#define SD_MMC  SD
#endif
#endif

#ifdef ARDUINO_DAISY_SEED
#include <STM32SD.h>

extern Sd2Card card;
extern SdFatFs fatFs;

#define SD_MMC  SD

#define FST SDClass
#else
#define FST fs::FS
#endif


#define FORMAT_LITTLEFS_IF_FAILED true


/*
 * static function declarations
 */
static void printDirectory(File dir, int numTabs);
#ifdef SD_MMC_ENABLED
static bool FS_SdCardInit(void);
#endif


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

#ifdef SD_MMC_ENABLED
static bool FS_SdCardInit(void)
{
#ifdef ESP32
    if (!SD_MMC.begin("/sdcard", false))
#else
    if (!card.init(SD_DETECT_NONE))
#endif
    {
        Status_LogMessage("Card Mount Failed");
        delay(1000);
        return false;
    }

#ifdef ESP32
    uint8_t cardType = SD_MMC.cardType();

    if (cardType == CARD_NONE)
    {
        Status_LogMessage("No SD card attached");

        delay(1000);
        return false;
    }

    if (cardType == CARD_MMC)
    {
        Status_LogMessage("Card Access: MMC");
    }
    else if (cardType == CARD_SD)
    {
        Status_LogMessage("Card Access: SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Status_LogMessage("Card Access: SDHC");
    }
    else
    {
        Status_LogMessage("Card Access: UNKNOWN");
    }
#else
    Serial.println("A card is present.");

    Serial.print("\nCard type: ");
    switch (card.type())
    {
    case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
    case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
    case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
    default:
        Serial.println("Unknown");
    }

    if (!fatFs.init())
    {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        return false;
    }

    uint64_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(fatFs.fatType(), DEC);
    Serial.println();

    volumesize = fatFs.blocksPerCluster(); // clusters are collections of blocks
    volumesize *= fatFs.clusterCount(); // we'll have a lot of clusters
    volumesize *= 512; // SD card blocks are always 512 bytes
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize);
    Serial.print("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);

    delay(200);
#endif

    {
        File dir = SD_MMC.open("/");
        printDirectory(dir, 0);
        dir.close();
    }

    return true;
}
#endif

void FS_Setup(void)
{
#ifdef SD_MMC_ENABLED
    FS_SdCardInit();
    Serial.printf("SD card init!\n");
#else
    Serial.printf("no sd card\n");
#endif

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.printf("LittleFS Mount Failed");
        return ;
    }

    Serial.printf("LittleFS Mount Successful!\n");

    {
        Serial.printf("List files ...\n\n");
        File dir = LittleFS.open("/");
        printDirectory(dir, 0);
        dir.close();
    }
#ifdef SD_MMC
#endif
}


#endif /* #ifdef ML_SYNTH_INLINE_DEFINITION */

#endif /* ESP32 */

