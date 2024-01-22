//CDC COMxx Interface Routines
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
#include "stdafx.h"		//This will include at least windows.h, setupapi.h, and USB_CDC_Routines.h, which are needed by this .cpp file
#include "USB_CDC_Routines.h"

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
using namespace System::Runtime::InteropServices;  





//------------------------------------------------------------------------------------------------------------
//Definitions and Constants
//------------------------------------------------------------------------------------------------------------

//Note: To use the SetupDiGetDeviceProperty(), one needs some custom types, which can be obtained from: 
//#include <devpkey.h>	//However, this header is from the Windows Driver Kit (WDK).  If you don't want 
//to download/install the WDK, you can just use this subset of definitions instead.
#ifndef DEVPROPKEY_DEFINED
#define DEVPROPKEY_DEFINED

typedef GUID  DEVPROPGUID, *PDEVPROPGUID;
typedef ULONG DEVPROPID,   *PDEVPROPID;

typedef struct _DEVPROPKEY {
	DEVPROPGUID fmtid;
	DEVPROPID   pid;
} DEVPROPKEY, *PDEVPROPKEY;
#endif // DEVPROPKEY_DEFINED


#ifdef UNICODE
#define	Seeifdef	Unicode
#else
#define Seeifdef	Ansi
#endif



//------------------------------------------------------------------------------------------------------------
//Private Prototypes
//------------------------------------------------------------------------------------------------------------
static String^ ContainerIDGUIDToString(GUID* pGUIDToConvert);





//------------------------------------------------------------------------------------------------------------
//DLL Imports
//------------------------------------------------------------------------------------------------------------

