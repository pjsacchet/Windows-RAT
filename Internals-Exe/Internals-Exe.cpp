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
    HINSTANCE hGetProcIDLL = LoadLibrary(L"C:\\WInternals-DLL.dll");

    if (!hGetProcIDLL)
    {
        std::cout << "Could not successfully load target DLL \n";
        return -1;
    }

    std::cout << " Successfully loaded target DLL!\n";

    // While our DLL handles commands from client continue to survey the system, calling DLL_PROCESS_DETACH once we suspect a threat
    // TODO: Implement further antivirus monitoring 

    return 1;
}


