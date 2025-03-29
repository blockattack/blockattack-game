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

#include "HelpCommon.hpp"
#include "HelpGamepadState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"
#include "gamecontroller.h"

static std::string getLabelForSupportedControllerNames() {
	std::string s = _("Only SDL2 compatible controllers are supported!\nSupported controllers: ");
	for (size_t i = 0 ; i<GetSupportedControllerNames().size(); ++i ) {
		if (i != 0) {
			s+= ", ";
		}
		s+= GetSupportedControllerNames().at(i);
	}
	return s;
}


HelpGamepadState::HelpGamepadState() {
	setHelp30Font(&globalData.spriteHolder->GetDataHolder(), moveLabel, _("Move cursor"));
	setHelp30Font(&globalData.spriteHolder->GetDataHolder(), pushLabel, _("Push line"));
	setHelp30Font(&globalData.spriteHolder->GetDataHolder(), backLabel, _("Back (Menu)"));
	setHelp30Font(&globalData.spriteHolder->GetDataHolder(), switchLabel, _("Switch"));
	setHelp30Font(&globalData.spriteHolder->GetDataHolder(), confirmLabel, _("Confirm"));
	std::string s = getLabelForSupportedControllerNames();
	setHelp30Font(&globalData.spriteHolder->GetDataHolder(), supportedControllers, s.c_str());
	supportedControllers.SetMaxWidth(740);
}

HelpGamepadState::~HelpGamepadState() {
}

void HelpGamepadState::ProcessInput(const SDL_Event& event, bool& processed) {
	if (GameControllerIsConnectionEvent(event)) {
		UnInitGameControllers();
		InitGameControllers();
		std::string s = getLabelForSupportedControllerNames();
		setHelp30Font(&globalData.spriteHolder->GetDataHolder(), supportedControllers, s.c_str());
		processed = true;
	}
	HelpCommonState::ProcessInput(event, processed);
}

#define OFFSETX (-512+globalData.xsize/2)

static void RenderDrawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2) {
	globalData.logicalResize.LogicalToPhysical(&x1, &y1);
	globalData.logicalResize.LogicalToPhysical(&x2, &y2);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void HelpGamepadState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	const sago::SagoSprite& gamepad = globalData.spriteHolder->GetSprite("help_controller");
	gamepad.Draw(target, SDL_GetTicks(), globalData.xsize/2-480/2, 100, &globalData.logicalResize);
	SDL_SetRenderDrawColor(target, 0, 0, 0, SDL_ALPHA_OPAQUE);
	RenderDrawLine(target, 100+OFFSETX, 210, globalData.xsize/2-480/2+130, 210);
	RenderDrawLine(target, 100+OFFSETX, 298, globalData.xsize/2-480/2+158, 298);
	RenderDrawLine(target, 100+OFFSETX, 210, 100+OFFSETX, 400);
	moveLabel.Draw(target, 100+OFFSETX, 404, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
	//Push lines
	RenderDrawLine(target, globalData.xsize/2-480/2+140, 90, globalData.xsize/2-480/2+140, 105);
	RenderDrawLine(target, globalData.xsize/2+480/2-140, 90, globalData.xsize/2+480/2-140, 105);
	RenderDrawLine(target, globalData.xsize/2-480/2+140-38, 90, globalData.xsize/2+480/2-140+38, 90);
	RenderDrawLine(target, globalData.xsize/2-480/2+140-38, 90, globalData.xsize/2-480/2+140-38, 110);
	RenderDrawLine(target, globalData.xsize/2+480/2-140+38, 90, globalData.xsize/2+480/2-140+38, 110);
	RenderDrawLine(target, globalData.xsize/2, 80, globalData.xsize/2, 90);
	pushLabel.Draw(target, globalData.xsize/2, 80, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::bottom, &globalData.logicalResize);
	//Back lines
	RenderDrawLine(target, 800+OFFSETX, 158, 800+OFFSETX, 207);
	RenderDrawLine(target, 490+OFFSETX, 180, 800+OFFSETX, 180);
	RenderDrawLine(target, 490+OFFSETX, 180, 490+OFFSETX, 195);
	RenderDrawLine(target, 663+OFFSETX, 207, 800+OFFSETX, 207);
	backLabel.Draw(target, 800+OFFSETX, 156, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::bottom, &globalData.logicalResize);
	RenderDrawLine(target, 625+OFFSETX, 241, 900+OFFSETX, 241);
	RenderDrawLine(target, 900+OFFSETX, 241, 900+OFFSETX, 400);
	switchLabel.Draw(target, 900+OFFSETX, 404, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
	confirmLabel.Draw(target, 900+OFFSETX, 404+30, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
	supportedControllers.Draw(target, 10, 600, sago::SagoTextField::Alignment::left, &globalData.logicalResize);
	HelpCommonState::Draw(target);
}

