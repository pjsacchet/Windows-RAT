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
		// The file we'll be writing to for debug output - add time/date field
		string fileName;
		fstream debugFile;

		// Default constructor will just use output.txt as filename; creates actual file in default append mode 
		DebugOutput()
		{
			fileName = "output.txt";

			debugFile.open(fileName, ios::app);
			debugFile << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n";
			debugFile << "DEBUG START: \n";
			debugFile << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n";
			debugFile.close();
		}

		// Take user input for file name and mode 
		DebugOutput(string newFileName, char mode)
		{
			fileName = newFileName;

			debugFile.open(fileName, mode);
			debugFile << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n";
			debugFile << "DEBUG START: \n";
			debugFile << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n";
			debugFile.close();
		}

		static DebugOutput* createFile();
		void writeFile(string fileInput, char mode);
		void deleteFile();


	};





}