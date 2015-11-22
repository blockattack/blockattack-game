#include "puzzlehandler.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include "stats.h"
#include <physfs.h>         //Abstract file system. To use containers
#include "physfs_stream.hpp" //To use C++ style file streams

using namespace std;

static const int maxNrOfPuzzleStages = 50; //Maximum number of puzzle stages
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
}

void PuzzleSetSavePath(const std::string& filepath) {
	puzzleSavePath = filepath;
}

void PuzzleSetClear(int Level) {
	if(puzzleCleared[Level]==false) {
		Stats::getInstance()->addOne("puzzlesSolved");
	}
	puzzleCleared[Level] = true;
	std::ofstream outfile;
	outfile.open(puzzleSavePath.c_str(), ios::binary |ios::trunc);
	if (!outfile)
	{
		std::cerr << "Error writing to file: " << puzzleSavePath << endl;
	}
	else
	{
		for (int i=0; i<nrOfPuzzles; i++)
		{
			bool tempBool = puzzleCleared[i];
			outfile.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
		}
		outfile.close();
	}
}

/*Loads all the puzzle levels*/
int LoadPuzzleStages( )
{
	if (!PHYSFS_exists(((std::string)("puzzles/"+puzzleName)).c_str()))
	{
		std::cerr << "Warning: File not in blockattack.data: " << ("puzzles/"+puzzleName) << endl;
		return -1; //file doesn't exist
	}
	PhysFS::ifstream inFile(((std::string)("puzzles/"+puzzleName)).c_str());

	inFile >> nrOfPuzzles;
	if (nrOfPuzzles>maxNrOfPuzzleStages) {
		nrOfPuzzles=maxNrOfPuzzleStages;
	}
	for (int k=0; (k<nrOfPuzzles) /*&&(!inFile.eof())*/ ; k++)
	{
		inFile >> nrOfMovesAllowed[k];
		for (int i=11; i>=0; i--)
			for (int j=0; j<6; j++)
			{
				inFile >> puzzleLevels[k][j][i];
			}
	}
	bool tempBool;
	std::ifstream puzzleFile(puzzleSavePath.c_str(), std::ios::binary);
	if (puzzleFile)
	{
		for (int i=0; (i<nrOfPuzzles)&&(!puzzleFile.eof()); i++)
		{
			puzzleFile.read(reinterpret_cast<char*>(&tempBool),sizeof(bool));
			puzzleCleared[i] = tempBool;
		}
		puzzleFile.close();
	}
	else
	{
		tempBool = false;
		for (int i=0; i<nrOfPuzzles; i++) {
			puzzleCleared[i] = tempBool;
		}
	}
	return 0;
}