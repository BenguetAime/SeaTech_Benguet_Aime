/*
 * LCD_LAUNCHPAD.h
 *
 *  Created on: 9 févr. 2020
 *      Author: sebas
 */

#ifndef LCD_LAUNCHPAD_LCD_LAUNCHPAD_H_
#define LCD_LAUNCHPAD_LCD_LAUNCHPAD_H_

// Defines for the ST7735 registers.
// ref: https://www.crystalfontz.com/products/document/3277/ST7735_V2.1_20100505.pdf
#define ST7735_SLPOUT   (0x11)
#define ST7735_DISPON   (0x29)
#define ST7735_CASET    (0x2A)
#define ST7735_RASET    (0x2B)
#define ST7735_RAMWR    (0x2C)
#define ST7735_RAMRD    (0x2E)
#define ST7735_MADCTL   (0x36)
#define ST7735_COLMOD   (0x3A)
#define ST7735_FRMCTR1  (0xB1)
#define ST7735_FRMCTR2  (0xB2)
#define ST7735_FRMCTR3  (0xB3)
#define ST7735_INVCTR   (0xB4)
#define ST7735_PWCTR1   (0xC0)
#define ST7735_PWCTR2   (0xC1)
#define ST7735_PWCTR3   (0xC2)
#define ST7735_PWCTR4   (0xC3)
#define ST7735_PWCTR5   (0xC4)
#define ST7735_VMCTR1   (0xC5)
#define ST7735_GAMCTRP1 (0xE0)
#define ST7735_GAMCTRN1 (0xE1)



void Initialize_LCD(void);
/*
 * LCD_LAUNCHPAD.c
 *
 *  Created on: 9 févr. 2020
 *      Author: sebas
 */

#include <stdint.h>
//#include <LCD_LAUNCHPAD/LCD_LAUNCHPAD.h>
//#include <ti/drivers/GPIO.h>
//#include "Board.h"
//#include <ti/sysbios/knl/Clock.h>
//#include <ti/sysbios/knl/Task.h>

void Initialize_LCD(void);
void Set_LCD_for_write_at_X_Y(uint8_t x, uint8_t y);
void Set_LCD_for_write_at_X_Y2(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B);
void Fill_LCD_Acc(uint8_t R, uint8_t G, uint8_t B);
void Put_Pixel(uint8_t x, uint8_t y, uint8_t R, uint8_t G, uint8_t B);
void Put_Pixel3(uint8_t x, uint8_t y, uint8_t R, uint8_t G, uint8_t B, uint8_t times);
void LCD_Circle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t R, uint8_t G, uint8_t B);
void LCD_Number(uint8_t Number, uint8_t x0, uint8_t y0, uint8_t R, uint8_t G, uint8_t B);
void LCDString(const char  *characters);
void SPI_sendData(unsigned char data);
void SPI_sendData2(unsigned char data[3], uint16_t nWrite);
void SPI_sendData3(unsigned char data[3], uint16_t nWrite);
void SPI_sendData4(unsigned char data[], uint16_t NW,uint16_t times);
void SPI_sendData40(unsigned char data[], uint16_t NW);
void SPI_sendCommand(unsigned char command);

#endif /* LCD_LAUNCHPAD_LCD_LAUNCHPAD_H_ */
