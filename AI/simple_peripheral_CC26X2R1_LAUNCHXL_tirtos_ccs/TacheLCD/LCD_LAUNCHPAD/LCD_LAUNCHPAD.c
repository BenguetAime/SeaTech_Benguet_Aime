/*
 * LCD_LAUNCHPAD.c
 *
 *  Created on: 9 févr. 2020
 *      Author: sebas
 */

#include <TacheLCD/SPISMIOT/SPISMIOT.h>
#include <TacheLCD/LCD_LAUNCHPAD/LCD_LAUNCHPAD.h>
//#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>

#define LCD_ASCII_OFFSET 0x20 //0x20, ASCII character for Space, The font table starts with this character

//unsigned char ASCII_FIVE[][8]  = {
//    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //
//    {0x00,0x5f,0x00,0x00,0x00,0x00,0x00,0x00}, // !
//    {0x00,0x03,0x00,0x03,0x00,0x00,0x00,0x00}, // "
//    {0x0a,0x1f,0x0a,0x1f,0x0a,0x00,0x00,0x00}, // #
//    {0x24,0x2a,0x2a,0x7f,0x2a,0x2a,0x12,0x00}, // $
//    {0x00,0x47,0x25,0x17,0x08,0x74,0x52,0x71}, // %
//    {0x00,0x36,0x49,0x49,0x49,0x41,0x41,0x38}, // &
//    {0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00}, // '
//    {0x00,0x3e,0x41,0x00,0x00,0x00,0x00,0x00}, // (
//    {0x41,0x3e,0x00,0x00,0x00,0x00,0x00,0x00}, // )
//    {0x04,0x15,0x0e,0x15,0x04,0x00,0x00,0x00}, // *
//    {0x08,0x08,0x3e,0x08,0x08,0x00,0x00,0x00}, // +
//    {0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00}, // ,
//    {0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x00}, // -
//    {0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // .
//    {0x40,0x20,0x10,0x08,0x04,0x02,0x01,0x00}, // /
//    {0x00,0x3e,0x61,0x51,0x49,0x45,0x43,0x3e}, // 0
//    {0x00,0x00,0x01,0x01,0x7e,0x00,0x00,0x00}, // 1
//    {0x00,0x71,0x49,0x49,0x49,0x49,0x49,0x46}, // 2
//    {0x41,0x49,0x49,0x49,0x49,0x49,0x36,0x00}, // 3
//    {0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x7f}, // 4
//    {0x00,0x4f,0x49,0x49,0x49,0x49,0x49,0x31}, // 5
//    {0x00,0x3e,0x49,0x49,0x49,0x49,0x49,0x30}, // 6
//    {0x01,0x01,0x01,0x01,0x01,0x01,0x7e,0x00}, // 7
//    {0x00,0x36,0x49,0x49,0x49,0x49,0x49,0x36}, // 8
//    {0x00,0x06,0x49,0x49,0x49,0x49,0x49,0x3e}, // 9
//    {0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // :
//    {0x40,0x34,0x00,0x00,0x00,0x00,0x00,0x00}, // ;
//    {0x08,0x14,0x22,0x00,0x00,0x00,0x00,0x00}, // <
//    {0x14,0x14,0x14,0x14,0x14,0x00,0x00,0x00}, // =
//    {0x22,0x14,0x08,0x00,0x00,0x00,0x00,0x00}, // >
//    {0x00,0x06,0x01,0x01,0x59,0x09,0x09,0x06}, // ?
//    {0x00,0x3e,0x41,0x5d,0x55,0x5d,0x51,0x5e}, // @
//    {0x00,0x7e,0x01,0x09,0x09,0x09,0x09,0x7e}, // A
//    {0x00,0x7f,0x41,0x49,0x49,0x49,0x49,0x36}, // B
//    {0x00,0x3e,0x41,0x41,0x41,0x41,0x41,0x22}, // C
//    {0x00,0x7f,0x41,0x41,0x41,0x41,0x41,0x3e}, // D
//    {0x00,0x3e,0x49,0x49,0x49,0x49,0x49,0x41}, // E
//    {0x00,0x7e,0x09,0x09,0x09,0x09,0x09,0x01}, // F
//    {0x00,0x3e,0x41,0x49,0x49,0x49,0x49,0x79}, // G
//    {0x00,0x7f,0x08,0x08,0x08,0x08,0x08,0x7f}, // H
//    {0x00,0x7f,0x00,0x00,0x00,0x00,0x00,0x00}, // I
//    {0x00,0x38,0x40,0x40,0x41,0x41,0x41,0x3f}, // J
//    {0x00,0x7f,0x08,0x08,0x08,0x0c,0x0a,0x71}, // K
//    {0x00,0x3f,0x40,0x40,0x40,0x40,0x40,0x40}, // L
//    {0x00,0x7e,0x01,0x01,0x7e,0x01,0x01,0x7e}, // M
//    {0x00,0x7e,0x01,0x01,0x3e,0x40,0x40,0x3f}, // N
//    {0x00,0x3e,0x41,0x41,0x41,0x41,0x41,0x3e}, // O
//    {0x00,0x7e,0x09,0x09,0x09,0x09,0x09,0x06}, // P
//    {0x00,0x3e,0x41,0x41,0x71,0x51,0x51,0x7e}, // Q
//    {0x00,0x7e,0x01,0x31,0x49,0x49,0x49,0x46}, // R
//    {0x00,0x46,0x49,0x49,0x49,0x49,0x49,0x31}, // S
//    {0x01,0x01,0x01,0x7f,0x01,0x01,0x01,0x00}, // T
//    {0x00,0x3f,0x40,0x40,0x40,0x40,0x40,0x3f}, // U
//    {0x00,0x0f,0x10,0x20,0x40,0x20,0x10,0x0f}, // V
//    {0x00,0x3f,0x40,0x40,0x3f,0x40,0x40,0x3f}, // W
//    {0x00,0x63,0x14,0x08,0x08,0x08,0x14,0x63}, // X
//    {0x00,0x07,0x08,0x08,0x78,0x08,0x08,0x07}, // Y
//    {0x00,0x71,0x49,0x49,0x49,0x49,0x49,0x47}, // Z
//};


