// Patrick Sacchet
// Header file for defining debugging related functionality 

#pragma once

#include "pch.h"

using namespace std;


namespace InternalDebug
{

	class DebugOutput
	{
	public:
		// The file we'll be writing to for debug output
		string fileName;

		DebugOutput()
		{
			fileName = "output.txt";
		}

		static fstream createDebugFile();

		static BOOL writeFile(fstream &debugFile, string fileInput);
		
	};





}