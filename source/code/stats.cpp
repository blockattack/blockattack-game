/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2016 Poul Sander

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
http://www.blockattack.net
===========================================================================
*/

#include "stats.h"
#include "common.h"
#include "os.hpp"
#include "sago/SagoMisc.hpp"
#include <sstream>

using std::string;
using std::stringstream;
using std::cerr;
using std::map;
using std::vector;

Stats* Stats::instance = nullptr;

const char* const statsFileName = "statsFile";

Stats::Stats() {
	statMap.clear();
	load();
}

void Stats::load() {
	string fileContent = sago::GetFileContent(statsFileName);
	stringstream inFile(fileContent);
	string key;
	string value;
	if (inFile) {
		while (!inFile.eof()) {
			inFile >> key; // The key is first on line
			inFile.get(); //Take the space
			getline(inFile, value); //The rest of the line is the value.
			statMap[key] = str2int(value);
		}
	}
}

Stats* Stats::getInstance() {
	if (Stats::instance==nullptr) {
		Stats::instance = new Stats();

	}
	return Stats::instance;
}

void Stats::save() {
	std::stringstream outFile;
	map<string,unsigned int>::iterator iter;
	for (iter = statMap.begin(); iter != statMap.end(); ++iter) {
		outFile << iter->first << " " << iter->second << "\n";
	}
	sago::WriteFileContent(statsFileName, outFile.str());
}

unsigned int Stats::getNumberOf(const string& statName) {
	if (exists(statName)) {
		return statMap[statName];
	}
	else {
		return 0;
	}
}

void Stats::addOne(const string& statName) {
	map<string,unsigned int>::iterator iter = statMap.find(statName);
	if (iter == statMap.end()) {
		statMap[statName] = 1;
	}
	else {
		iter->second++;
	}
}

bool Stats::exists(const string& statName) {
	//Using that 'find' returns an iterator to the end of the map if not found
	return statMap.find(statName) != statMap.end();
}
