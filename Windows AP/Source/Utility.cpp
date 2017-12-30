#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include "Utility.h"

#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace std;

HANDLE Utility::fileHandle = NULL;

Utility::Utility()
{

}

Utility::~Utility()
{

}

int Utility::init(int *type, int *nc, int *nr)
{
	//*type = COLOR_IMAGE;
	*type = GRAY_IMAGE;
	*nc = 640;
	*nr = 480;

	return TRUE;
}

int Utility::connectCamera()
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


	if (serialPorts == nullptr)
		return 0;

	//String^  port = "\\\\.\\" + serialPorts[0];
	//String^  port = serialPorts[0];

	//LPCSTR lpString = new TCHAR[port.size() + 1];
	//strcpy(lpString, port.c_str());
	//TCHAR*pcCommPort = TEXT(COM7);

	fileHandle = CreateFile(
		L"\\\\.\\COM7",									// address of name of the communications device
		GENERIC_READ | GENERIC_WRITE,			// access (read-write) mode
		0,										// share mode
		NULL,									// address of security descriptor
		OPEN_EXISTING,							// how to create
		0,	//FILE_FLAG_OVERLAPPED for asynchronous									// file attributes
		NULL									// handle of file with attributes to copy
	);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		//Console::WriteLine("Fail to open "+ lpString);
		Console::WriteLine("Fail to open serial port");

		return 0;
	}
	else
	{
		COMMTIMEOUTS CommTimeouts;
		DCB          comSettings;

		Console::WriteLine("Success to open serial port");

#if 1
		// Set timeouts in milliseconds
		CommTimeouts.ReadIntervalTimeout = 0;
		CommTimeouts.ReadTotalTimeoutMultiplier = 0;
		CommTimeouts.ReadTotalTimeoutConstant = 100;
		CommTimeouts.WriteTotalTimeoutMultiplier = 0;
		CommTimeouts.WriteTotalTimeoutConstant = 100;

		Console::WriteLine("SetCommTimeouts start");

		if (SetCommTimeouts(fileHandle, &CommTimeouts) == 0)
		{
			Console::WriteLine("Fail to SetCommTimeouts");

			CloseHandle(fileHandle);

			return 0;
		}

		Console::WriteLine("SetCommTimeouts finish");

		Console::WriteLine("GetCommState start");

		// Set Port parameters.
		if (GetCommState(fileHandle, &comSettings) == 0)
		{
			Console::WriteLine("Fail to GetCommState");

			CloseHandle(fileHandle);

			return 0;
		}

		Console::WriteLine("GetCommState finish");
#endif

		/*
		Console::WriteLine("comSettings.BaudRate ="+comSettings.BaudRate);
		Console::WriteLine("comSettings.StopBits ="+comSettings.StopBits);
		Console::WriteLine("comSettings.ByteSize ="+comSettings.ByteSize);
		Console::WriteLine("comSettings.Parity ="+comSettings.Parity);
		Console::WriteLine("comSettings.fParity ="+comSettings.fParity);
		*/
		comSettings.BaudRate = 256000; //460800;
		comSettings.StopBits = ONE5STOPBITS;
		comSettings.ByteSize = 8;
		comSettings.Parity = NOPARITY;
		comSettings.fParity = FALSE;

		Console::WriteLine("SetCommState start");

		if (SetCommState(fileHandle, &comSettings) == 0)
		{
			Console::WriteLine("Fail to SetCommState");

			CloseHandle(fileHandle);

			return 0;
		}

		Console::WriteLine("SetCommState finish");

		return 1;
	}
}

void Utility::disconnectCamera()
{
	CloseHandle(fileHandle);
	fileHandle = NULL;
}

//check whether image is ready to read
//Return 0:Not ready; 1:Ready
int Utility::checkImageFlag()
{
	return 1;
}

int Utility::startCapture()
{
	return 0;
}

//captureImage
int Utility::captureImage(unsigned char** pData, int type, int nc, int nr)
{
	char data = 0xA5;
	DWORD	bytes_written = 0;
	DWORD	bytesRead = 0;
	DWORD	imageSize = type * nr * nc / 8;
	DWORD	bytesReadTotal = 0;
	unsigned char*	pBufIndex;
	const int perData = 16380;

	if (WriteFile(fileHandle, &data, 1, &bytes_written, NULL) == FALSE)
	{
		// error processing code here
		printf("Fail to write(0x%x)", data);

		return FALSE;
	}

	if (*pData == NULL)
		*pData = pBufIndex = (unsigned char*)calloc(imageSize, sizeof(unsigned char));

	if (*pData == NULL)
	{
		printf("No enough memory space to allocation.\n");

		return FALSE;
	}

	printf("Start to read %d bytes\n", imageSize);

	while (bytesReadTotal < imageSize)
	{
		if (ReadFile(fileHandle, *pData + bytesReadTotal, perData, &bytesRead, NULL) != FALSE)
		{
			if (bytesRead > 0)
			{
				bytesReadTotal += bytesRead;

				//printf("Read %d bytes, total:%d\n", bytesRead, bytesReadTotal);
				/*
				int i;
				for (i = 0; i < bytesRead; i++)
				printf("[%d]= 0x%x\n", i, *(*pData+i));
				*/
			}
		}
	}

	printf("End to read %d bytes\n", bytesReadTotal);

	return TRUE;
}

int Utility::sendRoi(int x, int y, int w, int h)
{
	char data[32];
	DWORD bytesToWrite = 17;
	DWORD	bytesWritten = 0;
	int* intPoint;

	data[0] = 0xA6;
	intPoint = (int*)&data[1];
	*intPoint = x;

	intPoint++;
	*intPoint = y;

	intPoint++;
	*intPoint = w;

	intPoint++;
	*intPoint = h;

	if (WriteFile(fileHandle, &data, bytesToWrite, &bytesWritten, NULL) == FALSE)
	{
		// error processing code here
		printf("Fail to write(0x%x)", data[0]);

		return FALSE;
	}

	return TRUE;
}

int Utility::getRoiImage(unsigned char* pData, int type, int nc, int nr)
{
	DWORD	bytesRead = 0;
	DWORD	imageSize = type * nr * nc / 8;
	DWORD	bytesReadTotal = 0;
	const int perData = 16380;

	printf("[getRoiImage]nc:%d,nr:%d\n", nc, nr);

	if (pData == NULL)
	{
		printf("No enough memory space to allocation.\n");

		return FALSE;
	}

	printf("Start to read %d bytes\n", imageSize);

	while (bytesReadTotal < imageSize)
	{
		if (ReadFile(fileHandle, pData + bytesReadTotal, nc, &bytesRead, NULL) != FALSE)
		{
			if (bytesRead > 0)
			{
				bytesReadTotal += bytesRead;

				//printf("Read %d bytes, total:%d\n", bytesRead, bytesReadTotal);
			}
		}
	}

	printf("End to read %d bytes\n", bytesReadTotal);

	return TRUE;
}