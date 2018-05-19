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

#include "ShowFileState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"

const int xsize = 1024;
const int ysize = 768;
const int buttonOffset = 160;
extern sago::SagoSprite bExit;

ShowFileState::ShowFileState() {
}

ShowFileState::~ShowFileState() {
}

bool ShowFileState::IsActive() {
	return isActive;
}

void ShowFileState::ProcessInput(const SDL_Event& event, bool& processed) {

	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void ShowFileState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	bExit.Draw(globalData.screen, SDL_GetTicks(), xsize-buttonOffset, ysize-buttonOffset);
#if DEBUG
	static sago::SagoTextField mousePos;
	mousePos.SetHolder(&globalData.spriteHolder->GetDataHolder());
	mousePos.SetFontSize(16);
	mousePos.SetOutline(1, {128,128,128,255});
	mousePos.SetText(std::string("Mouse position: ")+std::to_string(globalData.mousex)+std::string(", ")+std::to_string(globalData.mousey));
	mousePos.Draw(target, 0,0);
#endif
}

void ShowFileState::Update() {
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