//============================================================================
void SPI_sendCommand(unsigned char command)
  {

  // Select the LCD's command register
//    GPIO_write(Board_LCD_RS, 0);
    GPIO_write(SPI_LCD_RS, 0);
  //Send the command via SPI:
    unsigned char commandToLCD[2];
    unsigned char resultsFromLCD[2];
    commandToLCD[0] = command;
    resultsFromLCD[0] = 0x00;

    SPI_Communication(commandToLCD,1, resultsFromLCD, 0,0x01);

  }
//----------------------------------------------------------------------------
void SPI_sendData(unsigned char data)
  {

  // Select the LCD's data register
//    GPIO_write(Board_LCD_RS, 1);
    GPIO_write(SPI_LCD_RS, 1);
  //Send the command via SPI:
    unsigned char commandToLCD[2];
    unsigned char resultsFromLCD[2];
    commandToLCD[0] = data;
    resultsFromLCD[0] = 0x00;
    SPI_Communication(commandToLCD,1, resultsFromLCD, 0,0x01);

  }
void SPI_sendData4(unsigned char data[], uint16_t NW,uint16_t times)
  {

  // Select the LCD's data register
//    GPIO_write(Board_LCD_RS, 1);
    GPIO_write(SPI_LCD_RS, 1);
  //Send the command via SPI:
    unsigned char commandToLCD[NW];
    unsigned char resultsFromLCD[2];
    for(int j=0;j<times;j++){
        for(int i=0;i<NW;i++)
        commandToLCD[j*NW + i] = data[i];
    }

    resultsFromLCD[0] = 0x00;

    SPI_Communication(commandToLCD,NW*times, resultsFromLCD, 0,0x01);

  }
void SPI_sendData40(unsigned char data[], uint16_t NW)
  {

  // Select the LCD's data register
//    GPIO_write(Board_LCD_RS, 1);
    GPIO_write(SPI_LCD_RS, 1);
  //Send the command via SPI:
    unsigned char commandToLCD[NW];
    unsigned char resultsFromLCD[2];
//        for(int i=0;i<NW;i++)
//        commandToLCD[NW] = data[i];

    resultsFromLCD[0] = 0x00;

    SPI_Communication(data,NW, resultsFromLCD, 0,0x01);

  }
void SPI_sendData2(unsigned char data[3], uint16_t nWrite)
  {

  // Select the LCD's data register
//    GPIO_write(Board_LCD_RS, 1);Q
    GPIO_write(SPI_LCD_RS, 1);
  //Send the command via SPI:
    unsigned char commandToLCD[nWrite];

    for(int i=0;i<nWrite;i+=3){
        commandToLCD[i]   = data[0];
        commandToLCD[i+1] = data[1];
        commandToLCD[i+2] = data[2];
    }
    unsigned char resultsFromLCD[2];
//    commandToLCD[0] = data;
    resultsFromLCD[0] = 0x00;
    SPI_Communication(commandToLCD,nWrite, resultsFromLCD, 0,0x01);

  }
