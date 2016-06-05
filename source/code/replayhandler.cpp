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

static void SaveReplayToFile(const SavedReplayStruct& sr, const char* filename) {
	std::stringstream ss;
	{
		cereal::JSONOutputArchive archive(ss,cereal::JSONOutputArchive::Options::NoIndent());
		archive(cereal::make_nvp("savedReplay", sr));
	}
	sago::WriteFileContent(filename, ss.str());
}

void SaveReplay(const BlockGameInfo& game1) {
	SavedReplayStruct sr;
	sr.numberOfPlayers = 1;
	sr.playerInfo.push_back(game1);
	SaveReplayToFile(sr, "SavedSinglePlayerGame");
}

void SaveReplay(const BlockGameInfo& game1, const BlockGameInfo& game2) {
	SavedReplayStruct sr;
	sr.numberOfPlayers = 2;
	sr.playerInfo.push_back(game1);
	sr.playerInfo.push_back(game2);
	SaveReplayToFile(sr, "SavedTwoPlayerGame");
}