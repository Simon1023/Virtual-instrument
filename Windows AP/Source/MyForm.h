#pragma once

namespace VirtualInstrument {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// MyForm 的摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm();

	protected:
		~MyForm();

	private:
		System::ComponentModel::Container ^components;
		System::Windows::Forms::Button^  connect;
		System::Windows::Forms::Button^  capture;
		System::Windows::Forms::Button^  ok;
		System::Windows::Forms::Button^  roiDigit;
		System::Windows::Forms::Button^  roiWave;
		System::Windows::Forms::Button^  video;
		System::Windows::Forms::Panel^  panel1;
		System::Windows::Forms::PictureBox^  pictureBox1;
		System::Windows::Forms::Label^  out_message;
		Bitmap^ resultImage;
		//宣告圖像區域(設定為欲讀取、欲修正的圖像區域)
		System::Drawing::Rectangle rect;
		//宣告為一BitmapData型態，表示像素值位置
		Imaging::BitmapData ^resultImageData;
		//宣告一int格式的指標
		IntPtr resultPtr;
		//宣告一個Byte格式的指標
		Byte *pout;
		Thread^ oThread;
		int roiX;
		int roiY;
		int roiW;
		int roiH;
		bool isDrag;
		System::Drawing::Rectangle theRectangle;
		Point startPoint;
		MouseEventArgs^ mouseEventDown;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		void MyForm::InitializeComponent(void)
		{
			this->connect = (gcnew System::Windows::Forms::Button());
			this->capture = (gcnew System::Windows::Forms::Button());
			this->video = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->out_message = (gcnew System::Windows::Forms::Label());
			this->ok = (gcnew System::Windows::Forms::Button());
			this->roiDigit = (gcnew System::Windows::Forms::Button());
			this->roiWave = (gcnew System::Windows::Forms::Button());
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// connect
			// 
			this->connect->Location = System::Drawing::Point(13, 522);
			this->connect->Name = L"connect";
			this->connect->Size = System::Drawing::Size(75, 23);
			this->connect->TabIndex = 0;
			this->connect->Text = L"connect";
			this->connect->UseVisualStyleBackColor = true;
			this->connect->Click += gcnew System::EventHandler(this, &MyForm::connect_Click);
			// 
			// capture
			// 
			this->capture->Location = System::Drawing::Point(107, 522);
			this->capture->Name = L"capture";
			this->capture->Size = System::Drawing::Size(75, 23);
			this->capture->TabIndex = 1;
			this->capture->Text = L"capture";
			this->capture->UseVisualStyleBackColor = true;
			this->capture->Click += gcnew System::EventHandler(this, &MyForm::capture_Click);
			// 
			// video
			// 
			this->video->Location = System::Drawing::Point(582, 522);
			this->video->Name = L"video";
			this->video->Size = System::Drawing::Size(75, 23);
			this->video->TabIndex = 2;
			this->video->Text = L"video";
			this->video->UseVisualStyleBackColor = true;
			this->video->Click += gcnew System::EventHandler(this, &MyForm::video_Click);
			// 
			// panel1
			// 
			this->panel1->AutoScroll = true;
			this->panel1->Controls->Add(this->pictureBox1);
			this->panel1->Location = System::Drawing::Point(13, 13);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(680, 503);
			this->panel1->TabIndex = 3;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::panel1_Paint);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(4, 4);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->SizeMode = PictureBoxSizeMode::AutoSize;
			this->pictureBox1->Size = System::Drawing::Size(640, 480);
			//this->pictureBox1->TabIndex = 0;
			//this->pictureBox1->TabStop = false;
			//this->pictureBox1->Click += gcnew System::EventHandler(this, &MyForm::pictureBox1_Click);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseDown);
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseMove);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseUp);
			// 
			// out_message
			// 
			this->out_message->AutoSize = true;
			this->out_message->Location = System::Drawing::Point(10, 560);
			this->out_message->Name = L"out_message";
			this->out_message->Size = System::Drawing::Size(85, 12);
			this->out_message->TabIndex = 4;
			this->out_message->Text = L"output message...";
			// 
			// ok
			// 
			this->ok->Location = System::Drawing::Point(379, 522);
			this->ok->Name = L"ok";
			this->ok->Size = System::Drawing::Size(75, 23);
			this->ok->TabIndex = 5;
			this->ok->Text = L"OK";
			this->ok->UseVisualStyleBackColor = true;
			this->ok->Click += gcnew System::EventHandler(this, &MyForm::ok_Click);
			// 
			// roiDigit
			// 
			this->roiDigit->Location = System::Drawing::Point(199, 522);
			this->roiDigit->Name = L"roiDigit";
			this->roiDigit->Size = System::Drawing::Size(75, 23);
			this->roiDigit->TabIndex = 6;
			this->roiDigit->Text = L"ROI(digit)";
			this->roiDigit->UseVisualStyleBackColor = true;
			this->roiDigit->Click += gcnew System::EventHandler(this, &MyForm::roiDigit_Click);
			// 
			// roiWave
			// 
			this->roiWave->Location = System::Drawing::Point(289, 522);
			this->roiWave->Name = L"roiWave";
			this->roiWave->Size = System::Drawing::Size(75, 23);
			this->roiWave->TabIndex = 7;
			this->roiWave->Text = L"ROI(wave)";
			this->roiWave->UseVisualStyleBackColor = true;
			this->roiWave->Click += gcnew System::EventHandler(this, &MyForm::roiWave_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(722, 581);
			this->Controls->Add(this->out_message);
			this->Controls->Add(this->panel1);
			//this->Controls->Add(this->video);
			this->Controls->Add(this->capture);
			this->Controls->Add(this->connect);
			this->Controls->Add(this->ok);
			this->Controls->Add(this->roiDigit);
			this->Controls->Add(this->roiWave);
			this->Name = L"MyForm";
			this->Text = L"Virtual instrument";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->panel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e);
		System::Void connect_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void capture_Click(System::Object^  sender, System::EventArgs^  e);
		void ThreadMethod(/*Object^ state*/);
		System::Void video_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void ok_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void roiDigit_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void roiWave_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
		void pictureBox1_MouseDown(Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		void pictureBox1_MouseMove(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e);
		void pictureBox1_MouseUp(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e);
};
}
