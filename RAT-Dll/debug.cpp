// Patrick Sacchet
// Defines functionality needed for easy debugging 

#include "pch.h"
#include "debug.h"

//using namespace std;

namespace InternalDebug
{

	DebugOutput* InternalDebug::DebugOutput::createFile()
	{
		DebugOutput debug;
		debug = DebugOutput();

		return &debug;
	}

	void InternalDebug::DebugOutput::writeFile(string fileInput, char mode)
	{
		this->debugFile.open(this->fileName, mode);
		debugFile << fileInput;
		debugFile.close();

		return;
	}

	void InternalDebug::DebugOutput::deleteFile()
	{
		remove((this->fileName).c_str());

		return;
	}

}

