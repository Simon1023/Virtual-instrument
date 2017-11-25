#pragma once

#include <windows.h>

#define packed __attribute__((packed))
#define TRUE 1
#define FALSE 0

//image type
#define BINARY_IMAGE 8
#define GRAY_IMAGE 8
#define COLOR_IMAGE 24

class Utility
{
public:
	Utility();
	~Utility();

	static int init(int *type, int *nc, int *nr);
	static int connectCamera();
	static void disconnectCamera();
	static int checkImageFlag();
	static int startCapture();
	static int captureImage(unsigned char** pData, int type, int nc, int nr);
	static int sendRoi(int x, int y, int w, int h);

private:
	static HANDLE fileHandle;
};

