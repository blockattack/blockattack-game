/*
Block Attack - Rise of the Blocks, SDL game, besed on Nintendo's Tetris Attack
Copyright (C) 2008 Poul Sander

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
    R�vehjvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
    http://blockattack.sf.net
*/

#include "common.h"

#ifdef WIN32
//Returns path to "my Documents" in windows:
string getMyDocumentsPath()
{
    TCHAR pszPath[MAX_PATH];
    if (SUCCEEDED(SHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, FALSE))) {
        // pszPath is now the path that you want
#ifdef DEBUG
        cout << "MyDocuments Located: " << pszPath << endl;
#endif
        string theResult= pszPath;
        return theResult;
    }
    else
    {
        cout << "Warning: My Documents not found!" << endl;
        string theResult ="";
        return theResult;
    }
}

#endif

string getPathToSaveFiles()
{
#ifdef __unix__
    return (string)getenv("HOME")+(string)"/.gamesaves/"+GAMENAME;
#elif WIN32
    return getMyDocumentsPath()+(string)"/My Games/"+GAMENAME;
#else
    return ".";
#endif
}



commonTime getTotalTime()
{
    commonTime ct;
    string filename = getPathToSaveFiles()+"/totalTime";
    ifstream inFile(filename.c_str());
    if(inFile)
    {
        inFile >> ct.days;
        inFile >> ct.hours;
        inFile >> ct.minutes;
        inFile >> ct.seconds;
        inFile.close();
    }
    else
    {
        ct.days = 0;
        ct.hours = 0;
        ct.minutes = 0;
        ct.seconds = 0;
    }
    return ct;
}

commonTime addTotalTime(commonTime toAdd)
{
    commonTime ct = getTotalTime();
    
    ct.seconds +=toAdd.seconds;
    ct.minutes +=ct.seconds/60;
    ct.seconds = ct.seconds%60;
    
    ct.minutes += toAdd.minutes;
    ct.hours += ct.minutes/60;
    ct.minutes = ct.minutes%60;
    
    ct.hours += toAdd.hours;
    ct.days += ct.hours/24;
    ct.hours = ct.hours%24;
    
    ct.days += toAdd.days;
    
    string filename = getPathToSaveFiles()+"/totalTime";
    ofstream outFile(filename.c_str(),ios::trunc);
    if(outFile)
    {
        outFile << ct.days << endl;
        outFile << ct.hours << endl;
        outFile << ct.minutes << endl;
        outFile << ct.seconds << endl;
        outFile.close();
    }
    else
        cout << "Error writing total time to: " << filename << endl;
    return ct;
}
