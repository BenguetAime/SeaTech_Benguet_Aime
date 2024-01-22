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

#include <xc.h>
#include "system.h"

#include "usb.h"
#include "fileio.h"


/** CONFIGURATION Bits **********************************************/
#pragma config PLLSEL   = PLL3X     // PLL Selection (3x clock multiplier)
#pragma config CFGPLLEN = OFF       // PLL Enable Configuration bit (PLL Disabled (firmware controlled))
#pragma config CPUDIV   = NOCLKDIV  // CPU System Clock Postscaler (CPU uses system clock (no divide))
#pragma config LS48MHZ  = SYS48X8   // Low Speed USB mode with 48 MHz system clock (System clock at 48 MHz, USB clock divider is set to 8)
#pragma config FOSC     = INTOSCIO  // Oscillator Selection (Internal oscillator)
#pragma config PCLKEN   = OFF       // Primary Oscillator Shutdown (Primary oscillator shutdown firmware controlled)
#pragma config FCMEN    = OFF       // Fail-Safe Clock Monitor (Fail-Safe Clock Monitor disabled)
#pragma config IESO     = OFF       // Internal/External Oscillator Switchover (Oscillator Switchover mode disabled)
#pragma config nPWRTEN  = OFF       // Power-up Timer Enable (Power up timer disabled)
#pragma config BOREN    = SBORDIS   // Brown-out Reset Enable (BOR enabled in hardware (SBOREN is ignored))
#pragma config BORV     = 190       // Brown-out Reset Voltage (BOR set to 1.9V nominal)
#pragma config nLPBOR   = ON        // Low-Power Brown-out Reset (Low-Power Brown-out Reset enabled)
#pragma config WDTEN    = SWON      // Watchdog Timer Enable bits (WDT controlled by firmware (SWDTEN enabled))
#pragma config WDTPS    = 32768     // Watchdog Timer Postscaler (1:32768)
#pragma config CCP2MX   = RC1       // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN   = OFF       // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config T3CMX    = RC0       // Timer3 Clock Input MUX bit (T3CKI function is on RC0)
#pragma config SDOMX    = RC7       // SDO Output MUX bit (SDO function is on RC7)
#pragma config MCLRE    = ON        // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled)
#pragma config STVREN   = ON        // Stack Full/Underflow Reset (Stack full/underflow will cause Reset)
#pragma config LVP      = OFF       // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT    = OFF       // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)
#pragma config XINST    = OFF       // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled)
#pragma config CP0      = OFF       // Block 0 Code Protect (Block 0 is not code-protected)
#pragma config CP1      = OFF       // Block 1 Code Protect (Block 1 is not code-protected)
#pragma config CP2      = OFF       // Block 2 Code Protect (Block 2 is not code-protected)
#pragma config CP3      = OFF       // Block 3 Code Protect (Block 3 is not code-protected)
#pragma config CPB      = OFF       // Boot Block Code Protect (Boot block is not code-protected)
#pragma config CPD      = OFF       // Data EEPROM Code Protect (Data EEPROM is not code-protected)
#pragma config WRT0     = OFF       // Block 0 Write Protect (Block 0 (0800-1FFFh) is not write-protected)
#pragma config WRT1     = OFF       // Block 1 Write Protect (Block 1 (2000-3FFFh) is not write-protected)
#pragma config WRT2     = OFF       // Block 2 Write Protect (Block 2 (04000-5FFFh) is not write-protected)
#pragma config WRT3     = OFF       // Block 3 Write Protect (Block 3 (06000-7FFFh) is not write-protected)
#pragma config WRTC     = OFF       // Configuration Registers Write Protect (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB     = OFF       // Boot Block Write Protect (Boot block (0000-7FFh) is not write-protected)
#pragma config WRTD     = OFF       // Data EEPROM Write Protect (Data EEPROM is not write-protected)
#pragma config EBTR0    = OFF       // Block 0 Table Read Protect (Block 0 is not protected from table reads executed in other blocks)
#pragma config EBTR1    = OFF       // Block 1 Table Read Protect (Block 1 is not protected from table reads executed in other blocks)
#pragma config EBTR2    = OFF       // Block 2 Table Read Protect (Block 2 is not protected from table reads executed in other blocks)
#pragma config EBTR3    = OFF       // Block 3 Table Read Protect (Block 3 is not protected from table reads executed in other blocks)
#pragma config EBTRB    = OFF       // Boot Block Table Read Protect (Boot block is not protected from table reads executed in other blocks)



/*********************************************************************
* Function: void SYSTEM_Initialize(void)
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
void SYSTEM_Initialize(void)
{
    //Configure oscillator settings for clock settings compatible with USB
    //operation.  Note: Proper settings depends on USB speed (full or low).
    #if(USB_SPEED_OPTION == USB_FULL_SPEED)
        OSCTUNE = 0x80; //3X PLL ratio mode selected
        OSCCON = 0x70;  //Switch to 16MHz HFINTOSC
        OSCCON2 = 0x10; //Enable PLL, SOSC, PRI OSC drivers turned off
        while(OSCCON2bits.PLLRDY != 1);   //Wait for PLL lock
        ACTCON = 0x90;  //Enable active clock tuning for USB operation
    #endif
   
   
//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN;
    #endif

//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2"
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile.h, and that an appropriate I/O pin has been mapped
//	to it in HardwareProfile.h.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;
    #endif

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
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