void SPI_sendData3(unsigned char data[], uint16_t nWrite)
  {

  // Select the LCD's data register
//    GPIO_write(Board_LCD_RS, 1);Q
    GPIO_write(SPI_LCD_RS, 1);
  //Send the command via SPI:
    unsigned char commandToLCD[nWrite];
//
    for(int i=0;i<nWrite;i++){
        commandToLCD[i]   = data[i];
    }
    unsigned char resultsFromLCD[2];
//    commandToLCD[0] = data;
    resultsFromLCD[0] = 0x00;
    SPI_Communication(commandToLCD,nWrite, resultsFromLCD, 0,0x01);

  }
void Write_LCD_Acc(uint8_t R, uint8_t G, uint8_t B)
  {
  register int
    i,j,k;
  int posx = 38;
  int posy = 23;
  uint8_t RGB[3];
  RGB[0] = R;   RGB[1] = G;   RGB[2] = B;
      for(k=0 ; k<3;k++){
//          for(j = 0; j < 20;j++){
              Set_LCD_for_write_at_X_Y2(posx, 127,128 - posy - k*24, 128 - posy  - 20 - k*24);

                SPI_sendData2(RGB,(128 -posx)*20*3);
//              //Fill display with a given RGB value
//              for (i = 0; i < 128 -posx; i++)
//                {
//                    SPI_sendData(B); //Blue
//                    SPI_sendData(G); //Green
//                    SPI_sendData(R); //Red
//                }
//          }
      }
  }

