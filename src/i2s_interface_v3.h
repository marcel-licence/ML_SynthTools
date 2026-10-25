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
 * @file i2s_interface_v3.h
 * @author Marcel Licence
 * @date 25.10.2026
 *
 * @brief  this file includes all required function to setup and drive the i2s interface
 *         This version uses the new I2S driver (driver/i2s_std.h) of the arduino-esp32 core >= 3.0.0
 *         For core 2.x use i2s_interface_v2.h (legacy driver)
 *         The internal DAC is not supported anymore by the new I2S driver (I2S_NODAC)
 * @see https://youtu.be/zoajxQ7X0Gk
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#if ((defined SAMPLE_BUFFER_SIZE) && (defined SAMPLE_RATE))


#ifdef ML_SYNTH_INLINE_DECLARATION

void setup_i2s();
bool i2s_write_stereo_samples_i16(const int16_t *fl_sample, const int16_t *fr_sample, const int buffLen);
bool i2s_write_stereo_samples_buff(const float *fl_sample, const float *fr_sample, const int buffLen);
void i2s_read_stereo_samples_buff(float *fl_sample, float *fr_sample, const int buffLen);
void i2s_read_stereo_samples_buff(int16_t *fl_sample, int16_t *fr_sample, const int buffLen);

#endif /* ML_SYNTH_INLINE_DECLARATION */


#ifdef ML_SYNTH_INLINE_DEFINITION


#ifdef ESP32

#include <esp_idf_version.h>
#if ESP_IDF_VERSION_MAJOR < 5
#error i2s_interface_v3.h requires the new I2S driver (arduino-esp32 core >= 3.0.0), use i2s_interface_v2.h
#endif

#include <driver/i2s_std.h>


#ifdef I2S_NODAC
#ifndef SOC_I2S_SUPPORTS_DAC
#error internal dac not supported by your current configuration
/* this message appears in case you cannot use the I2S interface to push audio data to the internal DAC */
#endif
#endif


#ifdef OUTPUT_SAW_TEST
int16_t sampleDataI16SawTest[SAMPLE_BUFFER_SIZE];
float sampleDataFSawTest[SAMPLE_BUFFER_SIZE];
#endif


#ifdef SAMPLE_SIZE_16BIT
#define BYTES_PER_SAMPLE    2
#define MULTIPLIER_CONST    (16383.0f)
#define SAMPLE_DATA_TYPE    int16_t
#endif
#ifdef SAMPLE_SIZE_32BIT
#define BYTES_PER_SAMPLE    4
#define MULTIPLIER_CONST    (1073741823.0f)
#define SAMPLE_DATA_TYPE    int32_t
#endif


#ifndef I2S_MCLK_PIN
#define I2S_MCLK_PIN I2S_GPIO_UNUSED
#endif

#ifndef I2S_DIN_PIN
#define I2S_DIN_PIN I2S_GPIO_UNUSED
#endif


#ifndef I2S_OVERSAMPLE
#define I2S_OVERSAMPLE  1
#endif


#ifdef SAMPLE_SIZE_32BIT
union sampleTUNT
{
    uint64_t sample;
    int32_t ch[2];
};
#endif

#ifdef SAMPLE_SIZE_24BIT
union sampleTUNT
{
    int32_t ch[2];
    uint8_t bytes[8];
};
#endif

#ifdef SAMPLE_SIZE_16BIT
union sampleTUNT
{
    uint32_t sample;
    int16_t ch[2];
};
#endif

/*
 * no dac not tested within this code
 * - it has the purpose to generate a quasy analog signal without a DAC
 */
//#define I2S_NODAC


#ifndef I2S_PORT_NUMBER
#define I2S_PORT_NUMBER I2S_NUM_0
#endif

const i2s_port_t i2s_port_number = I2S_PORT_NUMBER;

static i2s_chan_handle_t i2s_tx_handle = NULL;
static i2s_chan_handle_t i2s_rx_handle = NULL;

/*
 * please refer to https://www.hackster.io/janost/audio-hacking-on-the-esp8266-fa9464#toc-a-simple-909-drum-synth-0
 * for the following implementation
 */

