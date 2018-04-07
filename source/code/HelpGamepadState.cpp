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

#include "HelpGamepadState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"

HelpGamepadState::HelpGamepadState() {
}


HelpGamepadState::~HelpGamepadState() {
}

bool HelpGamepadState::IsActive() {
	return isActive;
}

void HelpGamepadState::ProcessInput(const SDL_Event& event, bool& processed) {
	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void HelpGamepadState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	const sago::SagoSprite& gamepad = globalData.spriteHolder->GetSprite("help_controller");
	gamepad.Draw(target, SDL_GetTicks(), globalData.xsize/2-480/2, 100);
}

void HelpGamepadState::Update() {
	
}