/*
 * OLED_Display.c
 *
 *  Created on: 11 févr. 2020
 *      Author: sebas
 */

/************ OLED Display.c *********/
#include "TacheLCD/LCD_LAUNCHPAD/OLED_Display.h"
#include <TacheLCD/LCD_LAUNCHPAD/LCD_LAUNCHPAD.h>
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"

extern unsigned long g_longTimeStamp;
//unsigned char OLED_Text_1 [ ] = "OLED Display";
//unsigned char OLED_Text_2 [ ] = "128 x 64";
//unsigned char OLED_Text_3 [ ] = "LINES";
//unsigned char OLED_Text_4 [ ] = "Voltage";
//unsigned char OLED_Text_5 [ 0x03 ];
//unsigned char OLED_Text_6 [ ] = "Rectangles";
//unsigned char OLED_Text_7 [ ] = "Circles";
//unsigned char OLED_Text_8 [ ] = "Bars";
//unsigned char OLED_Text_9 [ ] = "TEXTS";
//unsigned char OLED_Text_10 [ ] = "x1";
//unsigned char OLED_Text_11 [ ] = "x2";
//unsigned char OLED_Text_12 [ ] = "x3";
//unsigned char OLED_Text_13 [ ] = "Scroll To Right";
//unsigned char OLED_Text_14 [ ] = "Scroll To Left";
//unsigned char OLED_Text_15 [ ] = "Vertical & Right";
//unsigned char OLED_Text_16 [ ] = "Vertical & Left";
char OLED_Text_1 [ ] = "OLED Display";
char OLED_Text_2 [ ] = "128 x 64";
char OLED_Text_3 [ ] = "LINES";
char OLED_Text_4 [ ] = "Voltage";
char OLED_Text_5 [ 0x03 ];
char OLED_Text_6 [ ] = "Rectangles";
char OLED_Text_7 [ ] = "Circles";
char OLED_Text_8 [ ] = "Bars";
char OLED_Text_9 [ ] = "TEXTS";
char OLED_Text_10 [ ] = "x1";
char OLED_Text_11 [ ] = "x2";
char OLED_Text_12 [ ] = "x3";
char OLED_Text_13 [ ] = "Scroll To Right";
char OLED_Text_14 [ ] = "Scroll To Left";
char OLED_Text_15 [ ] = "Vertical & Right";
char OLED_Text_16 [ ] = "Vertical & Left";


unsigned char TEST [ ARRAY_HEIGHT ] [ ARRAY_WIDHT ];
// CHARACTERS GRTAPIC MODE // !!
const char TEXT[51][5] =
{0x00, 0x00, 0x00, 0x00, 0x00, // SPACE
0x00, 0x00, 0x5F, 0x00, 0x00, // !
0x00, 0x03, 0x00, 0x03, 0x00, // "
0x14, 0x3E, 0x14, 0x3E, 0x14, // #
0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
0x43, 0x33, 0x08, 0x66, 0x61, // %
0x36, 0x49, 0x55, 0x22, 0x50, // &
0x00, 0x05, 0x03, 0x00, 0x00, // '
0x00, 0x1C, 0x22, 0x41, 0x00, // (
0x00, 0x41, 0x22, 0x1C, 0x00, // )
0x14, 0x08, 0x3E, 0x08, 0x14, // *
0x08, 0x08, 0x3E, 0x08, 0x08, // +
0x00, 0x50, 0x30, 0x00, 0x00, // ,
0x08, 0x08, 0x08, 0x08, 0x08, // -
0x00, 0x60, 0x60, 0x00, 0x00, // .
0x20, 0x10, 0x08, 0x04, 0x02, // /
0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
0x04, 0x02, 0x7F, 0x00, 0x00, // 1
0x42, 0x61, 0x51, 0x49, 0x46, // 2
0x22, 0x41, 0x49, 0x49, 0x36, // 3
0x18, 0x14, 0x12, 0x7F, 0x10, // 4
0x27, 0x45, 0x45, 0x45, 0x39, // 5
0x3E, 0x49, 0x49, 0x49, 0x32, // 6
0x01, 0x01, 0x71, 0x09, 0x07, // 7
0x36, 0x49, 0x49, 0x49, 0x36, // 8
0x26, 0x49, 0x49, 0x49, 0x3E, // 9
0x00, 0x36, 0x36, 0x00, 0x00, // :
0x00, 0x56, 0x36, 0x00, 0x00, // ;
0x08, 0x14, 0x22, 0x41, 0x00, // <
0x14, 0x14, 0x14, 0x14, 0x14, // =
0x00, 0x41, 0x22, 0x14, 0x08, // >
0x02, 0x01, 0x51, 0x09, 0x06, // ?
0x3E, 0x41, 0x59, 0x55, 0x5E, // @
0x7E, 0x09, 0x09, 0x09, 0x7E, // A
0x7F, 0x49, 0x49, 0x49, 0x36, // B
0x3E, 0x41, 0x41, 0x41, 0x22, // C
0x7F, 0x41, 0x41, 0x41, 0x3E, // D
0x7F, 0x49, 0x49, 0x49, 0x41, // E
0x7F, 0x09, 0x09, 0x09, 0x01, // F
0x3E, 0x41, 0x41, 0x49, 0x3A, // G
0x7F, 0x08, 0x08, 0x08, 0x7F, // H
0x00, 0x41, 0x7F, 0x41, 0x00, // I
0x30, 0x40, 0x40, 0x40, 0x3F, // J
0x7F, 0x08, 0x14, 0x22, 0x41, // K
0x7F, 0x40, 0x40, 0x40, 0x40, // L
0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
0x7F, 0x02, 0x04, 0x08, 0x7F, // N
0x3E, 0x41, 0x41, 0x41, 0x3E, // O
0x7F, 0x09, 0x09, 0x09, 0x06, // P
0x1E, 0x21, 0x21, 0x21, 0x5E, // Q
0x7F, 0x09, 0x09, 0x09, 0x76};// R
const char TEXT2[44][5]={0x26, 0x49, 0x49, 0x49, 0x32, // S
0x01, 0x01, 0x7F, 0x01, 0x01, // T
0x3F, 0x40, 0x40, 0x40, 0x3F, // U
0x1F, 0x20, 0x40, 0x20, 0x1F, // V
0x7F, 0x20, 0x10, 0x20, 0x7F, // W
0x41, 0x22, 0x1C, 0x22, 0x41, // X
0x07, 0x08, 0x70, 0x08, 0x07, // Y
0x61, 0x51, 0x49, 0x45, 0x43, // Z
0x00, 0x7F, 0x41, 0x00, 0x00, // [
0x02, 0x04, 0x08, 0x10, 0x20, // \
0x00, 0x00, 0x41, 0x7F, 0x00, // ]
0x04, 0x02, 0x01, 0x02, 0x04, // ^
0x40, 0x40, 0x40, 0x40, 0x40, // _
0x00, 0x01, 0x02, 0x04, 0x00, // `
0x20, 0x54, 0x54, 0x54, 0x78, // a
0x7F, 0x44, 0x44, 0x44, 0x38, // b
0x38, 0x44, 0x44, 0x44, 0x44, // c
0x38, 0x44, 0x44, 0x44, 0x7F, // d
0x38, 0x54, 0x54, 0x54, 0x18, // e
0x04, 0x04, 0x7E, 0x05, 0x05, // f
0x08, 0x54, 0x54, 0x54, 0x3C, // g
0x7F, 0x08, 0x04, 0x04, 0x78, // h
0x00, 0x44, 0x7D, 0x40, 0x00, // i
0x20, 0x40, 0x44, 0x3D, 0x00, // j
0x7F, 0x10, 0x28, 0x44, 0x00, // k
0x00, 0x41, 0x7F, 0x40, 0x00, // l
0x7C, 0x04, 0x78, 0x04, 0x78, // m
0x7C, 0x08, 0x04, 0x04, 0x78, // n
0x38, 0x44, 0x44, 0x44, 0x38, // o
0x7C, 0x14, 0x14, 0x14, 0x08, // p
0x08, 0x14, 0x14, 0x14, 0x7C, // q
0x00, 0x7C, 0x08, 0x04, 0x04, // r
0x48, 0x54, 0x54, 0x54, 0x20, // s
0x04, 0x04, 0x3F, 0x44, 0x44, // t
0x3C, 0x40, 0x40, 0x20, 0x7C, // u
0x1C, 0x20, 0x40, 0x20, 0x1C, // v
0x3C, 0x40, 0x30, 0x40, 0x3C, // w
0x44, 0x28, 0x10, 0x28, 0x44, // x
0x0C, 0x50, 0x50, 0x50, 0x3C, // y
0x44, 0x64, 0x54, 0x4C, 0x44, // z
0x00, 0x08, 0x36, 0x41, 0x41, // {
0x00, 0x00, 0x7F, 0x00, 0x00, // |
0x41, 0x41, 0x36, 0x08, 0x00, // }
0x02, 0x01, 0x02, 0x04, 0x02};// ~

