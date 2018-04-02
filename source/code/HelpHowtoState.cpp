/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2018 Poul Sander

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

#include "HelpHowtoState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"

HelpHowtoState::HelpHowtoState() {
	box.SetHolder(&globalData.spriteHolder->GetDataHolder());
	box.SetFontSize(30);
	box.SetOutline(1, {128,128,128,255});
	box.SetText(_("The basic purpose of the game is to match 3 or more blocks of identical color to make them explode.\n"
	"You can only move blocks vertically."
	"The blocks are constantly raising from the bottom of the screen. Once they reach the top the game will start counting down to your"
	" imminent defeat."));
}

HelpHowtoState::~HelpHowtoState() {
}

bool HelpHowtoState::IsActive() {
	return isActive;
}

void HelpHowtoState::ProcessInput(const SDL_Event& event, bool& processed) {

	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void HelpHowtoState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	box.SetMaxWidth(1000);
	box.Draw(target, 10,10);
}

void HelpHowtoState::Update() {
	
}