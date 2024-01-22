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
*******************************************************************************/



#include "USB_CDC_Routines.h"
#include "AboutDialog.h"


namespace USB_Serial_Terminal {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Threading;

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
		static volatile bool COMPortOpen = false;
		static volatile unsigned int VendorID;
		static volatile unsigned int ProductID;
		static volatile unsigned int BaudRate = 57600;

		static volatile String^ sendTXFileStringContents = "";

		#define SHARED_RX_BUFFER_SIZE	500000	//Using a pseudo-fixed buffer size, so we don't have to runtime reallocate more/less RAM repeatedly (which could waste CPU performance).
		static array<wchar_t>^ sharedRXBuffer;
		static volatile int sharedRXBytesAvailable = 0;

		static array<wchar_t>^ nonSharedRXBuffer;	//Not shared between threads.  Local copy for use by GUI thread only.






	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  settingsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  clearBufferToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  localEchoEnabledToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  enableLinewrapToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  sendFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveOutputToFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  enableOutputPrintingToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  showBytesInASCIIToolStripMenuItem;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::Label^  rx_rate_label;
	private: System::Windows::Forms::Label^  tx_rate_label;
	private: System::Windows::Forms::Button^  insert_space_button;
	private: System::Windows::Forms::CheckBox^  show_rx_data_checkbox;
	private: System::ComponentModel::BackgroundWorker^  serialReaderThread;

	public: 
		static String^ COMString;
		static Mutex^ RXDataMutex = gcnew Mutex;
	private: System::Windows::Forms::Button^  clearBuffer_btn;
	private: System::ComponentModel::BackgroundWorker^  serialWriterThread;



	private: System::Windows::Forms::ToolStripMenuItem^  fontToolStripMenuItem;
	private: System::Windows::Forms::FontDialog^  fontDialog1;
	private: System::Windows::Forms::RichTextBox^  output_textbox;

	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  statusStrip;
	private: System::Windows::Forms::ToolStripStatusLabel^  rxRateStatusStrip;
	private: System::Windows::Forms::ToolStripContainer^  toolStripContainer1;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;




	public: 

