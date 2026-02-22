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

/*
 * ml_midi_file_stream.h
 *
 *  Created on: 28.05.2022
 *      Author: PC
 */

#ifndef SRC_ML_MIDI_FILE_STREAM_H_
#define SRC_ML_MIDI_FILE_STREAM_H_


#include <stdint.h>

/*
 * file access functions to allow individual implementation of file accessory functions
 */
struct file_access_f
{
    uint8_t (*open)(const char *path, const char *mode);
    int (*read)(void *buf, uint8_t unused __attribute__((unused)), size_t size, struct file_access_f *ff);
    int (*write)(void *buf, uint8_t unused __attribute__((unused)), size_t size, struct file_access_f *ff);
    void (*close)(struct file_access_f *ff);
    uint32_t file; /* just for compatibility */
    char (*getc)(struct file_access_f *ff);
    char (*putc)(char c, struct file_access_f *ff);
    int (*tell)(struct file_access_f *ff);
    char (*seek)(struct file_access_f *ff, int pos, uint8_t mode);
};

struct midi_time_sign_s
{
    uint8_t numerator;
    uint8_t denominator;
    uint8_t denominator_div_96;
    uint8_t reserved;
};

struct midi_proc_s
{
    struct file_access_f *ff;
    int file_format;
    union
    {
        uint16_t resolution;
        unsigned char division_type_and_resolution[2];
    };
    int32_t chunk_size;
    int32_t chunk_start;
    int number_of_tracks;
    int number_of_tracks_read;
    long tick;
    long previous_tick;
    int at_end_of_track;
    uint32_t midi_tempo;
    uint8_t running_status;
    struct midi_time_sign_s time_signature;
    void (*raw)(uint8_t *data, uint8_t data_len);
    void (*noteOn)(uint8_t ch, uint8_t note, uint8_t vel);
    void (*noteOff)(uint8_t ch, uint8_t note, uint8_t vel);
    void (*pitchBend)(uint8_t ch, float bend);
    void (*modWheel)(uint8_t ch, float value);
    void (*controlChange)(uint8_t ch, uint8_t number, uint8_t value);
    void (*meta)(uint8_t type);
};

int midi_file_stream(int argc, char **argv);
uint8_t *midi_file_stream_load(char *filename, struct midi_proc_s *midiP);
void midi_file_stream_close(struct midi_proc_s *midiP);

void MidiStreamReadTrackPrepare(struct midi_proc_s *midiP);
bool MidiStreamReadSingleEventTime(struct midi_proc_s *midiP, long *duration);
bool MidiStreamReadSingleEvent(struct midi_proc_s *midiP);
void MidiStreamReadTrackFinish(struct midi_proc_s *midiP);
void MidiStreamParseTrack(struct midi_proc_s *midiP);
void MidiStreamSkipTrack(struct midi_proc_s *midiP);
void MidiStreamRewind(struct midi_proc_s *midiP);

unsigned short interpret_uint16(unsigned char *buffer);

#endif /* SRC_ML_MIDI_FILE_STREAM_H_ */
