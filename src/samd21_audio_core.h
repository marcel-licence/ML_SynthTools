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
 * @file samd21_audio_core.ino
 * @author Marcel Licence
 * @date 22.10.2021
 *
 * @brief This file contains the audio core code (setup buffer and DMA for sample to DAC transmission)
 *  It might look like a bit of magic but this mechanism rescues a lot of processing power for the important things
 *
 * @see https://aykevl.nl/2019/09/samd21-dma
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DECLARATION


bool SAMD21_Synth_Process(void(*proc)(uint16_t *buff, size_t len));


#endif  /* ML_SYNTH_INLINE_DECLARATION */


#ifdef ML_SYNTH_INLINE_DEFINITION


#ifdef ARDUINO_SEEED_XIAO_M0


void SAMD21_Synth_DmaInit();
void SAMD21_Synth_TimerInit();
void SAMD21_Synth_DACInit();
bool SAMD21_Synth_DmaDone();
void DMAC_Handler();


// DMA descriptor list entry point (and writeback buffer) per channel
__attribute__((__aligned__(16))) static DmacDescriptor ///< 128 bit alignment
_descriptor[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR,  ///< Descriptor table
            _writeback[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR;   ///< Writeback table

uint16_t audio_buffer[2][SAMPLE_BUFFER_SIZE];


volatile DmacDescriptor wrb[4] __attribute__((aligned(16)));
DmacDescriptor dmaDescriptorArray[4] __attribute__((aligned(16)));

#define DMA_CHANNEL 0
#define DMA_TRIGGER TC5_DMAC_ID_OVF // TC5_DMAC_ID_MC_0

uint8_t complete = 1;
uint8_t bufferRdy[2] = {0, 0};

void SAMD21_Synth_DmaInit()
{
    memset(audio_buffer, 0, sizeof(audio_buffer));

    PM->AHBMASK.reg |= PM_AHBMASK_DMAC ;
    PM->APBBMASK.reg |= PM_APBBMASK_DMAC ;

    DMAC->BASEADDR.reg = (uint32_t)dmaDescriptorArray;
    DMAC->WRBADDR.reg = (uint32_t)wrb;

    DMAC->CHID.reg = DMAC_CHID_ID(DMA_CHANNEL);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << DMA_CHANNEL));
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) |
                        DMAC_CHCTRLB_TRIGSRC(DMA_TRIGGER) | DMAC_CHCTRLB_TRIGACT_BEAT;

    dmaDescriptorArray[0].DESCADDR.reg = (uint32_t) 0; /* -> run to dma complete */
    dmaDescriptorArray[0].SRCADDR.reg = (uint32_t)audio_buffer[0] + sizeof(audio_buffer[0]);
    dmaDescriptorArray[0].DSTADDR.reg = (uint32_t)&DAC->DATA.reg;
    dmaDescriptorArray[0].BTCNT.reg = (uint16_t)(SAMPLE_BUFFER_SIZE);
    dmaDescriptorArray[0].BTCTRL.reg =
        DMAC_BTCTRL_BLOCKACT_NOACT | DMAC_BTCTRL_EVOSEL_DISABLE | DMAC_BTCTRL_STEPSIZE_X1 | DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID | DMAC_BTCTRL_STEPSEL_SRC;

    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);

    DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
    NVIC_EnableIRQ(DMAC_IRQn);

    DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
}

void SAMD21_Synth_TimerInit()
{
    /*
     * SetupTimer
     */

    PM->APBCMASK.reg |= PM_APBCMASK_TC5;

    TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
    while (TC5->COUNT16.CTRLA.bit.SWRST);

    Serial.printf("SetupTimer\n");
    delay(100);

    TC5->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

    TC5->COUNT16.CTRLBCLR.reg = TC_CTRLBCLR_MASK;

    TC5->COUNT16.CC[0].reg = (uint16_t)(SystemCoreClock / SAMPLE_RATE);
    while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
}

void SAMD21_Synth_DACInit()
{
    Serial.printf("Setup DAC\n");
    delay(100);

    PM->APBCMASK.reg |= PM_APBCMASK_DAC;
    /*
     * Control A
     */
    DAC->CTRLA.reg = DAC_CTRLA_SWRST;
    while (1 == DAC->STATUS.bit.SYNCBUSY);

    DAC->CTRLA.reg = DAC_CTRLA_ENABLE;
    while (1 == DAC->STATUS.bit.SYNCBUSY);

    /*
     * Control B
     *
     * - Internal voltage reference
     */
    DAC->CTRLB.reg = DAC_CTRLB_IOEN | DAC_CTRLB_EOEN | DAC_CTRLB_REFSEL_INT1V | DAC_CTRLB_BDWP;
    while (1 == DAC->STATUS.bit.SYNCBUSY);

    /*
     * Event Control
     */
    DAC->EVCTRL.reg = DAC_EVCTRL_STARTEI | DAC_EVCTRL_EMPTYEO;
    while (1 == DAC->STATUS.bit.SYNCBUSY);

    /*
     * setup interrupts
     */
    DAC->INTFLAG.reg = DAC_INTFLAG_EMPTY; /* clear pending flags */
    while (1 == DAC->STATUS.bit.SYNCBUSY);

    DAC->INTENSET.reg = DAC_INTENSET_EMPTY;
    while (1 == DAC->STATUS.bit.SYNCBUSY);

    DAC->CTRLA.reg = DAC_CTRLA_ENABLE;
    while (1 == DAC->STATUS.bit.SYNCBUSY);
}

void SAMD21_Synth_Init()
{
    PM->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;

    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
    while (GCLK->STATUS.bit.SYNCBUSY);

    SAMD21_Synth_DmaInit();
    SAMD21_Synth_TimerInit();
    SAMD21_Synth_DACInit();
}

bool SAMD21_Synth_Process(void(*proc)(uint16_t *buff, size_t len))
{
    bool ret = false;
    if (bufferRdy[0])
    {
        bufferRdy[0] = 0;
        proc(audio_buffer[0], SAMPLE_BUFFER_SIZE);
        if (bufferRdy[1])
        {
            Serial.printf("UDR 0\n"); /* buffer underrun */
        }
        ret = true;
    }
    if (bufferRdy[1])
    {
        bufferRdy[1] = 0;
        proc(audio_buffer[1], SAMPLE_BUFFER_SIZE);
        if (bufferRdy[0])
        {
            Serial.printf("UDR 1\n"); /* buffer underrun */
        }
        ret = true;
    }
    return ret;
}

volatile bool dmaComplete = false;

bool SAMD21_Synth_DmaDone()
{
    bool ret = dmaComplete;
    dmaComplete = false;
    return ret;
}

void DMAC_Handler()
{
    dmaComplete = true;
    if (DMAC->CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL)
    {
        if (dmaDescriptorArray[0].SRCADDR.reg == (uint32_t)audio_buffer[0] + sizeof(audio_buffer[0]))
        {
            dmaDescriptorArray[0].SRCADDR.reg = (uint32_t)audio_buffer[1] + sizeof(audio_buffer[1]);
            DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
            bufferRdy[0] = complete;
        }
        else
        {
            dmaDescriptorArray[0].SRCADDR.reg = (uint32_t)audio_buffer[0] + sizeof(audio_buffer[0]);
            DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
            bufferRdy[1] = complete;
        }
    }

    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_MASK; /* clear flag to avoid retrigger */
}

#endif

#endif /* ML_SYNTH_INLINE_DEFINITION */