		Form1(void)
		{
			InitializeComponent();


			//Populate the COMxx port listbox with the detected COMxx ports.
			array<String^>^ serialPortNames = serialPort1->GetPortNames();
			for each(String^ portName in serialPortNames)
			{
				checkedListBox1->Items->Add(portName, false);
			}

			//Set the selected/active control on the form to the output textbox (instead of the default which would be the VID entry box).
			this->ActiveControl = output_textbox;


			//-------------------------------------------------------------------------
			//Note: Although the serialPort1->GetPortNames() API mostly works, it isn't
			//perfect.  According to the docs, it gets it's info from the registry.  However,
			//it is possible for the registry on a user's machine to be somehow corrupted
			//and for the list to come back somehow incorrectly.  It may therefore be desireable
			//to use some other method to fetch the COMxx port list names, possibly including QueryDosDevice() or
			//something similar.  For now, the below code is commented out, as it isn't being used.
			/*
				WCHAR dosData[100000];
				DWORD numChars = QueryDosDevice(NULL, (LPWSTR)&dosData, 100000);
				array<String^>^ stringsArray = {""};
				String^ dosDataString;

				int i = 0;
				int j = 0;
				while(i < numChars)
				{
					dosDataString = gcnew String(&dosData[i]);
					output_textbox->AppendText("\r\n" + dosDataString);
					i += dosDataString->Length + 1;
				}
			*/
			//-------------------------------------------------------------------------


			//Try to load any previously saved user application settings, if they exist on the hard drive.
			//NOTE: This should only be called AFTER doing a serialPort1->GetPortNames() operation and populating
			//the checkedListBox1 with COMxx strings.  The order is important, since the user may have stored
			//an open COMxx number value in the UserSettings.ini file, and in order to recheck the checkbox,
			//it must first be observed present on the machine.
			RestoreApplicationSettingsFromFile("UserSettings.ini");

			//Make the serial port and other settings (that may have been just loaded from the file) take effect.
			SettingsChangeHandler();


			//Initialize size of the managed array to 500000, so it can be used to pass data from the serial port reader thread to the GUI thread,
			//without having to continuously resize the buffer.  Make sure this initialization is done before starting the serialReaderThread or timer1.
			sharedRXBuffer->Resize(sharedRXBuffer, SHARED_RX_BUFFER_SIZE);
			sharedRXBytesAvailable = 0;

			//Allocate enough ram in the non-shared buffer to hold at least as much as the shared buffer, for making a local copy of everything.
			nonSharedRXBuffer->Resize(nonSharedRXBuffer, SHARED_RX_BUFFER_SIZE);

			//Enable the timer that updates the GUI and makes the COMx port connection dynamically.
			timer1->Enabled = true;

			//Start the serial port reader thread.
			serialReaderThread->RunWorkerAsync();

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			//Save application settings and shut things down gracefully.
			ApplicationShutdownTasks();


			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TextBox^  usb_vid_textbox;
	private: System::Windows::Forms::TextBox^  usb_pid_textbox;
	protected: 


	private: System::Windows::Forms::CheckedListBox^  checkedListBox1;
private: System::Windows::Forms::RadioButton^  connect_by_vid_pid_radio_button;

	private: System::Windows::Forms::RadioButton^  radioButton2;
	private: System::Windows::Forms::TextBox^  baudrate_textbox;





	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;


	private: System::Windows::Forms::Label^  connection_status_label;
	private: System::IO::Ports::SerialPort^  serialPort1;
private: System::Windows::Forms::Timer^  timer1;
	private: System::ComponentModel::IContainer^  components;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->usb_vid_textbox = (gcnew System::Windows::Forms::TextBox());
			this->usb_pid_textbox = (gcnew System::Windows::Forms::TextBox());
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->connect_by_vid_pid_radio_button = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->baudrate_textbox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->connection_status_label = (gcnew System::Windows::Forms::Label());
			this->serialPort1 = (gcnew System::IO::Ports::SerialPort(this->components));
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sendFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveOutputToFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->clearBufferToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->localEchoEnabledToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->enableLinewrapToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->enableOutputPrintingToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showBytesInASCIIToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->fontToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->rx_rate_label = (gcnew System::Windows::Forms::Label());
			this->tx_rate_label = (gcnew System::Windows::Forms::Label());
			this->insert_space_button = (gcnew System::Windows::Forms::Button());
			this->show_rx_data_checkbox = (gcnew System::Windows::Forms::CheckBox());
			this->serialReaderThread = (gcnew System::ComponentModel::BackgroundWorker());
			this->clearBuffer_btn = (gcnew System::Windows::Forms::Button());
			this->serialWriterThread = (gcnew System::ComponentModel::BackgroundWorker());
			this->fontDialog1 = (gcnew System::Windows::Forms::FontDialog());
			this->output_textbox = (gcnew System::Windows::Forms::RichTextBox());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->statusStrip = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->rxRateStatusStrip = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->menuStrip1->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->toolStripContainer1->BottomToolStripPanel->SuspendLayout();
			this->toolStripContainer1->ContentPanel->SuspendLayout();
			this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			this->toolStripContainer1->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// usb_vid_textbox
			// 
			this->usb_vid_textbox->Location = System::Drawing::Point(10, 6);
			this->usb_vid_textbox->MaxLength = 6;
			this->usb_vid_textbox->Name = L"usb_vid_textbox";
			this->usb_vid_textbox->Size = System::Drawing::Size(68, 20);
			this->usb_vid_textbox->TabIndex = 1;
			this->usb_vid_textbox->Text = L"0x04D8";
			this->usb_vid_textbox->Leave += gcnew System::EventHandler(this, &Form1::usb_vid_textbox_Leave);
			// 
			// usb_pid_textbox
			// 
			this->usb_pid_textbox->Location = System::Drawing::Point(10, 32);
			this->usb_pid_textbox->MaxLength = 6;
			this->usb_pid_textbox->Name = L"usb_pid_textbox";
			this->usb_pid_textbox->Size = System::Drawing::Size(68, 20);
			this->usb_pid_textbox->TabIndex = 2;
			this->usb_pid_textbox->Text = L"0x00DD";
			this->usb_pid_textbox->Leave += gcnew System::EventHandler(this, &Form1::usb_pid_textbox_Leave);
			// 
			// checkedListBox1
			// 
			this->checkedListBox1->CheckOnClick = true;
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Location = System::Drawing::Point(433, 6);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->ScrollAlwaysVisible = true;
			this->checkedListBox1->Size = System::Drawing::Size(173, 79);
			this->checkedListBox1->TabIndex = 3;
			// 
			// connect_by_vid_pid_radio_button
			// 
			this->connect_by_vid_pid_radio_button->AutoSize = true;
			this->connect_by_vid_pid_radio_button->Checked = true;
			this->connect_by_vid_pid_radio_button->Location = System::Drawing::Point(255, 6);
			this->connect_by_vid_pid_radio_button->Name = L"connect_by_vid_pid_radio_button";
			this->connect_by_vid_pid_radio_button->Size = System::Drawing::Size(148, 17);
			this->connect_by_vid_pid_radio_button->TabIndex = 4;
			this->connect_by_vid_pid_radio_button->TabStop = true;
			this->connect_by_vid_pid_radio_button->Text = L"Auto Connect by VID/PID";
			this->connect_by_vid_pid_radio_button->UseVisualStyleBackColor = true;
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(255, 27);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(156, 17);
			this->radioButton2->TabIndex = 5;
			this->radioButton2->Text = L"Manually Connect by COMx";
			this->radioButton2->UseVisualStyleBackColor = true;
			// 
			// baudrate_textbox
			// 
			this->baudrate_textbox->Location = System::Drawing::Point(630, 24);
			this->baudrate_textbox->Name = L"baudrate_textbox";
			this->baudrate_textbox->Size = System::Drawing::Size(68, 20);
			this->baudrate_textbox->TabIndex = 6;
			this->baudrate_textbox->Text = L"115200";
			this->baudrate_textbox->Leave += gcnew System::EventHandler(this, &Form1::baudrate_textbox_Leave);
			this->baudrate_textbox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::baudrate_textbox_KeyPress);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(627, 8);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(105, 13);
			this->label1->TabIndex = 9;
			this->label1->Text = L"Baud Rate (Decimal)";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(84, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(129, 13);
			this->label2->TabIndex = 10;
			this->label2->Text = L"USB Vendor ID (Hex VID)";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(84, 35);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(132, 13);
			this->label3->TabIndex = 11;
			this->label3->Text = L"USB Product ID (Hex PID)";
			// 
			// connection_status_label
			// 
			this->connection_status_label->AutoSize = true;
			this->connection_status_label->Location = System::Drawing::Point(252, 47);
			this->connection_status_label->Name = L"connection_status_label";
			this->connection_status_label->Size = System::Drawing::Size(93, 13);
			this->connection_status_label->TabIndex = 14;
			this->connection_status_label->Text = L"Connection: None";
			this->connection_status_label->Visible = false;
			// 
			// serialPort1
			// 
			this->serialPort1->BaudRate = 57600;
			// 
			// timer1
			// 
			this->timer1->Interval = 2;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// menuStrip1
			// 
			this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->fileToolStripMenuItem, 
				this->settingsToolStripMenuItem, this->helpToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(862, 24);
			this->menuStrip1->TabIndex = 17;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->sendFileToolStripMenuItem, 
				this->saveOutputToFileToolStripMenuItem, this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// sendFileToolStripMenuItem
			// 
			this->sendFileToolStripMenuItem->Name = L"sendFileToolStripMenuItem";
			this->sendFileToolStripMenuItem->Size = System::Drawing::Size(173, 22);
			this->sendFileToolStripMenuItem->Text = L"Send File";
			this->sendFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::sendFileToolStripMenuItem_Click);
			// 
			// saveOutputToFileToolStripMenuItem
			// 
			this->saveOutputToFileToolStripMenuItem->Name = L"saveOutputToFileToolStripMenuItem";
			this->saveOutputToFileToolStripMenuItem->Size = System::Drawing::Size(173, 22);
			this->saveOutputToFileToolStripMenuItem->Text = L"Save Output to File";
			this->saveOutputToFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveOutputToFileToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(173, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->clearBufferToolStripMenuItem, 
				this->localEchoEnabledToolStripMenuItem, this->enableLinewrapToolStripMenuItem, this->enableOutputPrintingToolStripMenuItem, 
				this->showBytesInASCIIToolStripMenuItem, this->fontToolStripMenuItem});
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->settingsToolStripMenuItem->Text = L"Settings";
			// 
			// clearBufferToolStripMenuItem
			// 
			this->clearBufferToolStripMenuItem->Name = L"clearBufferToolStripMenuItem";
			this->clearBufferToolStripMenuItem->Size = System::Drawing::Size(252, 22);
			this->clearBufferToolStripMenuItem->Text = L"Clear Buffer";
			this->clearBufferToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::clearBufferToolStripMenuItem_Click);
			// 
			// localEchoEnabledToolStripMenuItem
			// 
			this->localEchoEnabledToolStripMenuItem->Checked = true;
			this->localEchoEnabledToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->localEchoEnabledToolStripMenuItem->Name = L"localEchoEnabledToolStripMenuItem";
			this->localEchoEnabledToolStripMenuItem->Size = System::Drawing::Size(252, 22);
			this->localEchoEnabledToolStripMenuItem->Text = L"Local Echo Enabled";
			this->localEchoEnabledToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::localEchoEnabledToolStripMenuItem_Click);
			// 
			// enableLinewrapToolStripMenuItem
			// 
			this->enableLinewrapToolStripMenuItem->Checked = true;
			this->enableLinewrapToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->enableLinewrapToolStripMenuItem->Name = L"enableLinewrapToolStripMenuItem";
			this->enableLinewrapToolStripMenuItem->Size = System::Drawing::Size(252, 22);
			this->enableLinewrapToolStripMenuItem->Text = L"Enable Linewrap";
			this->enableLinewrapToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::enableLinewrapToolStripMenuItem_Click);
			// 
			// enableOutputPrintingToolStripMenuItem
			// 
			this->enableOutputPrintingToolStripMenuItem->Checked = true;
			this->enableOutputPrintingToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->enableOutputPrintingToolStripMenuItem->Name = L"enableOutputPrintingToolStripMenuItem";
			this->enableOutputPrintingToolStripMenuItem->Size = System::Drawing::Size(252, 22);
			this->enableOutputPrintingToolStripMenuItem->Text = L"Enable Output Printing";
			this->enableOutputPrintingToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::enableOutputPrintingToolStripMenuItem_Click);
			// 
			// showBytesInASCIIToolStripMenuItem
			// 
			this->showBytesInASCIIToolStripMenuItem->Name = L"showBytesInASCIIToolStripMenuItem";
			this->showBytesInASCIIToolStripMenuItem->Size = System::Drawing::Size(252, 22);
			this->showBytesInASCIIToolStripMenuItem->Text = L"Show bytes in Hex instead of ASCII";
			this->showBytesInASCIIToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showBytesInASCIIToolStripMenuItem_Click);
			// 
			// fontToolStripMenuItem
			// 
			this->fontToolStripMenuItem->Name = L"fontToolStripMenuItem";
			this->fontToolStripMenuItem->Size = System::Drawing::Size(252, 22);
			this->fontToolStripMenuItem->Text = L"Font";
			this->fontToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::fontToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(43, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(106, 22);
			this->aboutToolStripMenuItem->Text = L"About";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::aboutToolStripMenuItem_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"*.*";
			// 
			// rx_rate_label
			// 
			this->rx_rate_label->AutoSize = true;
			this->rx_rate_label->Location = System::Drawing::Point(626, 69);
			this->rx_rate_label->Name = L"rx_rate_label";
			this->rx_rate_label->Size = System::Drawing::Size(91, 13);
			this->rx_rate_label->TabIndex = 18;
			this->rx_rate_label->Text = L"RX Rate: 0 b/sec";
			this->rx_rate_label->Visible = false;
			// 
			// tx_rate_label
			// 
			this->tx_rate_label->AutoSize = true;
			this->tx_rate_label->Location = System::Drawing::Point(627, 47);
			this->tx_rate_label->Name = L"tx_rate_label";
			this->tx_rate_label->Size = System::Drawing::Size(90, 13);
			this->tx_rate_label->TabIndex = 19;
			this->tx_rate_label->Text = L"TX Rate: 0 b/sec";
			this->tx_rate_label->Visible = false;
			// 
			// insert_space_button
			// 
			this->insert_space_button->Location = System::Drawing::Point(321, 62);
			this->insert_space_button->Name = L"insert_space_button";
			this->insert_space_button->Size = System::Drawing::Size(95, 23);
			this->insert_space_button->TabIndex = 20;
			this->insert_space_button->Text = L"Insert 3x Space";
			this->insert_space_button->UseVisualStyleBackColor = true;
			this->insert_space_button->Click += gcnew System::EventHandler(this, &Form1::insert_space_button_Click);
			// 
			// show_rx_data_checkbox
			// 
			this->show_rx_data_checkbox->AutoSize = true;
			this->show_rx_data_checkbox->Checked = true;
			this->show_rx_data_checkbox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->show_rx_data_checkbox->Location = System::Drawing::Point(116, 66);
			this->show_rx_data_checkbox->Name = L"show_rx_data_checkbox";
			this->show_rx_data_checkbox->Size = System::Drawing::Size(97, 17);
			this->show_rx_data_checkbox->TabIndex = 21;
			this->show_rx_data_checkbox->Text = L"Show RX Data";
			this->show_rx_data_checkbox->UseVisualStyleBackColor = true;
			this->show_rx_data_checkbox->CheckedChanged += gcnew System::EventHandler(this, &Form1::show_rx_data_checkbox_CheckedChanged);
			// 
			// serialReaderThread
			// 
			this->serialReaderThread->WorkerReportsProgress = true;
			this->serialReaderThread->WorkerSupportsCancellation = true;
			this->serialReaderThread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::serialReaderThread_DoWork);
			// 
			// clearBuffer_btn
			// 
			this->clearBuffer_btn->Location = System::Drawing::Point(10, 62);
			this->clearBuffer_btn->Name = L"clearBuffer_btn";
			this->clearBuffer_btn->Size = System::Drawing::Size(95, 23);
			this->clearBuffer_btn->TabIndex = 22;
			this->clearBuffer_btn->Text = L"Clear Buffer";
			this->clearBuffer_btn->UseVisualStyleBackColor = true;
			this->clearBuffer_btn->Click += gcnew System::EventHandler(this, &Form1::clearBuffer_btn_Click);
			// 
			// serialWriterThread
			// 
			this->serialWriterThread->WorkerReportsProgress = true;
			this->serialWriterThread->WorkerSupportsCancellation = true;
			this->serialWriterThread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Form1::serialWriterThread_DoWork);
			// 
			// fontDialog1
			// 
			this->fontDialog1->Font = (gcnew System::Drawing::Font(L"Courier New", 9.75F, System::Drawing::FontStyle::Bold));
			// 
			// output_textbox
			// 
			this->output_textbox->AcceptsTab = true;
			this->output_textbox->BackColor = System::Drawing::SystemColors::Window;
			this->output_textbox->DetectUrls = false;
			this->output_textbox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->output_textbox->Font = (gcnew System::Drawing::Font(L"Courier New", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(186)));
			this->output_textbox->HideSelection = false;
			this->output_textbox->Location = System::Drawing::Point(0, 0);
			this->output_textbox->Name = L"output_textbox";
			this->output_textbox->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::ForcedBoth;
			this->output_textbox->Size = System::Drawing::Size(862, 467);
			this->output_textbox->TabIndex = 0;
			this->output_textbox->Text = L"";
			this->output_textbox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::output_textbox_KeyDown);
			// 
			// statusStrip1
			// 
			this->statusStrip1->AllowMerge = false;
			this->statusStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->statusStrip, this->rxRateStatusStrip});
			this->statusStrip1->Location = System::Drawing::Point(0, 0);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(862, 22);
			this->statusStrip1->TabIndex = 23;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// statusStrip
			// 
			this->statusStrip->Margin = System::Windows::Forms::Padding(0, 3, 100, 2);
			this->statusStrip->Name = L"statusStrip";
			this->statusStrip->Overflow = System::Windows::Forms::ToolStripItemOverflow::Never;
			this->statusStrip->Size = System::Drawing::Size(101, 17);
			this->statusStrip->Text = L"Connection: None";
			// 
			// rxRateStatusStrip
			// 
			this->rxRateStatusStrip->Name = L"rxRateStatusStrip";
			this->rxRateStatusStrip->Size = System::Drawing::Size(88, 17);
			this->rxRateStatusStrip->Text = L"RX Rate: 0 b/sec";
			// 
			// toolStripContainer1
			// 
			// 
			// toolStripContainer1.BottomToolStripPanel
			// 
			this->toolStripContainer1->BottomToolStripPanel->Controls->Add(this->statusStrip1);
			// 
			// toolStripContainer1.ContentPanel
			// 
			this->toolStripContainer1->ContentPanel->Controls->Add(this->splitContainer1);
			this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(862, 556);
			this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
			this->toolStripContainer1->Name = L"toolStripContainer1";
			this->toolStripContainer1->Size = System::Drawing::Size(862, 602);
			this->toolStripContainer1->TabIndex = 24;
			this->toolStripContainer1->Text = L"toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->menuStrip1);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->clearBuffer_btn);
			this->splitContainer1->Panel1->Controls->Add(this->show_rx_data_checkbox);
			this->splitContainer1->Panel1->Controls->Add(this->insert_space_button);
			this->splitContainer1->Panel1->Controls->Add(this->tx_rate_label);
			this->splitContainer1->Panel1->Controls->Add(this->rx_rate_label);
			this->splitContainer1->Panel1->Controls->Add(this->connection_status_label);
			this->splitContainer1->Panel1->Controls->Add(this->label3);
			this->splitContainer1->Panel1->Controls->Add(this->label2);
			this->splitContainer1->Panel1->Controls->Add(this->label1);
			this->splitContainer1->Panel1->Controls->Add(this->radioButton2);
			this->splitContainer1->Panel1->Controls->Add(this->baudrate_textbox);
			this->splitContainer1->Panel1->Controls->Add(this->connect_by_vid_pid_radio_button);
			this->splitContainer1->Panel1->Controls->Add(this->usb_pid_textbox);
			this->splitContainer1->Panel1->Controls->Add(this->checkedListBox1);
			this->splitContainer1->Panel1->Controls->Add(this->usb_vid_textbox);
			this->splitContainer1->Panel1MinSize = 85;
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->output_textbox);
			this->splitContainer1->Panel2MinSize = 85;
			this->splitContainer1->Size = System::Drawing::Size(862, 556);
			this->splitContainer1->SplitterDistance = 85;
			this->splitContainer1->TabIndex = 25;
			this->splitContainer1->TabStop = false;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(862, 602);
			this->Controls->Add(this->toolStripContainer1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"USB Serial Terminal";
			this->SizeChanged += gcnew System::EventHandler(this, &Form1::Form1_SizeChanged);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->toolStripContainer1->BottomToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->BottomToolStripPanel->PerformLayout();
			this->toolStripContainer1->ContentPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->PerformLayout();
			this->toolStripContainer1->ResumeLayout(false);
			this->toolStripContainer1->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel1->PerformLayout();
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {

				 static int tickIndex = 0;
				 static ULONGLONG oldTickTime = 0;
				 static ULONGLONG newTickTime = 0;
				 static unsigned long int recentRXDataBytes = 0;
				 double RXDataRate = 0;
				 double secondsElapsed;
				 String^ newData;
				 static ULONGLONG oldTickWhenLastFocused = 0;
				 static bool unprocessedDataAvailable = false;
				 static bool lastCharacterPrintedWasCarriageReturn = false;
				 static int newBytesReceived = 0;

				 //Get the current time.  Needed for data rate measurements.
				 newTickTime = GetTickCount64();

				 tickIndex++;
				 //Periodically try to find/connect/disconnect to/from the COMx port, so the application has plug and play responsiveness.
				 if((tickIndex % 10) == 0)
				 {
					 //Try to find/connect (or close) it, depending on the current and previous connection state.
					 COMPortOpenClosePnPTasks();
				 }

				 //Periodically update the RX data rate label
				 if(COMPortOpen == true)
				 {
					 if((tickIndex % 50) == 0)
					 {
						 newTickTime = GetTickCount64();

						 secondsElapsed = (double)(newTickTime - oldTickTime) * (double)0.001;
						 if(secondsElapsed == 0)
						 {
							 secondsElapsed = 0.0001;
						 }

						 RXDataRate = (double)recentRXDataBytes / secondsElapsed;

						 rx_rate_label->Text = "RX Rate: " + RXDataRate.ToString("N0") + " b/sec";
						 rxRateStatusStrip->Text = rx_rate_label->Text;

						 oldTickTime = newTickTime;
						 recentRXDataBytes = 0;
					 }
				 }
				 else
				 {
					 //COMx port not open.  Data rate is gauranteed to be zero.
					 oldTickTime = GetTickCount64();
					 recentRXDataBytes = 0;
					 rx_rate_label->Text = "RX Rate: 0 b/sec";
					 rxRateStatusStrip->Text = rx_rate_label->Text;
				 }


				 //Try to fetch any (new) data that the serialReaderThread may have read
				 //from the COMx port, and then print that newly read data (if any) to the output terminal window.
				 //Only do so however, if we have finished processing the last data we read from the thread.
				 if(unprocessedDataAvailable == false)
				 {
					 if(RXDataMutex->WaitOne(100) == true)
					 {
						 //Copy any data in the shared buffer to a local buffer for printing to the output textbox.
						 newBytesReceived = sharedRXBytesAvailable;
						 if(newBytesReceived != 0)
						 {
							 //Check if we are actually going to use/need the data, before wasting time copying it.
							 if(enableOutputPrintingToolStripMenuItem->Checked == true)
							 {
								 //Copy each byte from the shared buffer to a local buffer that is not shared.
								 for(int i = 0; i < newBytesReceived; i++)
								 {
									 nonSharedRXBuffer[i] = sharedRXBuffer[i];
								 }
								 //We consumed all the shared buffer data, so set the buffer contents tracking variable back to 0.
								 sharedRXBytesAvailable = 0;
								 recentRXDataBytes += newBytesReceived;	//Keep track of recent data bytes received, for computing average data rate.
								 unprocessedDataAvailable = true;
							 }
							 else
							 {
								 //No point in even copying the received data, since we are just going to throw it away anyway.
								 //Just zero the byte count to "consume" the data by throwing it away.
								 sharedRXBytesAvailable = 0;
								 recentRXDataBytes += newBytesReceived;	//Keep track of recent data bytes received, for computing average data rate.
								 newBytesReceived = 0;
							 }
						 }
						 RXDataMutex->ReleaseMutex();
					 }
					 else
					 {
						 //We failed to get control of the shared resources.  In this case, we won't have any new data to process just yet.
					 }
				 }

				 //Convert from managed wchar_t array data, to a managed String^, then print it to the textbox.
				 if(unprocessedDataAvailable)
				 {
					 //Consuming/processing the data now.
					 unprocessedDataAvailable = false;

					 //Append any newly received text to the textbox, assuming the user is allowing printing.
					 if(enableOutputPrintingToolStripMenuItem->Checked == true)
					 {
						 //Convert from managed wchar_t array to managed String^
						 newData = gcnew String(nonSharedRXBuffer, 0, newBytesReceived);

						 //Check if the data should be formatted as an ASCII string or if it should be printed
						 //in hexadecimal format instead (ex: "A5 B6 7C 3F", etc.).
						 if(showBytesInASCIIToolStripMenuItem->Checked == true)
						 {
							 //Convert the data into hex formatted byte pairs (plus a space) then append the new string to the output textbox.
							 output_textbox->AppendText(ConvertStringToHexByteFormattedString(newData));
						 }
						 else
						 {
							 //Print it as an ASCII string.

							 //Optionally parse the newData String and check for special sequence "\r\r\n".  This can happen if a device sends
							 //"\r\n" using something like printf(), but the printf() routine implemented by the compiler they are using
							 //mangles the data (which is allowed by the C99 specs) by converting "\n" to "\r\n", resulting in "\r\n" convering
							 //to "\r\r\n", which doesn't show up well when printed directly to the textbox (it shows up as a single space character
							 //(instead of a carriage return/linefeed).
							 
							 //Note: Optionally disable the below code if you don't want to manipulate the data (which is good for human viewing, but bad if you
							 //are trying to receive binary files and want to keep them as close to binary exact as possible - which it should be mentioned isn't
							 //really feasible with the RichTextBox^ control, due to special handling of special character sequences anyway).
							 if(1)	
							 {
								 int index = 0;

								 while(index != -1)
								 {
									 //Try to find any special "\r\r\n" character sequences in the data set.
									 //Note: IndexOf() method returns -1 if no instances of the specified substring are found.
									 index = newData->IndexOf("\r\r\n", index);
									 if(index >= 0)
									 {
										 //At least one "\r\r\n" was found.  Remove one of the \r characters.
										 newData = newData->Remove(index, 1);
									 }
								 }
							 }

							 //Note: Printing \r\n to the RichTextBox^ with the AppendText() method has some special considerations.
							 //If for example a string is printed as follows:
							 //
							 //myRichTextBox->AppendText("\r\nSome initial text.\r\nSecond line text.");
							 //
							 //And you print that string directly in one call to AppendText(), everything will look like the user expects. Ex:
							 //Line 1: Some initial text.
							 //Line 2: Second line text.

							 //However, if the data set is split between the \r and the adjacent \n, and is printed to the RichTextBox 
							 //with two calls to the AppendText() method, you will get an unanticipated extra new line, eg. this code:
							 //myRichTextBox->AppendText("\r\nSome initial text.\r");
							 //myRichTextBox->AppendText("\nSecond line text.");
							 //
							 //Will print:
							 //Line1: Some initial text.
							 //Line2: 
							 //Line3: Second line text.
							 //
							 //To compensate/avoid this scenario/unwanted extra line, it is necessary to check for trailing \r characters,
							 //and if found, omit the next call's starting \n, if (and only if) indeed the next AppendText call does start with \n.
							 if(lastCharacterPrintedWasCarriageReturn == true)
							 {
								 //Check if the very next character we are about to print is a \n (which it often will, since
								 //user data streams will usually have \r\n sequences for starting a new line)
								 if(newData->Substring(0, 1) == "\n")
								 {
									 //Deliberately remove the \n starting character, since the immediately preceeding \r already
									 //caused a new line to start (when it was the last character printed to the RichTextBox^ using AppendText()).
									 if(newData->Length > 1)
									 {
										 //Remove the starting \n, but keep everything else intact.
										 newData = newData->Substring(1, newData->Length - 1);
									 }
									 else
									 {
										 //The \n character is the only one present in the string.
										 //Don't need to print anything.
										 newData = "";
									 }
								 }
								 lastCharacterPrintedWasCarriageReturn = false;									 
							 }

							 //Check if the above manipulation didn't make the string length 0.  Only print something if there is a non-zero length string remaining.
 							 if(newData->Length != 0)
							 {
								 //Check if the last character in the string we are about to print ends with \r.
								 if(newData->Substring(newData->Length - 1, 1) == "\r")
								 {
									 //Keep track of this fact, so we can omit the (potential future) immediately following \n character, if one arrives (before any other types of character)
									 lastCharacterPrintedWasCarriageReturn = true;
								 }

								 //Add the new data to the textbox.  This also scrolls the textbox to the bottom.  NOTE: Important to set the
								 //"HideSelection" textbox property to false, or else the scoll feature stops when the form is not the active Window.
								 output_textbox->AppendText(newData);
							 }


						 }//else of if(showBytesInASCIIToolStripMenuItem->Checked == true)
					 }//if(enableOutputPrintingToolStripMenuItem->Checked == true)
					 else
					 {
						 //Just throw the data away, since we don't actually need it for anything.
					 }
				 }//if(unprocessedDataAvailable)

}


//Helper function that converts plain text ASCII strings from something like "ABCD" to a hexadecimal byte value representation, ex: "65 66 67 68". 
String^ ConvertStringToHexByteFormattedString(String^ inputTextStringToConvertToHexView)
{
	 String^ newHexDataString = "";
	 String^ workingByte;
	 int i;

	 //Convert the input string into a wchar array.
	 array<wchar_t>^ rawHexDataBytes = inputTextStringToConvertToHexView->ToCharArray();

	 //Convert each wchar_t LSB byte upper and lower nibbles into new characters for the output string.
	 //For performance reasons, only convert the most recent 10,000 characters or less.
	 i = 0;
	 if(inputTextStringToConvertToHexView->Length > 10000)
	 {
		 i = inputTextStringToConvertToHexView->Length - 10000;
	 }
	 for(; i < inputTextStringToConvertToHexView->Length; i++)
	 {
		 //Convert the upper nibble to an ASCII character.
		 workingByte = (rawHexDataBytes[i] >> 4).ToString("X1");
		 //Append the upper nibble to the string.
		 newHexDataString += workingByte;
		 //Convert the lower nibble to an ASCII character.
		 workingByte = (rawHexDataBytes[i] & 0x0F).ToString("X1");
		 //Append the lower nibble plus a following space (to visually separate each byte) to the string.
		 newHexDataString += workingByte + " ";
	 }

	 return newHexDataString;
}


//Call this in response to the user changing something associated with their COMx port (ex: baud rate value, USB VID/PID, etc.).
void SettingsChangeHandler(void)
{
	static unsigned int oldBaudRate = 0; 
	static WORD oldVID = 0;
	static WORD oldPID = 0;

	//Try to parse the texbox fields that the user can enter into.
	//However, these operations may fail, if the user entered invalid/unparsable data.
	try
	{
		VendorID = Convert::ToUInt32(usb_vid_textbox->Text, 16);
		if(VendorID > 65535)
		{
			//User entered invalid data.  VIDs are 16-bit numbers, so cannot exceed 65535.
			VendorID = 0;
		}
	}
	catch(...)
	{
		VendorID = 0;
	}
	try
	{
		ProductID = Convert::ToUInt32(usb_pid_textbox->Text, 16);
		if(ProductID > 65535)
		{
			//User entered invalid data.  PIDs are 16-bit numbers, so cannot exceed 65535.
			ProductID = 0;
		}
	}
	catch(...)
	{
		ProductID = 0;
	}
	try
	{
		BaudRate = Convert::ToUInt32(baudrate_textbox->Text, 10);
	}
	catch(...)
	{
		BaudRate = 19200;
		baudrate_textbox->Text = "19200";
	}


	//Check for change in USB VID/PID.
	if((ProductID != oldPID) || (VendorID != oldVID))
	{
		//Close the COMx port if the port was previously open.
		if(COMPortOpen == true)
		{
			CloseCOMPort();
		}
		oldPID = ProductID;
		oldVID = VendorID;

		//Now attempt to re-open the COMx port with the new VID/PID value.
		COMPortOpenClosePnPTasks();
	}
	
	//Update the serial port baud rate, if the user just changed the value.
	if(BaudRate != oldBaudRate)
	{
		if(COMPortOpen == true)
		{
			try
			{
				serialPort1->BaudRate = BaudRate;
				connection_status_label->Text = "Connection: " + COMString + " Open";
				statusStrip->Text = connection_status_label->Text + " at " + serialPort1->BaudRate.ToString() + " baud";
			}
			catch(...)
			{
			}
		}
		oldBaudRate = BaudRate;
	}
}



bool COMPortOpenClosePnPTasks(void)
{
	bool retValue = false;

	try
	{
		//Check the previous device COMx port status.
		if(COMPortOpen == true)
		{
			//The COMx port was previously already open.  Check if it has recently become detached or the user wants to
			//close it.
			if(connect_by_vid_pid_radio_button->Checked == true)
			{
				if((VendorID > 0) && (VendorID < 65565) && (ProductID > 0) && (ProductID < 65565))
				{
					//Do a check to see if the USB hardware device implementing the COMx port is still present.
					COMString = USB_CheckIfPresentAndGetCOMString(VendorID, ProductID, 0, false, 0);
					if(COMString == "")
					{
						//The device is not currently attached to the machine.  This means that the user must have just unplugged it (or power cycled it, etc.).
						//On Windows OSes prior to Win 10, it is important to close the COMx port while the hardware is still detached,
						//prior to the user attempting to reattach/re-enumerate.
						retValue = false;
						if(serialPort1->IsOpen == true)
						{
							CloseCOMPort();
						}
					}
					else
					{
						retValue = true;
					}
				}
			}//if(connect_by_vid_pid_radio_button->Checked == true)
			else
			{
				//Check if the user is running in manual connect mode and they just unchecked the selected COMx port.
				if(checkedListBox1->CheckedItems->Count != 1)
				{
					retValue = false;
					if(serialPort1->IsOpen == true)
					{
						CloseCOMPort();
					}
				}
			}

			//return retValue;
		}//if(COMPortOpen == true)
		else
		{
			//The COM port was previously closed.  Check if the device is available/selected by the user and ready to be opened.

			//Check if we should try to auto connect by USB VID/PID.
			if(connect_by_vid_pid_radio_button->Checked == true)
			{
				if((VendorID > 0) && (VendorID < 65565) && (ProductID > 0) && (ProductID < 65565))
				{
					COMString = USB_CheckIfPresentAndGetCOMString(VendorID, ProductID, 0, false, 0);
					if((COMString != "") && (COMString->StartsWith("COM")))
					{
						try
						{
							if(serialPort1->IsOpen == true)
							{
								CloseCOMPort();
							}
							if(serialPort1->IsOpen == false)
							{
								//Try to open the COM port now.
								retValue = OpenCOMPort(COMString, BaudRate);
							}
						}
						catch(...)
						{
							COMPortOpen = false;
							connection_status_label->Text = "Connection: None (error attempting to open " + COMString + ")";
							statusStrip->Text = connection_status_label->Text;
						}
					}
					else
					{
						//Couldn't find any attached USB CDC devices with matching VID/PID.
						connection_status_label->Text = "Connection: None (no device with matching VID/PID)";
						statusStrip->Text = connection_status_label->Text;
					}
				}
				else
				{
					//Wuh?  Wrong VID/PID values.
					connection_status_label->Text = "Connection: None (invalid VID or PID)";
					statusStrip->Text = connection_status_label->Text;
				}
			}//if(connect_by_vid_pid_radio_button->Checked == true)
			else
			{
				//User wants to manually connect.

				//Find the checked COMx port in the list box.
				if(checkedListBox1->CheckedItems->Count == 1)
				{
					COMString = checkedListBox1->CheckedItems[0]->ToString();

					if(COMString->StartsWith("COM"))
					{
						if(serialPort1->IsOpen == true)
						{
							CloseCOMPort();
						}
						if(serialPort1->IsOpen == false)
						{
							//Try to open the COMx port now.
							retValue = OpenCOMPort(COMString, BaudRate);
						}
					}
				}

			}
		}



		//Update the COMx port list, if needed.
		RefreshCOMPortChecklistBox();

	}
	catch(...)
	{
		//Wuh?  Something went wrong?		
	}

	return retValue;
}


//This function attempts to opens the COMx port and set it to the specified baud rate.
//Returns true if it succeeded and the COM port is now open, or false if the COM port
//could not be opened due to an error.  If you call this function when the COMx port is already
//open, it will close the COMx port, block for ~50ms, then attempt to re-open it.
bool OpenCOMPort(String^ COMPortName, int baudRate)
{
	bool retValue = false;

	try
	{
		//First make sure it is not already open.  If so, close it first.
		if(serialPort1->IsOpen == true)
		{
			CloseCOMPort();
			Sleep(20);	//Wait some extra time to be extra sure it is really finished closing, since we are about to re-open it immediately.
		}
		if(serialPort1->IsOpen == false)
		{
			serialPort1->PortName = COMPortName;
			serialPort1->BaudRate = baudRate;
			serialPort1->Open();
			if(serialPort1->IsOpen == true)
			{
				COMPortOpen = true;
				connection_status_label->Text = "Connection: " + COMString + " Open";
				statusStrip->Text = connection_status_label->Text + " at " + serialPort1->BaudRate.ToString() + " baud";
				retValue = true;
			}
			else
			{
				//Wuh?  Something went wrong (possibly because not enough dwell while closed, or some other application software
				//on the machine already has the COM port open)?
				retValue = false;
				connection_status_label->Text = "Connection: Error attempting to open " + COMString;
				statusStrip->Text = connection_status_label->Text;
			}
		}
	}
	catch(Exception^ e)
	{
		//Wuh?  Something went wrong (possibly because not enough dwell while closed, or some other application software
		//on the machine already has the COM port open)?
		retValue = false;
		
		//Check for the access denied message, which is the most likely (due to the user having more than one program trying to open
		//the same COMx port number at the same time).
		if(e->Message->StartsWith("Access to the port"))
		{
			connection_status_label->Text = "Connection: None; " + e->Message + " (is it already open by another program?)";
		}
		else
		{
			connection_status_label->Text = "Connection: None; " + e->Message;
		}
		statusStrip->Text = connection_status_label->Text;
	}

	return retValue;
}



//This closes the COMx port and updates the connection_status_label to indicate
//that no connection is present.  Note: This function will block for ~30ms whenever
//it closes the COMx port, in an effort to reduce the chance of someone trying to re-open
//it again too quickly (and thus causing potential problems).  The MSDN documentation for 
//the serialPort class says that the close operation takes awhile to finish (ex: as a 
//background task), even though the close API returns promptly/is non-blocking.  If you
//try to re-open it too soon after closing it, this can cause an error/exception to get thrown
//during the re-open attempt.
bool CloseCOMPort(void)
{
	bool retValue = false;

	try
	{
		COMPortOpen = false;
		connection_status_label->Text = "Connection: None";
		statusStrip->Text = connection_status_label->Text;
		if(serialPort1->IsOpen == true)
		{
			serialPort1->Close();
			Sleep(30);	//Wait awhile for it to really finish closing.  MSDN docs say it takes awhile in the background to finish (even though serialPort1->Close() is non-blocking).
		}
		retValue = true;
	}
	catch(...)
	{
		//Wuh?  Couldn't even close it?
		connection_status_label->Text = "Connection: None (error closing port)";
		statusStrip->Text = connection_status_label->Text;
	}

	return retValue;
}



void RefreshCOMPortChecklistBox(void)
{
	int i;
	bool refreshNeeded = false;

	//Refresh the COMx port names list if needed.
	i = 0;
	//Get an array of strings, containing the COMxx strings (ex: COM1, COM3, COM20, etc.)
	array<String^>^ serialPortNames = serialPort1->GetPortNames();
	//Iterate through each array element, looking for newly changed COMxx strings that do no match the listbox.
	for each(String^ portName in serialPortNames)
	{
		if(i < checkedListBox1->Items->Count)
		{
			//Check if the listbox entry doesn't 100% match the recently read value.  If so, set flag to update the listbox.
			if(serialPortNames[i] != checkedListBox1->Items[i]->ToString())
			{
				refreshNeeded = true;
				break;
			}
		}
		i++;
	}

	if(i != checkedListBox1->Items->Count)
	{
		refreshNeeded = true;
	}

	if(refreshNeeded == true)
	{
		//Clear and repopulated the COMx port list.
		int index = 0;
		//checkedListBox1->Items->Clear();
		for each(String^ portName in serialPortNames)
		{
			unsigned int portNumber = 1;
			String^ portNumberString;
			bool portNameAppearsValid = false;
			if(portName->StartsWith("COM"))
			{
				//Try to extract only the port number string.
				portNumberString = portName->Remove(0, 3);	//Remove "COM" from the beginning (ex: "COM15" --> "15")

				//Now try to convert the portNumberString into an actual int type of number.
				try
				{
					//Convert base 10 string into a uint32.
					portNumber = Convert::ToUInt32((String^)portNumberString, 10);
					//If no exception, then the COMxx format must have been valid (where xx is a properly parsable number)
					portNameAppearsValid = true;
				}
				catch(...)
				{
					//If we get here, that probably means the portNumberString somehow contained invalid character(s), ex: "COM15z" --> "15z" --> exception/can't convert. 
					//This implies that the registry key storing the serialPortNames fetched by serialPort1->GetPortNames() is somehow corrupted
					//on the user's computer.
					portNameAppearsValid = false;
				}
			}

			if(portNameAppearsValid == true)
			{
				if(index >= checkedListBox1->Items->Count)
				{
					//No enough current positions in the listbox for the new string; add a new one to the list.
					checkedListBox1->Items->Add(portName, false);
				}
				else
				{
					//Replace the existing item entry instead, assuming the new string is actually different from the old
					if(portName != checkedListBox1->Items[index]->ToString())
					{
						checkedListBox1->Items->RemoveAt(index);
						checkedListBox1->Items->Insert(index, portName);
					}
				}
				
				index++;
			}
		}//for each(String^ portName in serialPortNames)

		//Make sure we have enough total items in the list box.  This can happen if the number of COMx ports has recently shrunk.
		//Remove the excess entries from the listbox.
		while(checkedListBox1->Items->Count > index)
		{
			try
			{
				checkedListBox1->Items->RemoveAt(index);
			}
			catch(...)
			{
				//Couldn't remove it?
				int asdf = 3;
			}
		}
	}//if(refreshNeeded == true)
}



//--------------------------------------------------

private: System::Void Form1_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			 //output_textbox->Size
		 }

