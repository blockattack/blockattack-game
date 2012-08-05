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

/*
Handles replay
*/

#include <string.h>

#include "replay.h"

extern int verboseLevel;

Replay::Replay()
{
	isLoaded = false;
	actions.clear();
}

Replay::Replay(const Replay& r)
{
	isLoaded = r.isLoaded;
	name = r.name;
	actions = r.actions;
}







bool Replay::saveReplay(string filename)
{
	//Saving as fileversion 4
	if(verboseLevel)
		cout << "Saving as version 4 save file" << endl;
	ofstream saveFile;
	saveFile.open(filename.c_str(),ios::binary|ios::trunc);
	if (saveFile)
	{
		saveFile << "0 16 PLAYER 1\n";
		saveFile << "0 16 NAME " << name << "\n";
		for(int i = 0;i < actions.size(); ++i) {
			saveFile << actions.at(i).time << " " << actions.at(i).action << " " << actions.at(i).param << "\n";
		}
		saveFile.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool Replay::saveReplay(string filename,Replay p2)
{
	//Saving as fileversion 4
	if(verboseLevel)
		cout << "Saving as version 4 save file (2 players)" << endl;
	ofstream saveFile;
	saveFile.open(filename.c_str(),ios::binary|ios::trunc);
	if (saveFile)
	{
		saveFile << "0 16 PLAYER 1\n";
		saveFile << "0 16 NAME " << name << "\n";
		for(int i = 0;i < actions.size(); ++i) {
			saveFile << actions.at(i).time << " " << actions.at(i).action << " " << actions.at(i).param << "\n";
		}
		saveFile << "0 16 PLAYER 2\n";
		saveFile << "0 16 NAME " << name << "\n";
		for(int i = 0;i < p2.getActions().size(); ++i) {
			saveFile << p2.getActions().at(i).time << " " << p2.getActions().at(i).action << " " << p2.getActions().at(i).param << "\n";
		}
		saveFile.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool Replay::loadReplay(string filename)
{
	isLoaded = true;
	actions.clear();
	ifstream loadFile;
	loadFile.open(filename.c_str());
	if (loadFile)
	{
		while(!loadFile.eof()) {
			Sint32 time;
			Uint32 action;
			string restOfLine;
			loadFile >> time >> action;
			if(action == 16)
			{
				string command;
				loadFile >> command;
				if(command == "NAME")
					getline(loadFile,name,'\n');
				if(command == "PLAYER") {
					getline(loadFile,restOfLine,'\n');
					if(restOfLine != " 1")
						break;
				}
			} else
			{
				getline(loadFile,restOfLine,'\n');
				Action a;
				a.time = time;
				a.action = action;
				if(restOfLine.length()>1)
					a.param = restOfLine.substr(1);
				else
					a.param = "";
				actions.push_back(a);
			}
			
		}
		loadFile.close();
		if(verboseLevel)
			cout << "Loaded 1 player, actions.size="<< actions.size() << endl;
	}
	else
	{
		cerr << "File not found or couldn't open: " << filename << endl;
		return false;
	}

}

bool Replay::loadReplay2(string filename)
{
	isLoaded = true;
	bool player2started = false; //set to true once the player two part is reached
	actions.clear();
	ifstream loadFile;
	loadFile.open(filename.c_str());
	if (loadFile)
	{
		while(!loadFile.eof()) {
			Sint32 time;
			Uint32 action;
			string restOfLine;
			loadFile >> time >> action;
			if(action == 16)
			{
				string command;
				loadFile >> command;
				if(command == "NAME")
					getline(loadFile,name,'\n');
				if(command == "PLAYER") {
					getline(loadFile,restOfLine,'\n');
					if(restOfLine == " 2")
						player2started = true;
				}
			} else
			{
				getline(loadFile,restOfLine,'\n');
				if(!player2started)
					continue;
				Action a;
				a.time = time;
				a.action = action;
				if(restOfLine.length()>1)
					a.param = restOfLine.substr(1);
				else
					a.param = "";
				actions.push_back(a);
			}
			
		}
		loadFile.close();
		if(verboseLevel)
			cout << "Loaded 2 player, actions.size="<< actions.size() << endl;
	}
	else
	{
		cerr << "File not found or couldn't open: " << filename << endl;
		return false;
	}

}

void Replay::setName(string name)
{
	this->name = name;
}

string Replay::getName() const
{
	return name;
}

vector<Action> Replay::getActions() const {
	return actions;
}

void Replay::addAction(int tick, int action, string param) 
{
#if DEBUG
	cout << tick << " " << action << " " << param << endl;
#endif
	Action a;
	a.time = tick;
	a.action = action;
	a.param = param;
	actions.push_back(a);
}
