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
 * @file wm8960.h
 * @author Marcel Licence
 * @date 30.03.2024
 *
 * @brief This module is used to initialize the WM8960 codec
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef ML_SYNTH_INLINE_DECLARATION
#ifdef WM8960_ENABLED


bool WM8960_Setup(void);


#endif // WM8960_ENABLED
#endif // ML_SYNTH_INLINE_DECLARATION


#ifdef ML_SYNTH_INLINE_DEFINITION
#ifdef WM8960_ENABLED


//#include <WM8960.h> /* requires library wm8978-esp32 from https://github.com/CelliesProjects/wm8978-esp32 */
#include <Wire.h>


static int ES8960_Write_Reg(uint8_t reg, uint16_t dat);
static void setVol(int vol);
static void hp_spk(void);
#if 0
static void dacToHp(void);
static void ES8960_Init2(void);
#endif
static uint16_t WM8960_ReadRegU16(uint8_t reg);
static uint8_t WM8960_ReadReg(uint8_t reg);


bool WM8960_Setup(void)
{
    pinMode(I2C_SDA, INPUT_PULLUP);
    pinMode(I2C_SCL, INPUT_PULLUP);
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(1000);
    pinMode(I2C_SDA, INPUT_PULLUP);
    pinMode(I2C_SCL, INPUT_PULLUP);

    {
        uint16_t address = WM8960_ADDRESS;
        byte r_error;
        Wire.beginTransmission(address);
        r_error = Wire.endTransmission();

        if (r_error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println("  !");

            //nDevices++;
        }
        else if (r_error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
        else
        {
            Serial.printf("No device found at address 0x%02x\n", address);
        }
    }


    for (uint8_t n = 0; n < 56; n++)
    {
        Serial.printf("%02x: %04x\n", n, WM8960_ReadRegU16(n));
    }


    //ES8960_Init2();
    hp_spk();
    Serial.println("1 SPK");
    delay(1000);
    setVol(6);
    Serial.println("1 vol");
    Serial.println(digitalRead(0));

    //Wire.setSDA(I2C_SDA);
    //Wire.setSCL(I2C_SCL);


    Serial.printf("%02x: %02x\n", 0x19, WM8960_ReadReg(0x19));
    Serial.printf("%02x: %02x\n", 0x05, WM8960_ReadReg(0x05));

    return true;
}



static int ES8960_Write_Reg(uint8_t reg, uint16_t dat)
{
    uint8_t res, I2C_Data[2];

    I2C_Data[0] = (reg << 1) | ((uint8_t)((dat >> 8) & 0x0001)); //RegAddr
    I2C_Data[1] = (uint8_t)(dat & 0x00FF); //RegValue


    //Wire.setClock(10000);
    Wire.beginTransmission(WM8960_ADDRESS); // transmit to device lsb=0 => write
    Wire.write(I2C_Data, 2); // buffer 1 byte lsb of val1
    res = Wire.endTransmission(true); // transmit buffer and then stop

    if (res != 0)
    {
        printf("write error %d", reg);
    }

    return res;
}


////////////////////////////////////////////////////////////////////////////////////////
// setVol
// vol 0=>10
////////////////////////////////////////////////////////////////////////////////////////
static void setVol(int vol)
{
    int val;
    if (vol > 10)
    {
        return;
    }
    if (vol < 0)
    {
        return;
    }
    val = vol * 5 + 70;
    if (vol == 0)
    {
        val = 0;
    }

    printf("%x \n", val);
    val = val | 0x100;

    int st;
    do
    {
        st = 0;

        printf("Vol setup raw %x \n", val);
        st += ES8960_Write_Reg(40, val);
        delay(10);
        st += ES8960_Write_Reg(41, val);
        delay(10);
        st += ES8960_Write_Reg(2, val);
        delay(10);
        st += ES8960_Write_Reg(3, val);
        delay(10);
    }
    while (st != 0);
}


static void hp_spk(void)
{

    int st;

    do
    {
        st = 0;

        st += ES8960_Write_Reg(15, 0x00);
        usleep(10000);
        //Power
        st += ES8960_Write_Reg(25, 0x1FC);
        usleep(500);
        st += ES8960_Write_Reg(26, 0x1F9);
        usleep(500);
        st += ES8960_Write_Reg(47, 0x03C);
        usleep(10000);
        //Clock PLL
        st += ES8960_Write_Reg(4, 0x001);
        usleep(10000);
        st += ES8960_Write_Reg(52, 0x027);
        usleep(10000);
        st += ES8960_Write_Reg(53, 0x086);
        usleep(10000);
        st += ES8960_Write_Reg(54, 0x0C2);
        usleep(10000);
        st += ES8960_Write_Reg(55, 0x026);
        usleep(10000);
        //ADC/DAC
        st += ES8960_Write_Reg(5, 0x000);
        usleep(10000);
        st += ES8960_Write_Reg(7, 0x002);
        usleep(10000);
        //Noise control
        st += ES8960_Write_Reg(20, 0x0F9);
        usleep(10000);
        //OUT1 volume
        st += ES8960_Write_Reg(2, 0x16F);
        usleep(500);
        st += ES8960_Write_Reg(3, 0x16F);
        usleep(500);
        //SPK volume
        st += ES8960_Write_Reg(40, 0x17F);
        usleep(500);
        st += ES8960_Write_Reg(41, 0x17F);
        usleep(500);
        // OUTPUTS 0 XX11 0111 class D amp
        // XX = 00 off , 11 speaker on R and L
        st += ES8960_Write_Reg(49, 0x0FF);


        usleep(10000);
        st += ES8960_Write_Reg(10, 0x1FF);
        usleep(10000);
        st += ES8960_Write_Reg(11, 0x1FF);
        usleep(10000);

        st += ES8960_Write_Reg(34, 0x100);
        usleep(10000);

        st += ES8960_Write_Reg(37, 0x100);
        usleep(10000);


        if (st == 0)
        {
            printf("init WS8960 OK....\n");
        }
        else
        {
            // printf(st);
            printf("init WS8960 failed...\n");
            delay(1000);
        }

    }
    while (st != 0);


}

#if 0
static void dacToHp(void)
{
    int st;
    do
    {
        st = 0;
        st += ES8960_Write_Reg(15, 0x00); //RESET
        delay(10);
        //ADC/DAC
        st += ES8960_Write_Reg(5, 0x004); //DAC att=0dB, ADC pol. not inverted, DAC mute, no de-emphasis
        delay(10);
        st += ES8960_Write_Reg(7, 0x002); //ADC ch normal, BCLK nrmal, slave mode, DAC ch normal,LRCK not inverted, 16bits, I2S
        delay(10);


        st += ES8960_Write_Reg(25, 0x0C0); // VMID = 50k VREF enabled
        delay(10);
        st += ES8960_Write_Reg(26, 0x1E0); // enable DAC L/R LOUT1 ROUT1
        delay(10);
        st += ES8960_Write_Reg(47, 0x00C); // enable output mixer L/R
        delay(10);
        st += ES8960_Write_Reg(34, 0x100); // L DAC to L output mixer
        delay(10);
        st += ES8960_Write_Reg(37, 0x100); // R DAC to R output mixer
        delay(10);
        st += ES8960_Write_Reg(2, 0x179); // LOUT1 volume
        delay(10);
        st += ES8960_Write_Reg(3, 0x179); // ROUT1 volume
        delay(10);
        st += ES8960_Write_Reg(5, 0x000); // unmute DAC
        delay(10);

        if (st == 0)
        {
            printf("init WS8960 OK....\n");
        }
        else
        {
            printf("init WS8960 failed...\n");
            delay(1000);
        }
    }
    while (st != 0);
}
#endif

#if 0
static void ES8960_Init2(void)
{


    int st;

    do
    {
        st = 0;

        st += ES8960_Write_Reg(15, 0x00);
        usleep(10000);
        //Power
        st += ES8960_Write_Reg(25, 0x1FC);
        usleep(500);
        st += ES8960_Write_Reg(26, 0x1F9);
        usleep(500);
        st += ES8960_Write_Reg(47, 0x03C);
        usleep(10000);
        //Clock PLL
        st += ES8960_Write_Reg(4, 0x001);
        usleep(10000);
        st += ES8960_Write_Reg(52, 0x027);
        usleep(10000);
        st += ES8960_Write_Reg(53, 0x086);
        usleep(10000);
        st += ES8960_Write_Reg(54, 0x0C2);
        usleep(10000);
        st += ES8960_Write_Reg(55, 0x026);
        usleep(10000);
        //ADC/DAC
        st += ES8960_Write_Reg(5, 0x000);
        usleep(10000);
        st += ES8960_Write_Reg(7, 0x002);
        usleep(10000);
        //Noise control
        st += ES8960_Write_Reg(20, 0x0F9);
        usleep(10000);
        //OUT1 volume
        st += ES8960_Write_Reg(2, 0x16F);
        usleep(500);
        st += ES8960_Write_Reg(3, 0x16F);
        usleep(500);
        //SPK volume
        st += ES8960_Write_Reg(40, 0x17F);
        usleep(500);
        st += ES8960_Write_Reg(41, 0x17F);
        usleep(500);
        // OUTPUTS
        // st += ES8960_Write_Reg(49, 0x0F7);
        usleep(10000);
        st += ES8960_Write_Reg(10, 0x1FF);
        usleep(10000);
        st += ES8960_Write_Reg(11, 0x1FF);
        usleep(10000);

        st += ES8960_Write_Reg(34, 0x100);
        usleep(10000);

        st += ES8960_Write_Reg(37, 0x100);
        usleep(10000);


        if (st == 0)
        {
            printf("init WS8960 OK....\n");
        }
        else
        {
            // printf(st);
            printf("init WS8960 failed...\n");
            delay(1000);
        }

    }
    while (st != 0);


}
#endif

static uint16_t WM8960_ReadRegU16(uint8_t reg)
{
    Wire.beginTransmission(WM8960_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    uint16_t val = 0u;
    int retRead = Wire.requestFrom(uint16_t(WM8960_ADDR), uint8_t(2), true);
    if (2 == retRead)
    {
        val = Wire.read();
        val <<= 8;
        val += Wire.read();
    }
    else
    {
        Serial.printf("WM8960_ReadRegU16 failed, %d!\n", retRead);
    }

    return val;
}


static uint8_t WM8960_ReadReg(uint8_t reg)
{
    Wire.beginTransmission(WM8960_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    uint8_t val = 0u;
    if (1 == Wire.requestFrom(uint16_t(WM8960_ADDR), uint8_t(1), true))
    {
        val = Wire.read();
    }

    return val;
}



#endif // WM8960_ENABLED
#endif // ML_SYNTH_INLINE_DEFINITION
