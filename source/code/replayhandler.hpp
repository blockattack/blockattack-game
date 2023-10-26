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

#include "BlockGame.hpp"
#include "cereal/types/vector.hpp"

struct SavedReplayStruct {
	int numberOfPlayers = 0;
	std::vector<BlockGameInfo> playerInfo;
	template <class Archive>
	void serialize( Archive& ar ) {
		ar( CEREAL_NVP(numberOfPlayers), CEREAL_NVP(playerInfo) );
	}
};

void SaveReplay(const BlockGameInfo& game1);

void SaveReplay(const BlockGameInfo& game1, const BlockGameInfo& game2);