char OLEDSetUp ( void )
{
#if defined RESET_PIN_NEEDED
    __delay_ms ( 1 );               // As OLDE VDD goes high sooner than VCC,
    OLED_RESET_PIN = OFF;           // we must wait to stabilize VCC
    __delay_ms ( 1 );               // OLED Reset pin.
    OLED_RESET_PIN = ON;
#endif

// 1. Fundamental Commands.
    OLEDCommand ( OLED_CONTRAST );              // 0x81
    OLEDCommand ( 0x4F );                       // was 0x9F. ******
    OLEDCommand ( OLED_DISPLAY_ALL_ON_RESUME ); // 0xA5
    OLEDCommand ( OLED_NORMAL_DISPLAY );        // 0xA6
    OLEDCommand ( OLED_DISPLAY_ON );            // 0xAF

// 2. Scrolling.

// 3. Addressing.
    OLEDCommand ( OLED_MEMORY_MODE );           // 0x20
    OLEDCommand ( OLED_HORIZONTAL_MODE );

// 4. Hardware Configuration.
    OLEDCommand ( OLED_SET_START_LINE | 0x00 ); // line #0
    OLEDCommand ( OLED_SEGMENT_REMAP | 0x01 );  // WTF???
    OLEDCommand ( OLED_SET_MULTIPLEX_RATIO );   // 0xA8
    OLEDCommand ( 0x3F );
    OLEDCommand ( OLED_COM_SCAN_DECREMENT );    // WTF???
    OLEDCommand ( OLED_DISPLAY_OFFSET );        // 0xD3
    OLEDCommand ( 0x0 );                        // no offset
    OLEDCommand ( OLED_SET_COM_PINS );          // 0xDA
    OLEDCommand ( 0x12 );

// 5. Timing.
    OLEDCommand ( OLED_DISPLAY_CLOCK_RATIO );   // 0xD5
    OLEDCommand ( OLED_CLOCK_RESET_RATIO );
    OLEDCommand ( OLED_SET_PRECHARGE );         // 0xd9
    OLEDCommand ( OLED_PRECHARGE_P1_P2 );       // 0xF1
    OLEDCommand ( OLED_SET_VCOM_H );            // 0xDB
    OLEDCommand ( 0x40 );                       // WTF???

// 6?. Other
    OLEDCommand ( OLED_CHARGE_PUMP );           // 0x8D
    OLEDCommand ( OLED_ENABLE_CHARGE );         // 0x14

    OLEDStopScroll ( );

    return 0x00;
    }


void OLEDCommand ( unsigned char c )
{
//    unsigned char command = 0x00;   // Co = 0, D/C=0, followed by 00 0000.
//
//    unsigned char data[1];
//    data[0]=c;
//    I2C1Write1( OLED_ADDRESS, command, c);
    SPI_sendCommand(c);
}


/********** prints out the Data to the GLCD ********/
void OLEDData ( unsigned char c )
{
//    unsigned char control_c = 0x40;   // Co = 0, D/C = 1
//        I2C1Write1( OLED_ADDRESS, control_c, c);
    SPI_sendData(c);
}


/********** prints out a pixel on the GLCD ********/
void OLEDPixel ( int x, int y, char color )
{
    unsigned char *ptr;
    unsigned char page;
    unsigned int axisX;
    unsigned int axisY;
    signed char bitNumber, bitPlace;
    unsigned int address;


    if ( ( x < ARRAY_WIDHT && y <= ARRAY_LENGHT ) &&
        ( x >= 0x00 && y >= 0x00 ) )
    {
//      axisY = y / 8;                          // Getting the row value.
//      axisX = x;                              // Getting the column value.
//      page = abs ( axisY );                   // Getting the Page number.
//      bitNumber = ( y % 8 );                  // Getting the bit value on the column.

/*      printf ( " \nX:%d, seg#:%d", x, axisX );
        printf ( " \nY:%d, PAGE:%d", y, page );
        printf ( " \nbit #:%d", bitNumber );
*/
        int temp;
        temp =  y / 8;
        temp *= ARRAY_WIDHT;
        temp += x;
        ptr = ( unsigned char * ) TEST;

        //MODIF VB
//////   Set Column Address.
////        OLEDCommand ( OLED_COLUMN_ADDRESS );
////        OLEDCommand ( axisX );                  // Column start address.
////        OLEDCommand ( axisX );                  // Column end address.
//////   Set Page Address.
////        OLEDCommand ( OLED_PAGE_ADDRESS );
////        OLEDCommand ( page );                   // Page start Address.
////        OLEDCommand ( page );                   // Page end Address.

        if ( color )    // bitSet ( x, x ) equivalent.
//          OLEDData ( *( ptr + temp ) |=  ( bitWise ( y % 8 ) ) );
            *( ptr + temp ) |=  ( bitWise ( y % 8 ) );

        else            // bitClear ( x, x ) equivalent.
//          OLEDData ( *( ptr + temp ) &=  ~( bitWise ( y % 8 ) ) );
            *( ptr + temp ) &=  ~( bitWise ( y % 8 ) );

        }   // If the axies are whithin the OLED margin.
    }   // test Pixel.



