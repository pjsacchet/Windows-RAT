# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with elevate functionality 

SUCCESS = 1
FAILURE = 0

ELEVATE = 14

'''
Description:
    Prints elevate usage for user
Params:
    N/A
Returns:
    SUCCESS or FAILURE  
'''
def printElevateHelp():
    print("""Required params: \n
                - N/A \n""")
    return SUCCESS


def elevate(sock):
    return SUCCESS