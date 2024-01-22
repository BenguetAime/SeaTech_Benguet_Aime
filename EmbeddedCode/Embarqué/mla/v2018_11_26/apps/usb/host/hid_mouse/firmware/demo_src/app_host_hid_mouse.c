/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

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
#include "usb.h"
#include "usb_host_hid.h"

#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// *****************************************************************************
// *****************************************************************************
// Type definitions
// *****************************************************************************
// *****************************************************************************

typedef enum _APP_STATE
{
    DEVICE_NOT_CONNECTED,
    WAITING_FOR_DEVICE,
    DEVICE_CONNECTED, /* Device Enumerated  - Report Descriptor Parsed */
    GET_INPUT_REPORT, /* perform operation on received report */
    INPUT_REPORT_PENDING,
    ERROR_REPORTED
} MOUSE_STATE;


typedef struct
{
    MOUSE_STATE state;
    bool inUse;

    uint8_t *buffer;
    uint16_t size;
    uint16_t pollRate;
    uint8_t address;

    struct
    {       
        struct
        {
            HID_DATA_DETAILS details;
            HID_USER_DATA_SIZE data[5];
        } parsed;
    } deflection;

    struct
    {
        struct
        {
            HID_DATA_DETAILS details;
            HID_USER_DATA_SIZE data[20];
        } parsed;
    } buttons;

} MOUSE;


// *****************************************************************************
// *****************************************************************************
// Local Variables
// *****************************************************************************
// *****************************************************************************

static MOUSE mouse;

// *****************************************************************************
// *****************************************************************************
// Local Function Prototypes
// *****************************************************************************
// *****************************************************************************
static void App_ProcessInputReport(void);

// *****************************************************************************
// *****************************************************************************
// Functions
// *****************************************************************************
// *****************************************************************************


/*********************************************************************
* Function: static void APP_HostHIDTimerHandler(void)
*
* Overview: starts a new request for a report if a device is connected
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
static void APP_HostHIDTimerHandler(void)
{
    if(mouse.state == DEVICE_CONNECTED)
    {
         mouse.state = GET_INPUT_REPORT;
    }
}

/*********************************************************************
* Function: void APP_HostHIDMouseInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostHIDMouseInitialize()
{
    mouse.state = DEVICE_NOT_CONNECTED;
    mouse.inUse = false;
    mouse.buffer = NULL;
    mouse.address = 0;
}

/*********************************************************************
* Function: void APP_HostHIDMouseTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized via
*   the APP_HostHIDMouseInitialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_HostHIDMouseTasks()
{
    uint8_t error;
    uint8_t count;
    
    if(mouse.address == 0)
    {
        mouse.address = USBHostHIDDeviceDetect();
    }
    else
    {
        if(USBHostHIDDeviceStatus(mouse.address) == USB_HID_DEVICE_NOT_FOUND)
        {
            mouse.state = DEVICE_NOT_CONNECTED;
            mouse.address = 0;
            mouse.inUse = false;

            if(mouse.buffer != NULL)
            {
                free(mouse.buffer);
                mouse.buffer = NULL;
            }
        }      
    }
    
    switch(mouse.state)
    {
        case DEVICE_NOT_CONNECTED:
            LED_Off(LED_USB_HOST_HID_MOUSE_DEVICE_READY);
            PRINT_ClearScreen();
            PRINT_String("Attach mouse\r\n", 17);
            mouse.state = WAITING_FOR_DEVICE;
            break;
            
        case WAITING_FOR_DEVICE:
            if( (mouse.address != 0) &&
                (USBHostHIDDeviceStatus(mouse.address) == USB_HID_NORMAL_RUNNING)
              ) /* True if report descriptor is parsed with no error */
            {
                SYSTEM_Initialize(SYSTEM_STATE_USB_HOST_HID_MOUSE);
                PRINT_ClearScreen();
                PRINT_String("L:0 R:0\r\nX:0x00 Y:0x00\r\n", 24);
                
                mouse.state = DEVICE_CONNECTED;
                TIMER_RequestTick(&APP_HostHIDTimerHandler, 10);
            }
            break;
            
        case DEVICE_CONNECTED:
            LED_On(LED_USB_HOST_HID_MOUSE_DEVICE_READY);
            break;

        case GET_INPUT_REPORT:
            if(USBHostHIDRead( 
                                mouse.address,
                                mouse.deflection.parsed.details.reportID,
                                mouse.deflection.parsed.details.interfaceNum,
                                mouse.deflection.parsed.details.reportLength,
                                mouse.buffer
                              )
              )
            {
                /* Host may be busy/error -- keep trying */
            }
            else
            {
                mouse.state = INPUT_REPORT_PENDING;
            }
            break;

        case INPUT_REPORT_PENDING:
            if(USBHostHIDReadIsComplete(mouse.address, &error, &count))
            {
                if(error || (count != mouse.deflection.parsed.details.reportLength))
                {
                    mouse.state = DEVICE_CONNECTED;
                }
                else
                {
                    mouse.state = DEVICE_CONNECTED;

                    App_ProcessInputReport();
                }
            }
            break;

        case ERROR_REPORTED:
            break;

        default:
            break;

    }
}