/********** prints out the 'color' Data to the GLCD ********/
//void OLEDFill ( unsigned char color )
//{
////  OLEDClearBuffer ( );
//
//    unsigned char *ptr;
//    unsigned char i, j;
//
//    ptr = ( unsigned char * ) TEST;
//
//     unsigned int k = 0x00;
//    for (  k = 0x00; k < ARRAY_HEIGHT; k++ )
//    {
//        unsigned int l = 0x00;
//        for (  l = 0x00; l < ARRAY_WIDHT; l++ )
//        {
//            *( ptr + ( l + k * ARRAY_WIDHT ) ) = color;
//        }
//    }
//
//
//// Set the Display with the 'color' Data.
//    OLEDCommand ( OLED_LOW_COLUMN_START );      //set lower column address.
//    OLEDCommand ( OLED_HIGH_COLUMN_START );     //set higher column address.
//    OLEDCommand ( OLED_PAGE_START );            //set page address.
//
//    for ( j = 0x00; j < ARRAY_HEIGHT; j++ )
//    {
//        OLEDCommand ( OLED_LOW_COLUMN_START );  //set lower column address.
//        OLEDCommand ( OLED_HIGH_COLUMN_START ); //set higher column address.
//        OLEDCommand ( OLED_PAGE_START + j );    //set page address.
//
//////        for ( i = 0x00; i < ARRAY_WIDHT; i++ )
//////            OLEDData ( *( ptr + ( j + i * ARRAY_WIDHT ) ) /*= color*/ );//MODIF VB
//        I2C1WriteNInterrupt( OLED_ADDRESS, OLED_SET_START_LINE,  ptr + j , ARRAY_WIDHT );
////          OLEDData ( color );
//        }   // Printing out the rows an then the columns.
//
//    }   // OLEDFill ( ).




// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

// Draw a PROGMEM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
//void OLEDdrawBitmap(int16_t x, int16_t y, uint8_t bitmap[], int16_t w, int16_t h, uint16_t color)
//{
//    unsigned char *ptr;
//
//
//    ptr = ( unsigned char * ) TEST;
//
//    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
//    uint8_t byte = 0;
//int16_t j=0;
//int16_t i=0;
////    startWrite();
//    for( j=0; j<h; j++, y++) {
//        for( i=0; i<w; i++) {
//            if(i & 7) byte <<= 1;
//            else      byte   = bitmap[j * byteWidth + i / 8];
//            if(byte & 0x80) OLEDPixel(x+i, y, color);
//        }
//    }
//
//// Set the Display with the 'color' Data.
//    OLEDCommand ( OLED_LOW_COLUMN_START );      //set lower column address.
//    OLEDCommand ( OLED_HIGH_COLUMN_START );     //set higher column address.
//    OLEDCommand ( OLED_PAGE_START );            //set page address.
//
//    for ( j = 0x00; j < ARRAY_HEIGHT; j++ )
//    {
//        OLEDCommand ( OLED_LOW_COLUMN_START );  //set lower column address.
//        OLEDCommand ( OLED_HIGH_COLUMN_START ); //set higher column address.
//        OLEDCommand ( OLED_PAGE_START + j );    //set page address.
//
//////        for ( i = 0x00; i < ARRAY_WIDHT; i++ )
//////            OLEDData ( *( ptr + ( j + i * ARRAY_WIDHT ) ) /*= color*/ );//MODIF VB
//        I2C1WriteNInterrupt( OLED_ADDRESS, OLED_SET_START_LINE,  ptr + j , ARRAY_WIDHT );
////          OLEDData ( color );
//        }   // Printing out the rows an then the columns.
////    endWrite();
//}
void OLEDFillFromArray( unsigned char *array )
{
//  OLEDClearBuffer ( );

    unsigned char *ptr;
    unsigned char PageAddress, j;

    ptr = ( unsigned char * ) TEST;

     unsigned int ColumnAddress = 0x00;
    for ( PageAddress = 0x00; PageAddress < ARRAY_HEIGHT; PageAddress++ )
        for ( ColumnAddress = 0x00; ColumnAddress < ARRAY_WIDHT; ColumnAddress++ )
            *( ptr + ( ColumnAddress + PageAddress * ARRAY_WIDHT ) ) = array[ColumnAddress+PageAddress*ARRAY_WIDHT];//CLEAR;


// Set the Display with the 'color' Data.
    OLEDCommand ( OLED_LOW_COLUMN_START );      //set lower column address.
    OLEDCommand ( OLED_HIGH_COLUMN_START );     //set higher column address.
    OLEDCommand ( OLED_PAGE_START );            //set page address.

    for ( j = 0x00; j < ARRAY_HEIGHT; j++ )
    {
        OLEDCommand ( OLED_LOW_COLUMN_START );  //set lower column address.
        OLEDCommand ( OLED_HIGH_COLUMN_START ); //set higher column address.
        OLEDCommand ( OLED_PAGE_START + j );    //set page address.

////        for ( i = 0x00; i < ARRAY_WIDHT; i++ )
////            OLEDData ( *( ptr + ( j + i * ARRAY_WIDHT ) ) /*= color*/ );//MODIF VB
   //     I2C1WriteNInterrupt( OLED_ADDRESS, OLED_SET_START_LINE,  ptr + j , ARRAY_WIDHT );
//          OLEDData ( color );
        }   // Printing out the rows an then the columns.

    }   // OLEDFill ( ).

void OLEDClearBuffer ( void )
{
    unsigned char *ptr;
    unsigned char ColumnAddress, PageAddress;

    ptr = ( unsigned char * ) TEST;

    for ( PageAddress = 0x00; PageAddress < ARRAY_HEIGHT; PageAddress++ )
        for ( ColumnAddress = 0x00; ColumnAddress < ARRAY_WIDHT; ColumnAddress++ )
            *( ptr + ( ColumnAddress + PageAddress * ARRAY_WIDHT ) ) = 0;//CLEAR;

    }



