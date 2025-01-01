/*
 * Copyright (c) 2025 Marcel Licence
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