bool i2s_write_sample_32ch2(uint64_t sample)
{
    static size_t bytes_written = 0;
    i2s_channel_write(i2s_tx_handle, (const char *)&sample, 8, &bytes_written, portMAX_DELAY);

    if (bytes_written > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef SAMPLE_SIZE_24BIT

bool i2s_write_sample_24ch2(uint8_t *sample);

bool i2s_write_sample_24ch2(uint8_t *sample)
{
    static size_t bytes_written1 = 0;
    static size_t bytes_written2 = 0;
    i2s_channel_write(i2s_tx_handle, (const char *)&sample[1], 3, &bytes_written1, portMAX_DELAY);
    i2s_channel_write(i2s_tx_handle, (const char *)&sample[5], 3, &bytes_written2, portMAX_DELAY);

    if ((bytes_written1 + bytes_written2) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif

bool i2s_write_stereo_samples(const float *fl_sample, const float *fr_sample)
{
    static union sampleTUNT sampleDataU;

    /*
     * using RIGHT_LEFT format
     */
    sampleDataU.ch[0] = (SAMPLE_DATA_TYPE)(*fr_sample * MULTIPLIER_CONST);
    sampleDataU.ch[1] = (SAMPLE_DATA_TYPE)(*fl_sample * MULTIPLIER_CONST);

    size_t bytes_written = 0;

    i2s_channel_write(i2s_tx_handle, (const char *)&sampleDataU.sample, 2 * BYTES_PER_SAMPLE, &bytes_written, portMAX_DELAY);

    if (bytes_written > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef SAMPLE_SIZE_16BIT
bool i2s_write_stereo_samples_i16(const int16_t *fl_sample, const int16_t *fr_sample)
{
    size_t bytes_written = 0;

    static union sampleTUNT sampleDataU;

    sampleDataU.ch[0] = *fl_sample;
    sampleDataU.ch[1] = *fr_sample;
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    i2s_channel_write(i2s_tx_handle, (const char *)&sampleDataU.sample, 4, &bytes_written, portMAX_DELAY);
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif
    if (bytes_written > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif

#ifdef SAMPLE_SIZE_16BIT
bool i2s_write_stereo_samples_i16(const int16_t *fl_sample, const int16_t *fr_sample, const int buffLen)
{
    size_t bytes_written = 0;

    static union sampleTUNT sampleDataU[SAMPLE_BUFFER_SIZE];

#ifdef OUTPUT_SAW_TEST
    for (int n = 0; n < buffLen; n++)
    {
        sampleDataU[n].ch[1] = sampleDataI16SawTest[n];
        sampleDataU[n].ch[0] = sampleDataI16SawTest[n];
    }
#else
    for (int n = 0; n < buffLen; n++)
    {
        sampleDataU[n].ch[1] = fr_sample[n];
        sampleDataU[n].ch[0] = fl_sample[n];
    }
#endif

#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    i2s_channel_write(i2s_tx_handle, (const char *)&sampleDataU[0].sample, 2 * BYTES_PER_SAMPLE * buffLen, &bytes_written, portMAX_DELAY);
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif

    if (bytes_written > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif

#ifdef SAMPLE_BUFFER_SIZE
bool i2s_write_stereo_samples_buff(const float *fl_sample, const float *fr_sample, const int buffLen)
{
    static union sampleTUNT sampleDataU[SAMPLE_BUFFER_SIZE];

    for (int n = 0; n < buffLen; n++)
    {
#ifdef ES8388_ENABLED
        /*
         * using LEFT_RIGHT format
         */
        sampleDataU[n].ch[1] = (SAMPLE_DATA_TYPE)(fr_sample[n] * MULTIPLIER_CONST);
        sampleDataU[n].ch[0] = (SAMPLE_DATA_TYPE)(fl_sample[n] * MULTIPLIER_CONST);
#else /* ES8388_ENABLED */
        /*
         * using RIGHT_LEFT format
         */
#ifdef SAMPLE_SIZE_16BIT
#ifdef I2S_NODAC
        float fr_sampl = fr_sample[n];
        float fl_sampl = fl_sample[n];

        /* shift signal */
        fr_sampl *= 0.5;
        fl_sampl *= 0.5;
        fr_sampl += 0.5f;
        fl_sampl += 0.5f;

        /* limit */
        fr_sampl = fr_sampl > 1.0f ? 1.0f : fr_sampl;
        fl_sampl = fl_sampl > 1.0f ? 1.0f : fl_sampl;
        fr_sampl = fr_sampl < 0.0f ? 0.0f : fr_sampl;
        fl_sampl = fl_sampl < 0.0f ? 0.0f : fl_sampl;

        /* convert */
        sampleDataU[n].ch[0] = uint16_t(fl_sampl * 0xFFFF); /* DAC_1 */
        sampleDataU[n].ch[1] = uint16_t(fr_sampl * 0xFFFF); /* DAC_2 */
#else
        sampleDataU[n].ch[0] = int16_t(fr_sample[n] * 16383.0f); /* some bits missing here */
        sampleDataU[n].ch[1] = int16_t(fl_sample[n] * 16383.0f);
#endif
#endif
#ifdef SAMPLE_SIZE_32BIT
        sampleDataU[n].ch[0] = (SAMPLE_DATA_TYPE)(fr_sample[n] * MULTIPLIER_CONST);
        sampleDataU[n].ch[1] = (SAMPLE_DATA_TYPE)(fl_sample[n] * MULTIPLIER_CONST);
#endif
#endif /* ES8388_ENABLED */
    }

#ifdef OUTPUT_SAW_TEST
    for (int n = 0; n < buffLen; n++)
    {
        sampleDataU[n].ch[1] = sampleDataI16SawTest[n];
        sampleDataU[n].ch[0] = sampleDataI16SawTest[n];
    }
#endif

    static size_t bytes_written = 0;

#ifdef CYCLE_MODULE_ENABLED
    calcCycleCountPre();
#endif
    i2s_channel_write(i2s_tx_handle, (const char *)&sampleDataU[0].sample, 2 * BYTES_PER_SAMPLE * buffLen, &bytes_written, portMAX_DELAY);
#ifdef CYCLE_MODULE_ENABLED
    calcCycleCount();
#endif

    if (bytes_written > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif /* #ifdef SAMPLE_BUFFER_SIZE */

void i2s_read_stereo_samples(float *fl_sample, float *fr_sample)
{
    static size_t bytes_read = 0;

    static union sampleTUNT sampleData;

    if (i2s_rx_handle == NULL)
    {
        *fl_sample = 0;
        *fr_sample = 0;
        return;
    }

    i2s_channel_read(i2s_rx_handle, (char *)&sampleData.sample, 4, &bytes_read, portMAX_DELAY);


    /*
     * using RIGHT_LEFT format
     */
    *fr_sample = ((float)sampleData.ch[0] * (5.5f / 65535.0f));
    *fl_sample = ((float)sampleData.ch[1] * (5.5f / 65535.0f));
}

#ifdef SAMPLE_BUFFER_SIZE
void i2s_read_stereo_samples_buff(int16_t *fl_sample, int16_t *fr_sample, const int buffLen)
{
#ifdef I2S_DIN_PIN
    static size_t bytes_read = 0;

    static union sampleTUNT sampleData[SAMPLE_BUFFER_SIZE];

    if (i2s_rx_handle == NULL)
    {
        for (int n = 0; n < buffLen; n++)
        {
            fr_sample[n] = 0;
            fl_sample[n] = 0;
        }
        return;
    }

    i2s_channel_read(i2s_rx_handle, (char *)&sampleData[0].sample, 2 * BYTES_PER_SAMPLE * buffLen, &bytes_read, portMAX_DELAY);


    for (int n = 0; n < buffLen; n++)
    {
        /*
         * using RIGHT_LEFT format
         */
        fr_sample[n] = ((float)sampleData[n].ch[0]);
        fl_sample[n] = ((float)sampleData[n].ch[1]);
    }
#endif
}

void i2s_read_stereo_samples_buff(float *fl_sample, float *fr_sample, const int buffLen)
{
#ifdef I2S_DIN_PIN
    static size_t bytes_read = 0;

    static union sampleTUNT sampleData[SAMPLE_BUFFER_SIZE];

    if (i2s_rx_handle == NULL)
    {
        for (int n = 0; n < buffLen; n++)
        {
            fr_sample[n] = 0;
            fl_sample[n] = 0;
        }
        return;
    }

    i2s_channel_read(i2s_rx_handle, (char *)&sampleData[0].sample, 2 * BYTES_PER_SAMPLE * buffLen, &bytes_read, 0);


    for (int n = 0; n < buffLen; n++)
    {
        /*
         * using RIGHT_LEFT format
         */

        fr_sample[n] = ((float)sampleData[n].ch[0] * (1.0f / MULTIPLIER_CONST));
        fl_sample[n] = ((float)sampleData[n].ch[1] * (1.0f / MULTIPLIER_CONST));
    }
#endif
}
#endif /* #ifdef SAMPLE_BUFFER_SIZE */

/*
 * i2s configuration
 */
#ifdef SAMPLE_SIZE_32BIT
#define I2S_DATA_BITS   I2S_DATA_BIT_WIDTH_32BIT
#endif
#ifdef SAMPLE_SIZE_24BIT
#define I2S_DATA_BITS   I2S_DATA_BIT_WIDTH_24BIT
#endif
#ifdef SAMPLE_SIZE_16BIT
#define I2S_DATA_BITS   I2S_DATA_BIT_WIDTH_16BIT
#endif

#if (defined I2S_BCLK_PIN) && (defined I2S_WCLK_PIN) && (defined I2S_DOUT_PIN)
void setup_i2s()
{
    /*
     * channel configuration (replaces dma_buf_count, dma_buf_len and tx_desc_auto_clear)
     */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(i2s_port_number, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 2;
    chan_cfg.dma_frame_num = SAMPLE_BUFFER_SIZE;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
    chan_cfg.auto_clear_after_cb = true;
#else
    chan_cfg.auto_clear = true;
#endif

    /*
     * rx channel is only required if a data in pin is defined
     */
    bool use_rx = (I2S_DIN_PIN != I2S_GPIO_UNUSED);

    esp_err_t err = i2s_new_channel(&chan_cfg, &i2s_tx_handle, use_rx ? &i2s_rx_handle : NULL);
    if (err != ESP_OK)
    {
        Serial.printf("i2s_new_channel failed: %d\n", err);
        return;
    }

    /*
     * standard mode configuration
     * (replaces sample_rate, bits_per_sample, communication_format, mclk_multiple, bits_per_chan, use_apll and the pins)
     */
#ifdef MAX_98357A_ENABLED
    i2s_std_slot_config_t slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BITS, I2S_SLOT_MODE_STEREO);
    slot_cfg.ws_width = (uint32_t)I2S_DATA_BITS; /* long frame sync, like I2S_COMM_FORMAT_STAND_PCM_LONG */
#else
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BITS, I2S_SLOT_MODE_STEREO);
#endif

    i2s_std_config_t std_cfg =
    {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE * I2S_OVERSAMPLE),
        .slot_cfg = slot_cfg,
        .gpio_cfg = {
            .mclk = (gpio_num_t)I2S_MCLK_PIN,
            .bclk = (gpio_num_t)I2S_BCLK_PIN,
            .ws = (gpio_num_t)I2S_WCLK_PIN,
            .dout = (gpio_num_t)I2S_DOUT_PIN,
            .din = (gpio_num_t)I2S_DIN_PIN,
        },
    };

#ifdef I2S_USE_APLL
    std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
#endif
#ifdef I2S_MCLK_MULTIPLE_DEFAULT
    std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT;
#elif (defined SAMPLE_SIZE_24BIT)
    std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384; /* 24 bit requires a multiple of 3 */
#else
    std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
#endif

    Serial.printf("i2s_configuration:\n");
    Serial.printf("\tdma_desc_num: %d\n", (int)chan_cfg.dma_desc_num);
    Serial.printf("\tdma_frame_num: %d\n", (int)chan_cfg.dma_frame_num);
    Serial.printf("\tsample_rate_hz: %d\n", (int)std_cfg.clk_cfg.sample_rate_hz);
    Serial.printf("\tmclk_multiple: %d\n", (int)std_cfg.clk_cfg.mclk_multiple);
    Serial.printf("\tdata_bit_width: %d\n", (int)std_cfg.slot_cfg.data_bit_width);
    Serial.printf("\tslot_bit_width: %d\n", (int)std_cfg.slot_cfg.slot_bit_width);
    Serial.printf("\trx enabled: %d\n", use_rx);

    err = i2s_channel_init_std_mode(i2s_tx_handle, &std_cfg);
    if (err != ESP_OK)
    {
        Serial.printf("i2s_channel_init_std_mode (tx) failed: %d\n", err);
        return;
    }
    if (use_rx)
    {
        err = i2s_channel_init_std_mode(i2s_rx_handle, &std_cfg);
        if (err != ESP_OK)
        {
            Serial.printf("i2s_channel_init_std_mode (rx) failed: %d\n", err);
            return;
        }
    }

    /*
     * start the channels (replaces i2s_start)
     */
    i2s_channel_enable(i2s_tx_handle);
    if (use_rx)
    {
        i2s_channel_enable(i2s_rx_handle);
    }

#ifdef ES8388_ENABLED
#ifdef PIN_CTRL
    REG_WRITE(PIN_CTRL, 0xFFFFFFF0);
#endif
#ifdef FUNC_GPIO0_CLK_OUT1
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
#endif
#endif

    Serial.printf("I2S_NUM_%d configured using following pins:\n", (int)i2s_port_number);
    Serial.printf("    BCLK,BCK: %d\n", (int)I2S_BCLK_PIN);
    Serial.printf("    WCLK,LCK: %d\n", (int)I2S_WCLK_PIN);
    Serial.printf("    DOUT: %d\n", (int)I2S_DOUT_PIN);
    Serial.printf("    DIN: %d\n", (int)I2S_DIN_PIN);
    Serial.printf("    MCLK: %d\n", (int)I2S_MCLK_PIN);

#ifdef OUTPUT_SAW_TEST
    /*
       * base frequency: SAMPLE_FREQ / SAMPLE_BUFFER_SIZE
       * for example: Fs : 44100Hz, Lsb = 48 -> Freq: 918.75 Hz
       */
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        float saw = ((float)i * 2.0f) / ((float)SAMPLE_BUFFER_SIZE);
        sampleDataFSawTest[i] = saw;
        saw -= 1;
        saw *= 0x7FFF;
        sampleDataI16SawTest[i] = saw;
    }
#endif
}
#endif /* (defined I2S_BCLK_PIN) && (defined I2S_WCLK_PIN) && (defined I2S_DOUT_PIN) */

#endif /* ESP32 */


#endif /* SAMPLE_RATE */


#endif /* ML_SYNTH_INLINE_DEFINITION */