//
//void OLEDUpdateDisplay ( unsigned char clear )
//{
//    unsigned char ColumnAddress, PageAddress;
//    unsigned char *ptr;
//
//    ptr = ( unsigned char * ) TEST;
//
//// Set Page Address.
//    OLEDCommand ( OLED_PAGE_ADDRESS );
//    OLEDCommand ( 0x00 );                   // Page start Address.
//    OLEDCommand ( 0x07 );                   // Page end Address.
//
//// Set Column Address.
//    OLEDCommand ( OLED_COLUMN_ADDRESS );
//    OLEDCommand ( 0x00 );                   // Column start address.
//    OLEDCommand ( 0x7F );                   // Column end address.
//
//    for ( PageAddress = 0x00; PageAddress < ARRAY_HEIGHT; PageAddress++ )
//    {
//// Set Page Address.
//        OLEDCommand ( OLED_PAGE_ADDRESS );
//        OLEDCommand ( 0x00 + PageAddress ); // Page start Address.
//        OLEDCommand ( 0x07 );               // Page end Address.
//// Set Column Address.
//        OLEDCommand ( OLED_COLUMN_ADDRESS );
//        OLEDCommand ( 0x00 );           // Column start address.
//        OLEDCommand ( 0x7F );           // Column end address.
//
//            I2C1WriteNInterrupt( OLED_ADDRESS, OLED_SET_START_LINE,  ptr + ( PageAddress * ARRAY_WIDHT), ARRAY_WIDHT );
//        }   // Printing out the rows an then the columns.
//
//    if ( clear )                // if erased DDRAM wanted on every Update.
//        OLEDClearBuffer ( );
//
//    }   // OLEDUpdateDisplay.



/********** prints out on the UART PORT the Arrays ********/
/********** For Testing Purposes ********/
void printArrayTest ( void )
{
    unsigned char i = 0;
    for (  i = 0; i < ARRAY_HEIGHT; i++ )
    {
        printf ( "\n" );
         unsigned char j = 0;
        for ( j = 0; j < ARRAY_WIDHT; j++ )
            printf ( "[%2X]", TEST [ i ] [ j ] );
        }
    printf ( "\n" );
    }



void OLEDInvertedDisplay ( unsigned char complement )
{
    if ( complement )
        OLEDCommand ( OLED_INVERTED_DISPLAY );
    else
        OLEDCommand ( OLED_NORMAL_DISPLAY );
    }




/*Continuous Horizontal Scroll Setup */
// The bigger the frequency, the longer the time to scroll.
void OLEDScrollToRight ( unsigned char start, unsigned char stop, unsigned char frequency  )
{
    OLEDCommand ( OLED_RIGHT_HORIZONTAL_SCROLL );
    OLEDCommand ( OLED_DUMMY_BYTE );                // A, Dummy byte.
    OLEDCommand ( start );                          // B, Define start page address.
    OLEDCommand ( frequency );                      // C, Frame frequency.
// Step D must be larger or equal to B.
    OLEDCommand ( stop );                           // D, Define end page address.
    OLEDCommand ( OLED_DUMMY_BYTE );                // E, Dummy byte (Set as 00h).
    OLEDCommand ( 0xFF );                           // F, Dummy byte (Set as ffh)
    OLEDCommand ( OLED_ACTIVATE_SCROLL );           // Start scrolling.
    }

void OLEDScrollToLeft ( unsigned char start, unsigned char stop, unsigned char frequency )
{
    OLEDCommand ( OLED_LEFT_HORIZONTAL_SCROLL );
    OLEDCommand ( OLED_DUMMY_BYTE );                // A, Dummy byte.
    OLEDCommand ( start );                          // B, Define start page address.
    OLEDCommand ( frequency );                      // C, Frame frequency.
// Step D must be larger or equal to B.
    OLEDCommand ( stop );                           // D, Define end page address.
    OLEDCommand ( OLED_DUMMY_BYTE );                // E, Dummy byte (Set as 00h).
    OLEDCommand ( 0xFF );                           // F, Dummy byte (Set as ffh)
    OLEDCommand ( OLED_ACTIVATE_SCROLL );           // Start scrolling.
    }


void OLEDScrollVerticalAndRight ( unsigned char start, unsigned char stop,
                                unsigned char frequency, unsigned char offset )
{
    OLEDCommand ( OLED_VERTICAL_RIGHT );
    OLEDCommand ( OLED_DUMMY_BYTE );                // A, Dummy byte.
    OLEDCommand ( start );                          // B, Define start page address.
    OLEDCommand ( frequency );                      // C, Frame frequency.
// Step D must be larger or equal to B.
    OLEDCommand ( stop );                           // D, Define end page address.
//No continuous vertical scrolling is available.
    OLEDCommand ( offset );                         // E, Vertical scrolling offset.
    OLEDCommand ( OLED_ACTIVATE_SCROLL );           // Start scrolling.
    }


void OLEDScrollVerticalAndLeft ( unsigned char start, unsigned char stop,
                                unsigned char frequency, unsigned char offset )
{
    OLEDCommand ( OLED_VERTICAL_LEFT );
    OLEDCommand ( OLED_DUMMY_BYTE );                // A, Dummy byte.
    OLEDCommand ( start );                          // B, Define start page address.
    OLEDCommand ( frequency );                      // C, Frame frequency.
// Step D must be larger or equal to B.
    OLEDCommand ( stop );                           // D, Define end page address.
//No continuous vertical scrolling is available.
    OLEDCommand ( offset );                         // E, Vertical scrolling offset.
    OLEDCommand ( OLED_ACTIVATE_SCROLL );           // Start scrolling.
    }


void OLEDScrollVertical ( unsigned char startRow, unsigned char stopRow, unsigned char frequency )
{
    OLEDCommand ( OLED_VERTICAL_SCROLL );
    OLEDCommand ( startRow );                   // A, Define start row address.
    OLEDCommand ( stopRow );                    // B, Define end row address.

//  OLEDCommand ( OLED_VERTICAL_LEFT );
//  OLEDCommand ( OLED_DUMMY_BYTE );            // A, Dummy byte.
//  OLEDCommand ( startRow );                   // B, Define start page address.
//  OLEDCommand ( frequency );                  // C, Frame frequency.
// Step D must be larger or equal to B.
//  OLEDCommand ( stopRow );                    // D, Define end page address.
//No continuous vertical scrolling is available.
//  OLEDCommand ( 0x01 );                       // E, Vertical scrolling offset.

    OLEDCommand ( OLED_ACTIVATE_SCROLL );       // Start scrolling.

    }


void OLEDStopScroll ( void )
{
    OLEDCommand ( OLED_DEACTIVATE_SCROLL );
    }




