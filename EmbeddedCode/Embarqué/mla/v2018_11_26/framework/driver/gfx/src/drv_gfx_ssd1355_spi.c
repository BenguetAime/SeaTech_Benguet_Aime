/*******************************************************************************
 Display Driver for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_ssd1355_spi.c

  Summary:
    Display Driver for use with the Microchip Graphics Library.

  Description:
    This module implements the display driver for the following controllers:
    * Solomon Systech SSD1351 LCD controller driver
    This module implements the basic Display Driver Layer API required by the
    Microchip Graphics Library to enable, initialize and render pixels
    to the display controller.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2018 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system.h"
#include <stdint.h>

#if defined(GFX_USE_DISPLAY_CONTROLLER_SSD1351)

#include "driver/gfx/drv_gfx_display.h"
#include "driver/gfx/drv_gfx_ssd1355_spi.h"
#include "driver/spi/drv_spi.h"
#include "gfx/gfx_primitive.h"

// *****************************************************************************
/*  Function:
    void DRV_GFX_Initialize(void)

    Summary:
        Initialize the graphics display driver.

    Description:
        This function initializes the graphics display driver. This
        function will be called by the application.

*/
// *****************************************************************************
void DRV_GFX_Initialize(void)
{
    DisplayPowerConfig();
    DisplayPowerOff();
    
    DisplayConfig();
    DisplayCmdDataConfig();
    DisplayResetConfig();
    DisplayBacklightConfig();  
    
    DisplayResetEnable();
    __delay_us(30);          //t1 of datasheet specifies "at least 2us"
    DisplayResetDisable();
    
    __delay_us(30);          //t1 of datasheet specifies "at least 2us" before powering Vcc
    DisplayPowerOn();

    __delay_ms(1);          //wait for Vcc to stabilize
       
    DisplayDisable();
    DisplaySetData();
    
    GFX_ScreenClear();
    
    DisplaySetCommand();
    DisplayEnable();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0xAF);
    DisplayDisable();
    
    __delay_ms(300);
    
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_PixelPut(
                    uint16_t    x,
                    uint16_t    y)

    Summary:
        Draw the pixel on the given position.

    Description:
        This routine draws the pixel on the given position.
        The color used is the color set by the last call to
        GFX_ColorSet().

        If position is not on the frame buffer, then the behavior
        is undefined. If color is not set, before this function
        is called, the output is undefined.

*/
// *****************************************************************************

GFX_STATUS GFX_PixelPut(uint16_t x, uint16_t y)
{
    GFX_COLOR currentColor = GFX_ColorGet();
    uint16_t newX, newY;
    
    #if (DISP_ORIENTATION == 0)
        newX = DRV_GFX_X_OFFSET + x;
        newY = DRV_GFX_Y_OFFSET + y;
    #elif (DISP_ORIENTATION == 180)
        newX = (DISP_HOR_RESOLUTION-1+DRV_GFX_X_OFFSET)-x;
        newY = (DISP_VER_RESOLUTION-1+DRV_GFX_Y_OFFSET)-y;
    #else
        #error "Not supported"
    #endif
    
    DisplaySetCommand();
    DisplayEnable();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x15);
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newX);
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newX);

    DisplaySetCommand();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x75);
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newY);
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newY);
   
    DisplaySetCommand();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x5C); 
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor>>8));
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor&0xFF));
    
    DisplayDisable();
    
    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_COLOR GFX_PixelGet(
                    uint16_t    x,
                    uint16_t    y)

    Summary:
        Gets color of the pixel on the given position.

    Description:
        This routine gets the pixel on the given position.

        If position is not on the frame buffer, then the behavior
        is undefined.

