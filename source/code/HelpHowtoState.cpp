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

const int xsize = 1024;
const int ysize = 768;
const int buttonOffset = 160;
extern sago::SagoSprite bExit;

/**
 * Draws bricks with a string like:
 * "aab" for two identical one and another
 * "aaB" the third one will have a bomb
 * The any char not in 'a' to 'g' or 'A' to 'G' the behavior is undefined. 
 * @param target Target to draw to
 * @param bricks description on what to draw as a string
 * @param x
 * @param y
 */
static void RenderRowOfBricks(SDL_Renderer* target, const std::string& brickStr, int x, int y) {
	int tick = SDL_GetTicks();
	for (size_t i = 0; i < brickStr.size(); ++i) {
		bool bomb = false;
		char brickChar = brickStr[i];
		if (brickChar >= 'A' && brickChar <= 'G') {
			bomb = true;
			brickChar = brickChar + 'a' - 'A';
		}
		if (brickChar >= 'a' &&  brickChar <= 'g') {
			bricks[brickChar - 'a'].Draw(target, tick, x+i*50, y);
			if (bomb) {
				globalData.spriteHolder->GetSprite("block_bomb").Draw(target, tick, x+i*50, y);
			}
		}
	}
}

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
	RenderRowOfBricks(target, "aa gB", 50, 50);
	bExit.Draw(globalData.screen, SDL_GetTicks(), xsize-buttonOffset, ysize-buttonOffset);
}

void HelpHowtoState::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;

		//The Score button:
		if ((globalData.mousex>xsize-buttonOffset) && (globalData.mousex<xsize-buttonOffset+bExit.GetWidth()) 
				&& (globalData.mousey>ysize-buttonOffset) && (globalData.mousey<ysize-buttonOffset+bExit.GetHeight())) {
			isActive = false;
		}

	}
}