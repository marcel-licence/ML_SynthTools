/*
 * Copyright (c) 2026 Marcel Licence
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
 * @file ml_track_selector.h
 * @author Marcel Licence
 * @data 08.03.2026
 *
 * @brief   Implementation of the track selector module which allows continouus track playback
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include "ml_track_selector.h"

#include <string.h>
#include <fs/fs_access.h>


static char s_filter[8] = "";


enum fileLoadStatus
{
    fls_ok,
    fls_open_error,
    fls_load_error,
    fls_ignored,
    fls_file_not_found,
};


static enum fileLoadStatus LoadFileFromIdx(int fileIdx);


static uint8_t g_fileIdx = 0;
static uint8_t g_totalFilesFound = 0;
static bool g_auto_reload = true;
static bool g_is_first_loaded_file = true;


void TrackSelector_Setup(const char *filter)
{
    strncpy(s_filter, filter, sizeof(s_filter));

    FS_Setup();

    g_totalFilesFound = getFileCount(s_filter);
    g_is_first_loaded_file = true;

    TrackSelector_DebugPrintf("Found %u %s files\n", g_totalFilesFound, s_filter);
}

void TrackSelector_LoadFirst(void)
{
    g_fileIdx = 0;
    LoadFileFromIdx(g_fileIdx);
}

void TrackSelector_Autostart(void)
{
    TrackSelector_LoadFileNext();
}

void TrackSelector_Process(void)
{
    if (g_auto_reload)
    {
        TrackSelector_Autostart();
    }
}

static enum fileLoadStatus LoadFileFromIdx(int fileIdx)
{
    TrackSelector_DebugPrintf("LoadFileFromIdx(%d)\n", fileIdx);

    char filename[64];

    if (!getFileFromIdx(fileIdx, filename, s_filter))
    {
        if (fileIdx > 0)
        {
            fileIdx = 0;
            if (!getFileFromIdx(fileIdx, filename, s_filter))
            {
                return fls_file_not_found;
            }
        }
        else
        {
            TrackSelector_DebugPrintf("unable to load files\n");
            return fls_file_not_found;
        }
    }
    else
    {
        TrackSelector_DebugPrintf("file %s found index %d\n", filename, fileIdx);

    }

    if (strcmp(filename, "/") == 0)
    {
        TrackSelector_DebugPrintf("file %s with index %d ignored\n", filename, fileIdx);
#if 0
        return fls_ignored;
#endif
    }

    if (TrackSelector_LoadTrack(filename))
    {
        return fls_ok;
    }
    else
    {
        return fls_load_error;
    }

    TrackSelector_DebugPrintf("file with index %d error on open\n", fileIdx);

    return fls_open_error;
}

void TrackSelector_LoadFirstFile(void)
{
    if (g_totalFilesFound > 0)
    {
        g_fileIdx = 0;
        uint8_t startIdx = g_fileIdx;

        while (LoadFileFromIdx((g_fileIdx) != fls_ok) && (g_fileIdx != startIdx))
        {
            g_fileIdx = (g_fileIdx + 1) % g_totalFilesFound;
        }
    }
}

void TrackSelector_LoadFileNext(void)
{
    if (g_totalFilesFound > 0)
    {
        uint8_t startIdx = g_fileIdx;

        do
        {
            if (!g_is_first_loaded_file)
            {
                g_fileIdx = (g_fileIdx + 1) % g_totalFilesFound;
            }
            else
            {
                g_is_first_loaded_file = false;
            }
        }
        while (LoadFileFromIdx(g_fileIdx) != fls_ok && g_fileIdx != startIdx);
    }
}

void TrackSelector_LoadFilePrev(void)
{
    if (g_totalFilesFound > 0)
    {
        uint8_t startIdx = g_fileIdx;

        do
        {
            if (!g_is_first_loaded_file)
            {
                g_fileIdx = (g_fileIdx == 0) ? (g_totalFilesFound - 1) : (g_fileIdx - 1);
            }
            else
            {
                g_is_first_loaded_file = false;
            }
        }
        while (LoadFileFromIdx(g_fileIdx) != fls_ok && g_fileIdx != startIdx);
    }
}

__attribute__((weak))
void TrackSelector_DebugPrintf(const char *fmt __attribute__((__unused__)), ...)
{

}