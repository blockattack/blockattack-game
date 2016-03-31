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
#include <vector>
#include <iostream>
#include "stats.h"
#include <physfs.h>         //Abstract file system. To use containers
#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/archives/json.hpp"
#include "sago/SagoMisc.hpp"

using namespace std;

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
		std::stringstream ss(readFileContent);
		{
			try {
				cereal::JSONInputArchive archive(ss);
				archive(cereal::make_nvp("cleared", puzzleCleared));
			}
			catch (cereal::Exception& e) {
				std::cerr << "Failed to read \"" << puzzleSavePath << "\". File will be regenerated. Reason: " << e.what() << std::endl;
				puzzleCleared.clear();
			}
		}
	}
	else {
		puzzleCleared.clear();
	}
	puzzleCleared.resize(nrOfPuzzles);
}

void SaveClearData() {
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(cereal::make_nvp("cleared", puzzleCleared));
	}
	sago::WriteFileContent(puzzleSavePath.c_str(), ss.str());
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
		std::cerr << "Warning: File not in blockattack.data: " << ("puzzles/"+puzzleName) << endl;
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
