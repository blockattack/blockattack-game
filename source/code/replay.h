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

//headerfile for replay.h
/*
replay is used to save a replay, there is saved 10 moves per second, should be
able to give a realistic replay
*/

#ifndef REPLAY_H
#define REPLAY_H 1


#include "SDL.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Action
{
	Uint32 time;
	Sint32 action;
	string param;
};

class Replay
{
private:
	vector<Action> actions;
//If we are loaded from a file, then we are read only
	bool isLoaded;
//Store player name
	string name;

public:



	Replay(); //Constructor
	Replay(const Replay& r); //Copy constructor

	//New replay type 2.0.0+:
	void addAction(int tick, int action, string param);

//Ok, I'll ignore this for some time, and just save to file, if however, we ever use a dynamic saving structure, we are fucked
	bool saveReplay(string);	//Saves a replay
	bool saveReplay(string,Replay p2); //saves a replay, plus another replay given as a parameter
	bool loadReplay(string); //laods a replay
	bool loadReplay2(string);
	void setName(string name);
	string getName() const;
	vector<Action> getActions() const; //loads the second part of the replay file, if it exists, returns false otherwise
};

#endif
