/*
listFiles.h
Copyright (C) 2005 Poul Sander

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Poul Sander
    R�veh�jvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
*/

//listFiles.h - List files in a given directory, 10 files at a time, at most 250 files
#include <string.h>
#include <iostream>
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
#define FIRST_FILE 3
#endif

using namespace std;

class ListFiles
{
private:
    int startFileNr;   //The first fileto belisted
    string filenames[MAX_NR_OF_FILES];
    int nrOfFiles;
#if defined(_WIN32)
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
#endif
    bool isInList(string name); //The name is already in the list
public:
    //ListFiles();
    //~ListFiles();
    void setDirectory(string dictory); //Find file in BlockAttack folder
    void setDirectory2(string dictory); //Second directory we also look in
    //void setDirecctoryInHome(string dictory); //Find files in home folder (it should work...)
    string getFileName(int);      //Returns the filename of a file
    bool fileExists(int);
    void forward();  //inclease startFile by 10
    void back();      //decrease startFile by 10
    string getRandom(); //Return the name of a random file in the directory, empty if none
};
