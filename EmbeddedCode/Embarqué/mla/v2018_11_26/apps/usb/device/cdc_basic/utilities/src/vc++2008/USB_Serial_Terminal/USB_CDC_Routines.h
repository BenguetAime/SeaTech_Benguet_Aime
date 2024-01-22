//USB CDC Routines Header file
#if !defined(__USB_CDC_ROUTINES_H)
#define __USB_CDC_ROUTINES_H
/*******************************************************************************
Copyright 2015 Microchip Technology Inc. (www.microchip.com)

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




//------------------------------------------------------------------------------------------------------------
//Headers and namespaces
//------------------------------------------------------------------------------------------------------------
#include <Windows.h>	//Definitions for various common and not so common types like DWORD, PCHAR, HANDLE, etc.
#include <setupapi.h>	//From Windows SDK.  Setupapi.h provides definitions needed for the SetupDixxx() 
						//functions, which are needed for finding the hardware on the system.

//-----------------------------------------------------------------------------------------------------------------------------------
//NOTE: To build and link this code successfully, you must configure your project to include the setupapi.lib, which is distributed
//with the Microsoft Windows Driver Kit (WDK) (and probably also the platform SDK?).  Assuming the Windows 7 WDK (v7600.16385.1) is 
//used, it can be found under: C:\WinDDK\7600.16385.1\lib\win7\i386
//In MS VC++ 2008, this setting is under: Tools-->Options-->Projects and Solutions-->VC++ Directories-->Show directories for-->Library Files.  
//From here, add the path to the setupapi.lib at the bottom of the existing list of folder paths.
//You also need to let the linker know it should use the setupapi.lib file, under:
//Project-->(project) Properties-->Configuration Properties-->Linker-->Input-->Additional Dependencies-->add [setupapi.lib] to the list (without the square brackets).
//-----------------------------------------------------------------------------------------------------------------------------------



using namespace System;
using namespace Microsoft::Win32;





//------------------------------------------------------------------------------------------------------------
//Public prototypes
//------------------------------------------------------------------------------------------------------------
String^	USB_CheckIfPresentAndGetCOMString(WORD USBVendorID, WORD USBProductID, DWORD deviceInstanceIndex, bool deviceIsComposite, BYTE compositeInterfaceIndex);


//Note: For USB CDC device Open/Close/Read/Write API functions, use the standard Visual Studio provided "SerialPort" object.
//The USB CDC class device will enumerate and look to Windows just like a normal legacy COMxx serial port device.  The COMxx port
//can be opened and read/written using the same methods that one would use to open/read/write to a normal legacy RS232 serial port.
//Ex: From the ToolBox, drag the SerialPort onto your form.  Then in the code, open and begin using the COMxx port with something like:
/*----------------------------------------------------------------------------------------------------------------
//Try to find if the device is attached, and if so, fetch the COMxx string associated with the USB CDC device.
String^ CDCCOMString;
CDCCOMString = USB_CheckIfPresentAndGetCOMString(0x04D8, 0x000A, 0, false, 0);
if(CDCCOMString != "")
{
	//We got the COMxx string associated with the USB CDC device.  Now try to open the COMxx port.
	try
	{
		serialPort1->PortName = CDCCOMString;
		serialPort1->BaudRate = 57600;
		serialPort1->Open();

		//If no exception was thrown (causing the catch() to execute), then we must have opened the COMxx port successfully.
		//Okay to call the serialPort1->WriteLine(), serialPort1->ReadExisting(), serialPort1->Close() and other APIs/methods now.
		serialPort1->WriteLine("\r\n\r\n------Hello World!!!-----");
	}
	catch(...){}
}
----------------------------------------------------------------------------------------------------------------*/





//------------------------------------------------------------------------------------------------------------
//Definitions and Constants
//------------------------------------------------------------------------------------------------------------









//------------------------------------------------------------------------------------------------------------
#endif	//end of #if !defined(__USB_CDC_ROUTINES_H)