void OLEDLine ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, char color )
{
    int  x, y, addx, addy, dx, dy;
    int P;
    int i;

    dx = abs ( ( int ) ( x2 - x1 ) );
    dy = abs ( ( int ) ( y2 - y1 ) );
    x = x1;
    y = y1;

    if ( x1 > x2 )
        addx = -1;
    else
        addx = 1;

    if ( y1 > y2 )
        addy = -1;
    else
        addy = 1;

    if ( dx >= dy )
    {
        P = 2 * dy - dx;

        for ( i = 0; i <= dx; ++i )
        {
            OLEDPixel ( x, y, color );

            if ( P < 0 )
            {
                P += 2 * dy;
                x += addx;
                }
            else
            {
                P += 2 * dy - 2 * dx;
                x += addx;
                y += addy;
                }
            }
        }
    else
    {
        P = 2 * dx - dy;

        for ( i = 0; i <= dy; ++i )
        {
            OLEDPixel ( x, y, color );

            if ( P < 0 )
            {
                P += 2 * dx;
                y += addy;
                }
            else
            {
                P += 2 * dx - 2 * dy;
                x += addx;
                y += addy;
                }
            }
        }
    }


void OLEDRectangular ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char fill, char color )
{
    if ( fill )
    {
        unsigned char y, ymax;                          // Find the y min and max

        if ( y1 < y2 )
        {
            y = y1;
            ymax = y2;
            }
        else
        {
            y = y2;
            ymax = y1;
            }

        for ( ; y <= ymax; ++y )                    // Draw lines to fill the rectangle
            OLEDLine ( x1, y, x2, y, color );
        }
    else
    {
        OLEDLine ( x1, y1, x2, y1, color );      // Draw the 4 sides
        OLEDLine ( x1, y2, x2, y2, color );
        OLEDLine ( x1, y1, x1, y2, color );
        OLEDLine ( x2, y1, x2, y2, color );
        }
    }


void OLEDAreaFill ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char fill, char color )
{
    unsigned char y, ymax;                          // Find the y min and max

    if ( y1 < y2 )
    {
        y = y1;
        ymax = y2;
        }
    else
    {
        y = y2;
        ymax = y1;
        }

    for ( ; y <= ymax; ++y )                    // Draw lines to fill the rectangle
        OLEDLine ( x1, y, x2, y, color );
    }


void OLEDCircle ( int x, int y, int radius, char fill, char color )
{
    int a, b, P;
    a = 0x00;
    b = radius;
    P = 0x01 - radius;

    do
    {
        if( fill )
        {
            OLEDLine ( x - a, y + b, x + a, y + b, color );
            OLEDLine ( x - a, y - b, x + a, y - b, color );
            OLEDLine ( x - b, y + a, x + b, y + a, color );
            OLEDLine ( x - b, y - a, x + b, y - a, color );
            }
        else
        {
            OLEDPixel ( a + x, b + y, color );
            OLEDPixel ( b + x, a + y, color );
            OLEDPixel ( x - a, b + y, color );
            OLEDPixel ( x - b, a + y, color );
            OLEDPixel ( b + x, y - a, color );
            OLEDPixel ( a + x, y - b, color );
            OLEDPixel ( x - a, y - b, color );
            OLEDPixel ( x - b, y - a, color );
            }

        if ( P < 0 )
            P += 3 + 2 * a++;
        else
            P += 5 + 2 * ( a++ - b-- );

        } while ( a <= b );
    }


void OLEDBar ( int x1, int y1, int x2, int y2, int width, char color )
{
    signed int  x, y, addx, addy, j;
    signed int P, dx, dy, c1, c2;
    char i;

    dx = abs ( ( signed int ) ( x2 - x1 ) );
    dy = abs ( ( signed int ) ( y2 - y1 ) );
    x = x1;
    y = y1;
    c1 = -dx * x1 - dy * y1;
    c2 = -dx * x2 - dy * y2;

    if ( x1 > x2 )
    {
        addx = -1;
        c1 = -dx * x2 - dy * y2;
        c2 = -dx * x1 - dy * y1;
        }
    else
        addx = 1;

    if ( y1 > y2 )
    {
        addy = -1;
        c1 = -dx*x2 - dy*y2;
        c2 = -dx*x1 - dy*y1;
        }
    else
        addy = 1;

    if ( dx >= dy )
    {
        P = 2*dy - dx;

        for ( i = 0; i <= dx; ++i )
        {
            for ( j =- ( width / 2 ); j < width / 2 + width % 2; ++j )
            {
                if ( dx * x + dy * ( y + j ) + c1 >= 0 && dx * x + dy * ( y + j ) + c2 <= 0 )
                    OLEDPixel ( x, y + j, color );
                }

            if ( P < 0 )
            {
                P += 2 * dy;
                x += addx;
                }
            else
            {
                P += 2 * dy - 2 * dx;
                x += addx;
                y += addy;
                }
            }
        }
    else
    {
        P = 2 * dx - dy;

        for ( i = 0; i <= dy; ++i )
        {
            if ( P < 0 )
            {
                P += 2 * dx;
                y += addy;
                }
            else
            {
                P += 2 * dx - 2 * dy;
                x += addx;
                y += addy;
                }

            for ( j =- ( width / 2 ); j < width / 2 + width % 2; ++j )
            {
                if ( dx * x + dy * ( y + j ) + c1 >= 0 && dx * x + dy * ( y + j ) + c2 <= 0 )
                OLEDPixel ( x + j, y, color );
                }
            }
        }
    }


//void OLEDText2( int x, int y, char* textptr, int size, char red, char green, char blue, char redB, char greenB, char blueB )
//{
//    unsigned char i, j, k, l, m;                        // Loop counters
//    unsigned char pixelData [ 5 ];                      // Stores character data
//
//    for ( i = 0; textptr[i] != 0x00; ++i, ++x )     // Loop through the passed string
//    {
//        if ( textptr[i] < 'S') // Checks if the letter is in the first text array
//            memcpy( pixelData, TEXT[ textptr [ i ] - ' '], 5 );
//
//        else if(textptr[i] <= '~') // Check if the letter is in the second array
//        {
//            if ( ( textptr[i] >= 'a' ) && ( textptr[i] <= 'z' ) )
//                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' - 1 ], 5 );
//            else
//                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' ], 5 );
//            }
//
//        else
//            memcpy( pixelData, TEXT[ 0 ], 5 );      // Default to space
//
//        if ( x + 5 * size >= ARRAY_WIDHT )              // Performs character wrapping
//        {
//            x = 0;                                      // Set x at far left position
//            y += 7*size + 1;                            // Set y at next position down
//            }
//
//        for ( j = 0; j < 5; ++j, x += size )            // Loop through character byte data
//        {
//            for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
//            {
//                if ( BIT_TEST ( pixelData [ j ], k ) )  // Check if the pixel should be set
//                {
////                    for ( l = 0; l < size; ++l )        // The next two loops change the
////                    {                                   // character's size
////                        for ( m = 0; m < size; ++m )
////                        {
////                            Put_Pixel(x + m, 128-(y + k * size + l), red, green, blue);
////                            //OLEDPixel( x + m, y + k * size + l, color ); // Draws the pixel
////                            }
////                        }
////                    }
//                }
//            }
//        }
//    }

