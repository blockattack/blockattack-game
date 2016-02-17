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
http://www.blockattack.net
===========================================================================
*/

#include "highscore.h"
#include "os.hpp"
#include "physfs_stream.hpp"
#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/archives/json.hpp"
#include "sago/SagoMisc.hpp"
#include <algorithm> 

namespace cereal {
	
template<class Archive>
void save(Archive & archive, record const & m) {
	archive( cereal::make_nvp("Name", m.name), cereal::make_nvp("Score", m.score) );
}

template<class Archive>
void load(Archive & archive, record & m) {
	archive( cereal::make_nvp("Name", m.name), cereal::make_nvp("Score", m.score) );
}

}

/*
 This sorts in reverse order. So the highest will be first
 */
bool record_sorter (const record& i,const record& j) { return (i.score > j.score); }

Highscore::Highscore(const std::string& type) {
	this->type = type;
	filename = type+".json.dat";
	std::string readFileContent = sago::GetFileContent(filename.c_str());
	if (readFileContent.length() > 0) {
		std::stringstream ss(readFileContent);
		{
			cereal::JSONInputArchive archive(ss);
			archive(cereal::make_nvp("highscore", table));
		}
	}
	if (table.size() < top) {
		for (int i = 0; i<top; i++) {
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
    std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss);
		archive(cereal::make_nvp("highscore", table));
	}
	sago::WriteFileContent(filename.c_str(), ss.str());
}

bool Highscore::isHighScore(int newScore) {
	if (newScore>table.back().score) {
		return true;
	}
	else {
		return false;
	}
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
	if(room < static_cast<int>(table.size()) ) {
		ret = table.at(room);
	}
	return ret;
}

