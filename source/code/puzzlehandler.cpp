/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2015 Poul Sander

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

#include "puzzlehandler.hpp"
#include "nlohmann/json.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include "stats.h"
#include <physfs.h>         //Abstract file system. To use containers
#include "sago/SagoMisc.hpp"

using json = nlohmann::json;

const int maxNrOfPuzzleStages = 50; //Maximum number of puzzle stages

static std::string puzzleSavePath;
static std::string puzzleName;              //The filename of
static std::vector<bool> puzzleCleared(maxNrOfPuzzleStages); //vector that tells if puzzle cleared
static std::vector<int> nrOfMovesAllowed(maxNrOfPuzzleStages);  //Moves to clear
static int puzzleLevels[maxNrOfPuzzleStages][6][12]; //Contains board layout;
static int nrOfPuzzles;    //How many are there actually?

int PuzzleNumberOfMovesAllowed(int level) {
	return nrOfMovesAllowed.at(level);
}

int PuzzleGetBrick(int level, int x, int y) {
	if (level >= maxNrOfPuzzleStages || x >= 6 || y >= 12 || level < 0 || x < 0 || y < 0) {
		return -1;
	}
	return puzzleLevels[level][x][y];
}

int PuzzleGetNumberOfPuzzles() {
	return nrOfPuzzles;
}

bool PuzzleIsCleared(int level) {
	return puzzleCleared.at(level);
}

const std::string& PuzzleGetName() {
	return puzzleName;
}

void PuzzleSetName(const std::string& name) {
	puzzleName = name;
	puzzleSavePath = name + ".json.save";
}

void LoadClearData() {
	std::string readFileContent = sago::GetFileContent(puzzleSavePath.c_str());
	if (readFileContent.length() > 0) {
		json j = json::parse(readFileContent);
		try {
			j.at("cleared").get_to(puzzleCleared);
		} catch (json::exception& e) {
			std::cerr << "Failed to read \"" << puzzleSavePath << "\". File will be regenerated. Reason: " << e.what() << "\n";
			puzzleCleared.clear();
		}
	}
	else {
		puzzleCleared.clear();
	}
	puzzleCleared.resize(nrOfPuzzles);
}

void SaveClearData() {
	json j = json{ { "cleared", puzzleCleared } };
	std::string s = j.dump(4);
	sago::WriteFileContent(puzzleSavePath.c_str(), s);
}

void PuzzleSetClear(int Level) {
	if (puzzleCleared[Level]==false) {
		Stats::getInstance()->addOne("puzzlesSolved");
	}
	puzzleCleared[Level] = true;
	SaveClearData();
}

/*Loads all the puzzle levels*/
int LoadPuzzleStages( ) {
	if (!PHYSFS_exists(((std::string)("puzzles/"+puzzleName)).c_str())) {
		std::cerr << "Warning: File not in blockattack.data: " << ("puzzles/"+puzzleName) << "\n";
		return -1; //file doesn't exist
	}
	std::string fileContent = sago::GetFileContent(((std::string)("puzzles/"+puzzleName)).c_str());
	std::stringstream inFile(fileContent);

	inFile >> nrOfPuzzles;
	if (nrOfPuzzles>maxNrOfPuzzleStages) {
		nrOfPuzzles=maxNrOfPuzzleStages;
	}
	if (nrOfPuzzles < 0) {
		nrOfPuzzles = 0;
	}
	for (int k=0; k<nrOfPuzzles ; k++) {
		inFile >> nrOfMovesAllowed.at(k);
		for (int i=11; i>=0; i--)
			for (int j=0; j<6; j++) {
				inFile >> puzzleLevels[k][j][i];
			}
	}
	LoadClearData();
	return 0;
}

int SavePuzzleStages() {
	if (puzzleName.length() == 0) {
		std::cerr << "Error: No puzzle name set. Cannot save.\n";
		return -1;
	}
	std::string fileContent = std::to_string(nrOfPuzzles) + "\n";
	for (int k=0; k<nrOfPuzzles ; k++) {
		fileContent += std::to_string(nrOfMovesAllowed.at(k)) + "\n";
		for (int i=11; i>=0; i--)
			for (int j=0; j<6; j++) {
				fileContent += std::to_string(puzzleLevels[k][j][i]) + " ";
			}
		fileContent += "\n";
	}
	sago::WriteFileContent(((std::string)("puzzles/"+puzzleName)).c_str(), fileContent);
	return 0;
}