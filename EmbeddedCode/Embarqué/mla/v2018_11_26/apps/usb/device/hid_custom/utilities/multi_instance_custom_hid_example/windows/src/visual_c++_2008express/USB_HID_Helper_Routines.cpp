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



//USB HID device related routines


//------------------------------------------------------------------------------------------------------------
//Headers and namespaces
//------------------------------------------------------------------------------------------------------------
#include "stdafx.h"		//This will include at least windows.h, setupapi.h, and USB_CDC_Routines.h, which are needed by this .cpp file

#include <Windows.h>
#include <Setupapi.h>
#include <Dbt.h>		//Need this for definitions of WM_DEVICECHANGE messages
#include "USB_HID_Helper_Routines.h"


using namespace System::Runtime::InteropServices;  





//------------------------------------------------------------------------------------------------------------
//Definitions and Constants
//------------------------------------------------------------------------------------------------------------
#ifdef UNICODE
	#define	SELECTED_CHAR_SET	Unicode
	#define STRING_CHARACTER_TYPE wchar_t
#else
	#define SELECTED_CHAR_SET	Ansi
	#define STRING_CHARACTER_TYPE char
#endif







//------------------------------------------------------------------------------------------------------------
//Private Prototypes
//------------------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------------------
//DLL Imports
//------------------------------------------------------------------------------------------------------------




//Returns a HDEVINFO type for a device information set (USB devices in
//our case).  We will need the HDEVINFO as in input parameter for calling many of
//the other SetupDixxx() functions.
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiGetClassDevs", CallingConvention=CallingConvention::Winapi)]		
extern "C" HDEVINFO  SetupDiGetClassDevsUM(
	LPGUID  ClassGuid,					//Input: Supply the class GUID here. 
	PCTSTR  Enumerator,					//Input: Use NULL here, not important for our purposes
	HWND  hwndParent,					//Input: Use NULL here, not important for our purposes
	DWORD  Flags);						//Input: Flags describing what kind of filtering to use.

//Gives us "PSP_DEVICE_INTERFACE_DATA" which contains the Interface specific GUID (different
//from class GUID).  We need the interface GUID to get the device path.
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiEnumDeviceInterfaces", CallingConvention=CallingConvention::Winapi)]				
extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInterfacesUM(
	HDEVINFO  DeviceInfoSet,			//Input: Give it the HDEVINFO we got from SetupDiGetClassDevs()
	PSP_DEVINFO_DATA  DeviceInfoData,	//Input (optional)
	LPGUID  InterfaceClassGuid,			//Input 
	DWORD  MemberIndex,					//Input: "Index" of the device you are interested in getting the path for.
	PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData);//Output: This function fills in an "SP_DEVICE_INTERFACE_DATA" structure.

//SetupDiDestroyDeviceInfoList() frees up memory by destroying a DeviceInfoList
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiDestroyDeviceInfoList", CallingConvention=CallingConvention::Winapi)]
extern "C" WINSETUPAPI BOOL WINAPI  SetupDiDestroyDeviceInfoListUM(			
	HDEVINFO  DeviceInfoSet);			//Input: Give it a handle to a device info list to deallocate from RAM.

//SetupDiEnumDeviceInfo() fills in an "SP_DEVINFO_DATA" structure, which we need for SetupDiGetDeviceRegistryProperty()
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiEnumDeviceInfo", CallingConvention=CallingConvention::Winapi)]
extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInfoUM(
	HDEVINFO  DeviceInfoSet,
	DWORD  MemberIndex,
	PSP_DEVINFO_DATA  DeviceInfoData);

//SetupDiGetDeviceRegistryProperty() gives us the hardware ID, which we use to check to see if it has matching VID/PID
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiGetDeviceRegistryProperty", CallingConvention=CallingConvention::Winapi)]
extern "C"	WINSETUPAPI BOOL WINAPI  SetupDiGetDeviceRegistryPropertyUM(
	HDEVINFO  DeviceInfoSet,
	PSP_DEVINFO_DATA  DeviceInfoData,
	DWORD  Property,
	PDWORD  PropertyRegDataType,
	PBYTE  PropertyBuffer,   
	DWORD  PropertyBufferSize,  
	PDWORD  RequiredSize);

//SetupDiGetDeviceInterfaceDetail() gives us a device path, which is needed before CreateFile() can be used.
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiGetDeviceInterfaceDetail", CallingConvention=CallingConvention::Winapi)]
extern "C" BOOL SetupDiGetDeviceInterfaceDetailUM(
	HDEVINFO DeviceInfoSet,										//Input: Wants HDEVINFO which can be obtained from SetupDiGetClassDevs()
	PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,				//Input: Pointer to an structure which defines the device interface.  
	PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,	//Output: Pointer to a strucutre, which will contain the device path.
	DWORD DeviceInterfaceDetailDataSize,						//Input: Number of bytes to retrieve.
	PDWORD RequiredSize,										//Output (optional): Te number of bytes needed to hold the entire struct 
	PSP_DEVINFO_DATA DeviceInfoData);							//Output