/****************************************************************************
  Function:
    BOOL USB_HID_DataCollectionHandler(void)
  Description:
    This function is invoked by HID client , purpose is to collect the
    details extracted from the report descriptor. HID client will store
    information extracted from the report descriptor in data structures.
    Application needs to create object for each report type it needs to
    extract.
    For ex: HID_DATA_DETAILS mouse.report.modifier.details;
    HID_DATA_DETAILS is defined in file usb_host_hid_appl_interface.h
    Each member of the structure must be initialized inside this function.
    Application interface layer provides functions :
    USBHostHID_ApiFindBit()
    USBHostHID_ApiFindValue()
    These functions can be used to fill in the details as shown in the demo
    code.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    true    - If the report details are collected successfully.
    false   - If the application does not find the the supported format.

  Remarks:
    This Function name should be entered in the USB configuration tool
    in the field "Parsed Data Collection handler".
    If the application does not define this function , then HID client
    assumes that Application is aware of report format of the attached
    device.
***************************************************************************/
bool APP_HostHIDMouseReportParser(void)
{
    uint8_t NumOfReportItem = 0;
    uint8_t i;
    USB_HID_ITEM_LIST* pitemListPtrs;
    USB_HID_DEVICE_RPT_INFO* pDeviceRptinfo;
    HID_REPORTITEM *reportItem;
    HID_USAGEITEM *hidUsageItem;
    uint8_t usageIndex;
    uint8_t reportIndex;
    uint8_t usageItem;
    bool foundX = false;
    bool foundY = false;

    /* The keyboard is already in use. */
    if(mouse.inUse == true)
    {
        return false;
    }

    pDeviceRptinfo = USBHostHID_GetCurrentReportInfo(); // Get current Report Info pointer
    pitemListPtrs = USBHostHID_GetItemListPointers();   // Get pointer to list of item pointers

    /* Find Report Item Index for Modifier Keys */
    /* Once report Item is located , extract information from data structures provided by the parser */
    NumOfReportItem = pDeviceRptinfo->reportItems;
    for(i=0;i<NumOfReportItem;i++)
    {
        reportItem = &pitemListPtrs->reportItemList[i];
        if((reportItem->reportType==hidReportInput) && (reportItem->dataModes == (HIDData_Variable|HIDData_Relative))&&
           (reportItem->globals.usagePage==USB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS))
        {
            if( (foundX == false) && (foundY == false) )
            {
                usageIndex = reportItem->firstUsageItem;

                for(usageItem = 0; usageItem < reportItem->usageItems; usageItem++, usageIndex++)
                {
                    hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];

                    if(hidUsageItem->usage == USB_HID_GENERIC_DESKTOP_X)
                    {
                        foundX = true;
                    }

                    if(hidUsageItem->usage == USB_HID_GENERIC_DESKTOP_Y)
                    {
                        foundY = true;
                    }
                }

                reportIndex = reportItem->globals.reportIndex;
                mouse.deflection.parsed.details.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
                mouse.deflection.parsed.details.reportID = (uint8_t)reportItem->globals.reportID;
                mouse.deflection.parsed.details.bitOffset = (uint8_t)reportItem->startBit;
                mouse.deflection.parsed.details.bitLength = (uint8_t)reportItem->globals.reportsize;
                mouse.deflection.parsed.details.count=(uint8_t)reportItem->globals.reportCount;
                mouse.deflection.parsed.details.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
            }

        }
        else if((reportItem->reportType==hidReportInput) && (reportItem->dataModes == HIDData_Variable)&&
           (reportItem->globals.usagePage==USB_HID_USAGE_PAGE_BUTTON))
        {
            /* Now make sure usage Min & Max are as per application */
            usageIndex = reportItem->firstUsageItem;
            hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];

            reportIndex = reportItem->globals.reportIndex;
            mouse.buttons.parsed.details.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
            mouse.buttons.parsed.details.reportID = (uint8_t)reportItem->globals.reportID;
            mouse.buttons.parsed.details.bitOffset = (uint8_t)reportItem->startBit;
            mouse.buttons.parsed.details.bitLength = (uint8_t)reportItem->globals.reportsize;
            mouse.buttons.parsed.details.count = (uint8_t)reportItem->globals.reportCount;
            mouse.buttons.parsed.details.interfaceNum = USBHostHID_ApiGetCurrentInterfaceNum();

        }
    }

    if(pDeviceRptinfo->reports == 1)
    {
        mouse.size = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
        mouse.buffer = (uint8_t*)malloc(mouse.size);
        mouse.pollRate = pDeviceRptinfo->reportPollingRate;

        if( (foundX == true) && (foundY == true) )
        {
            mouse.inUse = true;
        }
    }

    return(mouse.inUse);
}

