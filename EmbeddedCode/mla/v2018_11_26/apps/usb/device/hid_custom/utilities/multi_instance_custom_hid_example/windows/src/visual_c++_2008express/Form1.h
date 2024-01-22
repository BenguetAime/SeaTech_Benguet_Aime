#pragma once
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
*******************************************************************************

This program implements a basic example showing how to find and connect up to
multiple USB devices with the same USB Vendor ID (VID) and Product ID (PID).
When there are multiple instances of a given hardware product attached to
the same computer simultanesouly, each device has an "instance" number associated
with it, and each instance also has a unique device path.  Each unique device
path can be used to open read/write handles to that specific USB device hardware
instance.

Keeping track of multiple instances of hardware is somewhat complicated however,
as all USB devices are intrinsically plug and play devices.  Therefore it is possible
for the USB device instance counts and instance indexes to change dynamically at 
runtime, if the user adds or removes additional hardware instances from their computer.
*******************************************************************************/


#include "Windows.h"
#include "USB_HID_Helper_Routines.h"


namespace MultiHIDExample {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;



	int numberOfMatchingVIDPIDDevices = 0;
	TCHAR devicePath[33000];	//According to MSDN, max path can be "about" 32,767 chars (plus a few extra of prefix special chars).  So best to pre-allocate at least 33k buffer to hold max path string length.
	int instanceIndexForFetchingPath = 0;

	//Handles for each USB device, with matching VID/PID.
	HANDLE WriteHandle[100];
	HANDLE ReadHandle[100];


	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();

			int i;

			//Initialize all the read and write handles to INVALID_HANDLE_VALUE since no connections are initally present.
			for(i = 0; i < 100; i++)
			{
				WriteHandle[i] = INVALID_HANDLE_VALUE;
				ReadHandle[i] = INVALID_HANDLE_VALUE;
			}


			//Call the function that updates the device path list and device count values
			//on the form.
			GetDeviceCountAndOpenHandles();