//Note: This function is only supported in Windows Server 2003 and later (not implemented in XP).
//It is uncertain if this function could be used to re-initialize a "Status: Attached but broken." device.
[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiRestartDevices", CallingConvention=CallingConvention::Winapi)]					
extern "C" BOOL WINAPI SetupDiRestartDevicesUM(
	HDEVINFO  DeviceInfoSet,
	PSP_DEVINFO_DATA  DeviceInfoData);

[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiGetCustomDeviceProperty", CallingConvention=CallingConvention::Winapi)]
extern "C" BOOL SetupDiGetCustomDevicePropertyUM(
	HDEVINFO DeviceInfoSet,
	PSP_DEVINFO_DATA DeviceInfoData,
	PCTSTR CustomPropertyName,
	DWORD Flags,PDWORD PropertyRegDataType,
	PBYTE PropertyBuffer,DWORD PropertyBufferSize,
	PDWORD RequiredSize);

[DllImport("setupapi.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="SetupDiGetDeviceProperty", CallingConvention=CallingConvention::Winapi)]
extern "C" BOOL SetupDiGetDevicePropertyUM(
	HDEVINFO  DeviceInfoSet,
	PSP_DEVINFO_DATA  DeviceInfoData,
	CONST DEVPROPKEY  *PropertyKey,
	DEVPROPTYPE  *PropertyType,
	PBYTE  PropertyBuffer, OPTIONAL
	DWORD  PropertyBufferSize,
	PDWORD  RequiredSize, OPTIONAL
	DWORD  Flags);


	//Need this function for receiving all of the WM_DEVICECHANGE messages.  See MSDN documentation for
	//description of what this function does/how to use it. Note: name is remapped "RegisterDeviceNotificationUM" to
	//avoid possible build error conflicts.
	[DllImport("user32.dll" , CharSet = CharSet::SELECTED_CHAR_SET, EntryPoint="RegisterDeviceNotification", CallingConvention=CallingConvention::Winapi)]					
	extern "C" HDEVNOTIFY WINAPI RegisterDeviceNotificationUM(
		HANDLE hRecipient,
		LPVOID NotificationFilter,
		DWORD Flags);


//------------------------------------------------------------------------------------------------------------
//Global static variables
//------------------------------------------------------------------------------------------------------------
HANDLE privateHIDReadHandle = INVALID_HANDLE_VALUE;
HANDLE privateHIDWriteHandle = INVALID_HANDLE_VALUE;



//------------------------------------------------------------------------------------------------------------
//API Functions Implemented by this File
//------------------------------------------------------------------------------------------------------------


/*----------------------------------------------------------------------------------------------------------------------------
FUNCTION:	 DWORD HID_GetUSBDeviceCountAndOneDevicePath(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD instanceIndexForPath, TCHAR* pDevicePath, DWORD* pPathBufferSize, DWORD actualBufferSize)

DESCRIPTION: Checks if any USB device(s) with matching VID/PID (and GUID, if specified, and composite interface index, if
the device is composite) are currently attached to the system.  The function returns the total number of matching devices found,
and (optionally) the device path of a specified device instance index.

PARAMETERS:
WORD VendorID - Caller supplied 16-bit unsigned integer value that is the USB VID number for the USB device to find

WORD ProductID - Caller supplied 16-bit unsigned integer value that is the USB PID number for the USB device to find

BOOL deviceIsComposite - Caller supplied boolean TRUE/FALSE indicating if the device interface to be searched for is part of
       a composite USB device.  Use TRUE if the WinUSB interface is part of a multi-interface composite USB 
	   device, or, use FALSE if the WinUSB interface is implemented on a non-composite USB device.

WORD compositeInterfaceIndex - Caller supplied index number for the WinUSB interface of interest, on the composite device.
       This parameter is only relevant if the deviceIsComposite value is specified as TRUE.  When deviceIsComposite is FALSE,
	   this parameter may be set to NULL.  When deviceIsComposite is TRUE, the compositeInterfaceIndex value specified 
	   must match the WinUSB interface index number specified in the USB device's configuration descriptor set
	   implemented in the device firmware.  Zero is a valid compositeInterfaceIndex.

GUID* pGUID - Optional caller supplied pointer to a GUID (globally unique identifier, which is a 128-bit number) for the
       device's WinUSB function.  The GUID for the WinUSB function may be customized and is specified in the driver package 
	   .inf file (and optionally, also in the device firmware's MS OS descriptor set).  This parameter is optional, and may be
	   set to NULL.  If the pGUID is NULL, this function will search for and return the total count of USB devices with
	   matching VID/PID (and composite index, if deviceIsComposite = TRUE), regardless of their actual WinUSB GUIDs.  If instead
	   a valid pointer to a real GUID (matching the value in the WinUSB driver package .inf and device firmware MS OS descriptors),
	   then this function only returns the count of devices matching all of the VID + PID + GUID simultaneously. 

DWORD instanceIndexForPath - Caller supplied device index number, to fetch the path information for.  A computer may have more than
       one USB device attached to the same machine simultaneously, with matching VID/PID (and optional GUID).  If so, each device
	   instance will have a separate/unique device path string associated with it.  The instanceIndexForPath value specifies
	   what device instance the path information should be returned for, in the optional pDevicePath buffer.

	   If you only ever intend to write a PC application that can connect to only one instance of the hardware at a time (even if more than one
	   instance are physically attached), it is okay to use the instanceIndexForPath value of 1 (ex: find/attach to the only the first
	   device found, even if there are more).

	   If one device is attached to the machine, it will be located at instanceIndexForPath 1.  If four devices are attached, they will 
	   be initially located at index 1, 2, 3, and 4.  If the PC application fetches
	   the path info and connects up to all four device simultaneously, but then the user unplugs the hardware associated with instance indexs
	   2 and 4, the connections to instances 1 and 3 will remain open and valid.  However, if USB_GetUSBDeviceCountAndOneDevicePath() is called,
	   the return value will only be 2.  In this scenario, the former instance 3 device becomes effectively renumbered as instance 2 (ex:
	   if you call USB_GetUSBDeviceCountAndOneDevicePath() and get the path for instance index 2, it should match the formerly fetched path for
	   the previous instance index 3).  Passing instanceIndexForPath of 0 will result in instanceIndexForPath of 1 being used instead, since 0 itself
	   is not a valid value.

	   If subsequently the user plugs in five more devices, these new devices will also cause a re-numbering of the former instance indexes,
	   and may "appear" at instance indexes lower, higher, or intermingled with the former instance indexes, such that the final valid instance
	   indexes will start and 1 and be contiguous up to the maximum number of simultaneous matching devices attached to the system.  Ex: If
	   USB_GetUSBDeviceCountAndOneDevicePath() returns 7, then the valid instance indexes (at that exact moment) will be 1, 2, 3, 4, 5, 6, 7.

	   Although attaching new hardware devices (with matching VID/PID and optional GUID/composite interface index) will renumber the
	   indexes in unpredicatable ways, the paths of previously attached devices (that remain attached after the new devices are added)
	   should remain the same/unchanged.  Therefore, if a PC application will be connecting to multiple devices simultaneously, it is recommended
	   to save the paths for each instance index detected, and then after each plug and play event, check which device paths remain valid/unchanged,
	   which ones have disappeared (after looking at the paths of all current device instances), and which ones have newly appeared.  
	   
	   Presumably software should normally not attempt to parse the contents of path strings.  However path strings presumably contain 
	   embedded within them some information about what USB host controller number and USB host/hub port number that the USB device
	   is attached to.  Presumably, the path strings should be fairly static, so long as the location of the device doesn't change
	   (ex: due to the user unplugging the device and moving it to a different USB host/hub port number).

	   To uniquely and deterministically identify individual USB devices on a bus however, it is necessary for the device firmware
	   to implement a USB serial number descriptor, which must be different for every single instance of that hardware comming off the
	   production line.

Input/Output TCHAR* pDevicePath - Caller supplied pointer to a RAM buffer that has been allocated to receive the
       USB device path string, if the device is attached.  This parameter is optional, and
	   may be set = NULL, if you only want to detect if the device is present or not, or
	   are don't yet know how large of a buffer to allocate to hold the (variable length) 
	   device path string.

Input/Output DWORD* pPathBufferSize - Caller supplied pointer to a DWORD (32-bit unsigned int), which will get overwritten 
       with the required size (in number of bytes) for the pDevicePath buffer.  

Input DWORD actualBufferSize - The size of the caller's allocated buffer for receiving the path string, when pDevicePath is non-NULL.
       When pDevicePath is non-NULL, this function will only write up to the actualBufferSize number of bytes to the *pDevicePath.


	   
RETURN VALUE:
DWORD - 0 if no devices with matching VID/PID and GUID (if supplied) were found.  Otherwise, the return value is the number
of simultaneous device instances of hardware attached to the machine (with matching VID/PID and optionally GUID if supplied). 

If the function call succeeded, it should return ERROR_SUCCESS (upon checking the last value with GetLastError()).

If an internal error occurs (such as insufficient memory during malloc(), or an unexpected SetupAPI error), the return value
may be correct or may be less than the actual true number of devices attached to the system at that moment, and GetLastError()
should return something other than ERROR_SUCCESS.


REMARKS:
Since the device path string length is initially unknown (could vary by OS, host/hub port number, etc.), the caller should first
call this function with pDevicePath = NULL, but with a valid pPathBufferSize pointer.  Upon the first
call, if the function returns TRUE (indicating USB device detected), the pPathBufferSize destination DWORD
will get updated with the required RAM size for the pDevicePath buffer.  The caller should then use the
pPathBufferSize DWORD to malloc an equivalent sized RAM buffer, which can then be used on a second call
to this function, this time with a valid/non-NULL pDevicePath input.  This function will then return
the actual device path info (which can later be used by CreateFile() to open a handle to the device).

Alternatively, and for best performance, a pre-allocated buffer may be used, so long as the actualBufferSize is
correctly specified, to limit the returned data size to avoid buffer overflow.  It is possible in this case that
the actual path could be larger than the pre-allocated buffer, causing only a partial path to be returned.
If this occurs, the caller should re-size the buffer than call the function again to get the complete path information.

This function may be relatively slow executing (ex: milliseconds to return), with relatively high CPU utilization, as many 
of the SetupAPI function calls needed for finding the device are not especially fast.  Additionally, some of these 
Windows API functions (first implemented in the Windows 9x era) have known issues (such as returning illegal/improper 
results if you call the same functions back to back too quickly).  Therefore, for best performance and most reliable 
results, it is preferrable to pre-allocate a RAM buffer for retreiving the device path string, of at least 66000 bytes long,
prior to calling this function for the first time.  

According to the https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
the max path length is ~32767 characters (but with unicode = 65534 bytes) + a few bytes of not fully specified/deterministic overhead. 
It is conceivable that this max path length could expand in some future hypothetical OS, so it is still recommended to verify
the pPathBufferSize value was < actualBufferSize, prior to assuming that the retreived path is valid (and if it is not, calling
USB_GetUSBDeviceCountAndOneDevicePath() again with a new larger buffer, to get the full path).

Additionally, due to the unexpected behavior upon calling some of the SetupAPI functions back to back too quickly, it is recommended
to wait at least 1-2ms or more between consecutive calls to USB_GetUSBDeviceCountAndOneDevicePath().  It is also recommended to
assume that this function is not thread safe, and that it should only be called from a single thread at a time.
-----------------------------------------------------------------------------------------------------------------------------*/
DWORD HID_GetUSBDeviceCountAndOneDevicePath(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD instanceIndexForPath, TCHAR* pDevicePath, DWORD* pPathBufferSize, DWORD actualBufferSize)
{
	/* 
	Before we can "connect" our application to our USB embedded device, we must first find the device.
	A USB bus can have many devices simultaneously connected, so somehow we have to find our device, and only
	our device.  This is done with the Vendor ID (VID) and Product ID (PID).  Each USB product line should have
	a unique combination of VID and PID.  USB device interfaces also have Globally Unique Identifiers (GUIDs) 
	associated with them.  The USB device interface will have a GUID associated with it, which for some types 
	of devices is pre-defined, while for other classes (such as the custom/vendor class using WinUSB), the GUID 
	can be user configured through the values in the .inf file (or sometimes USB descriptors).

	Microsoft has created a number of functions which are useful for finding plug and play devices.  Documentation
	for each function used can be found in the MSDN library.  We will be using the following functions:

	SetupDiGetClassDevs()					//provided by setupapi.dll, which comes with Windows
	SetupDiEnumDeviceInterfaces()			//provided by setupapi.dll, which comes with Windows
	GetLastError()							//provided by kernel32.dll, which comes with Windows
	SetupDiDestroyDeviceInfoList()			//provided by setupapi.dll, which comes with Windows
	SetupDiGetDeviceInterfaceDetail()		//provided by setupapi.dll, which comes with Windows
	SetupDiGetDeviceRegistryProperty()		//provided by setupapi.dll, which comes with Windows
	malloc()								//part of C runtime library, msvcrt.dll?
	CreateFile()							//provided by kernel32.dll, which comes with Windows

	We will also be using the following unusual data types and structures.  Documentation can be found in
	the MSDN library:

	SP_DEVICE_INTERFACE_DATA
	SP_DEVICE_INTERFACE_DETAIL_DATA
	SP_DEVINFO_DATA
	HDEVINFO
	HANDLE
	GUID

	The ultimate objective of the following code is to:
	1.  Determine if any USB device(s) of matching VID/PID is attached to the machine.
	2.  If an instance is attached to the machine, get the device path string to the device.  The device path
	is needed when trying to call CreateFile() to get a handle to the device, which may in turn be used by
	separate device class specific API functions (such as WinUsb_Initialize() for vendor/custom class devices
	using the WinUSB driver), which can then be used to actually open the device for actual data transfers.

	Getting the device path is a multi-step round about process, which requires calling several of the 
	SetupDixxx() functions provided by setupapi.dll (which comes with/is a core part of Windows).
	*/

	//Globally Unique Identifier (GUID). Windows uses GUIDs to identify things.  This GUID needs to match
	//the GUID that is used in the .INF file used to install the WinUSB driver onto the system.
	//The INF file creates a register entry which associates a GUID with the WinUSB device.  In order for
	//a user mode application (such as this one) to find the USB device on the bus, it needs to known the
	//correct GUID that got put into the registry.
	HDEVINFO DeviceInfoTable = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDetailedInterfaceData = NULL;
	SP_DEVINFO_DATA DevInfoData;

	DWORD internalListIndex = 0;
	DWORD StatusLastError = 0;
	DWORD dwRegType;
	DWORD dwRegSize;
	DWORD StructureSize = 0;
	PBYTE pPropertyValueBuffer = NULL;
	bool MatchFound = false;
	DWORD ErrorStatus;
	BOOL BoolStatus = FALSE;
	DWORD LoopCounter = 0;
	DWORD UserBufferSize = actualBufferSize;	//Save the user's specified device path string buffer size.
	DWORD PathSize = 0;
	DWORD instanceCount = 0;
	String^ fullCompositeInterfaceSpecificDeviceID;
	String^ DeviceIDFromRegistry;

	//Check if the user is searching for instance index 0.  This isn't really a legal value, since a device will only be present
	//with instance index of 1 or higher.  However, in order to allow the user to call this function with 0 specified, we just
	//change the value 0 to 1 instead.
	if(instanceIndexForPath == 0)
	{
		instanceIndexForPath = 1;
	}


	//When searching for matching devices, we need to know what GUID to search for.  There are two valid options:
	//The "GUID_DEVINTERFACE_USB_DEVICE" GUID can be used for searching for any kind of USB device.  However,
	//WinUSB devices can have a separate GUID associated with them as specified in their respective driver package
	//.inf file (and MS OS Descriptors in the device firmware, if the firmware implements MS OS descriptors, which is recommended).

	//For composite devices, it is strongly preferrable to search for the device using the the WinUSB interface GUID from the .inf/MS 
	//OS descriptors in the firmware.  When you search for a composite WinUSB device using only the VID/PID and general purpose
	//GUID_DEVINTERFACE_USB_DEVICE, you will only be able to directly find the composite "parent" device (running the usbccgp.sys driver),
	//instead of the WinUSB interface that you are actually interested in communicating with.  One cannot connect up to the parent device
	//and expect to be able to use the WinUSB APIs to read/write to it.  However, finding the WinUSB interface in a composite device, from looking
	//at the parent device entry, is diffcult, particularly on OSes prior to Windows 7.  On Windows 7+, the composite device parent entry
	//in the registry will have a "Container ID" number, which can be cross referenced with other entries in the registry (that are also related
	//to that VID/PID, and will also have Container IDs associated with them), to find the actual WinUSB interface.

	//If the device is composite, and you search for the device with the application specific WinUSB interface GUID, this is preferrable,
	//since you will be able to find your desired WinUSB interface directly, without having to cross reference it against the parent entry
	//based on the Container ID.
	//GUID InterfaceClassGuid = {0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed};	//GUID_DEVINTERFACE_USB_DEVICE (common for any USB device) - used to find the USB device
	//GUID InterfaceClassGuid = {0x745a17a0, 0x74d3, 0x11d0, 0xb6, 0xfe, 0x00, 0xa0, 0xc9, 0x0f, 0x57, 0xda}; //The HID setup class/device manager entry GUID?
	GUID InterfaceClassGuid = {0x4d1e55b2, 0xf16f, 0x11cf, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}; //The HID class GUID?

	//Check if the user specified a custom GUID for their WinUSB device interface.  If so, use that GUID to find the device instead, since it will be more selective
	//than just looking for all USB devices of matching VID/PID.
	if(pGUID != NULL)
	{
		InterfaceClassGuid = *pGUID;
	}


	//Convert the user supplied VID/PID unsigned int numbers, into the equivalent Windows formatted
	//hardware ID string.  A Windows hardware ID string should look like this: "Vid_04d8&Pid_0062"
	//To create this string, it is necessary to use the "x4" formatting string values, to create 
	//hexadecimal formatted four digit string representations of the VID and PID values.  See also:
	//http://msdn.microsoft.com/en-us/library/dwhawy9k.aspx
	String^ DeviceIDToFind = "Vid_" + VendorID.ToString("x4") + "&Pid_" + ProductID.ToString("x4");
	DeviceIDToFind = DeviceIDToFind->ToLowerInvariant();	//Convert string content to all lower case.  Better for doing string contains comparisons.

	if(deviceIsComposite == TRUE)
	{
		//If the WinUSB interface is part of a composite device, then the full ID string becomes formatted
		//like: VID_04D8&PID_0207&MI_02   (where the "&MI_02" represents the interface index 2 from the USB descriptors in the device).
		fullCompositeInterfaceSpecificDeviceID = DeviceIDToFind + "&MI_" + compositeInterfaceIndex.ToString("x2");
		fullCompositeInterfaceSpecificDeviceID = fullCompositeInterfaceSpecificDeviceID->ToLowerInvariant();
	}


	//First populate a list of plugged in devices (by specifying "DIGCF_PRESENT"), which are of the specified class GUID. 
	DeviceInfoTable = SetupDiGetClassDevsUM((LPGUID)&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	

	//Now look through the list we just populated.  We are trying to see if any of them match our device. 
	while(true)
	{
		DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		if(SetupDiEnumDeviceInterfacesUM(DeviceInfoTable, NULL, &InterfaceClassGuid, internalListIndex, &DeviceInterfaceData))
		{
			ErrorStatus = GetLastError();
			if(ErrorStatus == ERROR_NO_MORE_ITEMS)	//Did we reach the end of the list of matching devices in the DeviceInfoTable?
			{	
				//We have looked through every item in the list with matching GUID.  We are all done now, go ahead and clean up/return.
				SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
				SetLastError(ERROR_SUCCESS);
				return instanceCount;
			}
		}
		else	//Else some other kind of unknown error ocurred...
		{
			ErrorStatus = GetLastError();
			SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
			if(ErrorStatus != ERROR_SUCCESS)
				SetLastError(ErrorStatus);
			else
				SetLastError(ERROR_BAD_UNIT);
			return instanceCount;
		}

		//Now retrieve the hardware ID from the registry.  The hardware ID contains the VID and PID, which we will then 
		//check to see if it is the correct device or not.

		//Initialize an appropriate SP_DEVINFO_DATA structure.  We need this structure for SetupDiGetDeviceRegistryProperty().
		DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		SetupDiEnumDeviceInfoUM(DeviceInfoTable, internalListIndex, &DevInfoData);

		//First query for the size of the hardware ID, so we can know how big a buffer to allocate for the data.
		SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwRegSize);

		//Allocate a buffer for the hardware ID.
		DWORD bufferSize = dwRegSize;
		pPropertyValueBuffer = (PBYTE) malloc (dwRegSize);
		if(pPropertyValueBuffer == NULL)	//if null, error, couldn't allocate enough memory
		{	//Can't really recover from this situation, just exit instead.
			SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return instanceCount;		
		}

		//Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a 
		//REG_MULTI_SZ (array of null terminated strings).  To find a device, we only care about the very first string in the
		//buffer, which will be the "device ID".  The device ID is a string which contains the VID and PID, in the example 
		//format "Vid_04d8&Pid_003f".
		SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, pPropertyValueBuffer, dwRegSize, NULL);



		STRING_CHARACTER_TYPE* pString = (STRING_CHARACTER_TYPE*)pPropertyValueBuffer;
		MatchFound = false;
		while(pString < (STRING_CHARACTER_TYPE*)(pPropertyValueBuffer + bufferSize))
		{
			//Convert from an unmanaged char (or wchar_t when UNICODE defined) RAM array to the managed String^ type
			DeviceIDFromRegistry = gcnew String((STRING_CHARACTER_TYPE*)pString);
			if(DeviceIDFromRegistry == "")
				break;
			pString += (DeviceIDFromRegistry->Length + 1);	//+1 is to account for null terminator at the end
			
			//Convert both strings to lower case.  This makes the code more robust/portable accross OS Versions
			DeviceIDFromRegistry = DeviceIDFromRegistry->ToLowerInvariant();	
			//DeviceIDToFind = DeviceIDToFind->ToLowerInvariant();	//Already did this at start of function (Save CPU cycles: doesn't need to be done every loop iteration).
			//Now check if the hardware ID we are looking at contains the correct VID/PID (and possibly interface index, if a composite device)
			MatchFound = DeviceIDFromRegistry->Contains(DeviceIDToFind);

			//If the device is composite, also try to verify it contains the correct "&MI_xx" (xx is a number, such as 01) suffix,
			//associated with the specific composite interface of interest.  However, we should only do this if we found the device based on
			//a custom/application specific GUID, since the parent device entry (found using the non-cusom/generic USB GUID) won't actually
			//have the suffix on it.
			if((deviceIsComposite == TRUE) && (pGUID != NULL))
			{
				MatchFound = DeviceIDFromRegistry->Contains(fullCompositeInterfaceSpecificDeviceID);
			}
			if(MatchFound == true)
				break;
		}

		free(pPropertyValueBuffer);		//No longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks
		pPropertyValueBuffer = NULL;


		if(MatchFound == true)
		{
			//Yay, we found a device with matching VID/PID and optionally GUID and optionally composite interface index.
			instanceCount++;

			//Check if the current instance we are looking at matches the caller's desired instance that they want the path
			//information for.  If so, retreive the path and pass it to the caller.
			if(instanceCount == instanceIndexForPath)
			{
				//Prior to opening a WinUSB interface handle to the device, it will be necessary
				//to get a device handle with CreateFile().  However, to get the device handle, one needs to get the
				//device path string for the interface on the device.
				//We can get the device path string by calling SetupDiGetDeviceInterfaceDetail(), however, we have to call
				//this function twice:  The first time to get the size of the required structure/buffer to hold the 
				//detailed interface data, then a second time to actually get the structure (after we have allocated 
				//enough memory for the structure.)

				//First call populates "StructureSize" with the required RAM bytes needed for the structure with the path.
				//Error checking note: You can't use the BOOL result from below call, since it will (correctly) claim "FALSE",
				//because the device interface detail pointer is NULL.  However, the function does succeed, and
				//does correctly return the required StructureSize value.  The MSDN documentation seems to claim
				//the GetLastError() would be ERROR_INSUFFICIENT_BUFFER in this case, but it doesn't seem to do 
				//this (observed on Win XP 32 bit, SP3, and instead reports ERROR_SUCCESS).
				SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, &DeviceInterfaceData, NULL, 0, &StructureSize, NULL);

				//Let the calling function know how much buffer size it needs to malloc or otherwise allocate for
				//the device path string return value.  
				//Note: The user's buffer only needs to be the size of the TCHAR[] array (which holds the path string),
				//and doesn't need to include the cbSize DWORD portion of the structure.
				if(StructureSize >= sizeof(DWORD))
				{
					PathSize = StructureSize - sizeof(DWORD);
				}
				if(pPathBufferSize != NULL)
				{
					*pPathBufferSize = PathSize;
				}

				//Check user's input parameters to make sure they are valid before we use them.  Also useful if the user 
				//was just trying to get the length of the device path string, and doesn't actually want the path yet.
				if((pDevicePath != NULL) && (UserBufferSize != 0))
				{
					//Check to see if the user's specified buffer was smaller than the required size for the entire
					//path data.  If so, only copy the number of path string bytes that will fit into the 
					//user's buffer.  Note: This can result in path truncation, if the user specified too small of
					//a buffer.  The calling function is therefore responsible for allocating a large enough buffer,
					//either by calling this function twice (once to get the required size, then malloc the necessary
					//RAM, then call it again with the *pPathBufferSize = (the required size for the whole path)), or
					//by assuming a large enough value from the start (which wouldn't be safe and isn't recommended).
					if(UserBufferSize < PathSize)
					{
						//Sets PathSize to the smaller of the two.  
						PathSize = UserBufferSize;
					}

					//Now use malloc to allocate ourselves some RAM to hold the (now known size) detailed interface data structure.
					pDetailedInterfaceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(malloc(StructureSize));		//Allocate enough memory for the whole strucutre
					if(pDetailedInterfaceData == NULL)	//if null, error, couldn't allocate enough memory
					{	
						//Can't really recover from this situation, just exit instead.
						SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
						SetLastError(ERROR_NOT_ENOUGH_MEMORY);
						return instanceCount;		
					}

					//Now call SetupDiGetDeviceInterfaceDetail() a second time to receive the detailed interface data structure
					//info, which contains the device path "string".  However, before doing this, must initialize the structure cbSize
					//parameter to the correct value.
					pDetailedInterfaceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
					if(SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, &DeviceInterfaceData, pDetailedInterfaceData, StructureSize, NULL, NULL) != TRUE)
					{
						//Some kind of unknown error occurred, that can't be readily rectified.
						//Just clean things up and return.  Save and Set the last error to the error code returned
						//by the failing SetupDiGetDeviceInterfaceDetail() call.
						DWORD lastErr = GetLastError();
						if(pDetailedInterfaceData != NULL)
							free(pDetailedInterfaceData);
						SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
						if(lastErr != ERROR_SUCCESS)
							SetLastError(lastErr);
						else
							SetLastError(ERROR_BAD_UNIT);
						return instanceCount;
					}

					//If we get to here, not only did we find the device based on VID and PID, but we
					//also successfully fetched the device path string from the OS.

					//Copy the device path out of the local buffer, into the user's supplied device path buffer.
					//Note: The PathSize is in number of bytes, while the pDetailedInterfaceData->DevicePath[]
					//array elements are in TCHARs.  The TCHAR can be either 1 byte or 2 bytes (when in UNICODE mode)
					//wide.  Therefore, need to terminate loop on: i < (PathSize / sizeof(TCHAR), rather than on i < PathSize.
					DWORD bytesToCopy = PathSize;
					//bytesToCopy = (PathSize / sizeof(TCHAR));
					//Copy lesser of: total size of path string, or, the caller's claimed buffer size.
					if(bytesToCopy > actualBufferSize)
					{
						bytesToCopy = actualBufferSize;
					}
					//Copy the data over to the caller's buffer
					memcpy(pDevicePath, &pDetailedInterfaceData->DevicePath[0], bytesToCopy);

					//Now need to free the RAM that got malloc'ed, to avoid potential memory leakage
					if(pDetailedInterfaceData != NULL)
					{
						free(pDetailedInterfaceData);	
					}

					//The caller now has the desired path string to the device instance index that they
					//were interested in.  However, keep on looping until we have searched through every device
					//in the list, in case there are more than one device attached with matching VID/PID (and optional GUID/composite interface index).
				}//if((pDevicePath != NULL) && (UserBufferSize != 0))

			}//if(instanceCount == instanceIndexForPath)
		}//if(MatchFound == true)

		internalListIndex++;	
		//Keep looping until we either find a device with matching VID and PID, or until we run out of devices to check.
		//However, just in case some unexpected error occurs, keep track of the number of loops executed.
		//If the number of loops exceeds a very large number, exit anyway, to prevent inadvertent infinite looping.
		LoopCounter++;
		if(LoopCounter >= 10000000)	//Surely there aren't more than 10 million devices attached to any forseeable PC...
		{
			SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);	//Clean up the old structure we no longer need.
			SetLastError(ERROR_INVALID_ADDRESS);
			return instanceCount;
		}

	}//end of while(true)
}


