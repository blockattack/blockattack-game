/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2012 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
http://blockattack.sf.net
===========================================================================
*/

//listFiles.h - List files in a given directory, 10 files at a time, at most 250 files
#include <string.h>
#include <iostream>
#include <stdlib.h>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__)
#include	<dirent.h>
#endif
#include <string>
#include <vector>

#define MAX_NR_OF_FILES 253
#if defined (_WIN32)
#define FIRST_FILE 2
#else
#define FIRST_FILE 1
#endif

class ListFiles
{
private:
	int startFileNr;   //The first fileto belisted
	std::string filenames[MAX_NR_OF_FILES];
	int nrOfFiles;
#if defined(_WIN32)
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
#endif
	bool isInList(const std::string &name); //The name is already in the list
public:
	//ListFiles();
	//~ListFiles();
	void setDirectory(const std::string &dictory); //Find file in BlockAttack folder
	void setDirectory2(const std::string &dictory); //Second directory we also look in
	//void setDirecctoryInHome(string dictory); //Find files in home folder (it should work...)
	std::string getFileName(int);      //Returns the filename of a file
	bool fileExists(int);
	void forward();  //inclease startFile by 10
	void back();      //decrease startFile by 10
	std::string getRandom(); //Return the name of a random file in the directory, empty if none
};
