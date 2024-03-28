# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with process functionality 

SUCCESS = 1
FAILURE = 0

# Command value
PROCESSLIST = 10
PROCESSTART = 11
PROCESSSTOP = 12
PROCESSRESTART = 13

'''
Description:
    Prints process listing usage for user
Params:
    N/A
Returns:
    SUCCESS or FAILURE  
'''
def processListHelp():
    print("""Required params: \n
                - N/A \n""")
    return SUCCESS


def processQuery():
    return SUCCESS


'''
Description:
    Performs process list functionality; communicates with C2 server and handles responses
Params:
    sock - sock object with connection already established
Returns:
    SUCCESS or FAILURE 
'''
def processList(sock):
    return SUCCESS