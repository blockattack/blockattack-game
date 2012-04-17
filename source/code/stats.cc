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

#include "stats.h"
#include "common.h"

Stats* Stats::instance = NULL;

Stats::Stats()
{
	statMap.clear();
	load();
}

void Stats::load()
{
	string filename = getPathToSaveFiles()+"/statsFile";
	ifstream inFile(filename.c_str());
	string key;
	char value[MAX_VAR_LENGTH];
	if(inFile)
	{
		while(!inFile.eof())
		{
			inFile >> key; // The key is first on line
			inFile.get(); //Take the space
			inFile.getline(value,MAX_VAR_LENGTH); //The rest of the line is the value.
			statMap[key] = str2int(value);
		}
		inFile.close();
	}
}

Stats* Stats::getInstance()
{
	if(Stats::instance==NULL)
	{
		Stats::instance = new Stats();

	}
	return Stats::instance;
}

void Stats::save()
{
	string filename = getPathToSaveFiles()+"/statsFile";
	ofstream outFile(filename.c_str(),ios::trunc);

	if(outFile)
	{
		//outFile << statMap.size() << endl;
		map<string,unsigned int>::iterator iter;
		for(iter = statMap.begin(); iter != statMap.end(); iter++)
		{
			outFile << iter->first << " " << iter->second << endl;
		}
	}
}

unsigned int Stats::getNumberOf(string statName)
{
	if(exists(statName))
	{
		return statMap[statName];
	}
	else
		return 0;
}

void Stats::addOne(string statName)
{
	map<string,unsigned int>::iterator iter = statMap.find(statName);
	if(iter == statMap.end())
	{
		statMap[statName] = 1;
	}
	else
	{
		iter->second++;
	}
}

bool Stats::exists(string statName)
{
	//Using that 'find' returns an iterator to the end of the map if not found
	return statMap.find(statName) != statMap.end();
}
