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
 * @file RP2040_AudioPwm.h
 * @author Marcel Licence
 * @date 01.12.2022
 *
 * @brief   This is a PWM based PWM audio driver for the RP2040
 *          It supports stereo sound and requires a passive filter for DC offset removal
 *
 * @see Short demonstration: https://youtu.be/b_vXfD0tSy4
 */


#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)


#include "RP2040_AudioPwm.h"

#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/pwm.h>


static void RP2040_Audio_Pwm_dma_irq_handler();
static void RP2040_Audio_Pwm_Start_Audio(uint32_t *audio_buffer_a, uint32_t *audio_buffer_b, uint8_t len);
static void RP2040_Audio_Pwm_Stop_Audio();


static int audio_dma_ch = 0;
static unsigned int audio_pwm_slice_num = 0;

static volatile bool bufferReady = false;
static uint32_t *lastRead = NULL;
static uint32_t *lastRead2 = NULL;
static uint8_t buffSize = 0;


void RP2040_Audio_Pwm_Init(uint8_t gpio_pin_number, float sample_rate, uint32_t *audio_buffer_a, uint32_t *audio_buffer_b, uint8_t len)
{
    /*************************/
    /* Configure PWM output. */
    /*************************/
    gpio_set_function(gpio_pin_number, GPIO_FUNC_PWM);
    gpio_set_function(gpio_pin_number + 1, GPIO_FUNC_PWM);

    audio_pwm_slice_num = pwm_gpio_to_slice_num(gpio_pin_number);


    /*
     * default wrap is 16 bit
     * 125.000.000Hz / (1<<16) -> ~ 1907Hz
     */

    pwm_config config = pwm_get_default_config();
    pwm_init(audio_pwm_slice_num, &config, true);

    /* setup for 48000 sample rate */
    float pwm_wrap_value = ((float)125000000 / (float)sample_rate);
    pwm_set_wrap(audio_pwm_slice_num, pwm_wrap_value);

    RP2040_Audio_Pwm_Start_Audio(audio_buffer_a, audio_buffer_b, len);
}


bool RP2040_Audio_Pwm_BufferReady()
{
    return bufferReady;
}

uint32_t *RP2040_Audio_Pwm_getFreeBuff()
{
    bufferReady = false;
    return lastRead2;
}

static void RP2040_Audio_Pwm_dma_irq_handler()
{
#if 1
    dma_hw->ints0 = 1u << audio_dma_ch;


    dma_start_channel_mask(1 << audio_dma_ch);

    dma_channel_set_read_addr(audio_dma_ch, lastRead2, true);
#endif

    uint32_t *temp = lastRead2;
    lastRead2 = lastRead;
    lastRead = temp;

    bufferReady = true;
}

static void RP2040_Audio_Pwm_Stop_Audio()
{
    if ((audio_dma_ch != 0) && dma_channel_is_busy(audio_dma_ch))
    {
        dma_channel_abort(audio_dma_ch);
    }
}

static void RP2040_Audio_Pwm_Start_Audio(uint32_t *audio_buffer_a, uint32_t *audio_buffer_b, uint8_t len)
{
    buffSize = len;
    bufferReady = false;

    lastRead = audio_buffer_a;
    lastRead2 = audio_buffer_b;

    dma_channel_config audio_dma_ch_cfg;

    if (!audio_dma_ch)
    {
        audio_dma_ch = dma_claim_unused_channel(true);
    }

    /*********************************************/
    /* Stop playing audio if DMA already active. */
    /*********************************************/
    RP2040_Audio_Pwm_Stop_Audio();

    /****************************************************/
    /* Don't start playing audio if DMA already active. */
    /****************************************************/
    if (!dma_channel_is_busy(audio_dma_ch))
    {
        /****************************************************/
        /* Configure state machine DMA from WAV PWM memory. */
        /****************************************************/
        audio_dma_ch_cfg = dma_channel_get_default_config(audio_dma_ch);

        channel_config_set_read_increment(&audio_dma_ch_cfg, true);
        channel_config_set_write_increment(&audio_dma_ch_cfg, false);
        channel_config_set_transfer_data_size(&audio_dma_ch_cfg, DMA_SIZE_32);

        /* Select a transfer request signal in a channel configuration object */
        channel_config_set_dreq(&audio_dma_ch_cfg, pwm_get_dreq(audio_pwm_slice_num));


        /*
         * using 8 gives 344 base frequency
         * using 7 bits -> 689 hz
         */
        volatile void *pwm_counter_compare_addr = &pwm_hw->slice[audio_pwm_slice_num].cc; /* cc = counter compare */

        dma_channel_configure(
            audio_dma_ch, /* DMA channel */
            &audio_dma_ch_cfg, /* Pointer to DMA config structure */
            pwm_counter_compare_addr, /* Initial write address */
            & (audio_buffer_a[0]), /* Initial read address */
            buffSize, /* Number of transfers to perform */
            false /* True to start the transfer immediately */
        );

        dma_channel_set_irq0_enabled(audio_dma_ch, true);
        irq_set_exclusive_handler(DMA_IRQ_0, RP2040_Audio_Pwm_dma_irq_handler);
        irq_set_enabled(DMA_IRQ_0, true);

        /**********************/
        /* Start WAV PWM DMA. */
        /**********************/
        dma_hw->ints0 = (1 << audio_dma_ch);
        dma_start_channel_mask(1 << audio_dma_ch);
    }
}

#endif /* #if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040) */
