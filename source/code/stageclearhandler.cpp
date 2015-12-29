/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "stageclearhandler.hpp"
#include "SDL.h"
#include <fstream>
#include <vector>
#include <iostream>

//paths
std::string stageClearSavePath;


std::vector<bool> stageCleared(nrOfStageLevels);		//vector that tells if a stage is cleared
std::vector<Uint32> stageTimes(nrOfStageLevels);             //For statistical puposes
std::vector<Uint32> stageScores(nrOfStageLevels);            //--||--
Uint32 totalScore = 0;
Uint32 totalTime = 0;

using namespace std;

void StageClearSetClear(int Level, int score, int time) {
	stageCleared[Level] = true;
	int gameEndedAfter = time;
	if (stageScores[Level]<score) {
		stageScores[Level] = score;
		stageTimes[Level] = gameEndedAfter;
	}

	ofstream outfile;
	outfile.open(stageClearSavePath.c_str(), ios::binary |ios::trunc);
	if (!outfile) {
		cerr << "Error writing to file: " << stageClearSavePath << endl;
	}
	else {
		for (int i=0; i<nrOfStageLevels; i++) {
			bool tempBool = stageCleared[i];
			outfile.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
		}
		for (int i=0; i<nrOfStageLevels; i++) {
			Uint32 tempUInt32 = stageScores[i];
			outfile.write(reinterpret_cast<char*>(&tempUInt32), sizeof(Uint32));
		}
		for (int i=0; i<nrOfStageLevels; i++) {
			Uint32 tempUInt32 = stageTimes[i];
			outfile.write(reinterpret_cast<char*>(&tempUInt32), sizeof(Uint32));
		}
		outfile.close();
	}
}

void LoadStageClearStages() {
	bool tempBool;
	Uint32 tempUInt32;
	ifstream stageFile(stageClearSavePath.c_str(),ios::binary);
	if (stageFile) {
		for (int i = 0; i<nrOfStageLevels; i++) {
			stageFile.read(reinterpret_cast<char*>(&tempBool),sizeof(bool));
			stageCleared[i]=tempBool;
		}
		if (!stageFile.eof()) {
			for (int i=0; i<nrOfStageLevels; i++) {
				tempUInt32 = 0;
				if (!stageFile.eof()) {
					stageFile.read(reinterpret_cast<char*>(&tempUInt32),sizeof(Uint32));
				}
				stageScores[i]=tempUInt32;
				totalScore+=tempUInt32;
			}
			for (int i=0; i<nrOfStageLevels; i++) {
				tempUInt32 = 0;
				if (!stageFile.eof()) {
					stageFile.read(reinterpret_cast<char*>(&tempUInt32),sizeof(Uint32));
				}
				stageTimes[i]=tempUInt32;
				totalTime += tempUInt32;
			}
		}
		else {
			for (int i=0; i<nrOfStageLevels; i++) {
				stageScores[i]=0;
				stageTimes[i]=0;
			}
		}
		stageFile.close();
	}
	else {
		for (int i=0; i<nrOfStageLevels; i++) {
			stageCleared[i]= false;
			stageScores[i]=0;
			stageTimes[i]=0;
		}
	}
}

int GetTotalScore() {
	return totalScore;
}
int GetTotalTime() {
	return totalTime;
}

int GetNrOfLevels() {
	return nrOfStageLevels;
}

bool IsStageCleared(int level) {
	return stageCleared.at(level);
}


int GetStageScores(int level) {
	return stageScores.at(level);
}

int GetStageTime(int level) {
	return stageTimes.at(level);
}