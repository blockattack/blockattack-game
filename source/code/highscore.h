/*
highscore.h
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

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <stdlib.h>

#ifdef _WIN32
#ifndef CSIDL_PERSONAL
#define CSIDL_PERSONAL                  0x0005
#endif
#define _WIN32_IE  0x0400
#include "windows.h"
#include <shlobj.h>
#endif

using namespace std;

const int top = 10;

struct record
{
    char name[30];
    int score;
};

class Highscore
{
private:
    record tabel[top];
    char* filename;
    int ourType;     //This is ugly, remove me, plz!
    void writeFile();
public:

    Highscore()
    {
    }

    Highscore(int type);

    bool isHighScore(int);
    void addScore(char[],int);
    int getScoreNumber(int);
    char* getScoreName(int);
};
