/*
highscore.cpp
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
    Rævehøjvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
*/

#include "highscore.h"

#ifdef WIN32

//Returns path to "my Documents" in windows:
string getMyDocumentsPath1()
{
    TCHAR pszPath[MAX_PATH];
    //if (SUCCEEDED(SHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, FALSE))) {
    if (SUCCEEDED(SHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, FALSE))) {
        // pszPath is now the path that you want
        cout << "MyDocuments Located: " << pszPath << endl;
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

Highscore::Highscore(int type)
{
#if defined(__unix__)
    string home = getenv("HOME");
    string filename1 = home+"/.gamesaves/blockattack/endless.dat";
    string filename2 = home+"/.gamesaves/blockattack/timetrial.dat";
#elif defined(_WIN32)
    string home = getMyDocumentsPath1();
    string filename1, filename2;
    if (home.length())
    {
        filename1 = home+"/My Games/blockattack/endless.dat";
        filename2 = home+"/My Games/blockattack/timetrial.dat";
    }
    else
    {
        filename1 = "endless.dat";
        filename2 = "timetrial.dat";
    }
#else
    string filename1 = "endless.dat";
    string filename2 = "timetrial.dat";
#endif
    ourType = type;
    if (type == 1) filename = (char*)malloc(filename1.length()+1);
    if (type == 2) filename = (char*)malloc(filename2.length()+1);
    if (filename == NULL){
        cout << "Out of memory" << endl;
        exit(1);
    }
    if (type == 1) memcpy(filename,filename1.c_str(),filename1.length()+1);
    if (type == 2) memcpy(filename,filename2.c_str(),filename2.length()+1);
    ifstream scorefile(filename, ios::binary);
    if (scorefile)
    {
        for (int i = 0; i<top; i++)
        {
            scorefile.read(tabel[i].name,30*sizeof(char));
            scorefile.read(reinterpret_cast<char*>(&tabel[i].score), sizeof(int));
        }
    }
    else
    {
        for (int i = 0; i<top; i++)
        {
            strcpy(tabel[i].name,"Poul Sander                  \0");
            tabel[i].score = 2000 - i*100;
        }
    }
    scorefile.close();
    free(filename);
    writeFile();
}

void Highscore::writeFile()
{
#if defined(__unix__)
    string home = getenv("HOME");
    string filename1 = home+"/.gamesaves/blockattack/endless.dat";
    string filename2 = home+"/.gamesaves/blockattack/timetrial.dat";
#elif defined(_WIN32)
    string home = getMyDocumentsPath1();
    string filename1, filename2;
    if (home.length())
    {
        filename1 = home+"/My Games/blockattack/endless.dat";
        filename2 = home+"/My Games/blockattack/timetrial.dat";
    }
    else
    {
        filename1 = "endless.dat";
        filename2 = "timetrial.dat";
    }
#else
    string filename1 = "endless.dat";
    string filename2 = "timetrial.dat";
#endif
    if (ourType == 1) filename = (char*)malloc(filename1.length()+1);
    if (ourType == 2) filename = (char*)malloc(filename2.length()+1);
    if (filename == NULL){
        cout << "Out of memory" << endl;
        exit(1);
    }
    if (ourType == 1) memcpy(filename,filename1.c_str(),filename1.length()+1);
    if (ourType == 2) memcpy(filename,filename2.c_str(),filename2.length()+1);

    ofstream outfile;
    outfile.open(Highscore::filename, ios::binary |ios::trunc);
    if (!outfile)
    {
        cout << "Error writing to file: " << filename << endl;
        exit(1);
    }
    for (int i = 0;i<top;i++)
    {
        outfile.write(tabel[i].name,30*sizeof(char));
        outfile.write(reinterpret_cast<char*>(&tabel[i].score),sizeof(int));
    }
    outfile.close();
    free(filename);
}

bool Highscore::isHighScore(int newScore)
{
    if (newScore>tabel[top-1].score)
        return true;
    else
        return false;
}

void Highscore::addScore(char newName[], int newScore)
{
    int ranking = top-1;
    while ((tabel[ranking-1].score<newScore) && (ranking != 0))
        ranking--;
    for (int i=top-1; i>ranking; i--)
    {
        tabel[i].score = tabel[i-1].score;
        strcpy(tabel[i].name,"                            \0");
        strcpy(tabel[i].name,tabel[i-1].name);
    }
    tabel[ranking].score = newScore;
    strcpy(tabel[ranking].name,"                            \0");
    strcpy(tabel[ranking].name,newName);
    Highscore::writeFile();
}

int Highscore::getScoreNumber(int room)
{
    return tabel[room].score;
}

char* Highscore::getScoreName(int room)
{
    return &tabel[room].name[0];
}
