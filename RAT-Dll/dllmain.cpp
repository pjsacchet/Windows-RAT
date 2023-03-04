// Patrick Sacchet
// Defines the entry point for the DLL application; main handler for user specific calls

#include "pch.h"
#include "debug.h"
#include "dllmain.h"

using namespace InternalDebug;

BOOL startListen(DebugOutput* debug)
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
        debug->writeFile("ERROR: Wsastartup did not complete successfully \n", ios::app);
        string errorCode = "Error code: %u \n", status;
        debug->writeFile(errorCode, ios::app);
        return FALSE;
    }
    debug->writeFile("WSAStartup was successful \n", ios::app);
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
        debug->writeFile("ERROR: Could not successfully create socket \n", ios::app);
        return FALSE;
    }

    if (bind(serverSock, (LPSOCKADDR)&client, sizeof(client)) == SOCKET_ERROR)
    {
        debug->writeFile("ERROR: Could not bind socket \n", ios::app);
        return FALSE;
    }
    printf("Listening on port: %d \n", PORT_NUM);

    status = listen(serverSock, SOMAXCONN);
    if (status == SOCKET_ERROR)
    {
        printf("Socket error; closing socket... \n");
        closesocket(serverSock);
        WSACleanup();
        return FALSE;
    }

    clientSock = accept(serverSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET)
    {
        printf("Invalid socket from client \n");
        closesocket(serverSock);
        WSACleanup();
        return FALSE;
     }

    // Close server socket since we no longer need it 
    closesocket(serverSock);

    // Receive from client until the peer shuts down the connection
    do
    {
        printf("Waiting for more bytes... \n");
        status = recv(clientSock, recvBuf, recvBufLen, 0);
        if (status > 0)
        {
            printf( "Bytes received: %d \n", status);

            sendStatus = send(clientSock, recvBuf, status, 0);
            if (sendStatus == SOCKET_ERROR)
            {
                printf( "Send error: %d \n", sendStatus);
                closesocket(clientSock);
                WSACleanup();
                return FALSE;
            }
            printf("Bytes sent: %d \n", sendStatus);
        }
        else if (status == 0)
        {
            printf("Closing socket... \n");
        }
        else
        {
            printf("Recv failed with error %d \n", WSAGetLastError());
            closesocket(clientSock);
            WSACleanup();
            return FALSE;
        }

    } while (status > 0);

    // shutdown connection since we're done
    status = shutdown(clientSock, SD_SEND);
    if (status == SOCKET_ERROR)
    {
        printf("Shutdown failed with error: %d \n", WSAGetLastError());
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
    BOOL status;
    DebugOutput* debug;

    debug = debug->createFile();

    switch (ul_reason_for_call)
    {
        // Executable has surveyed environment and deemed it safe to operate; make calls specific to functionality here
        case DLL_PROCESS_ATTACH:
        {
            printf("Inside DLLMain... \n");
            // Pass control to our listening function and wait for user input 
            status = startListen(debug);
            printf("Finished actions in process attach, exiting... \n");
            break;
        }
        case DLL_THREAD_ATTACH:
        {
            // Pass control to our listening function and wait for user input 
            status = startListen(debug);
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

