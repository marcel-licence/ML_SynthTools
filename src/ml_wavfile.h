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
 * @file ml_wavfile.h
 * @author Marcel Licence
 * @data 27.06.2023
 *
 * @brief   Wave file structures etc.
 */


#ifndef ML_WAVFILE_H_
#define ML_WAVFILE_H_


union wav_tag__header_u
{
    struct
    {
        char tag_name[4];
        uint32_t tag_data_size;
    };
    uint8_t wavHdr[8];
};

union wavHeader
{
    struct
    {
        char riff[4]; /*!< 'RIFF' */
        uint32_t fileSize; /*!< bytes to write containing all data (header + data) */
        char waveType[4]; /*!< 'WAVE' */

        char format[4]; /*!< 'fmt ' */
        uint32_t lengthOfData; /*!< length of the fmt header (16 bytes) */
        uint16_t format_tag; /*!< 0x0001: PCM */
        uint16_t numberOfChannels; /*!< 'WAVE' */
        uint32_t sampleRate;
        uint32_t byteRate;

        uint16_t bytesPerSample;
        uint16_t bitsPerSample;
        wav_tag__header_u nextTag;
    };
    uint8_t wavHdr[44];
};

union wav_tag__smpl_u
{
    struct
    {
        uint32_t manufacturer; /* 0x08 */
        uint32_t product; /* 0x0C */
        uint32_t sample_period; /* 0x10 */
        uint32_t MIDI_unity_note; /* 0x14 */
        uint32_t MIDI_pitch_fraction; /* 0x18 */
        uint32_t SMPTE_format;
        uint32_t SMPTE_offset;
        uint32_t number_of_sample_loops;
        uint32_t sample_data;
        struct
        {
            uint32_t ID;
            uint32_t type;
            uint32_t start;
            uint32_t end;
            uint32_t fraction;
            uint32_t number_of_times_to_play_the_loop;
        };
    };
    uint8_t raw[4 * 15];
};


#endif /* ML_WAVFILE_H_ */
