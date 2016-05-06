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
http://www.blockattack.net
===========================================================================
*/

#include "stageclearhandler.hpp"
#include "SDL.h"
#include <vector>
#include <iostream>
#include <sstream>
#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/archives/json.hpp"
#include "sago/SagoMisc.hpp"
#include "Libs/include/cereal/details/helpers.hpp"

//paths
const char* const stageClearSaveName = "stageClear.json.SCsave";

struct StageClearElement {
	bool cleared = false;
	int time = 0;
	int score = 0;

	template<class Archive>
	void serialize(Archive& archive) {
		archive( cereal::make_nvp("cleared", cleared), cereal::make_nvp("time", time), cereal::make_nvp("score", score) );
	}
};

std::vector<StageClearElement> stages(nrOfStageLevels);

std::vector<bool> stageCleared(nrOfStageLevels);        //vector that tells if a stage is cleared
std::vector<Sint32> stageTimes(nrOfStageLevels);             //For statistical puposes
std::vector<Sint32> stageScores(nrOfStageLevels);            //--||--
Sint32 totalScore = 0;
Sint32 totalTime = 0;

using std::string;
using std::cerr;
using std::vector;

static void SaveStageClearStages() {
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(cereal::make_nvp("stages", stages));
	}
	sago::WriteFileContent(stageClearSaveName, ss.str());
}

void StageClearSetClear(int Level, int score, int time) {

	stages.at(Level).cleared = true;
	int gameEndedAfter = time;
	if (stages.at(Level).score<score) {
		stages[Level].score = score;
		stages[Level].time = gameEndedAfter;
	}
	SaveStageClearStages();
}



void LoadStageClearStages() {
	std::string readFileContent = sago::GetFileContent(stageClearSaveName);
	if (readFileContent.length() > 0) {
		std::stringstream ss(readFileContent);
		{
			try {
				cereal::JSONInputArchive archive(ss);
				archive(cereal::make_nvp("stages", stages));
			}
			catch (cereal::Exception& e) {
				std::cerr << "Failed to load file \"" << stageClearSaveName << "\". Reason: " << e.what() << "\n";
				stages.clear();
			}
		}
	}
	else {
		stages.clear();
	}
	stages.resize(nrOfStageLevels);
	totalScore = 0;
	totalTime = 0;
	for (const StageClearElement& s : stages) {
		totalScore += s.score;
		totalTime += s.time;
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
	return stages.at(level).cleared;
}


int GetStageScores(int level) {
	return stages.at(level).score;
}

int GetStageTime(int level) {
	return stages.at(level).time;
}
