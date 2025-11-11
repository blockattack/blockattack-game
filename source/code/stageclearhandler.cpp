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

#include "nlohmann/json.hpp"
#include "stageclearhandler.hpp"
#include "SDL.h"
#include <vector>
#include <iostream>
#include <sstream>
#include "sago/SagoMisc.hpp"

using json = nlohmann::json;

//paths
const char* const stageClearSaveName = "stageClear.json.SCsave";

struct StageClearElement {
	bool cleared = false;
	int time = 0;
	int score = 0;
};

void to_json(json& j, const StageClearElement& p) {
	j = json{ {"cleared", p.cleared}, {"time", p.time}, {"score", p.score}};
}

void from_json(const json& j, StageClearElement& p) {
	p.cleared = j.at("cleared").get<bool>();
	p.time = j.at("time").get<int>();
	p.score = j.at("score").get<int>();
}

std::vector<StageClearElement> stages(nrOfStageLevels);

//std::vector<bool> stageCleared(nrOfStageLevels);        //vector that tells if a stage is cleared
//std::vector<Sint32> stageTimes(nrOfStageLevels);             //For statistical puposes
//std::vector<Sint32> stageScores(nrOfStageLevels);            //--||--
std::vector<StageClearElement> parScores(nrOfStageLevels);
Sint32 totalScore = 0;
Sint32 totalTime = 0;


static void SaveStageClearStages() {
	json j = json{ {"stages", stages} };
	std::string s = j.dump(4);
	sago::WriteFileContent(stageClearSaveName, s);
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
		json j = json::parse(readFileContent);
		try {
			j.at("stages").get_to(stages);
		}
		catch (json::exception& e) {
			std::cerr << "Failed to read highscore " << stageClearSaveName << " due to formatting errors. Resetting the file. Reason: " <<
			          e.what() << "\n";
			stages.clear();
		}
	}
	else {
		stages.clear();
	}
	readFileContent = sago::GetFileContent("misc/stage_clear_par.json");
	json j = json::parse(readFileContent);
	try {
		j.at("stages").get_to(parScores);
	}
	catch (json::exception& e) {
		std::cerr << "Failed to read par times. Reason: " << e.what() << "\n";
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

int GetStageParScore(int level) {
	return parScores.at(level).score;
}

int GetStageScores(int level) {
	return stages.at(level).score;
}

int GetStageTime(int level) {
	return stages.at(level).time;
}