private: System::Void enableLinewrapToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //Toggle the wordwrap setting for the output textbox.  When wordwrap is on, the text flows into the next line
			 //whenever it gets too long to fit on the screen without a scrollbar.  When wordwrap is off, lines can get super long in the x direction,
			 //until the COMx device sends carriage return/linefeed bytes.
			 if(enableLinewrapToolStripMenuItem->Checked == true)
			 {
				 enableLinewrapToolStripMenuItem->Checked = false;
				 output_textbox->WordWrap = false;
			 }
			 else
			 {
				 enableLinewrapToolStripMenuItem->Checked = true;
				 output_textbox->WordWrap = true;
			 }
		 }


private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 Application::Exit();
		 }


private: System::Void localEchoEnabledToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 
			 //The user just clicked the Settings->Local echo toggle item.
			 if(localEchoEnabledToolStripMenuItem->Checked == true)
			 {
				 localEchoEnabledToolStripMenuItem->Checked = false;
			 }
			 else
			 {
				 localEchoEnabledToolStripMenuItem->Checked = true;
			 }
		 }


private: System::Void clearBufferToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 output_textbox->Text = "";
		 }


private: System::Void enableOutputPrintingToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(enableOutputPrintingToolStripMenuItem->Checked == true)
			 {
				 enableOutputPrintingToolStripMenuItem->Checked = false;
				 show_rx_data_checkbox->Checked = false;
			 }
			 else
			 {
				 enableOutputPrintingToolStripMenuItem->Checked = true;
				 show_rx_data_checkbox->Checked = true;
			 }
		 }


