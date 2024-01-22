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

#include "system.h"
#include "usb.h"
#include "adc.h"

/** CONFIGURATION Bits **********************************************/
 #pragma config WDTEN = OFF          //WDT disabled (enabled by SWDTEN bit)
 #pragma config PLLDIV = 3           //Divide by 3 (12 MHz oscillator input)
 #pragma config STVREN = ON          //stack overflow/underflow reset enabled
 #pragma config XINST = OFF          //Extended instruction set disabled
 #pragma config CPUDIV = OSC1        //No CPU system clock divide
 #pragma config CP0 = OFF            //Program memory is not code-protected
 #pragma config OSC = HSPLL          //HS oscillator, PLL enabled, HSPLL used by USB
 #pragma config FCMEN = OFF          //Fail-Safe Clock Monitor disabled
 #pragma config IESO = OFF           //Two-Speed Start-up disabled
 #pragma config WDTPS = 32768        //1:32768
 #pragma config DSWDTOSC = INTOSCREF //DSWDT uses INTOSC/INTRC as clock
 #pragma config RTCOSC = T1OSCREF    //RTCC uses T1OSC/T1CKI as clock
 #pragma config DSBOREN = OFF        //Zero-Power BOR disabled in Deep Sleep
 #pragma config DSWDTEN = OFF        //Disabled
 #pragma config DSWDTPS = 8192       //1:8,192 (8.5 seconds)
 #pragma config IOL1WAY = OFF        //IOLOCK bit can be set and cleared
 #pragma config MSSP7B_EN = MSK7     //7 Bit address masking
 #pragma config WPFP = PAGE_1        //Write Protect Program Flash Page 0
 #pragma config WPEND = PAGE_0       //Start protection at page 0
 #pragma config WPCFG = OFF          //Write/Erase last page protect Disabled
 #pragma config WPDIS = OFF          //WPFP[5:0], WPEND, and WPCFG bits ignored
 #pragma config T1DIG = ON           //Sec Osc clock source may be selected
 #pragma config LPT1OSC = OFF        //high power Timer1 mode

typedef enum
{
    SOFT_START_POWER_OFF,
    SOFT_START_POWER_START,
    SOFT_START_POWER_ENABLED,
    SOFT_START_POWER_READY
} SOFT_START_STATUS;

static SOFT_START_STATUS softStartStatus = SOFT_START_POWER_OFF;

