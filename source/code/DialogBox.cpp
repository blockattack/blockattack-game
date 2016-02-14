/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2016 Poul Sander

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
http://www.blockattack.net
===========================================================================
*/

#include "DialogBox.hpp"
#include "global.hpp"
#include "common.h"
#include "ReadKeyboard.h"

static void NFont_Write(SDL_Renderer* target, int x, int y, const std::string& text) {
	nf_standard_blue_font.draw(target, x, y, "%s", text.c_str());
}

bool OpenDialogbox(int x, int y, std::string& name) {
	DialogBox d(x, y, name);
	RunGameState(d);
	if (d.IsUpdated()) {
		name = d.GetName();
		return true;
	}
	return false;
}

DialogBox::DialogBox(int x, int y, const std::string& name) {
	this->x = x;
	this->y = y;
	SetName(name);
}


DialogBox::~DialogBox() {
}

bool DialogBox::IsActive() {
	return isActive;
}


void DialogBox::Draw(SDL_Renderer* target) {
	backgroundImage.Draw(screen, SDL_GetTicks(), 0, 0);
	dialogBox.Draw(screen, SDL_GetTicks(), x, y);
	NFont_Write(screen, x+40,y+76,rk->GetString());
	std::string strHolder = rk->GetString();
	strHolder.erase((int)rk->CharsBeforeCursor());

	if (((SDL_GetTicks()/600)%2)==1) {
		NFont_Write(screen, x+40+nf_standard_blue_font.getWidth( "%s", strHolder.c_str()),y+76,"|");
	}
}

void DialogBox::ProcessInput(const SDL_Event& event, bool &processed) {
	if (event.type == SDL_TEXTINPUT) {
		if ((rk->ReadKey(event))&&(SoundEnabled)&&(!NoSound)) {
			Mix_PlayChannel(1, typingChunk, 0);
		}
	}

	if ( event.type == SDL_KEYDOWN ) {
		if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) ) {
			name = rk->GetString();
			updated = true;
			isActive = false;
		}
		else if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
			isActive = false;
		}
		else {
			if ((rk->ReadKey(event))&&(SoundEnabled)&&(!NoSound)) {
				Mix_PlayChannel(1,typingChunk,0);
			}
		}
	}
	processed = true;
}

void DialogBox::SetName(const std::string& name) {
	this->name = name;
	rk = std::make_shared<ReadKeyboard>(name.c_str());
}

std::string DialogBox::GetName() const {
	return name;
}

bool DialogBox::IsUpdated() const {
	return updated;
}