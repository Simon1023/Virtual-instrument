#include <stdio.h>
#include <time.h>
#include "MyForm.h"
#include "Utility.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;
using namespace VirtualInstrument;

MyForm::MyForm()
{
	InitializeComponent();
	roiX = roiY = roiW = roiH = 0;
}

MyForm::~MyForm()
{
	if (components)
	{
		delete components;
	}
}

Void MyForm::MyForm_Load(System::Object^  sender, System::EventArgs^  e)
{
	capture->Enabled = false;
	video->Enabled = false;
	roiDigit->Enabled = false;
	roiWave->Enabled = false;
	ok->Enabled = false;
}

Void MyForm::connect_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (Utility::connectCamera()) {
		capture->Enabled = true;
		connect->Enabled = false;
		//video->Enabled = true;
		out_message->Text = "get device\n";
	}
	else {
		//capture->Enabled = false;
		//video->Enabled = false;
		out_message->Text = "fail to get device\n";
	}

	//System::Drawing::Rectangle theRectangle = System::Drawing::Rectangle::Rectangle(50, 50, 200, 200);
	//ControlPaint::DrawReversibleFrame(theRectangle, Color::FromName("Red"), FrameStyle::Thick);
}

Void MyForm::capture_Click(System::Object^  sender, System::EventArgs^  e) 
{

	unsigned char *pData = NULL, *pin;
	int type, nc, nr;
	clock_t start, end;

	Utility::init(&type, &nc, &nr);
	resultImage = gcnew Bitmap(nc, nr, Imaging::PixelFormat::Format24bppRgb);
	rect = System::Drawing::Rectangle(0, 0, nc, nr);

	start = clock();

	Utility::startCapture();
	Utility::captureImage(&pData, type, nc, nr);

	end = clock();
	printf("The time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

	//Lock欲處理的像素範圍(避免其他程序修改該向素值)，參數一為限定像素處理範圍，參數二為設定處理模式(讀取、寫入、讀取寫入，第三個像素為設定該像素模式bit? channel?)
	resultImageData = resultImage->LockBits(rect, Imaging::ImageLockMode::ReadWrite,Imaging::PixelFormat::Format24bppRgb);

	//將int指標指向Image像素資料的最前面位置
	resultPtr = resultImageData->Scan0;

	//設定指標
	pin = pData;
	pout = (Byte*)((Void*)resultPtr);

	//巡迴每一個像素
	for (int y = 0; y < nr; y++) {
		for (int x = 0; x < nc; x++) {
			//像素值填入
			if (type == GRAY_IMAGE) {
				pout[0] = pout[1] = pout[2] = (Byte)*pin;	//填入像素值 channel 0 (Blue), 填入像素值 channel 1 (Green), 填入像素值 channel 2 (Red)
				pin++;
			}
			else if (type == COLOR_IMAGE) {
				pout[0] = (Byte)*pin;
				pout[1] = (Byte)*(pin + 1);
				pout[2] = (Byte)*(pin + 2);
				pin += 3;
			}

			//指到下一個像素資訊
			pout += 3;
		}
	}

	//Unlock處理完畢的像素範圍
	resultImage->UnlockBits(resultImageData);

	//將影像顯示在pictureBox
	pictureBox1->Image = resultImage;

	roiDigit->Enabled = true;
	roiWave->Enabled = true;
}

void MyForm::ThreadMethod(/*Object^ state*/)
{

	unsigned char *pData = NULL, *pin;
	int type, nc, nr;
	clock_t start, end;

	Utility::init(&type, &nc, &nr);
	resultImage = gcnew Bitmap(nc, nr, Imaging::PixelFormat::Format24bppRgb);
	rect = System::Drawing::Rectangle(0, 0, nc, nr);

	while (1) {
		start = clock();
		Utility::startCapture();
		Utility::captureImage(&pData, type, nc, nr);
		end = clock();
		printf("The time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

		//Lock欲處理的像素範圍(避免其他程序修改該向素值)，參數一為限定像素處理範圍，參數二為設定處理模式(讀取、寫入、讀取寫入，第三個像素為設定該像素模式bit? channel?)
		resultImageData = resultImage->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

		//將int指標指向Image像素資料的最前面位置
		resultPtr = resultImageData->Scan0;

		//設定指標
		pin = pData;
		pout = (Byte*)((Void*)resultPtr);

		//巡迴每一個像素
		for (int y = 0; y < nr; y++) {
			for (int x = 0; x < nc; x++) {
				//像素值填入
				if (type == GRAY_IMAGE) {
					pout[0] = pout[1] = pout[2] = (Byte)*pin;	//填入像素值 channel 0 (Blue), 填入像素值 channel 1 (Green), 填入像素值 channel 2 (Red)
					pin++;
				}
				else if (type == COLOR_IMAGE) {
					pout[0] = (Byte)*pin;
					pout[1] = (Byte)*(pin + 1);
					pout[2] = (Byte)*(pin + 2);
					pin += 3;
				}

				//指到下一個像素資訊
				pout += 3;
			}
		}

		//Unlock處理完畢的像素範圍
		resultImage->UnlockBits(resultImageData);

		//將影像顯示在pictureBox
		pictureBox1->Image = resultImage;
	}
}

Void MyForm::video_Click(System::Object^  sender, System::EventArgs^  e) {
	//disable other button
	if (video->Text == "video") {
		capture->Enabled = false;
		video->Text = "stop video thread";
		oThread = gcnew Thread(gcnew ThreadStart(this, &MyForm::ThreadMethod));
		oThread->Start();
	}
	else {
		capture->Enabled = true;
		video->Text = "video";
		oThread->Abort();
	}
}

Void MyForm::ok_Click(System::Object^  sender, System::EventArgs^  e) 
{
	int type = GRAY_IMAGE;
	unsigned char *pData = NULL, *pin = NULL;

	roiDigit->Enabled = false;
	roiWave->Enabled = false;

	resultImage = gcnew Bitmap(roiW, roiH, Imaging::PixelFormat::Format24bppRgb);
	rect = System::Drawing::Rectangle(0, 0, roiW, roiH);

	pData  = (unsigned char*)calloc(roiW*roiH*type/8, sizeof(unsigned char));

	Utility::sendRoi(roiX, roiY, roiW, roiH);

	Utility::getRoiImage(pData, type, roiW, roiH);

	//Lock欲處理的像素範圍(避免其他程序修改該向素值)，參數一為限定像素處理範圍，參數二為設定處理模式(讀取、寫入、讀取寫入，第三個像素為設定該像素模式bit? channel?)
	resultImageData = resultImage->LockBits(rect, Imaging::ImageLockMode::ReadWrite, Imaging::PixelFormat::Format24bppRgb);

	//將int指標指向Image像素資料的最前面位置
	resultPtr = resultImageData->Scan0;

	//設定指標
	pin = pData;
	pout = (Byte*)((Void*)resultPtr);

	//巡迴每一個像素
	for (int y = 0; y < roiH; y++) {
		for (int x = 0; x < roiW; x++) {
			//像素值填入
			if (type == GRAY_IMAGE) {
				pout[0] = pout[1] = pout[2] = (Byte)*pin;	//填入像素值 channel 0 (Blue), 填入像素值 channel 1 (Green), 填入像素值 channel 2 (Red)
				pin++;
			}
			else if (type == COLOR_IMAGE) {
				pout[0] = (Byte)*pin;
				pout[1] = (Byte)*(pin + 1);
				pout[2] = (Byte)*(pin + 2);
				pin += 3;
			}

			//指到下一個像素資訊
			pout += 3;
		}
	}

	//Unlock處理完畢的像素範圍
	resultImage->UnlockBits(resultImageData);

	//將影像顯示在pictureBox
	pictureBox1->Image = resultImage;

	free(pData);
}

Void MyForm::roiDigit_Click(System::Object^  sender, System::EventArgs^  e) 
{
	roiWave->Enabled = false;
}

System::Void MyForm::roiWave_Click(System::Object^  sender, System::EventArgs^  e) 
{
	roiDigit->Enabled = false;
}

System::Void MyForm::pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) 
{

}

Void MyForm::panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
{

}

void MyForm::pictureBox1_MouseDown(Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{

	if (roiDigit->Enabled != true && roiWave->Enabled != true)
		return;

	printf("MouseDown\n");

	// Set the isDrag variable to true and get the starting point 
	// by using the PointToScreen method to convert form 
	// coordinates to screen coordinates.
	if (e->Button == System::Windows::Forms::MouseButtons::Left)
	{

		printf("Reset Rectangle\n");

		// Reset the rectangle.
		//delete(theRectangle);
		theRectangle = System::Drawing::Rectangle::Rectangle(0, 0, 0, 0);


		isDrag = true;

		Control^ control = dynamic_cast<Control^>(sender);

		// Calculate the startPoint by using the PointToScreen 
		// method.
		startPoint = control->PointToScreen(Point(e->X, e->Y));
		mouseEventDown = e;
	}
}

void MyForm::pictureBox1_MouseMove(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e)
{
	//printf(" Form1_MouseMove\n");

	// If the mouse is being dragged, 
	// undraw and redraw the rectangle as the mouse moves.
	if (isDrag)
	{
		ControlPaint::DrawReversibleFrame(theRectangle, Color::Yellow, FrameStyle::Thick);

		// Calculate the endpoint and dimensions for the new 
		// rectangle, again using the PointToScreen method.
		Point endPoint = this->PointToScreen(Point(e->X, e->Y));
		int width = endPoint.X - startPoint.X;
		int height = endPoint.Y - startPoint.Y;
		theRectangle = Rectangle::Rectangle(startPoint.X, startPoint.Y, width, height);

		// Draw the new rectangle by calling DrawReversibleFrame
		// again.  
		ControlPaint::DrawReversibleFrame(theRectangle, Color::Yellow, FrameStyle::Thick);
	}
}

void MyForm::pictureBox1_MouseUp(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e)
{
	// If the MouseUp event occurs, the user is not dragging.
	if (e->Button == System::Windows::Forms::MouseButtons::Left && isDrag == true)
	{
		printf("MouseUp\n");

		isDrag = false;

		ok->Enabled = true;

		if (mouseEventDown->X < e->X)
		{
			roiX = mouseEventDown->X;
			roiW = e->X - roiX;
		}
		else
		{
			roiX = e->X;
			roiW = mouseEventDown->X - roiX;
		}

		//20171230 Simon: A workaround for a strange issue.It makes the image abornormal.
		if (roiW % 4 != 0)
			roiW = (roiW / 4 + 1) * 4;

		if (mouseEventDown->Y < e->Y)
		{
			roiY = mouseEventDown->Y;
			roiH = e->Y - roiY;
		}
		else
		{
			roiY = e->Y;
			roiH = mouseEventDown->Y - roiY;
		}

		//Point startPoint = theRectangle.Location;

		char str[128];

		sprintf(str, "roiX=%d , roiY=%d , roiW=%d , roiH=%d\n", roiX, roiY, roiW, roiH);
		out_message->Text = gcnew String(str);

	}
	// Draw the rectangle to be evaluated. Set a dashed frame style 
	// using the FrameStyle enumeration.
	//ControlPaint::DrawReversibleFrame(theRectangle, this->BackColor, FrameStyle::Dashed);

	// Find out which controls intersect the rectangle and 
	// change their color. The method uses the RectangleToScreen  
	// method to convert the Control's client coordinates 
	// to screen coordinates.
	/*
	System::Drawing::Rectangle controlRectangle;
	for (int i = 0; i < Controls->Count; i++)
	{
	controlRectangle = Controls[i]->RectangleToScreen(Controls[i]->ClientRectangle);
	if (controlRectangle.IntersectsWith(theRectangle))
	{
	Controls[i]->BackColor = Color::BurlyWood;
	}

	}
	*/

	// Reset the rectangle.
	//theRectangle = System::Drawing::Rectangle::Rectangle(0, 0, 0, 0);
	//delete(theRectangle);
}
/*
#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;


void getSerialPorts()
{
	array<String^>^ serialPorts = nullptr;
	try
	{
		// Get a list of serial port names.
		serialPorts = SerialPort::GetPortNames();
	}
	catch (Win32Exception^ ex)
	{
		Console::WriteLine(ex->Message);
	}

	Console::WriteLine("The following serial ports were found:");

	// Display each port name to the console.
	for each(String^ port in serialPorts)
	{
		Console::WriteLine(port);
	}
}
*/

[STAThread]
int main(){
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	VirtualInstrument::MyForm form;
	Application::Run(%form);
	return 0;
}