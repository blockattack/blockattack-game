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

#ifndef _COMMON_H
#define	_COMMON_H

#define GAMENAME "blockattack"

#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

struct commonTime{
    unsigned int days;
    unsigned int hours;
    unsigned int minutes;
    unsigned int seconds;
};

string getPathToSaveFiles();

commonTime getTotalTime();

commonTime addTotalTime(commonTime toAdd);

#endif	/* _COMMON_H */

