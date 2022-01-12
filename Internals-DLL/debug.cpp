// Patrick Sacchet
// Defines functionality needed for easy debugging 

#include "pch.h"
#include "debug.h"

using namespace std;

fstream InternalDebug::DebugOutput::createDebugFile()
{
	InternalDebug::DebugOutput debug;
	fstream DebugFile(debug.fileName);
	//fstream DebugFile("debugoutput.txt");
	DebugFile.close();

	return DebugFile;
}


BOOL InternalDebug::DebugOutput::writeFile(fstream &debugFile, string fileInput)
{
	debugFile << fileInput;

	debugFile.close(); 

	return TRUE;
}


