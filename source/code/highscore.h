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
http://blockattack.net
===========================================================================
*/

#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <vector>

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

const int top = 10;

struct record
{
	std::string name;
	int score = 0;
};


class Highscore
{
private:
	std::vector<record> table;
	std::string filename;
	std::string type;
	void writeFile();
	double speed = 0.5;
public:
	Highscore(const std::string& type, double speed);

	bool isHighScore(int);
	void addScore(const std::string& newName, int);
	record getScoreNumber(int);
};

#endif