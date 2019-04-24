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
#include "gamecontroller.h"

const int buttonOffset = 160;

static void setHelpGamepadFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(30);
	field.SetOutline(2, {0,0,0,255});
	field.SetText(text);
}

static void setHelpGamepadFont(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(30);
	field.SetOutline(2, {0,0,0,255});
	field.SetText(text);
}


HelpGamepadState::HelpGamepadState() {
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), moveLabel, _("Move cursor"));
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), pushLabel, _("Push line"));
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), backLabel, _("Back (Menu)"));
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), switchLabel, _("Switch"));
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), confirmLabel, _("Confirm"));
	std::string s = _("Only SDL2 compatible controllers are supported!\nSupported controllers: ");
	for (size_t i = 0 ; i<GetSupportedControllerNames().size(); ++i ) {
		if (i != 0) {
			s+= ", ";
		}
		s+= GetSupportedControllerNames().at(i);
	}
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), supportedControllers, s.c_str());
	supportedControllers.SetMaxWidth(740);
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

extern sago::SagoSprite bExit;

#define OFFSETX (-512+globalData.xsize/2)

void HelpGamepadState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	const sago::SagoSprite& gamepad = globalData.spriteHolder->GetSprite("help_controller");
	gamepad.Draw(target, SDL_GetTicks(), globalData.xsize/2-480/2, 100);
	SDL_RenderDrawLine(target, 100, 210, globalData.xsize/2-480/2+130, 210);
	SDL_RenderDrawLine(target, 100, 298, globalData.xsize/2-480/2+158, 298);
	SDL_RenderDrawLine(target, 100, 210, 100, 400);
	moveLabel.Draw(target, 100, 404, sago::SagoTextField::Alignment::center);
	//Push lines
	SDL_RenderDrawLine(target, globalData.xsize/2-480/2+140, 90, globalData.xsize/2-480/2+140, 105);
	SDL_RenderDrawLine(target, globalData.xsize/2+480/2-140, 90, globalData.xsize/2+480/2-140, 105);
	SDL_RenderDrawLine(target, globalData.xsize/2-480/2+140-38, 90, globalData.xsize/2+480/2-140+38, 90);
	SDL_RenderDrawLine(target, globalData.xsize/2-480/2+140-38, 90, globalData.xsize/2-480/2+140-38, 110);
	SDL_RenderDrawLine(target, globalData.xsize/2+480/2-140+38, 90, globalData.xsize/2+480/2-140+38, 110);
	SDL_RenderDrawLine(target, globalData.xsize/2, 80, globalData.xsize/2, 90);
	pushLabel.Draw(target, globalData.xsize/2, 80, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::bottom);
	//Back lines
	SDL_RenderDrawLine(target, 625+OFFSETX, 168, 800+OFFSETX, 168);
	SDL_RenderDrawLine(target, 800+OFFSETX, 158, 800+OFFSETX, 195);
	SDL_RenderDrawLine(target, 490+OFFSETX, 195, 800+OFFSETX, 195);
	backLabel.Draw(target, 800+OFFSETX, 156, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::bottom);
	SDL_RenderDrawLine(target, 625+OFFSETX, 241, 900+OFFSETX, 241);
	SDL_RenderDrawLine(target, 663+OFFSETX, 207, 900+OFFSETX, 207);
	SDL_RenderDrawLine(target, 900+OFFSETX, 207, 900+OFFSETX, 400);
	switchLabel.Draw(target, 900+OFFSETX, 404, sago::SagoTextField::Alignment::center);
	confirmLabel.Draw(target, 900+OFFSETX, 404+30, sago::SagoTextField::Alignment::center);
	bExit.Draw(globalData.screen, SDL_GetTicks(), globalData.xsize-buttonOffset, globalData.ysize-buttonOffset);
	supportedControllers.Draw(target, 10, 600);

#if DEBUG
	static sago::SagoTextField mousePos;
	mousePos.SetHolder(&globalData.spriteHolder->GetDataHolder());
	mousePos.SetFontSize(16);
	mousePos.SetOutline(1, {128,128,128,255});
	mousePos.SetText(std::string("Mouse position: ")+std::to_string(globalData.mousex)+std::string(", ")+std::to_string(globalData.mousey));
	mousePos.Draw(target, 0,0);
#endif
}

void HelpGamepadState::Update() {
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