*/
// *****************************************************************************
GFX_COLOR GFX_PixelGet(uint16_t x, uint16_t y)
{
    return 0;
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ScreenClear(void)

    Summary:
        Clears the screen to the currently set color (GFX_ColorSet()).

    Description:
        This function clears the screen with the current color and sets
        the line cursor position to (0, 0).

        If color is not set, before this function is called, the output
        is undefined.

        If the function returns GFX_STATUS_FAILURE, clearing is not
        yet finished. Application must call the function again to continue
        the clearing.

*/
// *****************************************************************************
GFX_STATUS GFX_ScreenClear(void)
{
    uint8_t    x, y;

    GFX_COLOR currentColor = GFX_ColorGet();
    
    DisplaySetCommand();
    DisplayEnable();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x15);
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, DRV_GFX_X_OFFSET);
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (DRV_GFX_X_OFFSET+DISP_HOR_RESOLUTION-1));
    
    DisplaySetCommand();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x75);
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, DRV_GFX_Y_OFFSET);
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (DRV_GFX_Y_OFFSET+DISP_VER_RESOLUTION-1));
    
    DisplaySetCommand();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x5C); 
    DisplaySetData();
    
    DisplayEnable();
    
    for(y = DRV_GFX_Y_OFFSET; y < (DRV_GFX_Y_OFFSET+DISP_VER_RESOLUTION); y++)
    {
        for(x = DRV_GFX_X_OFFSET; x < (DRV_GFX_X_OFFSET+DISP_HOR_RESOLUTION); x++)
        {   
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor>>8));
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor&0xFF));
        }
    }

    DisplayDisable();
    GFX_LinePositionSet(0, 0);
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BarDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a bar shape using the currently set fill
        style and color.

    Description:
        This function renders a bar shape with the currently set
        fill style (See GFX_FillStyleGet() and GFX_FillStyleSet() for
        details of fill style):
        - solid color - when the fill style is set to
                        GFX_FILL_STYLE_COLOR
        - alpha blended fill - when the fill style is set to
                        GFX_FILL_STYLE_ALPHA_COLOR.

        Any other selected fill style will be ignored and will assume
        a solid color fill will be used. The parameters left, top, right
        bottom will define the shape dimension.

        When fill style is set to GFX_FILL_STYLE_ALPHA_COLOR, the bar
        can also be rendered with an option to select the type of
        background.
        GFX_BACKGROUND_NONE - the bar will be rendered with no alpha blending.
        GFX_BACKGROUND_COLOR - the bar will be alpha blended with the
                               currently set background color.
        GFX_BACKGROUND_IMAGE - the bar will be alpha blended with the
                               currently set background image.
        GFX_BACKGROUND_DISPLAY_BUFFER - the bar will be alpha blended
                               with the current contents of the frame buffer.

        The background type is set by the GFX_BackgroundTypeSet().

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_BarDraw(
                        uint16_t left,
                        uint16_t top,
                        uint16_t right,
                        uint16_t bottom)
{
    uint16_t   x, y;
    
    uint16_t newLeft, newTop, newRight, newBottom;

    GFX_COLOR currentColor = GFX_ColorGet();
    
    #if (DISP_ORIENTATION == 0)
        newLeft = left + DRV_GFX_X_OFFSET;
        newRight = right + DRV_GFX_X_OFFSET;

        newTop = top + DRV_GFX_Y_OFFSET;
        newBottom = bottom + DRV_GFX_Y_OFFSET;

    #elif (DISP_ORIENTATION == 180)
        newLeft = (DISP_HOR_RESOLUTION-1+DRV_GFX_X_OFFSET)-right;
        newRight = (DISP_HOR_RESOLUTION-1+DRV_GFX_X_OFFSET)-left;

        newTop = (DISP_VER_RESOLUTION-1+DRV_GFX_Y_OFFSET)-bottom;
        newBottom = (DISP_VER_RESOLUTION-1+DRV_GFX_Y_OFFSET)-top;
    #else
        #error "Not supported"
    #endif

    DisplaySetCommand();
    DisplayEnable();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x15);
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newLeft);
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newRight);
    
    DisplaySetCommand();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x75);
    DisplaySetData();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newTop);
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newBottom);
    
    DisplaySetCommand();
    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x5C); 
    DisplaySetData();
    
    DisplayEnable();
    
    for(y = newTop; y < newBottom + 1; y++)
    {
        for(x = newLeft; x < newRight + 1; x++)
        {
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor>>8));
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor&0xFF));
        }
    }

    DisplayDisable();

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RenderStatusGet()

    Summary:
        This function returns the driver's status on rendering.

    Description:
        The controller has no hardware accelerated rendering routines.
        Therefore, this function will always return GFX_STATUS_READY_BIT.
        This means that the driver's rendering status is always ready.
        
        Valid values returned are GFX_STATUS_BUSY_BIT or
        GFX_STATUS_READY_BIT.

