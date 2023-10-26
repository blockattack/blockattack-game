/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2017 Poul Sander

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

#ifndef REPLAYPLAYER_HPP
#define REPLAYPLAYER_HPP

#include "sago/GameStateInterface.hpp"

class ReplayPlayer : public sago::GameStateInterface {
public:
	ReplayPlayer();
	ReplayPlayer(const ReplayPlayer& orig) = delete;
	virtual ~ReplayPlayer();

	bool IsActive() override;
	void Draw(SDL_Renderer* target) override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Update() override;
private:
	bool isActive = true;
};

#endif /* REPLAYPLAYER_HPP */