////Returns a HDEVINFO type for a device information set (USB devices in
////our case).  We will need the HDEVINFO as in input parameter for calling many of
////the other SetupDixxx() functions.
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetClassDevs", CallingConvention=CallingConvention::Winapi)]		
//extern "C" HDEVINFO  SetupDiGetClassDevsUM(
//	LPGUID  ClassGuid,					//Input: Supply the class GUID here. 
//	PCTSTR  Enumerator,					//Input: Use NULL here, not important for our purposes
//	HWND  hwndParent,					//Input: Use NULL here, not important for our purposes
//	DWORD  Flags);						//Input: Flags describing what kind of filtering to use.
//
////Gives us "PSP_DEVICE_INTERFACE_DATA" which contains the Interface specific GUID (different
////from class GUID).  We need the interface GUID to get the device path.
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiEnumDeviceInterfaces", CallingConvention=CallingConvention::Winapi)]				
//extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInterfacesUM(
//	HDEVINFO  DeviceInfoSet,			//Input: Give it the HDEVINFO we got from SetupDiGetClassDevs()
//	PSP_DEVINFO_DATA  DeviceInfoData,	//Input (optional)
//	LPGUID  InterfaceClassGuid,			//Input 
//	DWORD  MemberIndex,					//Input: "Index" of the device you are interested in getting the path for.
//	PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData);//Output: This function fills in an "SP_DEVICE_INTERFACE_DATA" structure.
//
////SetupDiDestroyDeviceInfoList() frees up memory by destroying a DeviceInfoList
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiDestroyDeviceInfoList", CallingConvention=CallingConvention::Winapi)]
//extern "C" WINSETUPAPI BOOL WINAPI  SetupDiDestroyDeviceInfoListUM(			
//	HDEVINFO  DeviceInfoSet);			//Input: Give it a handle to a device info list to deallocate from RAM.
//
////SetupDiEnumDeviceInfo() fills in an "SP_DEVINFO_DATA" structure, which we need for SetupDiGetDeviceRegistryProperty()
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiEnumDeviceInfo", CallingConvention=CallingConvention::Winapi)]
//extern "C" WINSETUPAPI BOOL WINAPI  SetupDiEnumDeviceInfoUM(
//	HDEVINFO  DeviceInfoSet,
//	DWORD  MemberIndex,
//	PSP_DEVINFO_DATA  DeviceInfoData);
//
////SetupDiGetDeviceRegistryProperty() gives us the hardware ID, which we use to check to see if it has matching VID/PID
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetDeviceRegistryProperty", CallingConvention=CallingConvention::Winapi)]
//extern "C"	WINSETUPAPI BOOL WINAPI  SetupDiGetDeviceRegistryPropertyUM(
//	HDEVINFO  DeviceInfoSet,
//	PSP_DEVINFO_DATA  DeviceInfoData,
//	DWORD  Property,
//	PDWORD  PropertyRegDataType,
//	PBYTE  PropertyBuffer,   
//	DWORD  PropertyBufferSize,  
//	PDWORD  RequiredSize);
//
////SetupDiGetDeviceInterfaceDetail() gives us a device path, which is needed before CreateFile() can be used.
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetDeviceInterfaceDetail", CallingConvention=CallingConvention::Winapi)]
//extern "C" BOOL SetupDiGetDeviceInterfaceDetailUM(
//	HDEVINFO DeviceInfoSet,										//Input: Wants HDEVINFO which can be obtained from SetupDiGetClassDevs()
//	PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,				//Input: Pointer to an structure which defines the device interface.  
//	PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,	//Output: Pointer to a strucutre, which will contain the device path.
//	DWORD DeviceInterfaceDetailDataSize,						//Input: Number of bytes to retrieve.
//	PDWORD RequiredSize,										//Output (optional): Te number of bytes needed to hold the entire struct 
//	PSP_DEVINFO_DATA DeviceInfoData);							//Output
//
////Note: This function is only supported in Windows Server 2003 and later (not implemented in XP).
////It is uncertain if this function could be used to re-initialize a "Status: Attached but broken." device.
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiRestartDevices", CallingConvention=CallingConvention::Winapi)]					
//extern "C" BOOL WINAPI SetupDiRestartDevicesUM(
//HDEVINFO  DeviceInfoSet,
//PSP_DEVINFO_DATA  DeviceInfoData);
//
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetCustomDeviceProperty", CallingConvention=CallingConvention::Winapi)]
//extern "C" BOOL SetupDiGetCustomDevicePropertyUM(
//	HDEVINFO DeviceInfoSet,
//	PSP_DEVINFO_DATA DeviceInfoData,
//	PCTSTR CustomPropertyName,
//	DWORD Flags,PDWORD PropertyRegDataType,
//	PBYTE PropertyBuffer,DWORD PropertyBufferSize,
//	PDWORD RequiredSize);
//
//[DllImport("setupapi.dll" , CharSet = CharSet::Seeifdef, EntryPoint="SetupDiGetDeviceProperty", CallingConvention=CallingConvention::Winapi)]
//extern "C" BOOL SetupDiGetDevicePropertyUM(
//	HDEVINFO  DeviceInfoSet,
//	PSP_DEVINFO_DATA  DeviceInfoData,
//	CONST DEVPROPKEY  *PropertyKey,
//	DEVPROPTYPE  *PropertyType,
//	PBYTE  PropertyBuffer, OPTIONAL
//	DWORD  PropertyBufferSize,
//	PDWORD  RequiredSize, OPTIONAL
//	DWORD  Flags);




//------------------------------------------------------------------------------------------------------------
//API Functions Implemented by this File
//------------------------------------------------------------------------------------------------------------