//----------------------------------------------------------------------------
void Initialize_LCD(void)
  {
  //Reset the LCD controller
//  CLR_RESET;
//  delay(1);//10µS min
//  SET_RESET;
//  delay(150);//120mS max

  //SLPOUT (11h): Sleep Out ("Sleep Out"  is chingrish for "wake")
  //The DC/DC converter is enabled, Internal display oscillator
  //is started, and panel scanning is started.
  SPI_sendCommand(ST7735_SLPOUT);
//  delay(120);


//  Task_sleep(150000/Clock_tickPeriod);// Delay 150ms

  //FRMCTR1 (B1h): Frame Rate Control (In normal mode/ Full colors)
  //Set the frame frequency of the full colors normal mode.
  // * Frame rate=fosc/((RTNA + 20) x (LINE + FPA + BPA))
  // * 1 < FPA(front porch) + BPA(back porch) ; Back porch ?0
  //Note: fosc = 333kHz
  SPI_sendCommand(ST7735_FRMCTR1);//In normal mode(Full colors)
  SPI_sendData(0x02);//RTNB: set 1-line period
  SPI_sendData(0x35);//FPB:  front porch
  SPI_sendData(0x36);//BPB:  back porch

  //FRMCTR2 (B2h): Frame Rate Control (In Idle mode/ 8-colors)
  //Set the frame frequency of the Idle mode.
  // * Frame rate=fosc/((RTNB + 20) x (LINE + FPB + BPB))
  // * 1 < FPB(front porch) + BPB(back porch) ; Back porch ?0
  //Note: fosc = 333kHz
  SPI_sendCommand(ST7735_FRMCTR2);//In Idle mode (8-colors)
  SPI_sendData(0x02);//RTNB: set 1-line period
  SPI_sendData(0x35);//FPB:  front porch
  SPI_sendData(0x36);//BPB:  back porch

  //FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
  //Set the frame frequency of the Partial mode/ full colors.
  // * 1st parameter to 3rd parameter are used in line inversion mode.
  // * 4th parameter to 6th parameter are used in frame inversion mode.
  // * Frame rate=fosc/((RTNC + 20) x (LINE + FPC + BPC))
  // * 1 < FPC(front porch) + BPC(back porch) ; Back porch ?0
  //Note: fosc = 333kHz
  SPI_sendCommand(ST7735_FRMCTR3);//In partial mode + Full colors
  SPI_sendData(0x02);//RTNC: set 1-line period
  SPI_sendData(0x35);//FPC:  front porch
  SPI_sendData(0x36);//BPC:  back porch
  SPI_sendData(0x02);//RTND: set 1-line period
  SPI_sendData(0x35);//FPD:  front porch
  SPI_sendData(0x36);//BPD:  back porch

  //INVCTR (B4h): Display Inversion Control
  SPI_sendCommand(ST7735_INVCTR);
  SPI_sendData(0x07);
  // 0000 0ABC
  // |||| ||||-- NLC: Inversion setting in full Colors partial mode
  // |||| |||         (0=Line Inversion, 1 = Frame Inversion)
  // |||| |||--- NLB: Inversion setting in idle mode
  // |||| ||          (0=Line Inversion, 1 = Frame Inversion)
  // |||| ||---- NLA: Inversion setting in full Colors normal mode
  // |||| |----- Unused: 0

  //PWCTR1 (C0h): Power Control 1
  SPI_sendCommand(ST7735_PWCTR1);
  SPI_sendData(0x02);// VRH[4:0] (0-31) Sets GVDD
                     // VRH=0x00 => GVDD=5.0v
                     // VRH=0x1F => GVDD=3.0v
                     // Each tick is a variable step:
                     // VRH[4:0] |  VRH | GVDD
                     //   00000b | 0x00 | 5.00v
                     //   00001b | 0x01 | 4.75v
                     //   00010b | 0x02 | 4.70v <<<<<
                     //   00011b | 0x03 | 4.65v
                     //   00100b | 0x04 | 4.60v
                     //   00101b | 0x05 | 4.55v
                     //   00110b | 0x06 | 4.50v
                     //   00111b | 0x07 | 4.45v
                     //   01000b | 0x08 | 4.40v
                     //   01001b | 0x09 | 4.35v
                     //   01010b | 0x0A | 4.30v
                     //   01011b | 0x0B | 4.25v
                     //   01100b | 0x0C | 4.20v
                     //   01101b | 0x0D | 4.15v
                     //   01110b | 0x0E | 4.10v
                     //   01111b | 0x0F | 4.05v
                     //   10000b | 0x10 | 4.00v
                     //   10001b | 0x11 | 3.95v
                     //   10010b | 0x12 | 3.90v
                     //   10011b | 0x13 | 3.85v
                     //   10100b | 0x14 | 3.80v
                     //   10101b | 0x15 | 3.75v
                     //   10110b | 0x16 | 3.70v
                     //   10111b | 0x17 | 3.65v
                     //   11000b | 0x18 | 3.60v
                     //   11001b | 0x19 | 3.55v
                     //   11010b | 0x1A | 3.50v
                     //   11011b | 0x1B | 3.45v
                     //   11100b | 0x1C | 3.40v
                     //   11101b | 0x1D | 3.35v
                     //   11110b | 0x1E | 3.25v
                     //   11111b | 0x1F | 3.00v
  SPI_sendData(0x02);// 010i i000
                     // |||| ||||-- Unused: 0
                     // |||| |----- IB_SEL0:
                     // ||||------- IB_SEL1:
                     // |||-------- Unused: 010
                     // IB_SEL[1:0] | IB_SEL | AVDD
                     //         00b | 0x00   | 2.5µA   <<<<<
                     //         01b | 0x01   | 2.0µA
                     //         10b | 0x02   | 1.5µA
                     //         11b | 0x03   | 1.0µA

                     //PWCTR2 (C1h): Power Control 2
                     // * Set the VGH and VGL supply power level
                     //Restriction: VGH-VGL <= 32V
  SPI_sendCommand(ST7735_PWCTR2);
  SPI_sendData(0xC5);// BT[2:0] (0-15) Sets GVDD
                     // BT[2:0] |    VGH      |     VGL
                     //    000b | 4X |  9.80v | -3X |  -7.35v
                     //    001b | 4X |  9.80v | -4X |  -9.80v
                     //    010b | 5X | 12.25v | -3X |  -7.35v
                     //    011b | 5X | 12.25v | -4X |  -9.80v
                     //    100b | 5X | 12.25v | -5X | -12.25v
                     //    101b | 6X | 14.70v | -3X |  -7.35v   <<<<<
                     //    110b | 6X | 14.70v | -4X |  -9.80v
                     //    111b | 6X | 14.70v | -5X | -12.25v

                     //PWCTR3 (C2h): Power Control 3 (in Normal mode/ Full colors)
                     // * Set the amount of current in Operational amplifier in
                     //   normal mode/full colors.
                     // * Adjust the amount of fixed current from the fixed current
                     //   source in the operational amplifier for the source driver.
                     // * Set the Booster circuit Step-up cycle in Normal mode/ full
                     //   colors.
  SPI_sendCommand(ST7735_PWCTR3);
  SPI_sendData(0x0D);// AP[2:0] Sets Operational Amplifier Bias Current
                     // AP[2:0] | Function
                     //    000b | Off
                     //    001b | Small
                     //    010b | Medium Low
                     //    011b | Medium
                     //    100b | Medium High
                     //    101b | Large          <<<<<
                     //    110b | reserved
                     //    111b | reserved
  SPI_sendData(0x00);// DC[2:0] Booster Frequency
                     // DC[2:0] | Circuit 1 | Circuit 2,4
                     //    000b | BCLK / 1  | BCLK / 1  <<<<<
                     //    001b | BCLK / 1  | BCLK / 2
                     //    010b | BCLK / 1  | BCLK / 4
                     //    011b | BCLK / 2  | BCLK / 2
                     //    100b | BCLK / 2  | BCLK / 4
                     //    101b | BCLK / 4  | BCLK / 4
                     //    110b | BCLK / 4  | BCLK / 8
                     //    111b | BCLK / 4  | BCLK / 16

                     //PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
                     // * Set the amount of current in Operational amplifier in
                     //   normal mode/full colors.
                     // * Adjust the amount of fixed current from the fixed current
                     //   source in the operational amplifier for the source driver.
                     // * Set the Booster circuit Step-up cycle in Normal mode/ full
                     //   colors.
  SPI_sendCommand(ST7735_PWCTR4);
  SPI_sendData(0x8D);// AP[2:0] Sets Operational Amplifier Bias Current
                     // AP[2:0] | Function
                     //    000b | Off
                     //    001b | Small
                     //    010b | Medium Low
                     //    011b | Medium
                     //    100b | Medium High
                     //    101b | Large          <<<<<
                     //    110b | reserved
                     //    111b | reserved
  SPI_sendData(0x1A);// DC[2:0] Booster Frequency
                     // DC[2:0] | Circuit 1 | Circuit 2,4
                     //    000b | BCLK / 1  | BCLK / 1
                     //    001b | BCLK / 1  | BCLK / 2
                     //    010b | BCLK / 1  | BCLK / 4  <<<<<
                     //    011b | BCLK / 2  | BCLK / 2
                     //    100b | BCLK / 2  | BCLK / 4
                     //    101b | BCLK / 4  | BCLK / 4
                     //    110b | BCLK / 4  | BCLK / 8
                     //    111b | BCLK / 4  | BCLK / 16

                     //PPWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
                     // * Set the amount of current in Operational amplifier in
                     //   normal mode/full colors.
                     // * Adjust the amount of fixed current from the fixed current
                     //   source in the operational amplifier for the source driver.
                     // * Set the Booster circuit Step-up cycle in Normal mode/ full
                     //   colors.
  SPI_sendCommand(ST7735_PWCTR5);
  SPI_sendData(0x8D);// AP[2:0] Sets Operational Amplifier Bias Current
                     // AP[2:0] | Function
                     //    000b | Off
                     //    001b | Small
                     //    010b | Medium Low
                     //    011b | Medium
                     //    100b | Medium High
                     //    101b | Large          <<<<<
                     //    110b | reserved
                     //    111b | reserved
  SPI_sendData(0xEE);// DC[2:0] Booster Frequency
                     // DC[2:0] | Circuit 1 | Circuit 2,4
                     //    000b | BCLK / 1  | BCLK / 1
                     //    001b | BCLK / 1  | BCLK / 2
                     //    010b | BCLK / 1  | BCLK / 4
                     //    011b | BCLK / 2  | BCLK / 2
                     //    100b | BCLK / 2  | BCLK / 4
                     //    101b | BCLK / 4  | BCLK / 4
                     //    110b | BCLK / 4  | BCLK / 8  <<<<<
                     //    111b | BCLK / 4  | BCLK / 16

                     //VMCTR1 (C5h): VCOM Control 1
  SPI_sendCommand(ST7735_VMCTR1);
  SPI_sendData(0x51);// Default: 0x51 => +4.525
                     // VMH[6:0] (0-100) Sets VCOMH
                     // VMH=0x00 => VCOMH= +2.5v
                     // VMH=0x64 => VCOMH= +5.0v
  SPI_sendData(0x4D);// Default: 0x4D => -0.575
                     // VML[6:0] (4-100) Sets VCOML
                     // VML=0x04 => VCOML= -2.4v
                     // VML=0x64 => VCOML=  0.0v

                     //GMCTRP1 (E0h): Gamma ‘+’polarity Correction Characteristics Setting
  SPI_sendCommand(ST7735_GAMCTRP1);
  SPI_sendData(0x0a);
  SPI_sendData(0x1c);
  SPI_sendData(0x0c);
  SPI_sendData(0x14);
  SPI_sendData(0x33);
  SPI_sendData(0x2b);
  SPI_sendData(0x24);
  SPI_sendData(0x28);
  SPI_sendData(0x27);
  SPI_sendData(0x25);
  SPI_sendData(0x2C);
  SPI_sendData(0x39);
  SPI_sendData(0x00);
  SPI_sendData(0x05);
  SPI_sendData(0x03);
  SPI_sendData(0x0d);

                     //GMCTRN1 (E1h): Gamma ‘-’polarity Correction Characteristics Setting
  SPI_sendCommand(ST7735_GAMCTRN1);
  SPI_sendData(0x0a);
  SPI_sendData(0x1c);
  SPI_sendData(0x0c);
  SPI_sendData(0x14);
  SPI_sendData(0x33);
  SPI_sendData(0x2b);
  SPI_sendData(0x24);
  SPI_sendData(0x28);
  SPI_sendData(0x27);
  SPI_sendData(0x25);
  SPI_sendData(0x2D);
  SPI_sendData(0x3a);
  SPI_sendData(0x00);
  SPI_sendData(0x05);
  SPI_sendData(0x03);
  SPI_sendData(0x0d);

                     //COLMOD (3Ah): Interface Pixel Format
                     // * This command is used to define the format of RGB picture
                     //   data, which is to be transferred via the MCU interface.
  SPI_sendCommand(ST7735_COLMOD);
  SPI_sendData(0x06);// Default: 0x06 => 18-bit/pixel
                     // IFPF[2:0] MCU Interface Color Format
                     // IFPF[2:0] | Format
                     //      000b | reserved
                     //      001b | reserved
                     //      010b | reserved
                     //      011b | 12-bit/pixel
                     //      100b | reserved
                     //      101b | 16-bit/pixel
                     //      110b | 18-bit/pixel   <<<<<
                     //      111b | reserved

                     //DISPON (29h): Display On
                     // * This command is used to recover from DISPLAY OFF mode. Output
                     //   from the Frame Memory is enabled.
                     // * This command makes no change of contents of frame memory.
                     // * This command does not change any other status.
                     // * The delay time between DISPON and DISPOFF needs 120ms at least
  SPI_sendCommand(ST7735_DISPON);//Display On
  Task_sleep(1000/Clock_tickPeriod);// Delay 500ms
//  delay(1);

  //MADCTL (36h): Memory Data Access Control
  SPI_sendCommand(ST7735_MADCTL);
  SPI_sendData(0x40);// YXVL RH--
                     // |||| ||||-- Unused: 0
                     // |||| ||---- MH: Horizontal Refresh Order
                     // |||| |        0 = left to right
                     // |||| |        1 = right to left
                     // |||| |----- RGB: RGB vs BGR Order
                     // ||||          0 = RGB color filter panel
                     // ||||          1 = BGR color filter panel
                     // ||||------- ML: Vertical Refresh Order
                     // |||           0 = top to bottom
                     // |||           1 = bottom to top
                     // |||-------- MV: Row / Column Exchange
                     // ||--------- MX: Column Address Order  <<<<<
                     // |---------- MY: Row Address Order

  }
