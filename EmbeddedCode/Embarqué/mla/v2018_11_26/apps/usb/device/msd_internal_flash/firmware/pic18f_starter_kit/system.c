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
#include "fileio.h"

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
    unsigned int pll_startup_counter;

    //On the PIC18F46J50 Family of USB microcontrollers, the PLL will not power up and be enabled
    //by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
    //This allows the device to power up at a lower initial operating frequency, which can be
    //advantageous when powered from a source which is not gauranteed to be adequate for 48MHz
    //operation.  On these devices, user firmware needs to manually set the OSCTUNE<PLLEN> bit to
    //power up the PLL.
    OSCTUNEbits.PLLEN = 1;  //Enable the PLL and wait 2+ms until the PLL locks before enabling USB module
    pll_startup_counter = 600;
    while(pll_startup_counter--);

    //Device switches over automatically to PLL output after PLL is locked and ready.

    //Configure all I/O pins to use digital input buffers.  The PIC18F87J50 Family devices
    //use the ANCONx registers to control this, which is different from other devices which
    //use the ADCON1 register for this purpose.
    ANCON0 = 0xFF;                  // Default all pins to digital
    ANCON1 = 0x3F;                  // Default all pins to digital

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
