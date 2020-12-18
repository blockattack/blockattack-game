/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2020 Poul Sander

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

#include "ModConfigMenuState.hpp"
#include "MenuSystem.h"
#include "global.hpp"
#include "MenuSystem.h"
#include "sago/SagoMisc.hpp"

const int buttonOffset = 160;
extern sago::SagoSprite bExit;


ModConfigMenuState::ModConfigMenuState() {
	mods_available = sago::GetFileList("mods");
}

ModConfigMenuState::~ModConfigMenuState() {}

bool ModConfigMenuState::IsActive() {
	return isActive;
}

void ModConfigMenuState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	standardButton.getLabel(_("Mod config"))->Draw(target, 50, 50);
	for (size_t i = 0; i < mods_available.size(); ++i) {
		standardButton.getLabel(mods_available[i])->Draw(target, 60, 80+20*i);
	}
	bExit.Draw(globalData.screen, SDL_GetTicks(), globalData.xsize-buttonOffset, globalData.ysize-buttonOffset);
}

void ModConfigMenuState::ProcessInput(const SDL_Event& event, bool &processed) {
	if (isEscapeEvent(event)) {
		isActive = false;
	}
}

void ModConfigMenuState::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;

		//The Score button:
		if ((globalData.mousex>globalData.xsize-buttonOffset) && (globalData.mousex<globalData.xsize-buttonOffset+bExit.GetWidth())
		        && (globalData.mousey>globalData.ysize-buttonOffset) && (globalData.mousey<globalData.ysize-buttonOffset+bExit.GetHeight())) {
			isActive = false;
		}

	}
}