//============================================================================
void Set_LCD_for_write_at_X_Y(uint8_t x, uint8_t y)
  {
  //CASET (2Ah): Column Address Set
  // * The value of XS [15:0] and XE [15:0] are referred when RAMWR
  //   command comes.
  // * Each value represents one column line in the Frame Memory.
  // * XS [15:0] always must be equal to or less than XE [15:0]
  SPI_sendCommand(ST7735_CASET); //Column address set
  //Write the parameters for the "column address set" command
  SPI_sendData(0x00);     //Start MSB = XS[15:8]
  SPI_sendData(0x02 + x); //Start LSB = XS[ 7:0]
  SPI_sendData(0x00);     //End MSB   = XE[15:8]
  SPI_sendData(0x81);     //End LSB   = XE[ 7:0]
  //Write the "row address set" command to the LCD
  //RASET (2Bh): Row Address Set
  // * The value of YS [15:0] and YE [15:0] are referred when RAMWR
  //   command comes.
  // * Each value represents one row line in the Frame Memory.
  // * YS [15:0] always must be equal to or less than YE [15:0]
  SPI_sendCommand(ST7735_RASET); //Row address set
  //Write the parameters for the "row address set" command
  SPI_sendData(0x00);     //Start MSB = YS[15:8]
  SPI_sendData(0x01 + y); //Start LSB = YS[ 7:0]
  SPI_sendData(0x00);     //End MSB   = YE[15:8]
  SPI_sendData(0x80);     //End LSB   = YE[ 7:0]
  //Write the "write data" command to the LCD
  //RAMWR (2Ch): Memory Write
  SPI_sendCommand(ST7735_RAMWR); //write data
  }
