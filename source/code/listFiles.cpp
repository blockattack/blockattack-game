/*
listFiles.cpp
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

/*
listFiles.cpp
*/

#include "listFiles.h"

/*ListFiles::ListFiles()
{

}

ListFiles::~ListFiles()
{

}*/


void ListFiles::setDirectory(string directory)
{
    for (int i=0;i<MAX_NR_OF_FILES;i++)
        filenames[i]="";
#if defined(_WIN32)
    DWORD dwError;
    dictory+="/*";
    hFind = FindFirstFile(dictory.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf ("Invalid file handle. Error is %u\n", GetLastError());
    }
    else
    {
        nrOfFiles=0;
        filenames[nrOfFiles] = FindFileData.cFileName;
        printf("File: %s\n",FindFileData.cFileName);
        while ((FindNextFile(hFind, &FindFileData) != 0)&&(nrOfFiles<MAX_NR_OF_FILES-1))
        {
            nrOfFiles++;
            filenames[nrOfFiles] = FindFileData.cFileName;
            printf("File: %s\n",FindFileData.cFileName);
        }

        dwError = GetLastError();
        FindClose(hFind);
        if (dwError != ERROR_NO_MORE_FILES)
        {
            printf ("FindNextFile error. Error is %s\n", dwError);
        }
    }
#elif defined(__unix__)
    DIR *DirectoryPointer;
    struct dirent *dp;
    nrOfFiles=0;
    //printf("Will look in: %s\n",directory.c_str());
    DirectoryPointer = opendir(directory.c_str());
    if(!DirectoryPointer)
        return;
    while ((dp=readdir(DirectoryPointer))&&(nrOfFiles<MAX_NR_OF_FILES-1))
    {
        nrOfFiles++;
        filenames[nrOfFiles] = (string)(char*)dp->d_name;
    }
    closedir(DirectoryPointer);


#endif
    startFileNr=FIRST_FILE;
    //Put code here
}

bool ListFiles::isInList(string name)
{
    for (int i=0;(i<=nrOfFiles);i++)
    {
        if (0==strcmp(name.c_str(),filenames[i].c_str()))
        {
            return true;
        }
    }
    return false;
}

void ListFiles::setDirectory2(string dic)
{
#if defined(__unix__)
    DIR *DirectoryPointer;
    struct dirent *dp;
    //printf("Will look in: %s\n",dic.c_str());
    DirectoryPointer = opendir(dic.c_str());
    if(!DirectoryPointer)
        return;
    while ((dp=readdir(DirectoryPointer))&&(nrOfFiles<MAX_NR_OF_FILES-1))
    {
        string name = (string)(char*)dp->d_name;
        if (!isInList(name))
        {
            nrOfFiles++;
            filenames[nrOfFiles] = name;
        }
    }
    closedir(DirectoryPointer);
    
    startFileNr=FIRST_FILE;
#endif
}

string ListFiles::getFileName(int nr)
{
    if (startFileNr+nr<MAX_NR_OF_FILES)
        return filenames[startFileNr+nr];
    else
    {
        string emptyString="";
        return "";
    }
}

bool ListFiles::fileExists(int nr)
{
    string emptyString="";
    if (startFileNr+nr<MAX_NR_OF_FILES)
    {
        if (filenames[startFileNr+nr]==emptyString)
            return false;
        else
            return true;
    }
    else
        return false;
}

void ListFiles::back()
{
    if (startFileNr>FIRST_FILE)
        startFileNr = startFileNr-10;
    if (startFileNr<FIRST_FILE)
        startFileNr = FIRST_FILE;
}

void ListFiles::forward()
{
    if (startFileNr<nrOfFiles-FIRST_FILE)
        startFileNr = startFileNr+10;
}

string ListFiles::getRandom()
{
    int numberOfFiles = nrOfFiles-FIRST_FILE+1;
    if(numberOfFiles<1)
        return "";
    int select = rand()%numberOfFiles;
    return filenames[FIRST_FILE+select];
}