private: System::Void saveOutputToFileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 String^ filePath;

			 try
			 {
				 saveFileDialog1->ShowDialog();

				 filePath = saveFileDialog1->FileName;
				 if(Directory::Exists(Path::GetDirectoryName(filePath)))
				 {
					 //The folder the user selected exists.  Try to write the output textbox data to the file selected.
					 File::WriteAllText(filePath, output_textbox->Text);
				 }
			 }
			 //catch(System::Exception^ e)
			 catch(...)
			 {


			 }

			 //if(saveFileDialog1->FileName



		 }
private: System::Void output_textbox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {

			 BYTE keyStates[256];
			 String^ keyAsString = "";
			 WORD ASCIIKeyBuffer = 0;

			 //Get the current state of all the keys (useful for getting shift/caps lock state)
			 GetKeyboardState((PBYTE)&keyStates);

			 //Convert to ASCII
			 int retVal = ToAscii((UINT)e->KeyValue, 0x80, &keyStates[0], (LPWORD)&ASCIIKeyBuffer, 0);
			 if(retVal == 1)
			 {
				 keyAsString = gcnew String((const char*)&ASCIIKeyBuffer, 0, 1);
			 }

			 //Do special check to see if the user pressed "Control + a" or (some other Control + some char, like c), in which case they presumably want to copy
			 //all the textbox data to the clipboard, instead of sending the characters down to the DUT (which might have an effect on the DUT code, if it was waiting for a character).
			 if(GetKeyState(VK_CONTROL) & 0x8000) 
			 {
				 if(GetKeyState(0x41) & 0x8000)		//0x41 is the "a" key
				 {
					 //The user pressed "Control + a", so highlight all the items in the text box.
					 output_textbox->Select();
					 output_textbox->SelectionStart = 0;
					 output_textbox->SelectionLength = output_textbox->Text->Length;
				 }
			 }
			 else
			 {
				 //If the COMx port is open, send the key character to the device.
				 if((COMPortOpen == true) && (keyAsString != ""))
				 {
					 try
					 {
						 if(serialPort1->IsOpen == true)
						 {
							 serialPort1->Write(keyAsString);
						 }
					 }
					 catch(...)
					 {

					 }
				 }

				 //Check if we need to print a local echo copy of the character that was just pressed/sent.
				 if(localEchoEnabledToolStripMenuItem->Checked == true)
				 {
					 output_textbox->ReadOnly = false;
					 //output_textbox->AppendText(keyAsString);	//Note: Don't need to do this.  When the ReadOnly property is false, the textbox directly accepts/prints the character data by default anyway.
				 }
				 else
				 {
					 output_textbox->ReadOnly = true;
				 }
			 }



		 }