void Set_LCD_for_write_at_X_Y2(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
  {
  //CASET (2Ah): Column Address Set
  // * The value of XS [15:0] and XE [15:0] are referred when RAMWR
  //   command comes.
  // * Each value represents one column line in the Frame Memory.
  // * XS [15:0] always must be equal to or less than XE [15:0]
  SPI_sendCommand(ST7735_CASET); //Column address set
  //Write the parameters for the "column address set" command
  SPI_sendData(0x00);     //Start MSB = XS[15:8]
  SPI_sendData(0x02 + x1); //Start LSB = XS[ 7:0]
  SPI_sendData(0x00);     //End MSB   = XE[15:8]
  SPI_sendData(0x02 + x2);     //End LSB   = XE[ 7:0]

  //Write the "row address set" command to the LCD
  //RASET (2Bh): Row Address Set
  // * The value of YS [15:0] and YE [15:0] are referred when RAMWR
  //   command comes.
  // * Each value represents one row line in the Frame Memory.
  // * YS [15:0] always must be equal to or less than YE [15:0]
  SPI_sendCommand(ST7735_RASET); //Row address set
  //Write the parameters for the "row address set" command
  SPI_sendData(0x00);     //Start MSB = YS[15:8]
  SPI_sendData(0x01 + y1); //Start LSB = YS[ 7:0]
  SPI_sendData(0x00);     //End MSB   = YE[15:8]
  SPI_sendData(0x01 + y2);     //End LSB   = YE[ 7:0]
  //Write the "write data" command to the LCD
  //RAMWR (2Ch): Memory Write
  SPI_sendCommand(ST7735_RAMWR); //write data
  }
//============================================================================
//void Fill_LCD(uint8_t R, uint8_t G, uint8_t B)
//  {
//  register int
//    i;
//  Set_LCD_for_write_at_X_Y(0, 0);
//
//  //Fill display with a given RGB value
//  for (i = 0; i < (128 * 128); i++)
//    {
//    SPI_sendData(B); //Blue
//    SPI_sendData(G); //Green
//    SPI_sendData(R); //Red
//    }
//  }
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B)
  {
  Set_LCD_for_write_at_X_Y(0, 0);
  uint8_t RGB[3];
  RGB[0] = B;   RGB[1] = G;   RGB[2] = R;
    SPI_sendData2(RGB,13080);
    SPI_sendData2(RGB,13080);
    SPI_sendData2(RGB,13080);
    SPI_sendData2(RGB,9912);
  }
