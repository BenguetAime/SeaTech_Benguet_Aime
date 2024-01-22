#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace USB_Serial_Terminal {

	/// <summary>
	/// Summary for AboutDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class AboutDialog : public System::Windows::Forms::Form
	{
	public:
		AboutDialog(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AboutDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  ok_btn;
	private: System::Windows::Forms::TextBox^  aboutText_txtbx;
	protected: 


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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(AboutDialog::typeid));
			this->ok_btn = (gcnew System::Windows::Forms::Button());
			this->aboutText_txtbx = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// ok_btn
			// 
			this->ok_btn->Location = System::Drawing::Point(205, 324);
			this->ok_btn->Name = L"ok_btn";
			this->ok_btn->Size = System::Drawing::Size(75, 23);
			this->ok_btn->TabIndex = 0;
			this->ok_btn->Text = L"OK";
			this->ok_btn->UseVisualStyleBackColor = true;
			this->ok_btn->Click += gcnew System::EventHandler(this, &AboutDialog::ok_btn_Click);
			// 
			// aboutText_txtbx
			// 
			this->aboutText_txtbx->BackColor = System::Drawing::SystemColors::Window;
			this->aboutText_txtbx->Location = System::Drawing::Point(13, 13);
			this->aboutText_txtbx->Multiline = true;
			this->aboutText_txtbx->Name = L"aboutText_txtbx";
			this->aboutText_txtbx->ReadOnly = true;
			this->aboutText_txtbx->Size = System::Drawing::Size(458, 305);
			this->aboutText_txtbx->TabIndex = 1;
			this->aboutText_txtbx->Text = resources->GetString(L"aboutText_txtbx.Text");
			// 
			// AboutDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(483, 357);
			this->Controls->Add(this->aboutText_txtbx);
			this->Controls->Add(this->ok_btn);
			this->Name = L"AboutDialog";
			this->Text = L"About USB Serial Terminal";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void ok_btn_Click(System::Object^  sender, System::EventArgs^  e) {
				 AboutDialog::Close();
			 }
	};
}