void OLEDText( int x, int y, char* textptr, int size, char red, char green, char blue )
{
    unsigned char i, j, k, l, m;                        // Loop counters
    unsigned char pixelData [ 5 ];                      // Stores character data


    for ( i = 0; textptr[i] != 0x00; ++i, ++x )     // Loop through the passed string
    {
        if ( textptr[i] < 'S') // Checks if the letter is in the first text array
            memcpy( pixelData, TEXT[ textptr [ i ] - ' '], 5 );

        else if(textptr[i] <= '~') // Check if the letter is in the second array
        {
            if ( ( textptr[i] >= 'a' ) && ( textptr[i] <= 'z' ) )
                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' - 1 ], 5 );
            else
                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' ], 5 );
            }

        else
            memcpy( pixelData, TEXT[ 0 ], 5 );      // Default to space

        if ( x + 5 * size >= ARRAY_WIDHT )              // Performs character wrapping
        {
            x = 0;                                      // Set x at far left position
            y += 7*size + 1;                            // Set y at next position down
            }

        for ( j = 0; j < 5; ++j, x += size )            // Loop through character byte data
        {
            for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
            {
                if ( BIT_TEST ( pixelData [ j ], k ) )  // Check if the pixel should be set
                {
                    for ( l = 0; l < size; ++l )        // The next two loops change the
                    {                                   // character's size
                        for ( m = 0; m < size; ++m )
                        {
                            Put_Pixel(x + m, 128-(y + k * size + l), red, green, blue);
                            //OLEDPixel( x + m, y + k * size + l, color ); // Draws the pixel
                            }
                        }
                    }
                }
            }
        }
    }


void OLEDText22( int x, int y, char* textptr, int size, unsigned char red, unsigned char green, unsigned char blue )
{
    unsigned char i, j, k, l, m;                        // Loop counters
    unsigned char pixelData [ 5 ];                      // Stores character data
    unsigned char PixelsToSend[5*size*3][ 8*size ];                      // Stores character data
    unsigned char PixelsToSend2D[5*size*3*8*size];
    unsigned char PixelsToSend1D[5*size*3];

    for ( i = 0; textptr[i] != 0x00; ++i)     // Loop through the passed string
    {
        if ( textptr[i] < 'S') // Checks if the letter is in the first text array
            memcpy( pixelData, TEXT[ textptr [ i ] - ' '], 5 );

        else if(textptr[i] <= '~') // Check if the letter is in the second array
        {
            if ( ( textptr[i] >= 'a' ) && ( textptr[i] <= 'z' ) )
                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' - 1 ], 5 );
            else
                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' ], 5 );
            }

        else
            memcpy( pixelData, TEXT[ 0 ], 5 );      // Default to space

//        if ( x + 5 * size >= ARRAY_WIDHT )              // Performs character wrapping
//        {
//            x = 0;                                      // Set x at far left position
//            y += 7*size + 1;                            // Set y at next position down
//            }

        for ( j = 0; j < 5; ++j)            // Loop through character byte data
        {
            for ( k = 0; k < 7; ++k )              // Loop through the vertical pixels
            {
                if ( BIT_TEST ( pixelData [ j ], k ) )  // Check if the pixel should be set
                {
                        PixelsToSend[3*j][k] = red;
                        PixelsToSend[3*j+1][k] = green;
                        PixelsToSend[3*j+2][k] = blue;

                }
                else
                {
                        PixelsToSend[3*j][k] = 0x00;
                        PixelsToSend[3*j+1][k] = 0x00;
                        PixelsToSend[3*j+2][k] = 0xff;
                }
            }
        }
        int cnt =0;
//        Set_LCD_for_write_at_X_Y2((i*size*5 +1) + x,(i*size*5 +1) + x+size*5, y, y+size*7)
         for ( k = 0; k < 7; ++k )              // Loop through the vertical pixels
            {
                 for ( j = 0; j < 5; ++j)            // Loop through character byte data
                  {
                     for(int jsize = 0; jsize < size; jsize++){
                         PixelsToSend1D [j*3*size + jsize*3] = PixelsToSend[j*3][k];
                         PixelsToSend1D [j*3*size + jsize*3 + 1] = PixelsToSend[j*3 + 1][k];
                         PixelsToSend1D [j*3*size + jsize*3 + 2] = PixelsToSend[j*3 + 2][k];
//                     PixelsToSend1D [(6-k)*5*3 + j*3] = PixelsToSend[j*3][k];
//                     PixelsToSend1D [(6-k)*5*3 + j*3 + 1] = PixelsToSend[j*3 + 1][k];
//                     PixelsToSend1D [(6-k)*5*3 + j*3 + 2] = PixelsToSend[j*3 + 2][k];
//                     PixelsToSend2D[(k)*5*3 + (4-j)*3] = PixelsToSend1D [j*3];
//                     PixelsToSend2D[(k)*5*3 + (4-j)*3 +1] = PixelsToSend1D [j*3+1];
//                     PixelsToSend2D[(k)*5*3 + (4-j)*3 +2] = PixelsToSend1D [j*3+2];
                     }
                  }
//                 cnt++;
//                 if(cnt==2){
//                     cnt=0;
//                     Set_LCD_for_write_at_X_Y2((i*size*5 +1) + x,(i*size*5 +1) + x+size*5, 128-(y+2*k), 128-(y+k+10));
//                     SPI_sendData40(PixelsToSend2D, 2*3*size*5);
//                 }
                 for(int ksize=0;ksize<size;ksize++){
                     Set_LCD_for_write_at_X_Y2((i*size*5 +1) + x,(i*size*5 +1) + x+size*5, 128-(y+k*size+ksize), 128-(y+k*size+ksize+1));
                     SPI_sendData40(PixelsToSend1D, 3*size*5);
                 }


            }
//                 Set_LCD_for_write_at_X_Y2((i*size*5 +1) + x,(i*size*5 +1) + x+size*5,  y, (y+size*7));
//                 SPI_sendData40(PixelsToSend2D, 3*size*5*size*7);

    }


//        for ( j = 0; j < 5; ++j, x += size )            // Loop through character byte data
//        {
//            for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
//            {
//                if ( BIT_TEST ( pixelData [ j ], k ) )  // Check if the pixel should be set
//                {
//                    for ( l = 0; l < size; ++l )        // The next two loops change the
//                    {                                   // character's size
//                        for ( m = 0; m < size; ++m )
//                        {
//                            Put_Pixel(x + m, 128-(y + k * size + l), red, green, blue);
//                            //OLEDPixel( x + m, y + k * size + l, color ); // Draws the pixel
//                            }
//                        }
//                    }
//                }
//            }
        }