//============================================================================
void Fill_LCD_Acc(uint8_t R, uint8_t G, uint8_t B)
  {
  register int
    i,j,k;
  int posx = 38;
  int posy = 23;
  uint8_t RGB[3];
  RGB[0] = R;   RGB[1] = G;   RGB[2] = B;
      for(k=0 ; k<3;k++){
//          for(j = 0; j < 20;j++){
              Set_LCD_for_write_at_X_Y2(posx, 127,128 - posy - k*24, 128 - posy  - 20 - k*24);

                SPI_sendData2(RGB,(128 -posx)*20*3);
//              //Fill display with a given RGB value
//              for (i = 0; i < 128 -posx; i++)
//                {
//                    SPI_sendData(B); //Blue
//                    SPI_sendData(G); //Green
//                    SPI_sendData(R); //Red
//                }
//          }
      }
  }
//============================================================================
void Put_Pixel(uint8_t x, uint8_t y, uint8_t R, uint8_t G, uint8_t B)
  {
  Set_LCD_for_write_at_X_Y(x, y);
  //Write the single pixel's worth of data
  SPI_sendData(B); //Blue
  SPI_sendData(G); //Green
  SPI_sendData(R); //Red
  }
void Put_Pixel3(uint8_t x, uint8_t y, uint8_t R, uint8_t G, uint8_t B, uint8_t times)
  {
  Set_LCD_for_write_at_X_Y(x, y);
  //Write the single pixel's worth of data
  uint8_t RGB[3];
  RGB[0] = R;
  RGB[1] = G;
  RGB[2] = B;
//  SPI_sendData(B); //Blue
//  SPI_sendData(G); //Green
//  SPI_sendData(R); //Red

  SPI_sendData4(RGB, 3,times);
  }

