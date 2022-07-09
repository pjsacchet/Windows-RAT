// Patrick Sacchet
// Version 1.0
// Internals-Exe.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <windows.h>
#include <iostream>


int main()
{
    std::cout << "Initializing program... \n";

    // First we survey the system, paying particualr attention to Windows Defender specifics 
    // TODO: Windows Defender call

    // If we are in a 'operational' space load our DLL into memory so it can call back to our client 
    // TODO: Dll load call for Internals-DLL
    LPCWSTR dllName = L"Internals-DLL.dll";
    HMODULE hGetProcIDLL = LoadLibrary(dllName);

    if (!hGetProcIDLL)
    {
        std::cout << "Could not successfully load target DLL \n";
        DWORD lastError = GetLastError();
        
        printf("Last error: %08x) \n", lastError);
        return -1;
    }

    std::cout << " Successfully loaded target DLL!\n";

    // Grab proc address to call into 
    typedef UINT (WINAPI* DLLFUNCPOINT) (HMODULE,DWORD,LPVOID);
    DLLFUNCPOINT DllFuncPoint;
    DllFuncPoint = (DLLFUNCPOINT)GetProcAddress(hGetProcIDLL, "DllMain");

    if (!DllFuncPoint)
    {
        FreeLibrary(hGetProcIDLL);
        return false;
    }
    else
    {
        UINT returnValue = 0;
        returnValue = DllFuncPoint(hGetProcIDLL, DLL_PROCESS_ATTACH, NULL);

        if (!returnValue)
        {
            std::cout << " Call to DLL main failed\n";
        }
        else
        {
            std::cout << " Call to DLL main succeeded!\n";
            FreeLibrary(hGetProcIDLL);
            return true;
        }
    }

    // While our DLL handles commands from client continue to survey the system, calling DLL_PROCESS_DETACH once we suspect a threat
    // TODO: Implement further antivirus monitoring 

    return true;
}


