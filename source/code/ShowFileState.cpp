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

#include "DialogBox.hpp"
#include "HelpCommon.hpp"
#include "ShowFileState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"
#include <fmt/core.h>

const int buttonOffset = 160;
extern sago::SagoSprite bExit;

static void setHelpGamepadFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(30);
	field.SetOutline(1, {128,128,128,255});
	field.SetText(text);
}

ShowFileState::ShowFileState() {
	setHelpGamepadFont(&globalData.spriteHolder->GetDataHolder(), titleField, "");
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), infoBox, "");
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), filenameField, "");
}

ShowFileState::~ShowFileState() {
}

void ShowFileState::SetData(const std::string& filename, const std::string& header) {
	titleField.SetText(header);
	infoBox.SetText(sago::GetFileContent(filename));
	filenameField.SetText(fmt::format(_("Showing content of: {}"), filename));
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
	titleField.Draw(target, 50, 50);
	DrawRectYellow(target, 40, 90, 600, 900);
	infoBox.SetMaxWidth(850);
	infoBox.Draw(target, 50, 100);
	DrawRectYellow(target, 40, 700, 50, 900);
	filenameField.Draw(target, 50, 715);
	bExit.Draw(globalData.screen, SDL_GetTicks(), globalData.xsize-buttonOffset, globalData.ysize-buttonOffset);
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
		if ((globalData.mousex>globalData.xsize-buttonOffset) && (globalData.mousex<globalData.xsize-buttonOffset+bExit.GetWidth())
		        && (globalData.mousey>globalData.ysize-buttonOffset) && (globalData.mousey<globalData.ysize-buttonOffset+bExit.GetHeight())) {
			isActive = false;
		}

	}
}
