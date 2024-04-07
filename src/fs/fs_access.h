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
 * @file fs_access.h
 * @author Marcel Licence
 * @data 20.10.2023
 *
 * @brief   This file contains prototypes of file accessor functions
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifndef ML_SYNTH_INLINE_DEFINITION


/*
 * include files
 */
#include <stdint.h>

/*
 * datatypes
 */
typedef int fs_id_t;

/*
 * definitions
 */
#define FS_ID_LITTLEFS  ((fs_id_t)0)
#define FS_ID_SD_MMC    ((fs_id_t)1)

/*
 * function declarations
 */
void FS_Setup(void);
bool FS_OpenFile(fs_id_t id, const char *filename, const char *mode);
bool FS_OpenFile(fs_id_t id, const char *filename);
void FS_CloseFile(void);
void FS_UseTempFile(void);

uint32_t readBytes(uint8_t *buffer, uint32_t len);
uint32_t writeBytes(uint8_t *buffer, uint32_t len);
uint32_t availableBytes(void);
void fileSeekTo(uint32_t pos);
uint32_t getStaticPos(void);
uint32_t getCurrentOffset(void);
uint32_t readBytesFromAddr(uint8_t *buffer, uint32_t addr, uint32_t len);
bool getFileFromIdx(uint32_t idx, char *filename, char *filter);


#endif /* #ifndef ML_SYNTH_INLINE_DEFINITION */

