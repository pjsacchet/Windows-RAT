// Patrick Sacchet
// Defines the entry point for the DLL application; main handler for user specific calls
#include "pch.h"
#include "debug.h"

BOOL startListen(fstream &debugFile)
{
    WORD wsaVersion;
    WSADATA wsaData;
    int status = 0;

    wsaVersion = MAKEWORD(2, 2);

    status = WSAStartup(wsaVersion, &wsaData);
    if (!status)
    {
        InternalDebug::DebugOutput::writeFile(debugFile, "ERROR: Wsastartup did not complete successfully \n");
        InternalDebug::DebugOutput::writeFile(debugFile, "Error code: %u \n");
        return FALSE;
    }



    return TRUE;
}



BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        BOOL status;
        // Plan to call DLL for loading via rundll32 for now; make calls specific to functionality here
        case DLL_PROCESS_ATTACH:
        {
            // Pass control to our listening function and wait for user input 
            fstream debugFile = InternalDebug::DebugOutput::createDebugFile();
            status = InternalDebug::DebugOutput::writeFile(debugFile, "test \n");
            if (!status)
            {
                break;
            }
            status = startListen(debugFile);
            break;
        }
        case DLL_THREAD_ATTACH:
        {
            break;
        }
        case DLL_THREAD_DETACH:
        {
            break;
        }
        // General cleanup before detaching and de-loading out of memory
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    }

    return TRUE;
}

