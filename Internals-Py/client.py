# Patrick Sacchet
# Version 1.0
# Serves as the main CMD client interface which will provide tasking to our RAT on target

EXIT = 0
PORT_NUM = 1550

def testSend():
    return 1


def openSock():
    return 1

def printHelp():
    print("""Please select from the following options: \n
            1) Write a file to a location on target \n
            2) Get a file from a specific location on target\n
            0) Exit""")
    return 1

def main():
    print ("Attmepting to open socket on client... ")
    if (not openSock()):
        print ("Failed to open socket; exiting....")
        return -1
    print ("Attempting to reach out to implant....")
    if (not testSend()):
        print ("Failed to get response from implant; exiting...")
    # Now wait for user input and make appropiate call 
    userInput = 1
    while (int(userInput) != EXIT):
        printHelp()
        userInput = input("> ")
    return 1


if __name__ == '__main__':
    main()