*/
// *****************************************************************************
GFX_STATUS_BIT GFX_RenderStatusGet(void)
{
    return GFX_STATUS_READY_BIT;
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_PixelArrayPut(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)

    Summary:
        Renders an array of pixels to the frame buffer.

    Description:
        This renders an array of pixels starting from the
        location defined by x and y with the length
        defined by numPixels. The rendering will be performed
        in the increasing x direction. If the length is more
        than 1 line of the screen, the rendering may continue
        to the next line depending on the way the memory of
        the display controller is arranged.
        For example, in some display controller, if the parameters are
           GFX_PixelArrayPut(0, 0, ptrToArray, (320*240));
        The array is rendered on all the pixels of a QVGA screen.

        This function also supports transparent color feature.
        When the feature is enabled the pixel with the transparent
        color will not be rendered and will be skipped.
        x and y must define a location within the display
        buffer.
 
*/
// *****************************************************************************
uint16_t GFX_PixelArrayPut(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)
{
    GFX_COLOR currentColor;
    uint16_t newX, newY;
    
    #if (DISP_ORIENTATION == 0)
        newX = DRV_GFX_X_OFFSET + x;
        newY = DRV_GFX_Y_OFFSET + y;
        
        DisplaySetCommand();
        DisplayEnable();
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x15);
        DisplaySetData();
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newX);
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, GFX_MaxXGet());

        DisplaySetCommand();
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x75);
        DisplaySetData();
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, newY);
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, GFX_MaxYGet());

        DisplaySetCommand();
        DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x5C); 
        DisplaySetData();

        while(numPixels--)
        {
            currentColor = *pPixel++;

            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor>>8));
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor&0xFF));
        }
    
    #elif (DISP_ORIENTATION == 180)
        newX = (DISP_HOR_RESOLUTION-1)-x;
        newY = (DISP_VER_RESOLUTION-1)-y;
        
        {
            uint16_t numPixelsPrime = 0;
            uint16_t currentX, currentY, startX, startY;
            
            /* Compute the rotated starting point so we can harness the
             * controllers auto-increment feature. */
            if(numPixels > (newX+1))
            {
                numPixelsPrime = numPixels - newX - 1;
                currentY = newY-(1+(numPixelsPrime/DISP_HOR_RESOLUTION));
                currentX = DISP_HOR_RESOLUTION-(numPixelsPrime%DISP_HOR_RESOLUTION);
            }
            else
            {
                currentX = newX-(numPixels-1);
                currentY = newY;
            }
            
            startX = currentX;
            startY = currentY;
            
            /* Print the first row of the rotated data (might be partial) */
            DisplaySetCommand();
            DisplayEnable();
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x15);
            DisplaySetData();
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, currentX + DRV_GFX_X_OFFSET);
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, GFX_MaxXGet() + DRV_GFX_X_OFFSET);

            DisplaySetCommand();
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x75);
            DisplaySetData();
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, currentY + DRV_GFX_Y_OFFSET);
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, GFX_MaxYGet() + DRV_GFX_Y_OFFSET);

            DisplaySetCommand();
            DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x5C); 
            DisplaySetData();

            pPixel += (numPixels-1);

            while(currentX < DISP_HOR_RESOLUTION)
            {
                currentColor = *pPixel--;
                numPixels--;

                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor>>8));
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor&0xFF));

                currentX++;
            }
            currentY++;

            /* Print the rest of the data.  Last row might be partial, but that
             * is okay since we will run out of data at the same time and end
             * the function. */
            if(numPixels != 0)
            {
                DisplaySetCommand();
                DisplayEnable();
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x15);
                DisplaySetData();
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0 + DRV_GFX_X_OFFSET);
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, GFX_MaxXGet() + DRV_GFX_X_OFFSET);

                DisplaySetCommand();
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x75);
                DisplaySetData();
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, currentY + DRV_GFX_Y_OFFSET);
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, GFX_MaxYGet() + DRV_GFX_Y_OFFSET);

                DisplaySetCommand();
                DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, 0x5C); 
                DisplaySetData();

                while(numPixels--)
                {
                    currentColor = *pPixel--;

                    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor>>8));
                    DRV_SPI_Put(DRV_GFX_SSD1355_SPI_CHANNEL, (uint8_t)(currentColor&0xFF));
                }
            }
        }

    #else
        #error "Not supported"
    #endif
        
    DisplayDisable();
    
    return (GFX_STATUS_SUCCESS);
}

#endif