/****************************************************************************
  Function:
    void App_ProcessInputReport(void)

  Description:
    This function processes input report received from HID device.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
static void App_ProcessInputReport(void)
{
    char printBuffer[30];
    
   /* process input report received from device */
    USBHostHID_ApiImportData(   mouse.buffer,
                                mouse.size,
                                mouse.deflection.parsed.data,
                                &mouse.deflection.parsed.details
                            );
    
    USBHostHID_ApiImportData(   mouse.buffer,
                                mouse.size,
                                mouse.buttons.parsed.data,
                                &mouse.buttons.parsed.details
                            );

    /* For the string that we will print out.  It will be of the following form:
     * X: 0xAA Y: 0xBB
     * L: 0 R: 1
     */
    sprintf(printBuffer, "L:%01X R:%01X\r\nX:0x%02X Y:0x%02X\r\n", mouse.buttons.parsed.data[0], mouse.buttons.parsed.data[1], mouse.deflection.parsed.data[0], mouse.deflection.parsed.data[1]);

    if(mouse.buttons.parsed.data[0] == 1)
    {
        LED_On(LED_USB_HOST_HID_MOUSE_LEFT_BUTTON_PRESSED);
    }
    else
    {
        LED_Off(LED_USB_HOST_HID_MOUSE_LEFT_BUTTON_PRESSED);
    }
    
    if(mouse.buttons.parsed.data[1] == 1)
    {
        LED_On(LED_USB_HOST_HID_MOUSE_RIGHT_BUTTON_PRESSED);
    }
    else
    {
        LED_Off(LED_USB_HOST_HID_MOUSE_RIGHT_BUTTON_PRESSED);
    }
    
    PRINT_String(printBuffer, 24);
}