//void OLEDText3( int x, int y, char* textptr, int size, char red, char green, char blue )
//{
//    unsigned char i, j, k, l, m;                        // Loop counters
//    unsigned char pixelData [ 5 ];                      // Stores character data
//    uint16_t TableauToSend[10*3][10*3]={0};
//    uint16_t count = 0;
//    int x2 = 0;
//    int y2 = 0;
//    for ( i = 0; textptr[i] != 0x00; ++i, ++x2 )     // Loop through the passed string
//    {
//        if ( textptr[i] < 'S') // Checks if the letter is in the first text array
//            memcpy( pixelData, TEXT[ textptr [ i ] - ' '], 5 );
//
//        else if(textptr[i] <= '~') // Check if the letter is in the second array
//        {
//            if ( ( textptr[i] >= 'a' ) && ( textptr[i] <= 'z' ) )
//                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' - 1 ], 5 );
//            else
//                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' ], 5 );
//            }
//
//        else
//            memcpy( pixelData, TEXT[ 0 ], 5 );      // Default to space
//
//        if ( x + x2 + 5 * size >= ARRAY_WIDHT )              // Performs character wrapping
//        {
//            x2 = 0;                                      // Set x at far left position
//            y2 += 7*size + 1;                            // Set y at next position down
//            }
//
//        for ( j = 0; j < 5; ++j, x2 += size )            // Loop through character byte data
//        {
//            for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
//            {
//                if ( BIT_TEST ( pixelData [ j ], k ) )  // Check if the pixel should be set
//                {
//                    for ( l = 0; l < size; ++l )        // The next two loops change the
//                    {                                   // character's size
//                        for ( m = 0; m < size; ++m )
//                        {
//                            TableauToSend[ k * size +l][j*5 + 3*m] = red;
//                            TableauToSend[ k * size +l][j*5 + 3*m + 1] = green;
//                            TableauToSend[ k * size +l][j*5 + 3*m + 2] = blue;
//                            count++;
//                        }
////                            DataToSend1[7*size*3] = {0x00};
////                           char DataToSend2[7*size*3] = {0x00};
////
////                            Put_Pixel3(x + m, 128-(y + k * size + l), red, green, blue, size);
//                            //OLEDPixel( x + m, y + k * size + l, color ); // Draws the pixel
////                            }
//                        }
//                    }
//                else{
//                    for ( l = 0; l < size; ++l )        // The next two loops change the
//                    {                                   // character's size
//                        for ( m = 0; m < size; ++m )
//                        {
//                            TableauToSend[ k * size +l][j*5 + 3*m] = red;
//                            TableauToSend[ k * size +l][j*5 + 3*m + 1] = green;
//                            TableauToSend[ k * size +l][j*5 + 3*m + 2] = blue;
//                            count++;
//
////                            TableauToSend[y2 + k * size +l][x2 + 3*m] = 0xff;
////                            TableauToSend[y2 + k * size +l][x2 + 3*m + 1] = 0x00;
////                            TableauToSend[y2 + k * size +l][x2 + 3*m + 2] = 0x00;
////                            count++;
//                        }
////                            DataToSend1[7*size*3] = {0x00};
////                           char DataToSend2[7*size*3] = {0x00};
////
////                            Put_Pixel3(x + m, 128-(y + k * size + l), red, green, blue, size);
//                            //OLEDPixel( x + m, y + k * size + l, color ); // Draws the pixel
////                            }
//                        }
//                }
//                }
//            }
////        for(int ii=0;ii<7*size-1+size-1;ii++){
////            unsigned char datSend[x2+3*size];
////            for(int ji=0;ji<x2+3*size;ji++){
////                datSend[ji] = TableauToSend[ii][ji];
////            }
////            Set_LCD_for_write_at_X_Y(x + x2, y + y2);
////            SPI_sendData40(datSend, x2+3*size);
//        for(int ii=0;ii<7*size-1+size-1;ii++){
//            unsigned char datSend[x2+3*size];
//            for(int ji=0;ji<x2+3*size;ji++){
//                datSend[ji] = TableauToSend[ii][ji];
//            }
//            Set_LCD_for_write_at_X_Y(x+x2, 116-y+ii);
//            SPI_sendData40(datSend, x2+3*size);
//
//        }
//
//        }
//
//
////        for(int ii=0;ii<7*size-1+size-1;ii++){
////            unsigned char datSend[x2+3*size];
////            for(int ji=0;ji<x2+3*size;ji++){
////                datSend[ji] = TableauToSend[ii][ji];
////            }
////            Set_LCD_for_write_at_X_Y(x, 116-y+ii);
////            SPI_sendData40(datSend, x2+3*size);
////
////        }
//    }