/****************************************************************************
	Function:
		String^	USB_CheckIfPresentAndGetCOMString(WORD USBVendorID, WORD USBProductID, DWORD deviceInstanceIndex, bool deviceIsComposite, BYTE compositeInterfaceIndex)

	Summary:
		This function checks if a USB CDC device is currently plugged in with 
		a matching USB VID and PID string.  If a matching device is found, it
		returns the COMx number string associated with the USB device.

	Description:
		This function checks if a USB CDC device is currently plugged in with 
		a matching USB VID and PID string.  If a matching device is found, it
		returns the COMx number string associated with the USB device.

	Precondition:
		None

	Parameters:
		WORD USBVendorID - The 16-bit Vendor ID (VID) of the USB device to try to find the COMx port number for
		WORD USBProductID - The 16-bit Product ID (PID) of the USB device to try to find the COMx port number for
		DWORD deviceInstanceIndex - The "instance index" of the USB device.  Always use '0', unless there are more than one instance of the hardware attached to the same computer simultaneously.
		bool deviceIsComposite - Use "true" if the CDC function is part of a composite USB device, otherwise use "false"
		BYTE compositeInterfaceIndex - The CDC interface index number, when the device is a composite USB device.  Value is irrelevant and may be '0' when deviceIsComposite is false.

	Return Values:
		String^ containing the "COMxx" string number (useful when trying to open the COM port).
				If no USB CDC COMx ports are currently provided by a USB with matching VID/PID,
				then the return value is: ""

	Remarks:
		This function is passive and does not attempt to open/close any COMx ports.  All it
		does is search for attached USB devices, that are currently providing COMx ports.
		
		This function is currently written to work only with a single instance of the USB device
		attached to the computer, with the given VID/PID.  If more than one device of matching
		VID/PID is simultaneously attached to the machine, then more than one COMx port number
		will exist (one for each CDC device).  This function will only return the first COMx
		number that it finds for the matching VID/PID.  If more than one exists, the additional
		COMx numbers will remain unknown.  To expand the functionality of this function, it would
		be possible to remove the "return COMString;" line of code, and instead keep looping
		until the "if(ErrorStatus == ERROR_NO_MORE_ITEMS)" exit condition is hit instead.
		In this case, the loop will find multiple COMx numbers and instances, and these
		could be stored and returned in an array of strings instead.
***************************************************************************/
String^	USB_CheckIfPresentAndGetCOMString(WORD USBVendorID, WORD USBProductID, DWORD deviceInstanceIndex, bool deviceIsComposite, BYTE compositeInterfaceIndex)
{
	//Globally Unique Identifier (GUID) for COM port devices.  Windows uses GUIDs to identify things.  GUIDs are a structure containing 128-bits worth of numbers.
	GUID InterfaceClassGuid = {0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed};					//GUID_DEVINTERFACE_USB_DEVICE (common for any USB device) - used to find the USB device
	DEVPROPKEY devPropKeyContainerID = {{0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c}, 2};	//Needed for calling SetupDiGetDeviceProperty() on composite devices only.
	
	HDEVINFO DeviceInfoTable = INVALID_HANDLE_VALUE;
	PSP_DEVICE_INTERFACE_DATA InterfaceDataStructure = new SP_DEVICE_INTERFACE_DATA;
	PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;
	SP_DEVINFO_DATA DevInfoData;
	DWORD InterfaceIndex = 0;
	DWORD StatusLastError = 0;
	DWORD dwRegType;
	DWORD dwRegSize;
	DWORD StructureSize = 0;
	PBYTE PropertyValueBuffer;
	bool MatchFound = false;
	DWORD ErrorStatus;
	BOOL BoolStatus = FALSE;
	DWORD LoopCounter = 0;
	String^ deviceIDToFind;
	String^ completeIdentifier;
	String^ FriendlyNameString = "";
	int IndexOfStartofCOMChars;
	int IndexOfEndOfCOMChar = 0;
	String^ COMString = "";
	String^ registryPathString;
	RegistryKey^ regKey;
	RegistryKey^ regSubKey;
	DWORD i;
	DWORD instanceCounter = 0;
	String^ containerIDToCheck;
	String^ parentDeviceGUIDString;
	DEVPROPTYPE devPropType;
	GUID containerIDGUID;


	//Craft a Windows "HardwareID" string from the user's specified VID/PID/interface index info.
	//the HardwareID string is formatted like: "Vid_04d8&Pid_000a".  If the device is part of a composite
	//USB device however, then an additional "&MI_xx" gets appended at the end, where "MI" represents "multiple interface" and "xx" is a number 00, 01, 02, etc.,
	//representing the interface index of the first interface associated with the CDC function.  See: http://msdn.microsoft.com/en-us/library/windows/hardware/ff553356(v=vs.85).aspx
	deviceIDToFind = "Vid_" + USBVendorID.ToString("x4") + "&Pid_" + USBProductID.ToString("x4");	//The "x4" format strings make the 16-bit numbers into hexadecimal (0-9 + A-F) text strings.
	if(deviceIsComposite == true)
	{
		completeIdentifier = deviceIDToFind + "&MI_" + compositeInterfaceIndex.ToString("d2");	//Append &MI_xx to end of the string (ex: "Vid_04d8&Pid_0057&MI_01").
	}
	else
	{
		completeIdentifier = deviceIDToFind;
	}
	//Convert string to pure lower case, for better/more robust string comparision checks
	deviceIDToFind = deviceIDToFind->ToLowerInvariant();	

	//Now Populate a list of currently plugged in hardware devices (by specifying "DIGCF_PRESENT"), which are of the specified class GUID. 
	DeviceInfoTable = INVALID_HANDLE_VALUE;
	DeviceInfoTable = SetupDiGetClassDevs(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);	//Creates a device list in memory, kind of like looking at a cut down internal version of the Windows device manager.
	ErrorStatus = GetLastError();
	if(DeviceInfoTable == INVALID_HANDLE_VALUE)
	{
		//Failed to get the device list.  Not much we can do in this case, just bug out.
		delete InterfaceDataStructure;	
		delete DetailedInterfaceDataStructure;
		return "";
	}


	//Now look through the list we just populated.  We are trying to see if any of them match our device. 
	while(true)
	{
		InterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		if(SetupDiEnumDeviceInterfaces(DeviceInfoTable, NULL, &InterfaceClassGuid, InterfaceIndex, InterfaceDataStructure))
		{
			ErrorStatus = GetLastError();
			if(ErrorStatus == ERROR_NO_MORE_ITEMS)	//Did we reach the end of the list of matching devices in the DeviceInfoTable?
			{	//Cound not find the device.  Must not have been attached.
				SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
				delete InterfaceDataStructure;	
				delete DetailedInterfaceDataStructure;
				return "";		
			}
		}
		else	//Else some other kind of unknown error ocurred...
		{
			ErrorStatus = GetLastError();
			SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
			delete InterfaceDataStructure;	
			delete DetailedInterfaceDataStructure;
			return "";	
		}

		//Now retrieve the hardware ID from the registry.  The hardware ID contains the VID and PID, which we will then 
		//check to see if it is the correct device or not.

		//Initialize an appropriate SP_DEVINFO_DATA structure.  We need this structure for SetupDiGetDeviceRegistryProperty().
		DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		SetupDiEnumDeviceInfo(DeviceInfoTable, InterfaceIndex, &DevInfoData);

		//First query for the size of the hardware ID, so we can know how big a buffer to allocate for the data.
		SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwRegSize);

		//Allocate a buffer for the hardware ID.
		PropertyValueBuffer = (BYTE *) malloc (dwRegSize);
		if(PropertyValueBuffer == NULL)	//if null, error, couldn't allocate enough memory
		{	//Can't really recover from this situation, just exit instead.
			SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
			delete InterfaceDataStructure;	
			delete DetailedInterfaceDataStructure;
			return "";		
		}

		//Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a 
		//REG_MULTI_SZ (array of null terminated strings).  To find a device, we only care about the very first string in the
		//buffer, which will be the "device ID".  The device ID is a string which contains the VID and PID, in the example 
		//format "Vid_04d8&Pid_000a".
		SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, PropertyValueBuffer, dwRegSize, NULL);

		//Now check if the first string in the hardware ID matches the device ID of my USB device.
		#ifdef UNICODE
		String^ DeviceIDFromRegistry = gcnew String((wchar_t *)PropertyValueBuffer);
		#else
		String^ DeviceIDFromRegistry = gcnew String((char *)PropertyValueBuffer);
		#endif

		free(PropertyValueBuffer);		//No longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks

		//Convert string to lower case.  This makes the code more robust/portable accross OS Versions
		DeviceIDFromRegistry = DeviceIDFromRegistry->ToLowerInvariant();	
		//Now check if the hardware ID we are looking at contains the correct VID/PID
		MatchFound = DeviceIDFromRegistry->Contains(deviceIDToFind);		
		if(MatchFound == true)
		{
			//We just found a device with matching VID/PID.  

			//Check if the instance of the matching hardware matches the user's desired instance number.  If not,
			//just keep looking until we find a device with fully matching VID+PID+Instance Index.
			if(instanceCounter == deviceInstanceIndex)
			{
				//Device must have been found.  Now we should read the "FriendlyName".  The FriendlyName is the text
				//that appears in the device manager entry for the device.  The friendlyname for the virtual com port
				//is composed of the text from the .inf file used to install the driver, combined with the COMx number
				//assigned to the device.  The COMx number also appears in the registry "PortName" entry for the device.
				//First call the function to determine the size of the required buffer.
				SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_FRIENDLYNAME, &dwRegType, NULL, 0, &dwRegSize);
				//Allocate a buffer for the hardware ID.
				PropertyValueBuffer = (BYTE *) malloc (dwRegSize);
				if(PropertyValueBuffer == NULL)	//if null, error, couldn't allocate enough memory
				{	//Can't really recover from this situation, just exit instead.
					SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
					delete InterfaceDataStructure;	
					delete DetailedInterfaceDataStructure;
					return "";		
				}

				//Try to retrieve the FriendlyName string containing the COMx number.
				SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_FRIENDLYNAME, &dwRegType, PropertyValueBuffer, dwRegSize, NULL);

				//Now copy the resulting string (which is returned by the API as a plain byte array) into a managed String^ type for easier 
				//manipulation and comparisons using managed functions.
				#ifdef UNICODE
					FriendlyNameString = gcnew String((wchar_t *)PropertyValueBuffer);
				#else
					FriendlyNameString = gcnew String((char *)PropertyValueBuffer);
				#endif
				free(PropertyValueBuffer);		//No longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks

				//Check if it contains the COMx string like expected, and if so, extract it, so it can
				//be returned in the formatting used when opening a COMx port.
				if((FriendlyNameString->Contains("(COM")) && (FriendlyNameString->Contains(")")))
				{
					IndexOfStartofCOMChars = FriendlyNameString->IndexOf("COM");
					IndexOfEndOfCOMChar = FriendlyNameString->IndexOf(")");
					if((IndexOfEndOfCOMChar > IndexOfStartofCOMChars) && (IndexOfEndOfCOMChar <= FriendlyNameString->Length))
					{
						//Copy the "COMxx" string value from the FriendlyNameString to the COMString.
						//The COMString is formatted in the exact format needed when setting a serial port object's COM number.
						COMString = FriendlyNameString->Substring(IndexOfStartofCOMChars, IndexOfEndOfCOMChar - IndexOfStartofCOMChars);
					}

					//We have now presumable found the COMx number and should return.
					SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
					delete InterfaceDataStructure;	
					delete DetailedInterfaceDataStructure;
					return COMString;	//May contain "" if we couldn't find the device COMxx number.  Otherwise, returns "COMxxx" where xxx is the COM number (could be only one or two chars, depending upon COM number)
				}
				else
				{
					//The FriendlyNameString didn't contain the "COMxx" number string we were expecting.  This can happen (and is expected) if the 
					//USB CDC ACM function/interface is part of a composite USB device.  
					//In a composite device, at least three entries get made in the registry.  For example an MSD + CDC composite device would generate:
					//1. A top level "parent device" entry.
					//2. A lower level entry specific to the first function implemented in the composite device (ex: MSD).
					//3. Another lower level entry specific to the next function implemented in the composite device (ex: CDC).

					//In the code above, if the CDC function is part of a composite USB device, the above algorithm will find the
					//parent device (since it has matching VID/PID and other info).  However, the COMx port number information will
					//not be present in the top level parent device registry entries.  Instead, you have to find the proper corresponding
					//lower level entry, for the CDC interface on the device, that is specifically associated with that parent device.

					//Windows 7 and up implements the concept of "Container ID" numbers (which are 128-bit GUIDs), which link/associate
					//the parent device with the lower level functions.  It is therefore possible to cross reference the three+ registry entries
					//with each other, since the ContainerID for each entry will match.

					//Therefore the process for getting the COMxx number for a composite device is to:
					//1. Find the top level parent device based on VID/PID/current attachment status, index, etc.
					//2. Read and save the Container ID value for the parent device.
					//3. Find all the lower level CDC entries that also have matching VID/PID.
					//4. For each entry found in #3 above, read the Container ID and check it against the parent device Container ID.
					//5. If a match is found, read the COMx port "FriendlyName" string from the registry, which is stored in the subkey associated with that CDC function.

				
					//Going to do some registry read only operations, which is dymanic and can change at runtime based on plug and play
					//activity elsewhere in the OS.  Additionally, some of the below code will only work on >= Windows 7 (ex: ContainerIDs and the SetupDiGetDeviceProperty()
					//API doesn't exist on XP, so won't actually link to the setupapi.dll properly on the old OSes).  Therefore, we use a try/catch
					//block in case of this type of failure.
					try
					{
						//Get the "ContainerID" (Note: only available in Windows 7+) for the top level parent device
						if(SetupDiGetDeviceProperty(DeviceInfoTable, &DevInfoData, &devPropKeyContainerID, &devPropType, (PBYTE)&containerIDGUID, sizeof(GUID), &dwRegSize, 0))
						{
							ErrorStatus = GetLastError();
							parentDeviceGUIDString = ContainerIDGUIDToString(&containerIDGUID);
						}
						else
						{
							//Failed to get the ContainerID GUID for some reason.
							ErrorStatus = GetLastError();
							parentDeviceGUIDString = "";
						}

						//Now open the composite CDC "child" specific registry key for the device.
						//The full key will look something like:
						//"SYSTEM\CurrentControlSet\Enum\USB\VID_04D8&PID_0057&MI_01\9&2e9bdd3b&0&0001"		//Note the cryptic subkey "9&2e9bdd3b&0&0001" at the end (which changes, among other things, based on hub/port number the device is attached to).
						
						//Try to open (in read only mode) the upper level "SYSTEM\\CurrentControlSet\\Enum\\USB\\" registry key.
						regKey = Registry::LocalMachine->OpenSubKey("SYSTEM\\CurrentControlSet\\Enum\\USB\\", false);
						if(regKey != nullptr)
						{
							//Read all of the subkey names, since one of them may have matching VID/PID/MI_xx interface info that we are looking for.
							array<String^>^ subKeyNames = regKey->GetSubKeyNames();

							//Done with this top level key, close it.
							regKey->Close();

							//Check each sub key name to find the one(s) containing the correct VID/PID values of interest.
							for each(String^ subKeyName in subKeyNames)
							{
								if(subKeyName->ToLowerInvariant()->Contains(deviceIDToFind))
								{
									//We found a subkey that contains the VID/PID of the device we are trying to connect to.
									//Open the specific subkey and check if it happens to be the right one with the correct MI_xx value
									//for the CDC control interface index.  
									//Note: For a composite device, there will be multiple subkeys with matching VID/PID, ex:
									//SYSTEM\CurrentControlSet\Enum\USB\VID_04D8&PID_00DD\
									//SYSTEM\CurrentControlSet\Enum\USB\VID_04D8&PID_00DD&MI_00\
									//SYSTEM\CurrentControlSet\Enum\USB\VID_04D8&PID_00DD&MI_02\
									//(where one of the items corresponds with the top level device, and additional ones corresponds to other interfaces/functions on the device)
									//We need to look through each one, since only the interface specific entry for the CDC control interface on the device will have
									//the necessary registry subkey with the COMxx friendly name info in it.

									String^ subKeyWithMatchingVIDPID = "SYSTEM\\CurrentControlSet\\Enum\\USB\\" + subKeyName;

									registryPathString = subKeyWithMatchingVIDPID;
									//Try to open the registry key (without cryptic subkey, ex: SYSTEM\CurrentControlSet\Enum\USB\VID_04D8&PID_00DD&MI_00\)
									regKey = Registry::LocalMachine->OpenSubKey(registryPathString, false);
									if(regKey != nullptr)
									{
										//We successfully opened the key.  However, the actual FriendlyName value
										//is stored in an obscure named subkey to the key we just opened.  Therefore,
										//we need to figure out the name(s) of any subkeys first, then look through each of them.
										array<String^>^subKeyNames = regKey->GetSubKeyNames();
										
										//Done with the upper level key.  Close it.
										regKey->Close();

										//Look through each of the subkey(s) to extract the FriendlyName value for the desired device
										for(i = 0; i < (DWORD)subKeyNames->Length; i++)
										{
											regSubKey = Registry::LocalMachine->OpenSubKey(registryPathString + "\\" + subKeyNames[i]);
											if(regSubKey != nullptr)
											{
												//Fetch the ContainerID value from the current subkey that we are looking at.  This particular
												//subkey may or may not be the correct hardware device that is currently attached (ex: it could
												//be for a separate plug and play device node in the registry, for a device with matching VID/PID,
												//but which is not currently plugged in or, is attached to a different USB port from our device of interest).
												containerIDToCheck = (String^)regSubKey->GetValue("ContainerID");

												//We now have two ContainerID strings, one from this subkey, and one for the "parent" USB 
												//device (that is currently attached to the machine with matching VID/PID).

												//Convert both strings to all lower case, prior to comparisons, to ensure case insensitive hex number (as string) comparison.
												containerIDToCheck = containerIDToCheck->ToLowerInvariant();
												parentDeviceGUIDString = parentDeviceGUIDString->ToLowerInvariant();

												//Check if we have found the correct/matching registry entry, associated with the CDC function of the
												//device we are interested in, which is currently plugged in (which has matching ContainerID value
												//as the parent device that we found earlier).
												if(containerIDToCheck == parentDeviceGUIDString)
												{
													//We found a sub key with matching VID/PID and the correct ContainerID value for the currently attached hardware
													//device that was previously detected.  However, this sub key may or may not have the FriendlyName value string in it,
													//depending on if we are looking at the correct composite interface or not.

													//Try to read the "FriendlyName" string (if present, which when present contains the COMxx port info), stored in the registry entry.
													FriendlyNameString = (String^)regSubKey->GetValue("FriendlyName", "");
													if(FriendlyNameString != "")
													{
														//Check if the friendly name contains the desired COMx number information, and extract it if present.
														if((FriendlyNameString->Contains("(COM")) && (FriendlyNameString->Contains(")")))
														{
															IndexOfStartofCOMChars = FriendlyNameString->IndexOf("COM");
															IndexOfEndOfCOMChar = FriendlyNameString->IndexOf(")");
															if((IndexOfEndOfCOMChar > IndexOfStartofCOMChars) && (IndexOfEndOfCOMChar <= FriendlyNameString->Length))
															{
																//Copy the "COMxx" string value from the FriendlyNameString to the COMString.
																//The COMString is formatted in the exact format needed when setting a serial port object's COM number.
																COMString = FriendlyNameString->Substring(IndexOfStartofCOMChars, IndexOfEndOfCOMChar - IndexOfStartofCOMChars);
															}

															//We have now found the COMx number and should return.
															//Close the registry keys, now that we are done with it.
															regSubKey->Close();
															SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
															delete InterfaceDataStructure;	
															delete DetailedInterfaceDataStructure;
															return COMString;	//May contain "" if we couldn't find the device COMxx number.  Otherwise, returns "COMxxx" where xxx is the COM number (could be only one or two chars, depending upon COM number)
														}
													}
												}//if(containerIDToCheck == parentDeviceGUIDString)

												//Close the registry key, now that we are done with it.
												regSubKey->Close();
											}//if(regSubKey != nullptr)
										}//for(i = 0; i < subKeyNames->Length; i++)
									}//if(regKey != nullptr)
									else
									{
										//Failed to open the expected registry key with the crytptic subkeys...
										//Can't do much in this case.
									}

								}
							}
						}

						

					}//try
					catch(...)
					{
						//Most likely reason for failure is due to fail to obtain the SetupDiGetDeviceProperty() entry point
						//in the setupapi.dll.  Presumably, this API didn't exist in XP.  In this case, we can't really find
						//the device (with full confidence anyway), and the user must still manually figure out the COMxx 
						//number themselves and open it manually.
					};
				}//else of: if((FriendlyNameString->Contains("(COM")) && (FriendlyNameString->Contains(")")))
			}//if(instanceCounter == deviceInstanceIndex)
			else
			{
				//Although we found a device with matching VID/PID, the user didn't want to open it,
				//presumably because they wanted to attached to a higher order instance of the hardware.
				instanceCounter++;
			}
		}//if(MatchFound == true)

		//If we get to here, this means we just finished looking through one entry in the populated
		//virutal Windows device manager list of attached hardware devices (of matching interface GUID).
		//Since we didn't find a match this pass, we need to continue the loop to look at the next 
		//entry in the table.
		InterfaceIndex++;	

		//Keep looping until we either find a device with matching VID and PID, or until we run out of devices to check.
		//However, just in case some unexpected error occurs, keep track of the number of loops executed.
		//If the number of loops exceeds a very large number, exit anyway, to prevent inadvertent infinite looping.
		LoopCounter++;
		if(LoopCounter == 10000000)	//Surely there aren't more than 10 million devices attached to any forseeable PC...
		{
			//Bug out to avoid infinite looping, in case the "if(ErrorStatus == ERROR_NO_MORE_ITEMS)" 
			//check isn't working for whatever reason.
			SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
			delete InterfaceDataStructure;	
			delete DetailedInterfaceDataStructure;
			return "";
		}
	}//end of while(true)
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