//============================================================================
// From: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void LCD_Circle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t R, uint8_t G, uint8_t B)
  {
  uint8_t x = radius;
  uint8_t y = 0;
  int16_t radiusError = 1 - (int16_t) x;

  while (x >= y)
    {
    //11 O'Clock
    Put_Pixel(x0 - y, y0 + x, R, G, B);
    //1 O'Clock
    Put_Pixel(x0 + y, y0 + x, R, G, B);
    //10 O'Clock
    Put_Pixel(x0 - x, y0 + y, R, G, B);
    //2 O'Clock
    Put_Pixel(x0 + x, y0 + y, R, G, B);
    //8 O'Clock
    Put_Pixel(x0 - x, y0 - y, R, G, B);
    //4 O'Clock
    Put_Pixel(x0 + x, y0 - y, R, G, B);
    //7 O'Clock
    Put_Pixel(x0 - y, y0 - x, R, G, B);
    //5 O'Clock
    Put_Pixel(x0 + y, y0 - x, R, G, B);

    y++;
    if (radiusError < 0)
      radiusError += (int16_t)(2 * y + 1);
    else
      {
      x--;
      radiusError += 2 * (((int16_t) y - (int16_t) x) + 1);
      }
    }
  }

void LCD_Number(uint8_t Number, uint8_t x0, uint8_t y0, uint8_t R, uint8_t G, uint8_t B)
  {

    Put_Pixel(x0 + 1, y0 - 2, R, G, B);
    Put_Pixel(x0 + 2, y0 - 1, R, G, B);
    Put_Pixel(x0 + 2, y0 - 2, R, G, B);
    Put_Pixel(x0 + 2, y0 - 3, R, G, B);
    Put_Pixel(x0 + 2, y0 - 4, R, G, B);
    Put_Pixel(x0 + 2, y0 - 5, R, G, B);

  }


/* Function: LCDWrite
There are two  banks in the LCD, data and commands. This
function sets the DC pin high or low depending, and then sends
the data byte
 */
void LCDWrite(unsigned char data) {
    SPI_sendData(data);
}

/* Function: LCDCharacter.
 This function takes in a character, looks it up in the font table/array
And writes it to the screen
Each character is 8 bits tall and 5 bits wide. We pad one blank column of
pixels on each side of the character for readability.
 */
void LCDCharacter(char character)
{
    uint8_t column = 0;
    LCDWrite(0x00); //Blank vertical line padding , LHS
//    switch(_FontNumber)
//    {
//        case 1:
//            #ifdef NOKIA5110_FONT_1
//                for (column = 0 ; column < 5 ; column++){
//                    LCDWrite(LCD_DATA, (pgm_read_byte(&ASCII[character - LCD_ASCII_OFFSET][column])));
//                }
//            #endif
//        break;
//        case 2:
//            #ifdef NOKIA5110_FONT_2
//                for (column = 0 ; column < 7 ; column++) {
//                    LCDWrite(LCD_DATA, (pgm_read_byte(&ASCII_TWO[character - LCD_ASCII_OFFSET][column])));
//                }
//            #endif
//        break;
//        case 3:
//            #ifdef NOKIA5110_FONT_3
//                for (column = 0 ; column < 5 ; column++) {
//                    LCDWrite(LCD_DATA, (pgm_read_byte(&ASCII_THREE[character - LCD_ASCII_OFFSET][column])));
//                }
//            #endif
//        break;
//        case 4:
//            #ifdef NOKIA5110_FONT_4
//                for (column = 0 ; column < 4 ; column++) {
//                    LCDWrite(LCD_DATA, (pgm_read_byte(&ASCII_FOUR[character - LCD_ASCII_OFFSET][column])));
//                }
//            #endif
//        break;
//        case 5:
//            #ifdef NOKIA5110_FONT_5
//                for (column = 0 ; column < 8 ; column++){
//                    //LCDWrite((pgm_read_byte(&ASCII_FIVE[character - LCD_ASCII_OFFSET][column])));
//                    LCDWrite(ASCII_FIVE[character - LCD_ASCII_OFFSET][column]);
//                }
//            #endif
//        break;
//        case 6:
//            #ifdef NOKIA5110_FONT_6
//                for (column = 0 ; column < 3 ; column++){
//                    LCDWrite(LCD_DATA, (pgm_read_byte(&ASCII_SIX[character - LCD_ASCII_OFFSET][column])));
//                }
//            #endif
//        break;
//    }
    LCDWrite(0x00); //Blank vertical line padding RHS
}

/* Function: LCDString.
 Given a string of characters, one by one is passed to the LCD
 */
void LCDString(const char  *characters) {
    while (*characters)
    LCDCharacter(*characters++);
}