private: System::Void sendFileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 //Open the file selector dialog window so the user can select a file to send out the COMx port.
			 openFileDialog1->ShowDialog();
			 try
			 {
				 if(File::Exists(openFileDialog1->FileName))
				 {
					 if((COMPortOpen == true) && (serialPort1->IsOpen == true))
					 {
						 //No point in trying to send a new file, if the last file that was attempted to be sent is still being transmitted.
						 if(serialWriterThread->IsBusy == false)
						 {
							 //Read in all the file data to a String in RAM.
							 sendTXFileStringContents = File::ReadAllText(openFileDialog1->FileName);
							 if(sendTXFileStringContents != "")
							 {
								 //Now launch a thread to do the actual writing to the COMx port, since this operation
								 //could take a very long time, and we don't want to block the GUI thread from being 
								 //responsive (and consuming/printing the RX path data to the terminal window).
								 serialWriterThread->RunWorkerAsync();
							 }
						 }
					 }
				 }
			 }
			 catch(...)
			 {
			 }
		 }

private: System::Void showBytesInASCIIToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(showBytesInASCIIToolStripMenuItem->Checked == true)
			 {
				 showBytesInASCIIToolStripMenuItem->Checked = false;
			 }
			 else
			 {
				 showBytesInASCIIToolStripMenuItem->Checked = true;

				 output_textbox->Focus();

				 //Convert the existing data already in the textbox to the hex view format.
				 output_textbox->Text = ConvertStringToHexByteFormattedString(output_textbox->Text);
			 }
		 }