			//Register for WM_DEVICECHANGE notifications, so we can get a callback to fire when plug and play 
			//hardware events occur somewhere in the system.
			RegisterForDeviceChangeMessages((HANDLE)this->Handle, NULL);

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  numDevices_lbl;
	protected: 
	private: System::Windows::Forms::TextBox^  output_txtbx;
	private: System::Windows::Forms::TextBox^  vid_txtbx;
	private: System::Windows::Forms::TextBox^  pid_txtbx;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  refresh_btn;
	private: System::Windows::Forms::Label^  writeHandles_lbl;
	private: System::Windows::Forms::Label^  readHandles_lbl;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->numDevices_lbl = (gcnew System::Windows::Forms::Label());
			this->output_txtbx = (gcnew System::Windows::Forms::TextBox());
			this->vid_txtbx = (gcnew System::Windows::Forms::TextBox());
			this->pid_txtbx = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->refresh_btn = (gcnew System::Windows::Forms::Button());
			this->writeHandles_lbl = (gcnew System::Windows::Forms::Label());
			this->readHandles_lbl = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// numDevices_lbl
			// 
			this->numDevices_lbl->AutoSize = true;
			this->numDevices_lbl->Location = System::Drawing::Point(12, 72);
			this->numDevices_lbl->Name = L"numDevices_lbl";
			this->numDevices_lbl->Size = System::Drawing::Size(247, 13);
			this->numDevices_lbl->TabIndex = 0;
			this->numDevices_lbl->Text = L"Number of Matching Devices Currently Attached: 0";
			// 
			// output_txtbx
			// 
			this->output_txtbx->Location = System::Drawing::Point(12, 212);
			this->output_txtbx->Multiline = true;
			this->output_txtbx->Name = L"output_txtbx";
			this->output_txtbx->Size = System::Drawing::Size(651, 175);
			this->output_txtbx->TabIndex = 1;
			// 
			// vid_txtbx
			// 
			this->vid_txtbx->Location = System::Drawing::Point(15, 12);
			this->vid_txtbx->Name = L"vid_txtbx";
			this->vid_txtbx->Size = System::Drawing::Size(100, 20);
			this->vid_txtbx->TabIndex = 2;
			this->vid_txtbx->Text = L"0x04D8";
			// 
			// pid_txtbx
			// 
			this->pid_txtbx->Location = System::Drawing::Point(15, 38);
			this->pid_txtbx->Name = L"pid_txtbx";
			this->pid_txtbx->Size = System::Drawing::Size(100, 20);
			this->pid_txtbx->TabIndex = 3;
			this->pid_txtbx->Text = L"0x003F";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(121, 15);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(120, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"VID to Search For (Hex)";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(121, 41);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(120, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"PID to Search For (Hex)";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 196);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(446, 13);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Device path string for each instance found (each hardware instance will have a un" 
				L"ique path):";
			// 
			// refresh_btn
			// 
			this->refresh_btn->Location = System::Drawing::Point(304, 12);
			this->refresh_btn->Name = L"refresh_btn";
			this->refresh_btn->Size = System::Drawing::Size(123, 56);
			this->refresh_btn->TabIndex = 7;
			this->refresh_btn->Text = L"Manually Refresh Device Counts and Handles";
			this->refresh_btn->UseVisualStyleBackColor = true;
			this->refresh_btn->Click += gcnew System::EventHandler(this, &Form1::refresh_btn_Click);
			// 
			// writeHandles_lbl
			// 
			this->writeHandles_lbl->AutoSize = true;
			this->writeHandles_lbl->Location = System::Drawing::Point(12, 100);
			this->writeHandles_lbl->Name = L"writeHandles_lbl";
			this->writeHandles_lbl->Size = System::Drawing::Size(127, 13);
			this->writeHandles_lbl->TabIndex = 8;
			this->writeHandles_lbl->Text = L"Opened Write Handles: 0";
			// 
			// readHandles_lbl
			// 
			this->readHandles_lbl->AutoSize = true;
			this->readHandles_lbl->Location = System::Drawing::Point(12, 122);
			this->readHandles_lbl->Name = L"readHandles_lbl";
			this->readHandles_lbl->Size = System::Drawing::Size(128, 13);
			this->readHandles_lbl->TabIndex = 9;
			this->readHandles_lbl->Text = L"Opened Read Handles: 0";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(675, 399);
			this->Controls->Add(this->readHandles_lbl);
			this->Controls->Add(this->writeHandles_lbl);
			this->Controls->Add(this->refresh_btn);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->pid_txtbx);
			this->Controls->Add(this->vid_txtbx);
			this->Controls->Add(this->output_txtbx);
			this->Controls->Add(this->numDevices_lbl);
			this->Name = L"Form1";
			this->Text = L"Muti-HID Custom Example";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion





protected: virtual void WndProc( Message% m ) override{
		 //This is a callback function that gets called when a Windows message is received by the form.
		 // Listen for Windows messages.  We will receive various different types of messages, but the ones we really want to use are the WM_DEVICECHANGE messages.
		 if(m.Msg == WM_DEVICECHANGE)
		 {
			 if(((int)m.WParam == DBT_DEVICEARRIVAL) || ((int)m.WParam == DBT_DEVICEREMOVEPENDING) || ((int)m.WParam == DBT_DEVICEREMOVECOMPLETE) || ((int)m.WParam == DBT_CONFIGCHANGED) || ((int)m.WParam == DBT_DEVNODES_CHANGED))
			 {
				 //Refresh the device path listing and device count, since it may have just changed to a plug and play event.
				 GetDeviceCountAndOpenHandles();


			 }//if(((int)m.WParam == DBT_DEVICEARRIVAL) || ((int)m.WParam == DBT_DEVICEREMOVEPENDING) || ((int)m.WParam == DBT_DEVICEREMOVECOMPLETE) || ((int)m.WParam == DBT_CONFIGCHANGED) || ((int)m.WParam == DBT_DEVNODES_CHANGED))
		 }//if(m.Msg == WM_DEVICECHANGE)

		 //Now call the regular windows message handler
		 Form::WndProc( m );
	}//virtual void WndProc( Message% m ) override{





//Returns the count of the number of matching devices found.
//This function checks how many devices are currently attached to the device, with matching VID/PID
//as entered by the user in the respective textboxes.  The function also opens read/write handles
//to each of the devices it finds.  If you call it again, it will close the original handles
//that may have been open, and then will attempt to re-open handles to all the devices it finds.
int GetDeviceCountAndOpenHandles(void)
{
	WORD VendorID;
	WORD ProductID;
	DWORD requiredSize;
	int i;
	int readHandlesOpened = 0;
	int writeHandlesOpened = 0;

	//First close any existing handles that may have previously been opened, since this function re-opens them as part of the device refresh operation.
	//NOTE: There must always be a 1:1 relation of calls to CreateFile() and calls to CloseHandle().  Failure to maintain this 1:1 ratio
	//will inevitably lead to open but "stranded" handles (where the application no longer knows the actual value of a handle, which is still open).
	//Assuming file sharing is not supported (which for most USB drivers, sharing is not allowed/implemented, except for HID, where it is optional),
	//then this will lead to a scenario where the PC application will no longer be able to communicate with the hardware, since it will not know
	//the correct/valid handle of the device to use when trying to read/write to the device.
	//To help facilitate handle tracking, always make sure to set any closed handles to INVALID_HANDLE_VALUE.  Note that this is not automatic,
	//as the CloseHandle() routine doesn't write to the handle by itself.
	for(i = 0; i < 100; i++)
	{
		if(WriteHandle[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(WriteHandle[i]);
			WriteHandle[i] = INVALID_HANDLE_VALUE;
		}
		if(ReadHandle[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(ReadHandle[i]);
			ReadHandle[i] = INVALID_HANDLE_VALUE;
		}
	}


	//Clear the output textbox with the device paths shown
	output_txtbx->Text = "";


	//Convert the VID/PID textbox string values into numbers.
	try
	{
		VendorID = (WORD)Convert::ToUInt32(vid_txtbx->Text, 16);
		ProductID = (WORD)Convert::ToUInt32(pid_txtbx->Text, 16);
	}
	catch(...)
	{
		//Presumably can't convert the text into a number, because it contained illegal characters (ex: not 0-9 and A-F).
		VendorID = 0x04D8;
		ProductID = 0x003F;
	}

	//First fetch the path string for the first device instance index (with matching VID/PID).  If there are more than one device with
	//the same VID/PID attached, then we need to repeatedly call HID_GetUSBDeviceCountAndOneDevicePath() with different indexes, to get all the
	//device paths for all the devices.
	instanceIndexForFetchingPath = 1;

	//Call HID_GetUSBDeviceCountAndOneDevicePath() to fetch one device path (if any devices are connected), and to also fetch the number of devices present with matching VID/PID.
	numberOfMatchingVIDPIDDevices = HID_GetUSBDeviceCountAndOneDevicePath(VendorID, ProductID, FALSE, 0, NULL, instanceIndexForFetchingPath, &devicePath[0], &requiredSize, sizeof(devicePath));
	numDevices_lbl->Text = "Number of Matching Devices Currently Attached: " + numberOfMatchingVIDPIDDevices.ToString();


	//Now open handles to each device found
	instanceIndexForFetchingPath = 1;
	for(i = 0; i < numberOfMatchingVIDPIDDevices; i++)
	{
		HID_GetUSBDeviceCountAndOneDevicePath(VendorID, ProductID, FALSE, 0, NULL, instanceIndexForFetchingPath, &devicePath[0], &requiredSize, sizeof(devicePath));

		//Use the fetched path for the current device instance index to open handles to that peice of hardware
		WriteHandle[i] = CreateFile(&devicePath[0], GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(WriteHandle[i] != INVALID_HANDLE_VALUE)
		{
			//We successfully opened a WRITE handle to the device.
			writeHandlesOpened++;
		}
		ReadHandle[i] = CreateFile(&devicePath[0], GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(ReadHandle[i] != INVALID_HANDLE_VALUE)
		{
			//We successfully opened a READ handle to the device.
			readHandlesOpened++;
		}

		//Convert the TCHAR character array into a managed String^.
		String^ devicePathManagedString = gcnew String(&devicePath[0], 0, requiredSize);

		//Add the USB device path string to the output textbox.
		output_txtbx->AppendText("\r\nDevice Instance Index " + instanceIndexForFetchingPath.ToString() + " Path: " + devicePathManagedString);

		instanceIndexForFetchingPath++;

		//Only have 100 handles allocated, so even if there are more than 100 simultanesouly connected device with matching VID/PID,
		//don't bother opening handles to all of them.
		if(i >= 99)
		{
			break;
		}
	}

	//Update the form labels showing how many USB handles are currently open to the various hardware instances attached to the machine.
	writeHandles_lbl->Text = "Write Handles Open: " + writeHandlesOpened.ToString();
	readHandles_lbl->Text = "Read Handles Open: " + readHandlesOpened.ToString();


	return numberOfMatchingVIDPIDDevices;
}


	private: System::Void refresh_btn_Click(System::Object^  sender, System::EventArgs^  e) {

				 //Call the function that will update the device path list and device count values
				 //on the form.
				 GetDeviceCountAndOpenHandles();
			 }
};
}

