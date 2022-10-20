#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define DLL_EXPORT extern "C" __declspec(dllexport)

// Windows Header Files
#include <windows.h>
//#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


#include <iostream>
#include <fstream>

#pragma comment(lib,"WS2_32")


// User defined header files

