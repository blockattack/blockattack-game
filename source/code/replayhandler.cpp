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

#include "replayhandler.hpp"
#include <sstream>
#include "cereal/archives/json.hpp"
#include "sago/SagoMisc.hpp"

static std::tm GetLocalTime() {
	std::time_t t = std::time(nullptr);
	std::tm ret = *std::localtime(&t);
	return ret;
}

static std::string CreateFileName(const std::tm& t ) {
	std::string ret = "replays/blockattack_game_";
	char buffer[200];
	snprintf(buffer, sizeof(buffer), "replays/blockattack_game_%i-%02i-%02iT%02i_%02i_%02i_AUTO.replay", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	ret = buffer;
	return ret;
}

static void SaveReplayToFile(const SavedReplayStruct& sr, const std::string& filename) {
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss,cereal::JSONOutputArchive::Options::NoIndent());
		archive(cereal::make_nvp("savedReplay", sr));
	}
	sago::WriteFileContent(filename.c_str(), ss.str());
}

static void LoadReplayFromPhysFile(SavedReplayStruct& sr, const std::string& filename) {
	std::string filecontent = sago::GetFileContent(filename.c_str());
	std::stringstream ss(filecontent);
	{
		cereal::JSONInputArchive archive(ss);
		archive(cereal::make_nvp("savedReplay", sr));
	}
}

void LoadReplay(const std::string& filename, BlockGameInfo& game1, BlockGameInfo& game2) {
	SavedReplayStruct sr;
	LoadReplayFromPhysFile(sr, filename);
	if (sr.playerInfo.size() > 0) {
		game1 = sr.playerInfo.at(0);
	}
	if (sr.playerInfo.size() > 1) {
		game2 = sr.playerInfo.at(1);
	}
}

void SaveReplay(const BlockGameInfo& game1) {
	SavedReplayStruct sr;
	sr.numberOfPlayers = 1;
	sr.playerInfo.push_back(game1);
	std::tm t = GetLocalTime();
	std::string filename = CreateFileName(t);
	SaveReplayToFile(sr, filename);
}

void SaveReplay(const BlockGameInfo& game1, const BlockGameInfo& game2) {
	SavedReplayStruct sr;
	sr.numberOfPlayers = 2;
	sr.playerInfo.push_back(game1);
	sr.playerInfo.push_back(game2);
	std::tm t = GetLocalTime();
	std::string filename = CreateFileName(t);
	SaveReplayToFile(sr, filename);
}