#pragma once

#include "RAT-Dll-Main.h"

INT performDirList(__in char* dirPath, __inout char** dirFiles, __inout UINT32* numDirFiles);

INT sendDirFiles(__inout SOCKET clientSock, __in char** dirFiles, __in UINT32 numDirFiles);