void OLEDText2( int x, int y, char* textptr, int size, char red, char green, char blue, char redB, char greenB, char blueB )
{
    unsigned char i, j, k, l, m;                        // Loop counters
    unsigned char pixelData [ 5 ];                      // Stores character data
    //unsigned char DataEnviar [8][ 300 ];                      // Stores character data
    //unsigned char DataEnviar2 [ 1000 ] = {0xff};                      // Stores character data
    uint16_t count =0;
    uint16_t xinit = x;
    uint16_t yinit = y;
//    for ( i = 0; textptr[i] != 0x00; ++i, ++x )     // Loop through the passed string
//    {
//        if ( textptr[i] < 'S') // Checks if the letter is in the first text array
//            memcpy( pixelData, TEXT[ textptr [ i ] - ' '], 5 );
//
//        else if(textptr[i] <= '~') // Check if the letter is in the second array
//        {
//            if ( ( textptr[i] >= 'a' ) && ( textptr[i] <= 'z' ) )
//                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' - 1 ], 5 );
//            else
//                memcpy( pixelData, TEXT2[ textptr [ i ] - 'S' ], 5 );
//            }
//
//        else
//            memcpy( pixelData, TEXT[ 0 ], 5 );      // Default to space
//
////        if ( x + 5 * size >= ARRAY_WIDHT )              // Performs character wrapping
////        {
////            x = 0;                                      // Set x at far left position
////            y += 7*size + 1;                            // Set y at next position down
////            }
//
////        for ( j = 0; j < 5; ++j, x += size )            // Loop through character byte data
////        {
////            for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
////            {
//                for ( j = 0; j < 5; ++j)            // Loop through character byte data
//                {
//                    for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
//                    {
//                        if ( BIT_TEST ( pixelData [ j ], k ) )  // Check if the pixel should be set
//                        {
//                            DataEnviar[k][i*105 + j*21] = red;
//                            DataEnviar[k][i*105 + j*21 + 1] = green;
//                            DataEnviar[k][i*105 + j*21 + 2] = blue;
//                            count++;
//                        }
//                        else{
//                            DataEnviar[k][i*105 + j*21] = redB;
//                            DataEnviar[k][i*105 + j*21 + 1] = greenB;
//                            DataEnviar[k][i*105 + j*21 + 2] = blueB;
//                            count++;
//                        }
//                    }
//                }
//                for ( k = 0; k <7 *size; ++k )              // Loop through the vertical pixels
//                {
//                    DataEnviar[k][i*105 + (5)*21 ] = redB;
//                    DataEnviar[k][i*105 + (5)*21 + 1] = greenB;
//                    DataEnviar[k][i*105 + (5)*21 + 2] = blueB;
//                    count++;
//                }
//    }

//    for ( k = 0; k <7; ++k )
//    {
//        for ( j = 0; j <count; ++j )
//        {
//        DataEnviar2[j] = DataEnviar[k][j];
//        Set_LCD_for_write_at_X_Y2(xinit, 127,128 - yinit - k*24 - j, 128 - yinit - j - 20 - k*24);
//        SPI_sendData3(DataEnviar2,count);
//        }
//
//    }

//    Set_LCD_for_write_at_X_Y2(64, 74
//                              ,64, 65);
//    SPI_sendData3(DataEnviar2,10*3);

//    Set_LCD_for_write_at_X_Y2(xinit, xinit + count ,128 - yinit, 128 - yinit - 7);
//    SPI_sendData3(DataEnviar2,count*7);




    }

unsigned long waitTimeMs;
unsigned char demoState=0;
void OLED_DemoStateMachine(void)
{
    switch(demoState)
    {
        case 0://Demo name
            OLEDLine( 2, 15, 112, 15, WHITE );
            OLEDText( 2, 20, OLED_Text_1, SIZE_TWO, 0xFF, 0xFF, 0xFF );
            OLEDText( 18, 37, OLED_Text_2, SIZE_TWO, 0xFF, 0xFF, 0xFF );
            OLEDLine( 2, 55, 112, 55, WHITE );
            OLEDUpdateDisplay ( DDGRAM_CLEAR );
            waitTimeMs=g_longTimeStamp;
            demoState++;
            break;
        case 1://WaitTime and print Text size 1
            if(waitTimeMs+3000<g_longTimeStamp)
            {
                OLEDText( 28, 5, OLED_Text_3, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDLine( 12, 15, 112, 58, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 2://WaitTime and print Line 1
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDLine( 22, 10, 102, 43, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 3://WaitTime and print Line 2
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDLine( 12, 58, 112, 58, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 4://WaitTime and print Line 3
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDLine( 12, 53, 112, 18, WHITE );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 5://WaitTime and print Rectangle 1
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDText( 28, 5, OLED_Text_6, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDRectangular( 2, 17, 82, 25, NO_FILL, WHITE );
                OLEDUpdateDisplay( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 6://WaitTime and print Rectangle 2
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDRectangular ( 12, 12, 42, 55, NO_FILL, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 7://WaitTime and print Rectangle 3
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDRectangular( 22, 22, 36, 35, NO_FILL, WHITE );
                OLEDUpdateDisplay( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 8://WaitTime and print Rectangle 4
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDRectangular ( 43, 23, 112, 61, NO_FILL, WHITE );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 9://WaitTime and print Circle 1
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDText( 28, 5, OLED_Text_7, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDCircle( 25, 35, 15, NO_FILL, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 10://WaitTime and print Circle 2
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDCircle( 65, 41, 25, NO_FILL, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 11://WaitTime and print Circle 3
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDCircle( 36, 51, 5, NO_FILL, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 12://WaitTime and print Circle 4
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDCircle( 56, 10, 15, NO_FILL, WHITE );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 13://WaitTime and print Bar 1
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDText( 28, 5, OLED_Text_8, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDBar( 2, 44, 78, 53, 5, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 14://WaitTime and print Bar 2
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDBar( 12, 14, 118, 53, 2, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 15://WaitTime and print Bar 3
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDBar( 2, 44, 115, 60, 3, WHITE );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 16://WaitTime and print Bar 4
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDBar( 22, 34, 88, 37, 4, WHITE );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 17://WaitTime and print Text size 1
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDText( 28, 5, OLED_Text_9, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDText( 28, 15, OLED_Text_10, SIZE_ONE, 0xFF, 0xFF, 0xFF );      //Size 1
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 18://WaitTime and print Text size 2
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDText( 32, 25, OLED_Text_11, SIZE_TWO, 0xFF, 0xFF, 0xFF );
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 19://WaitTime and print Text size 3
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDText ( 38, 40, OLED_Text_12, SIZE_THREE, 0xFF, 0xFF, 0xFF );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 20://WaitTime and clear screen and print scrolling text
            if(waitTimeMs+1000<g_longTimeStamp)
            {
                OLEDUpdateDisplay ( DDGRAM_NO_CLEAR );

                OLEDText( 38, 40, OLED_Text_13, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDScrollToRight ( 2, 120, OLED_2_FRAMES );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 21://WaitTime and and print scrolling text
            if(waitTimeMs+2000<g_longTimeStamp)
            {
                OLEDText ( 38, 40, OLED_Text_14, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDScrollToLeft ( 2, 120, OLED_2_FRAMES );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 22://WaitTime and and print scrolling text
            if(waitTimeMs+2000<g_longTimeStamp)
            {
                OLEDText( 38, 20, OLED_Text_15, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDScrollVerticalAndRight ( 2, 120, OLED_128_FRAMES, 60 );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 23://WaitTime and and print scrolling text
            if(waitTimeMs+2000<g_longTimeStamp)
            {
                OLEDText( 38, 20, OLED_Text_16, SIZE_ONE, 0xFF, 0xFF, 0xFF );
                OLEDScrollVerticalAndLeft ( 2, 120, OLED_128_FRAMES, 60 );
                OLEDUpdateDisplay ( DDGRAM_CLEAR );
                waitTimeMs=g_longTimeStamp;
                demoState++;
            }
            break;
        case 24://Stop scrolling
            if(waitTimeMs+2000<g_longTimeStamp)
            {
                OLEDStopScroll ( );
                waitTimeMs=g_longTimeStamp;
                demoState=0;
            }
            break;
    }
}



