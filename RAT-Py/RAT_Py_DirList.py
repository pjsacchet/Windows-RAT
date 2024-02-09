# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with dir list functionality 

SUCCESS = 1
FAILURE = 0

# Command value
DIR = 3

def dirList(sock, dirPath):
    try:
        # First tell our implant we want a DIR list performed
        print("Sending command...\n")
        sock.send(bytes(str(DIR), "utf-8") + b'\x00')

        # Send filepath to write to 
        print("Sending dir path...\n")
        sock.send(dirPath + b'\x00')

        print("Files in %s:\n" & dirPath)

        # Keep getting files back until we get our terminator
        while (data != b'\x00\x00'):
            data = sock.recv(1024)
            data = data.strip()
            data = data.decode('utf-8')
            print("%s\n" % data)

        # Recieved our terminator so now should receive SUCCESS...
        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful dirl ist!\n")

        else:
            print("Implant returned FAILURE!\n")


    except Exception as e:
        print("ERROR: Could not send dir list to implant: ", e)
        return FAILURE

    return SUCCESS