//Simplified function overload.  Assume device is non-composite, and we only care about the first device instance.
DWORD HID_GetUSBDeviceCountAndOneDevicePath(WORD VendorID, WORD ProductID, TCHAR* pDevicePath, DWORD* pPathBufferSize, DWORD actualBufferSize)
{
	return HID_GetUSBDeviceCountAndOneDevicePath(VendorID, ProductID, FALSE, 0, NULL, 1, pDevicePath, pPathBufferSize, actualBufferSize);
}


//Returns TRUE if at least one device of matching VID/PID is present.  Otherwise, FALSE.
BOOL HID_CheckIfDeviceIsPresent(WORD VendorID, WORD ProductID, TCHAR* pDevicePathOutputBuffer, DWORD DevicePathBufferSize)
{
	if(HID_GetUSBDeviceCountAndOneDevicePath(VendorID, ProductID, pDevicePathOutputBuffer, NULL, DevicePathBufferSize) > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//Function overload that just checks if one or more devices with matching VID/PID is present, without trying to open it or save the device path.
BOOL HID_CheckIfDeviceIsPresent(WORD VendorID, WORD ProductID)
{
	return HID_CheckIfDeviceIsPresent(VendorID, ProductID, NULL, 0);
}



//This function tries to find and open the device based on the provided VID/PID/instance/GUID (optional).  If successful it returns TRUE and the pHIDReadHandle and pHIDWriteHandle pointed to
//locations will get updated with open/valid handles.  Otherwise, it returns FALSE and sets the pointed to handles to INVALID_HANDLE_VALUE.
BOOL HID_AttemptInitDevice(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD currentInstanceIndex, PHANDLE pHIDReadHandle, PHANDLE pHIDWriteHandle)
{
	TCHAR pathString[66000];
	DWORD necessaryPathBufferSize;
	DWORD instancesFound;
	BOOL longPath = FALSE;
	DWORD errorStatus;
	TCHAR* pLongPathString = NULL;
	BOOL foundDeviceButCouldNotOpenIt = FALSE;
	DWORD savedError;
	HANDLE localReadHandle;
	HANDLE localWriteHandle;


	//Check if the caller passed bogus read/write handle pointers
	if((pHIDReadHandle == NULL) || (pHIDWriteHandle == NULL))
	{
		//Can't go on.  The whole point of this function is to open handles to the device and give the user the handles.
		return FALSE;
	}
	//Initialize the user's handles to the invalid value.  Will be updated with real value if we successfully open the device and open the interface handles.
	*pHIDReadHandle = INVALID_HANDLE_VALUE;
	*pHIDWriteHandle = INVALID_HANDLE_VALUE;


	//Check if the user is looking for instance index 0.  If so, make them look for instance 1 instead, since 0 is not a sensible value.
	if(currentInstanceIndex == 0)
	{
		currentInstanceIndex = 1;
	}


	//Try to get the device path and check how many devices are present.
	instancesFound = HID_GetUSBDeviceCountAndOneDevicePath(VendorID, ProductID, deviceIsComposite, compositeInterfaceIndex, pGUID, currentInstanceIndex, &pathString[0], &necessaryPathBufferSize, sizeof(pathString));

	//Check if there were 0 matching devices.  If so, no point in going on.
	if(instancesFound == 0)
	{
		return FALSE;
	}

	//Check to make sure we fetched the full path string, and not a partial path, due to pre-allocated RAM buffer size limitation.
	if(necessaryPathBufferSize > sizeof(pathString))
	{
		//We only fetched a partial path.  Must allocate RAM and call it again
		pLongPathString = (TCHAR*)malloc(necessaryPathBufferSize*2);
		if(pLongPathString == NULL)
		{
			return FALSE;
		}
		longPath = TRUE;

		instancesFound = HID_GetUSBDeviceCountAndOneDevicePath(VendorID, ProductID, deviceIsComposite, compositeInterfaceIndex, pGUID, currentInstanceIndex, pLongPathString, &necessaryPathBufferSize, necessaryPathBufferSize * 2);
	}


	//Check if the caller's specified index is is actually present.
	if(currentInstanceIndex >= instancesFound)
	{
		//Try to open the HID read/write interface handles
		localReadHandle = INVALID_HANDLE_VALUE;
		localWriteHandle = INVALID_HANDLE_VALUE;
		if(longPath == TRUE)
		{
			localReadHandle = CreateFile(pLongPathString, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		}
		else
		{
			localReadHandle = CreateFile(pathString, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		}
		//See if we opened the read handle successfully.
		errorStatus = GetLastError();
		if(errorStatus == ERROR_SUCCESS)
		{
			//Now try to open the write handle
			if(longPath == TRUE)
			{
				localWriteHandle = CreateFile(pLongPathString, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			}
			else
			{
				localWriteHandle = CreateFile(pathString, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			}
			//Check if the above CreateFile() was successful.
			errorStatus = GetLastError();
			if(errorStatus == ERROR_SUCCESS)
			{
				//Everything looks okay, but also make sure the handles look legit.
				if((localWriteHandle != INVALID_HANDLE_VALUE) && (localReadHandle != INVALID_HANDLE_VALUE))
				{
					//Both the read and write handles opened successfully.  Update the caller's variables with the new/valid handles.
					*pHIDReadHandle = localReadHandle;
					*pHIDWriteHandle = localWriteHandle;

					//Free the RAM we may have allocated and return.  We were successful.
					if(longPath == TRUE)
					{
						free(pLongPathString);
						pLongPathString = NULL;
					}
					return TRUE;
				}
			}
			else
			{
				//We opened the read handle successfully but couldn't open the write handle??
				CloseHandle(localReadHandle);
			}
		}
		else
		{
			//Although we managed to find the device, we couldn't use CreateFile() to get a handle to the device.
			//This normally happens if there are more than one PC applications open simultaneously trying to access the
			//same device.
			savedError = errorStatus;
			foundDeviceButCouldNotOpenIt = TRUE;
		}
	}
	

	//If we get to here, then a failure must have occurred...
	if(longPath == TRUE)
	{
		free(pLongPathString);
	}
	if(foundDeviceButCouldNotOpenIt == TRUE)
	{
		SetLastError(savedError);
	}
	return FALSE;
}


//Function overload, uses internal static non-WinUSB device handle instead of requiring the user to provide a pointer to an external handle.
BOOL HID_AttemptInitDevice(WORD VendorID, WORD ProductID, BOOL deviceIsComposite, WORD compositeInterfaceIndex, GUID* pGUID, DWORD currentInstanceIndex)
{
	return HID_AttemptInitDevice(VendorID, ProductID, deviceIsComposite, compositeInterfaceIndex, pGUID, currentInstanceIndex, &privateHIDReadHandle, &privateHIDWriteHandle);
}


//Simplified function overload.  Assumes non-composite device, only one or zero instances of the hardware, use default GUID (for any USB device) when searching for the device.
//Returns false if the HID device with matching VID/PID was not present, or was present, but
//was already opened by some other code.
BOOL HID_AttemptInitDevice(WORD VendorID, WORD ProductID)
{
	return HID_AttemptInitDevice(VendorID, ProductID, FALSE, 0, NULL, 1, &privateHIDReadHandle, &privateHIDWriteHandle);
}



//Closes the HID read/write handles, and sets them equal to INVALID_HANDLE_VALUE.
BOOL HID_CloseHandles(PHANDLE pHIDReadHandle, PHANDLE pHIDWriteHandle)
{
	BOOL returnValue;

	//Assume success unless otherwise determined later.
	returnValue = TRUE;


	//Check if the pointer is valid
	if(pHIDReadHandle != NULL)
	{
		//Make sure the handle was actually open before trying to close it.
		if(*pHIDReadHandle != INVALID_HANDLE_VALUE)
		{
			//Close the handle.
			if(CloseHandle(*pHIDReadHandle) == FALSE)
			{
				//Wut?  The handle failed to close?  We can't necessarily recover from such a scenario.
				returnValue = FALSE;
			}
			//Set the callers variablet to the invalid state, so that it is clearly known to be a closed/inactive handle.
			*pHIDReadHandle = INVALID_HANDLE_VALUE;
		}
	}


	//Check if the pointer is valid
	if(pHIDWriteHandle != NULL)
	{
		//Make sure the handle was actually open before trying to close it.
		if(*pHIDWriteHandle != INVALID_HANDLE_VALUE)
		{
			//Close the handle.
			if(CloseHandle(*pHIDWriteHandle) == FALSE)
			{
				//Wut?  The handle failed to close?  We can't necessarily recover from such a scenario.
				returnValue = FALSE;
			}
			//Set the callers variablet to the invalid state, so that it is clearly known to be a closed/inactive handle.
			*pHIDWriteHandle = INVALID_HANDLE_VALUE;
		}
	}

	return returnValue;
}


//This function would normally only be called if the device was opened without using the external handle pointers.
BOOL HID_CloseInternalHandles(void)
{
	BOOL returnValue = TRUE;

	//Close the read handle, if it was actually open in the first place.
	if(privateHIDReadHandle != INVALID_HANDLE_VALUE)
	{
		if(CloseHandle(privateHIDReadHandle) == FALSE)
		{
			returnValue = FALSE;
		}
		privateHIDReadHandle = INVALID_HANDLE_VALUE;
	}

	//Close the write handle, if it was actually open in the first place.
	if(privateHIDWriteHandle != INVALID_HANDLE_VALUE)
	{
		if(CloseHandle(privateHIDWriteHandle) == FALSE)
		{
			returnValue = FALSE;
		}
		privateHIDWriteHandle = INVALID_HANDLE_VALUE;
	}

	return returnValue;
}




//This function registers for WM_DEVICECHANGE notifications, so we can get a form callback when PnP hardware events such as USB attach/detach occur.
//Note: The formHandle should be supplied by the caller from the form that wants to get the callbacks (ex: formHandle = (HANDLE)this->Handle;)
//Additionally, the form must implement an override function to the standard WndPrc() callback function,
//so that it can receive and subsequently process the extra WM_DEVICECHANGE messages received (beyond the other existing messages
//that it may already be receiving).
void RegisterForDeviceChangeMessages(HANDLE formHandle, GUID* pGUID)
{
	GUID genericUSBGUID = {0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}; //Generic Globally Unique Identifier (GUID) common for all USB peripheral devices
	DEV_BROADCAST_DEVICEINTERFACE myDeviceBroadcastHeader;

	//Initialize a device broadcast header structure, then call RegisterDeviceNotification() so we can start
	//getting the WM_DEVICECHANGE notification callbacks.
	myDeviceBroadcastHeader.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	myDeviceBroadcastHeader.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	myDeviceBroadcastHeader.dbcc_reserved = 0;	//Reserved says not to use...
	//Check if the user supplied a valid pointer to a GUID for their hardware (ex: the HID guid).
	//If not, use the generic GUID common for all USB devices.  The common GUID will work for most purposes, except
	//when the USB device is part of a composite device.
	if(pGUID != NULL)
	{
		myDeviceBroadcastHeader.dbcc_classguid = *pGUID;
	}
	else
	{
		myDeviceBroadcastHeader.dbcc_classguid = genericUSBGUID;
	}
	RegisterDeviceNotificationUM(formHandle, &myDeviceBroadcastHeader, DEVICE_NOTIFY_WINDOW_HANDLE);
}






/****************************************************************************
Function:
static String^ ContainerIDGUIDToString(GUID* pGUIDToConvert)

Summary:
This function converts a "GUID" type into a String^, formatted like: "{4f1763bd-8195-11e4-935e-3c970e83569e}"

Description:
This function converts a "GUID" type (a 128-bit integer) into a text String^, formatted like: "{4f1763bd-8195-11e4-935e-3c970e83569e}"

Precondition:
None

Parameters:
GUID* pGUIDToConvert - a pointer to a GUID that needs converting into the String^ format.

Return Values:
String^ containing the string formatted version of the GUID value.

Remarks:
The returned string contains the extra characters like the "{" and "}" braces,
as well as dashes separating number groups.  There are however different methods of
representing GUIDs as strings (ex: different digit grouping/dash locations), so make
sure this format ("{4f1763bd-8195-11e4-935e-3c970e83569e}") is what you actually want.

***************************************************************************/
static String^ ContainerIDGUIDToString(GUID* pGUIDToConvert)
{
	String^ returnString;
	String^ data4String = "";
	int i;


	//First convert the "Data4" portion of the GUID number.  Data4 is an 8-byte char array, but we want the formatted string to be like "xxxx-xxxxxxxxxxxx"
	data4String = pGUIDToConvert->Data4[0].ToString("x2") + pGUIDToConvert->Data4[1].ToString("x2") + "-";
	for(i = 2; i < 8; i++)
	{
		data4String += pGUIDToConvert->Data4[i].ToString("x2");
	}

	//Now convert the Data0-Data3 portions of the GUID (which are 32-bit long, 16-bit short, 16-bit short, separated by dashes, and append the {} braces.
	returnString = "{" + pGUIDToConvert->Data1.ToString("x8") + "-" + pGUIDToConvert->Data2.ToString("x4") + "-" + pGUIDToConvert->Data3.ToString("x4") + "-" + data4String + "}";

	//The final output should be something like this: "{4f1763bd-8195-11e4-935e-3c970e83569e}"
	return returnString;
}