private: System::Void insert_space_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 //Insert three arbitrary spaces in the output textbox.
			 //The normal purpose for doing this is to "justify" repeating data byte streams, when viewed in the
			 //hex formatted byte pair mode.  For example:
			 //01 02 03 04 05 00
			 //01 02 03 04 05 00
			 //01 02 03 04 05 00
			 //Can be repeatedly padded to become:
			 //00 01 02 03 04 05
			 //00 01 02 03 04 05
			 //00 01 02 03 04 05

			 output_textbox->Focus();
			 output_textbox->AppendText("   ");
		 }
private: System::Void show_rx_data_checkbox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 enableOutputPrintingToolStripMenuItem->Checked = show_rx_data_checkbox->Checked;
			 output_textbox->Focus();
		 }
private: System::Void serialReaderThread_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {

			 array<wchar_t>^ readBuffer;
			 int bytesAvailable = 0;
			 int unprocessedBytesAvailable = 0;
			 int alreadyBufferredBytes;

			 //Allocate ourselves a nice 200kB buffer for storing serial port input data.
			 //Note: Make sure it is less than the shared between threads common buffer.
			 #define RX_LOCAL_BUFFER_SIZE	200000
			 readBuffer->Resize(readBuffer, RX_LOCAL_BUFFER_SIZE);

			 //Continuous loop that reads data from the serial port and copies it over to a common
			 //buffer that the main loop will access/use/consume the data from.
			 bytesAvailable = 0;
			 unprocessedBytesAvailable = 0;
			 while(serialReaderThread->CancellationPending == false)
			 {
				 //Only try to read from the COMx port if it is actually open.
				 if((COMPortOpen == true) && (serialPort1->IsOpen == true))
				 {
					 //Check how many bytes are sitting in the serial port input buffer.
					 try
					 {
						 bytesAvailable = serialPort1->BytesToRead;
						 if(bytesAvailable == 0)
						 {
							 //No data available to read.  Nothing to do, so just sleep the thread breifly
							 //so we don't waste 100% CPU.
							 Sleep(1);
						 }
					 }
					 catch(...)
					 {
						 //Wuh?  Should never get here unless maybe someone deleted the serial port object while we were using it?
						 bytesAvailable = 0;
						 Sleep(1);
					 }

					 //Try to read from the serial port class object (which contains internally bufferred RX data),
					 //but only if we finished processing/consuming the old data (by pushing it accross the thread boundary).
					 if((unprocessedBytesAvailable == 0) && (bytesAvailable != 0))
					 {
						 //Prepare to read the lesser of the local buffer size, or the amount of bytes in the serial port class object buffer.
						 if(bytesAvailable > RX_LOCAL_BUFFER_SIZE)
						 {
							 bytesAvailable = RX_LOCAL_BUFFER_SIZE;
						 }

						 //Try to do the read.
						 try
						 {
							 //Try to read the bufferred data from the serial port class object, and put it in our local buffer.
							 unprocessedBytesAvailable = serialPort1->Read(readBuffer, 0, bytesAvailable);
						 }
						 catch(...)
						 {
							 //Some kind of failure during the read?  This probably implies the COMx port has become broken (ex: the user unplugged the USB cable for instance).
							 unprocessedBytesAvailable = 0;
							 Sleep(10);
						 }
					 }
				 }
				 else
				 {
					 //The COMx port is not open.  Don't bother trying to read from it.  Just sleep the thread for awhile
					 //so we don't waste 100% CPU.
					 Sleep(3);
				 }


				 //Check if we have any previously read data sitting in the local buffer.  If so,
				 //commandeer command of the shared/common buffer with the main/GUI thread, and copy the
				 //data over, so it can use the data.
				 if(unprocessedBytesAvailable != 0)
				 {
					 //Try to take control of the shared buffer.
					 if(RXDataMutex->WaitOne(500) == true)
					 {
						 //We got control of the shared resources, make sure there is 
						 //enough room in the shared buffer to copy the data over.
						 alreadyBufferredBytes = sharedRXBytesAvailable; //Fetch a non-volatile local copy of the byte count, for faster local access.
						 if((alreadyBufferredBytes + unprocessedBytesAvailable) < SHARED_RX_BUFFER_SIZE)
						 {
							 //Copy each byte of newly received data into the shared buffer.
							 for(int i = 0; i < unprocessedBytesAvailable; i++)
							 {
								 sharedRXBuffer[alreadyBufferredBytes + i] = readBuffer[i];
							 }
							 //Increase the data tracking variable by the amount we just added to the buffer, so
							 //the GUI thread knows how much valid data is currently in the buffer.
							 sharedRXBytesAvailable += unprocessedBytesAvailable;
							 RXDataMutex->ReleaseMutex();
							 
							 //We consumed all of the locally available data.
							 unprocessedBytesAvailable = 0;
						 }
						 else
						 {
							 //There isn't enough room in the sharedRXBuffer[] array to store all of the new data.
							 //This shouldn't normally happen, but if it does, it implies that the new data is arriving
							 //faster than the main GUI thread can keep up (ex: because of insufficient CPU speed).
							 //In this case, do nothing for a little while (so the other thread can catch up), then
							 //resume normal processing (but don't read from the serial port class object again, until
							 //we have finished processing the prior data.
							 RXDataMutex->ReleaseMutex();
							 Sleep(1);
						 }
					 }//if(RXDataMutex->WaitOne(500) == true)
					 else
					 {
						 //Couldn't get exclusive access to shared buffer for some reason.  
						 //This implies some kind of code bug or severe CPU limitation if it occurs...  
						 //We can't really do anything in this case, but keep on waiting, until we do eventually get access.
						 Sleep(1);
					 }
				 }//if(unprocessedBytesAvailable != 0)

			 }//while(serialReaderThread->CancellationPending == false)
		 }
