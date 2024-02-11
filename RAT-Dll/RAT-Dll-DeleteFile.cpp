// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-DeleteFile.cpp : Defines functionality for performing a delete file on target, i.e., deleting a file off target at a specified path

#include "RAT-Dll-DeleteFile.h"
#include "RAT-Dll-Connect.h"


INT performDeleteFile(__in char* filePath)
{
	INT status = SUCCESS;
	CHAR msgBuf[DEFAULT_BUF_LEN];

	sprintf_s(msgBuf, "RAT-Dll-DeleteFile::performDeleteFile - Deleting %s...\n", filePath);
	OutputDebugStringA(msgBuf);

	if (!DeleteFile((LPCWSTR)filePath))
	{
		OutputDebugStringA("RAT-Dll-DeleteFile::performDeleteFile - Failed to delete file!\n");
		goto cleanup;
	}

	OutputDebugStringA("RAT-Dll-DeleteFile::performDeleteFile - Successfully deleted file!\n");

cleanup:
	return status;
}