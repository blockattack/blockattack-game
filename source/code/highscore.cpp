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
https://blockattack.net
===========================================================================
*/


#include "nlohmann/json.hpp"
#include "highscore.h"
#include "os.hpp"
#include "sago/SagoMisc.hpp"
#include <algorithm>
#include "common.h"
#include <fmt/core.h>

using json = nlohmann::json;

void to_json(json& j, const record& p) {
	j = json{ {"Name", p.name}, {"Score", p.score}};
}

void from_json(const json& j, record& p) {
	p.name = j.at("Name").get<std::string>();
	p.score = j.at("Score").get<int>();
}

/*
 This sorts in reverse order. So the highest will be first
 */
bool record_sorter (const record& i,const record& j) {
	return (i.score > j.score);
}

Highscore::Highscore(const std::string& type, double speed) : filename(type+".json.dat"), type(type), speed(speed) {
	if (speed < 0.4) {
		//Use special filenames for higher speeds (higher speed = lower number)
		filename = fmt::format("{}_{:.4f}.json.dat", type, speed);
	}
	std::string readFileContent = sago::GetFileContent(filename.c_str());
	if (readFileContent.length() > 0) {
		json j = json::parse(readFileContent);
		try {
			j.at("highscore").get_to(table);
		}
		catch (json::exception& e) {
			std::cerr << "Failed to read highscore " << filename << " due to formatting errors. Resetting the file. Reason: " <<
			          e.what() << "\n";
			table.clear();
		}
	}
	if (table.size() < top) {
		for (size_t i = table.size(); i<top; i++) {
			record r;
			r.name = "Poul Sander";
			r.score = 2000 - i*100;
			table.push_back(r);
		}
	}
	std::stable_sort(table.begin(), table.end(), record_sorter);
	table.resize(top);
	writeFile();
}


void Highscore::writeFile() {
	json j = json{ { "highscore", table } };
	std::string s = j.dump(4);
	sago::WriteFileContent(filename.c_str(), s);
}

bool Highscore::isHighScore(int newScore) {
	if (newScore>table.back().score) {
		return true;
	}
	return false;
}

void Highscore::addScore(const std::string& newName, int newScore) {
	record r;
	r.name = newName;
	r.score = newScore;
	table.push_back(r);
	std::stable_sort(table.begin(), table.end(), record_sorter);
	table.resize(top);
	Highscore::writeFile();
}

record Highscore::getScoreNumber(int room) {
	record ret;
	if (room < static_cast<int>(table.size()) ) {
		ret = table.at(room);
	}
	return ret;
}

