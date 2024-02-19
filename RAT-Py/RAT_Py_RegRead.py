# Patrick Sacchet
# Version 1.0
# Serves to implement interfacing with registry reading functionality 

SUCCESS = 1
FAILURE = 0

# Command value
REGREAD = 6


def regRead(sock):
    try:

        return SUCCESS
    
    except Exception as e:
        print("ERROR: Could not send reg read to implant: ", e)
        return FAILURE
    
    return SUCCESS