/*********************************************************************
* Function: void SYSTEM_Initialize( SYSTEM_STATE state )
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input:  SYSTEM_STATE - the state to initialize the system into
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize( SYSTEM_STATE state )
{
    switch(state)
    {
        case SYSTEM_STATE_USB_START:
            //On the PIC18F46J50 Family of USB microcontrollers, the PLL will not power up and be enabled
            //by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
            //This allows the device to power up at a lower initial operating frequency, which can be
            //advantageous when powered from a source which is not gauranteed to be adequate for 48MHz
            //operation.  On these devices, user firmware needs to manually set the OSCTUNE<PLLEN> bit to
            //power up the PLL.
            {
                unsigned int pll_startup_counter = 600;
                OSCTUNEbits.PLLEN = 1;  //Enable the PLL and wait 2+ms until the PLL locks before enabling USB module
                while(pll_startup_counter--);
            }
            //Device switches over automatically to PLL output after PLL is locked and ready.

            
            BUTTON_Enable(BUTTON_USB_DEVICE_HID_CUSTOM);
			
            softStartStatus = SOFT_START_POWER_START;
            break;

        case SYSTEM_STATE_USB_SUSPEND: 
            //Configure device for low power consumption.  Turn off app power.
            AppPowerDisable();
            softStartStatus = SOFT_START_POWER_OFF;
            break;
            
        case SYSTEM_STATE_USB_RESUME:
            softStartStatus = SOFT_START_POWER_START;
            break;
    }
}

/*********************************************************************
* Function: void SYSTEM_Tasks(void)
*
* Overview: Runs system level tasks that keep the system running
*
* PreCondition: System has been initalized with SYSTEM_Initialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
void SYSTEM_Tasks(void)
{
    switch(softStartStatus)
    {
        case SOFT_START_POWER_OFF:
            break;

        case SOFT_START_POWER_START:
            if(USBGetDeviceState() != CONFIGURED_STATE)
            {
                break;
            }
            
            AppPowerEnable();
            softStartStatus = SOFT_START_POWER_ENABLED;
            break;

        case SOFT_START_POWER_ENABLED:
            if(AppPowerReady() == true)
            {
                softStartStatus = SOFT_START_POWER_READY;
                LED_Enable(LED_USB_DEVICE_STATE);
                LED_Enable(LED_USB_DEVICE_HID_CUSTOM);
				
                ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
                ADC_Enable(ADC_CHANNEL_POTENTIOMETER);
            }
            break;
            
        case SOFT_START_POWER_READY:
            break;
    }
}

/*********************************************************************
* Function: bool SYSTEM_UserSelfWriteUnlockVerification(void)
*
* Overview: Self erase/writes to flash memory could potentially corrupt the
*   firmware of the application, if the unlock sequence is ever executed
*   unintentionally, or if the table pointer is pointing to an invalid
*   range (not inside the MSD volume range).  Therefore, in order to ensure
*   a fully reliable design that is suitable for mass production, it is strongly
*   recommended to implement several robustness checks prior to actually
*   performing any self erase/program unlock sequence.  See additional inline 
*   code comments.
*
* PreCondition: None
*
* Input:  None
*
* Output: true - self write allowed, false - self write not allowed.
*
********************************************************************/
bool SYSTEM_UserSelfWriteUnlockVerification(void)
{
    //Should verify that the voltage on Vdd/Vddcore is high enough to meet
    //the datasheet minimum voltage vs. frequency graph for the device.
    //If the microcontroller is "overclocked" (ex: by running at maximum rated
    //frequency, but then not suppling enough voltage to meet the datasheet
    //voltage vs. frequency graph), errant code execution could occur.  It is
    //therefore strongly recommended to check the voltage prior to performing a 
    //flash self erase/write unlock sequence.  If the voltage is too low to meet
    //the voltage vs. frequency graph in the datasheet, the firmware should not 
    //initiate a self erase/program operation, and instead it should either:
    //1.  Clock switch to a lower frequency that does meet the voltage/frequency graph.  Or,
    //2.  Put the microcontroller to Sleep mode.
    
    //The method used to measure Vdd and/or Vddcore will depend upon the 
    //microcontroller model and the module features available in the device, but
    //several options are available on many of the microcontrollers, ex:
    //1.  HLVD module
    //2.  WDTCON<LVDSTAT> indicator bit
    //3.  Perform ADC operation, with the VBG channel selected, using Vdd/Vss as 
    //      references to the ADC.  Then perform math operations to calculate the Vdd.
    //      On some micros, the ADC can also measure the Vddcore voltage, allowing
    //      the firmware to calculate the absolute Vddcore voltage, if it has already
    //      calculated and knows the ADC reference voltage.
    //4.  Use integrated general purpose comparator(s) to sense Vdd/Vddcore voltage
    //      is above proper threshold.
    //5.  If the micrcontroller implements a user adjustable BOR circuit, enable
    //      it and set the trip point high enough to avoid overclocking altogether.
    
    //Example pseudo code.  Exact implementation will be application specific.
    //Please implement appropriate code that best meets your application requirements.
    //if(GetVddcoreVoltage() < MIN_ALLOWED_VOLTAGE)
    //{
    //    ClockSwitchToSafeFrequencyForGivenVoltage();    //Or even better, go to sleep mode.
    //    return false;       
    //}    


    //Should also verify the TBLPTR is pointing to a valid range (part of the MSD
    //volume, and not a part of the application firmware space).
    //Example code for PIC18 (commented out since the actual address range is 
    //application specific):
    //if((TBLPTR > MSD_VOLUME_MAX_ADDRESS) || (TBLPTR < MSD_VOLUME_START_ADDRESS)) 
    //{
    //    return false;
    //}  

    return true;
}
			
			
#if(__XC8_VERSION < 2000)
    #define INTERRUPT interrupt
#else
    #define INTERRUPT __interrupt()
#endif

void INTERRUPT SYS_InterruptHigh(void)
{
    #if defined(USB_INTERRUPT)
        USBDeviceTasks();
    #endif
}