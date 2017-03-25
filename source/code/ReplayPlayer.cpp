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

#include "ReplayPlayer.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"
//#include "BlockGameSdl.inc"



/*static void MoveBlockGameSdls( BlockGameSdl& game1, BlockGameSdl& game2 ) {
	game1.SetTopXY(50, globalData.ysize/2-284);
	game2.SetTopXY(globalData.xsize-500, globalData.ysize/2-284);
}*/


ReplayPlayer::ReplayPlayer() {
}

ReplayPlayer::~ReplayPlayer() {
}

bool ReplayPlayer::IsActive() {
	return isActive;
}

void ReplayPlayer::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	//MoveBlockGameSdls(theGame, theGame2);
	//DrawEverything(globalData.xsize, globalData.ysize, &theGame, &theGame2);
}

void ReplayPlayer::ProcessInput(const SDL_Event& event, bool& processed) {
	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void ReplayPlayer::Update() {

}