private: System::Void clearBuffer_btn_Click(System::Object^  sender, System::EventArgs^  e) {
			 output_textbox->Clear();
			 output_textbox->Focus();
		 }
private: System::Void usb_pid_textbox_Leave(System::Object^  sender, System::EventArgs^  e) {
			 SettingsChangeHandler();
		 }
private: System::Void usb_vid_textbox_Leave(System::Object^  sender, System::EventArgs^  e) {
			 SettingsChangeHandler();
		 }


private: System::Void baudrate_textbox_Leave(System::Object^  sender, System::EventArgs^  e) {
			 //This callback executes when the user clicks away focus from the baudrate textbox.
			 //If they are clicking away, this presumably means that they are finished editing the textbox,
			 //and we should now update the serial port baud rate to the new user setting.

			 //The baud rate also gets updated when the user presses "enter" on their keyboard, 
			 //in the baudrate_textbox_KeyPress() callback event handler.
			 SettingsChangeHandler();
		 }

private: System::Void baudrate_textbox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {

			 //This callback executes when the user presses a key in the baudrate textbox.
			 //We ignore all key presses except the enter key (indicating the user is finished editing the field and
			 //wants the change to take effect now).
			 wchar_t key = e->KeyChar;
			 if(key == VK_RETURN)
			 {
				 SettingsChangeHandler();
			 }
		 }


