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

#include "HelpCommon.hpp"
#include "MenuSystem.h"
#include "global.hpp"
#include "DialogBox.hpp"



template<typename T> void setHelp30FontTemplate(const sago::SagoDataHolder* holder, T& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(30);
	field.SetOutline(2, {0,0,0,255});
	field.SetText(text);
}

void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	setHelp30FontTemplate(holder, field, text);
}

void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text) {
	setHelp30FontTemplate(holder, field, text);
}

void setHelpBoxFont(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetColor({0,0,0,255});
	field.SetFontSize(20);
	field.SetOutline(0, {0,0,0,255});
	field.SetText(text);
}

void setHelp30FontThinOutline(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	setHelp30Font(holder,field, text);
	field.SetOutline(1, {128,128,128,255});
}

bool HelpCommonState::IsActive() {
	return isActive;
}

void HelpCommonState::ProcessInput(const SDL_Event& event, bool& processed) {

	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

extern sago::SagoSprite bExit;


void HelpCommonState::Draw(SDL_Renderer* target) {
	bExit.Draw(target, SDL_GetTicks(), globalData.xsize-buttonOffset, globalData.ysize-buttonOffset);
#if DEBUG
	static sago::SagoTextField mousePos;
	mousePos.SetHolder(&globalData.spriteHolder->GetDataHolder());
	mousePos.SetFontSize(16);
	mousePos.SetOutline(1, {128,128,128,255});
	mousePos.SetText(std::string("Mouse position: ")+std::to_string(globalData.mousex)+std::string(", ")+std::to_string(globalData.mousey));
	mousePos.Draw(target, 0,0);
#endif
}

void HelpTextBoxState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	titleField.Draw(target, 50, 50);
	DrawRectYellow(target, 40, 90, 600, 900);
	infoBox.SetMaxWidth(850);
	infoBox.Draw(target, 50, 100);
	if (filenameField.GetText().length() > 0) {
		DrawRectYellow(target, 40, 700, 50, 900);
		filenameField.Draw(target, 50, 715);
	}
	HelpCommonState::Draw(target);
}


void HelpCommonState::Update() {
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