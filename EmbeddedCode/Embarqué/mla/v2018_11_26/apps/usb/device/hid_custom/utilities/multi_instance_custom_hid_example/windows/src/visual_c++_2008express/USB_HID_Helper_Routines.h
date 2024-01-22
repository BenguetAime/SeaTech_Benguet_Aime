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



//USB HID Routines Header file
#if !defined(__USB_HID_HELPER_ROUTINES_H)
#define __USB_HID_HELPER_ROUTINES_H


//------------------------------------------------------------------------------------------------------------
//Headers and namespaces
//------------------------------------------------------------------------------------------------------------
#include <Windows.h>	//Definitions for various common and not so common types like DWORD, PCHAR, HANDLE, etc.
#include <setupapi.h>	//From Windows SDK.  Setupapi.h provides definitions needed for the SetupDixxx() 
						//functions, which are needed for finding the hardware on the system.
#include <Dbt.h>		//Need this for definitions of WM_DEVICECHANGE messages

using namespace System;
using namespace System::ComponentModel;
using namespace Microsoft::Win32;
using namespace System::Runtime::InteropServices;  //Need this to support "unmanaged" code.









//------------------------------------------------------------------------------------------------------------
//Public prototypes
//------------------------------------------------------------------------------------------------------------
BOOL HID_CheckIfDeviceIsPresent(WORD VendorID, WORD ProductID);
BOOL HID_CheckIfDeviceIsPresent(WORD VendorID, WORD ProductID, TCHAR* pDevicePathOutputBuffer, DWORD DevicePathBufferSize);
DWORD HID_GetUSBDeviceCountAndOneDevicePath(WORD VendorID, WORD ProductID, TCHAR* pDevicePath, DWORD* pPathBufferSize, DWORD actualBufferSize);
DWORD HID_GetUSBDeviceCountAndOneDevicePath(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD instanceIndexForPath, TCHAR* pDevicePath, DWORD* pPathBufferSize, DWORD actualBufferSize);
BOOL HID_AttemptInitDevice(WORD VendorID, WORD ProductID);
BOOL HID_AttemptInitDevice(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD currentInstanceIndex);
BOOL HID_AttemptInitDevice(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD currentInstanceIndex, PHANDLE pHIDReadHandle, PHANDLE pHIDWriteHandle);
BOOL HID_CloseHandles(PHANDLE pHIDReadHandle, PHANDLE pHIDWriteHandle);
BOOL HID_CloseInternalHandles(void);
void RegisterForDeviceChangeMessages(HANDLE formHandle, GUID* pGUID);

//------------------------------------------------------------------------------------------------------------
//Definitions and Constants
//------------------------------------------------------------------------------------------------------------










//------------------------------------------------------------------------------------------------------------
#endif	//end of #if !defined(__USB_HID_HELPER_ROUTINES_H)
