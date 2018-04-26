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
extern sago::SagoSprite bricks[7];

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
	Uint32 tick = SDL_GetTicks();
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

class HorizontalSwitchAnimation {
public:
	std::string brickStr = "abc";
	int cursorPos = 0;
	int state = 0; //0=move left, 1 = switch, 2 = move right, 3 = switch
	Uint32 lastTick = 0;
	Uint32 animationSpeed = 2000;
	
	void Update (Uint32 tick) {
		if (tick > lastTick + animationSpeed) {
			lastTick = tick;
			switch (state) {
				case 0:
					cursorPos = 1;
					break;
				case 1:  //fallthough
				case 3:
					std::swap(brickStr[cursorPos], brickStr[cursorPos + 1]);
					break;
				case 2:
					cursorPos = 0;
					break;
			}
			++state;
			if (state > 3) {
				state = 0;
			}
		}
	}
};

HorizontalSwitchAnimation switchAnimation;
sago::SagoTextField switchAnimationField;

HelpHowtoState::HelpHowtoState() {
	switchAnimationField.SetHolder(&globalData.spriteHolder->GetDataHolder());
	switchAnimationField.SetFontSize(30);
	switchAnimationField.SetOutline(1, {0,0,0,255});

	switchAnimationField.SetText(_("Switch block horizontally"));
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
	RenderRowOfBricks(target, switchAnimation.brickStr, 50, 50);
	globalData.spriteHolder->GetSprite("cursor").Draw(target, SDL_GetTicks(), 50+switchAnimation.cursorPos*50, 50);
	switchAnimationField.Draw(target, 50 +150+30, 50);
	RenderRowOfBricks(target, "a aa", 50, 150);
	RenderRowOfBricks(target, " AAA", 50+300, 150);
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
	switchAnimation.Update(SDL_GetTicks());
}