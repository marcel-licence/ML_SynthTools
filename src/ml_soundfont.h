/*
 * Copyright (c) 2023 Marcel Licence
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
 * @file    ml_soundfont.h
 * @author  Marcel Licence
 * @date    26.06.2023
 *
 * @brief   This file contains the prototypes used to load the soundfonts
 */


#ifndef SRC_ML_SOUNDFONT_H_
#define SRC_ML_SOUNDFONT_H_


#include <stdint.h>


struct sf2_soundfont_info_s
{
    uint32_t inst;
    uint32_t ibag;
    uint32_t igen;
    uint32_t imod;
    uint32_t shdr;
    uint32_t smpl;
    uint32_t pbag;
    uint32_t phdr;
    uint32_t pgen;

    uint32_t inst_cnt;
    uint32_t ibag_cnt;
    uint32_t igen_cnt;
    uint32_t imod_cnt;
    uint32_t shdr_cnt;
    uint32_t smpl_cnt;
    uint32_t pbag_cnt;
    uint32_t phdr_cnt;
    uint32_t pgen_cnt;
};

union range_u
{
    struct
    {
        uint8_t lowest;
        uint8_t highest;
    };
    uint16_t u16_value;
};


/**
 * @brief Structure is used to provide required information to load a sample into another module
 */
struct instrLoadInfo_s
{
    char name[21];
    uint32_t start;
    uint32_t end;
    uint32_t startLoop;
    uint32_t endLoop;
    uint8_t exClass;
    uint8_t rootKey;
    int8_t tune;
    uint32_t sampleRate;
    uint8_t sampleModus;

    union range_u keyRange;
    union range_u velRange;

    int16_t attackVolEnv;
    int16_t holdVolEnv;
    int16_t decayVolEnv;
    int16_t sustainVolEnv;
    int16_t releaseVolEnv;

    uint32_t smplDataOffset;
};


/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */

/**
 * @brief
 *
 * @param filename
 */

/**
 * @brief
 *
 * @param idx
 * @param infoPtr
 */
bool ML_SF2_GetInstrumentInfo(uint32_t idx, struct instrLoadInfo_s *infoPtr);

/**
 * @brief
 *
 * @param idx
 * @param cb
 */
bool ML_SF2_LoadPresetMultiBag(uint32_t idx, void(*cb)(struct instrLoadInfo_s *infoPtr));


/**
 * @brief
 *
 * @param idx
 * @param infoPtr
 */
bool ML_SF2_GetInstrumentInfoMultiBag(uint32_t idx, void(*cb)(struct instrLoadInfo_s *infoPtr));

/**
 * @brief
 *
 * @param idx
 * @param infoPtr
 * @return bool
 */
bool ML_SF2_LoadSamplesFromInfo(uint32_t idx, struct instrLoadInfo_s *infoPtr);

/**
 * @brief
 *
 */

/**
 * @brief
 *
 * @param buffer
 * @param len
 * @return uint
 */
uint32_t readBytes(uint8_t *buffer, uint32_t len);

/**
 * @brief
 *
 */
uint32_t getCurrentOffset();

/**
 * @brief
 *
 */
void fileSeekTo(uint32_t addr);

/**
 * @brief
 *
 * @param buffer
 * @param addr
 * @param len
 * @return uint
 */
uint32_t readBytesFromAddr(uint8_t *buffer, uint32_t addr, uint32_t len);

/**
 * @brief
 *
 */
bool openFile(const char *filename);

/**
 * @brief
 *
 */
void closeFile();

/**
 * @brief
 *
 * @param buffer
 * @param addr
 * @param len
 * @return uint
 */
uint32_t readBytesFromAddr(uint8_t *buffer, uint32_t addr, uint32_t len);

/**
 * @brief Will be called when a soundfont will be parsed and a preset will be identified
 *
 * @param preset
 * @param idx
 */
void sf2_preset_indication(union preset_hdr_s *preset, uint32_t idx);

void sf2_preset_generator_indication(union SF2PresetGenerator_u *pgen, uint32_t idx);

/**
 * @brief Will be called when a soundfont will be parsed and a sample will be identified
 *
 * @param sample
 * @param idx
 */
void sf2_sample_indication(union sf2_sample_hdr_s *sample, uint32_t idx);

/**
 * @brief Will be called when a soundfont will be parsed and a preset back will be identified
 *
 * @param pbag
 */
void sf2_preset_bag_indication(union SF2PresetBag_u *pbag);

/**
 * @brief
 *
 * @param len
 */
void sf2_sdta_smpl_indication(uint32_t len);

/**
 * @brief
 *
 * @param pmod
 */
void sf2_preset_modulator_indication(union SF2PresetModulator_u *pmod);

/**
 * @brief Will be called when a soundfont will be parsed and a sample will be identified
 *
 * @param inst
 * @param idx
 */
void sf2_instrument_indication(union SF2Instrument_u *inst, uint32_t idx);

/**
 * @brief Will be called when a soundfont will be parsed and a sample will be identified
 *
 * @param ibag
 * @param idx
 */
void sf2_instrument_bag_indication(union SF2InstrumentBag_u *ibag, uint32_t idx);

/**
 * @brief Will be called when a soundfont will be parsed and a sample will be identified
 *
 * @param igen
 * @param idx
 */
void sf2_instrument_generator_indication(union SF2InstrumentGenerator_u *igen, uint32_t idx);

/**
 * @brief
 *
 */

/**
 * @brief
 *
 */


struct sf2_soundfont_info_s *ML_SF2_GetSoundFontInfo();


#endif /* SRC_ML_SOUNDFONT_H_ */