bool SaveApplicationSettingsToFile(String^ fileNameToSaveTo)
{
	//Save the USB VID/PID, baud rate, COMx port number, and other application settings to an .ini file,
	//so that they can be restored to the same settings the next time the user launches this application.

	String^ savedSettingsString = "";

	savedSettingsString += "VID       " + usb_vid_textbox->Text;
	savedSettingsString += "\r\nPID       " + usb_pid_textbox->Text;
	savedSettingsString += "\r\nBaudRate  " + baudrate_textbox->Text;
	if(connect_by_vid_pid_radio_button->Checked == true)
	{
		savedSettingsString += "\r\nConnectBy USBVIDPID";
	}
	else
	{
		savedSettingsString += "\r\nConnectBy Manual";
	}

	if(localEchoEnabledToolStripMenuItem->Checked == true)
	{
		savedSettingsString += "\r\nLocalEcho Enabled";
	}
	else
	{
		savedSettingsString += "\r\nLocalEcho Disabled";
	}

	//Convert the output_textbox font info to a string and also save that in the .ini file.
	System::ComponentModel::TypeConverter^ converter = System::ComponentModel::TypeDescriptor::GetConverter(System::Drawing::Font::typeid);
	String^ fontToSave = converter->ConvertToInvariantString(output_textbox->Font);
	savedSettingsString += "\r\nFont      " + fontToSave;	


	//See if any of the manual connection COMx port boxes are check in the list box.
	if(checkedListBox1->CheckedItems->Count == 1)
	{
		COMString = checkedListBox1->CheckedItems[0]->ToString();
		if(COMString->StartsWith("COM"))
		{
			savedSettingsString += "\r\nManualConnectionSelectedCOMPortString " + COMString;
		}
	}


	//Now save the string to a file (in the same folder as the .exe file for this program).
	try
	{
		String^ exeFilePath = GetThisProgramsExePath();

		fileNameToSaveTo = exeFilePath + "\\" + fileNameToSaveTo;

		File::WriteAllText(fileNameToSaveTo, savedSettingsString);
		return true;
	}
	catch(...)
	{
	}


	return false;
}


//This function copies the data stored in a .ini file (if present) and loads
//them up for use in this application.  This function is meant to be called once
//when opening the application.  To save the setting just upon closing the application,
//see the SaveApplicationSettingsToFile() function.
bool RestoreApplicationSettingsFromFile(String^ nameOfFileWithSavedSettings)
{
	String^ fullFillPath = GetThisProgramsExePath() + "\\" + nameOfFileWithSavedSettings;
	String^ fileContents;
	
	array<wchar_t>^ lineSeperator = {0x000D, 0x000A};
	String^ infoString;
	int i;


	try
	{
		//First make sure the file really exists.  It may not be present, especially
		//if this was the first time the application has ever been run.
		if(File::Exists(fullFillPath) == true)
		{
			//The file exists.  Read all of its contents to a string.
			fileContents = File::ReadAllText(fullFillPath);

			//Now split the single big string into an array of strings, one for each line in the .ini file.
			array<String^>^ fileLines = fileContents->Split(lineSeperator, System::StringSplitOptions::RemoveEmptyEntries);
			
			//Parse each line, to load each of the parameters into this app.
			for each(String^ lineText in fileLines)
			{
				if(lineText->StartsWith("VID"))
				{
					usb_vid_textbox->Text = lineText->Substring(10);
				}
				else if(lineText->StartsWith("PID"))
				{
					usb_pid_textbox->Text = lineText->Substring(10);
				}
				else if(lineText->StartsWith("BaudRate"))
				{
					baudrate_textbox->Text = lineText->Substring(10);
				}
				else if(lineText->StartsWith("ConnectBy"))
				{
					infoString = lineText->Substring(10);
					if(infoString == "USBVIDPID")
					{
						connect_by_vid_pid_radio_button->Checked = true;
					}
					else
					{
						//Must be manual connection via manual COMx selection by the user.
						radioButton2->Checked = true;
					}
				}
				else if(lineText->StartsWith("LocalEcho"))
				{
					infoString = lineText->Substring(10);
					if(infoString == "Enabled")
					{
						localEchoEnabledToolStripMenuItem->Checked = true;
					}
					else
					{
						localEchoEnabledToolStripMenuItem->Checked = false;
					}

				}
				else if(lineText->StartsWith("Font"))
				{
					infoString = lineText->Substring(10);
					try
					{
						System::ComponentModel::TypeConverter^ converter = System::ComponentModel::TypeDescriptor::GetConverter( System::Drawing::Font::typeid );
						Object^ savedFontObject = converter->ConvertFromString(infoString);
						System::Drawing::Font^ savedFont = dynamic_cast<System::Drawing::Font^>(savedFontObject);
						output_textbox->Font = savedFont;					
					}
					catch(...)
					{
						//The font string info in the file was somehow corrupted and couldn't be properly converted back into the Font^ type.
						//Don't load the result, just use the default font selection.
					}
				}
				else if(lineText->StartsWith("ManualConnectionSelectedCOMPortString"))
				{
					infoString = lineText->Substring(38);

					for(i = 0; i < checkedListBox1->Items->Count; i++)
					{
						if(checkedListBox1->Items[i]->ToString() == infoString)
						{
							checkedListBox1->SetItemChecked(i, true);
						}
					}

				}
			}

			return true;
		}//if(File::Exists(fullFillPath) == true)
		else
		{
			//The file could not be found.  Nothing to load.
			return false;
		}
	}
	catch(...)
	{
		return false;
	}

}



void ApplicationShutdownTasks(void)
{
	int i;

	//Save the user's application settings to a file (like VID/PID/baud, etc.), so it can be re-used upon the next launch.
	SaveApplicationSettingsToFile("UserSettings.ini");

	//Try to gracefully shut down the threads
	serialReaderThread->CancelAsync();
	serialWriterThread->CancelAsync();
	for(i = 0; i < 20; i++)
	{
		if((i >= 15) && (serialPort1->IsOpen == true))
		{
			serialPort1->Close();
		}
		if((serialReaderThread->IsBusy == true) || (serialWriterThread->IsBusy == true))
		{
			Sleep(2);
		}
		else
		{
			break;
		}
	}
}



//Returns a string of the path that this program .exe file resides in.  Note: The returned string does
//not end with a "\" character, ex: "C:\SomeFolder" (without the quotes).
String^ GetThisProgramsExePath(void)
{
	//Note: Max path in Windows is "about" 32767 characters.  "About" is approximate according 
	//to MSDN and actually can be a few characters longer due to prefix slashes and special symbols.
	//Additionally, for wide chars, this means you need a 66000 byte buffer to be safe.  
	TCHAR fileNamePath[33000];		//Note: TCHARs change size from 1 bytes to 2 bytes, depending on if UNICODE is defined.
	String^ pathToThisProgramDirectory;

	//Get the path string to this executable file location.
	GetModuleFileName(NULL, &fileNamePath[0], (sizeof(fileNamePath) / sizeof(TCHAR)));
	pathToThisProgramDirectory = gcnew String(&fileNamePath[0]);
	//Strip off the filename.exe off the end of the path
	//Note: GetDirectoryName() method also strips off the ending slash character.
	pathToThisProgramDirectory = Path::GetDirectoryName(pathToThisProgramDirectory);

	return pathToThisProgramDirectory;
}



private: System::Void serialWriterThread_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {

			 String^ fileToSend;
			 int copySize;
			 if(sendTXFileStringContents != "")
			 {
				 if((COMPortOpen == true) && (serialPort1->IsOpen == true))
				 {
					 //Make a local copy of the string
					 fileToSend = (String^)sendTXFileStringContents;
					 sendTXFileStringContents = "";

					 //Now start sending the data out the serial port.
					 try
					 {
						 if(fileToSend->Length < 64000)
						 {
							 serialPort1->Write(fileToSend);		
						 }
						 else
						 {
							 //Send in chunks, if the size is large.
							 while(fileToSend->Length != 0)
							 {
								 //Check for thread exit indication (because the application is trying to close)
								 if(serialWriterThread->CancellationPending == true)
								 {
									 //Abandon any long file send operation.  Use is aborting things.
									 break;
								 }

								 //Send the lesser of 64kB or the remaining unsent data, in a loop ieration.
								 copySize = 64000;
								 if(fileToSend->Length < copySize)
								 {
									 copySize = fileToSend->Length;
								 }
								 //Try to send the data out over the COMx port now.
								 serialPort1->Write(fileToSend->Substring(0, copySize));

								 //Just send some of the data, remove it from our remaining data set.
								 fileToSend = fileToSend->Remove(0, copySize);
							 }
						 }
					 }
					 catch(...)
					 {
						 //Wuh?  Some kind of error...
					 }
				 }
				 else
				 {
					 //Can't send the file contents, since the COMxx port isn't open.
					 //Just make the data vaporize.
					 sendTXFileStringContents = "";
				 }
			 }
		 }



private: System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 //Show the Help-->About dialog window.
			 AboutDialog^ aboutForm = gcnew AboutDialog();
			 aboutForm->Show();
		 }


private: System::Void fontToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 //Launch the font dialog window so the user can change the formatting in the main output textbox.
			 fontDialog1->ShowDialog();
			 output_textbox->Font = fontDialog1->Font;
		 }
};//public ref class Form1
}//Namespace USB_Serial_Terminal 

