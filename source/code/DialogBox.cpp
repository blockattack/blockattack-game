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

static void setButtonFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text){
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(24);
	field.SetOutline(1, {0,0,0,255});
	field.SetText(text);
}

static void NFont_Write(SDL_Renderer* target, int x, int y, const std::string& text) {
	globalData.standard_blue_font.draw(target, x, y, text);
}

static void DrawRect(SDL_Renderer* target, int topx, int topy, int height, int width, const std::string& name) {
	const int size = 32;
	SDL_Rect bounds_ns = {topx, topy+size, width, height-2*size};  //bounds for south
	SDL_Rect bounds_e = {topx, topy, width-size, height};
	const sago::SagoSprite& n = globalData.spriteHolder->GetSprite(name+"n");
	const sago::SagoSprite& s = globalData.spriteHolder->GetSprite(name+"s");
	const sago::SagoSprite& e = globalData.spriteHolder->GetSprite(name+"e");
	const sago::SagoSprite& w = globalData.spriteHolder->GetSprite(name+"w");
	const sago::SagoSprite& fill = globalData.spriteHolder->GetSprite(name+"fill");
	for (int i = 1; i < width/size; ++i) {
		n.DrawBounded(target, SDL_GetTicks(), topx+i*size, topy, bounds_e);
		for (int j = 1; j < height/size; ++j) {
			w.DrawBounded(target, SDL_GetTicks(), topx, topy+j*size, bounds_ns);
			fill.Draw(target, SDL_GetTicks(),topx+i*size, topy+j*size);
			e.DrawBounded(target, SDL_GetTicks(), topx+width-size, topy+j*size, bounds_ns);
		}
		s.DrawBounded(target, SDL_GetTicks(), topx+i*size, topy+height-size, bounds_e);
	}
	//Corners
	const sago::SagoSprite& nw = globalData.spriteHolder->GetSprite(name+"nw");
	const sago::SagoSprite& ne = globalData.spriteHolder->GetSprite(name+"ne");
	const sago::SagoSprite& se = globalData.spriteHolder->GetSprite(name+"se");
	const sago::SagoSprite& sw = globalData.spriteHolder->GetSprite(name+"sw");
	nw.Draw(target, SDL_GetTicks(), topx, topy);
	ne.Draw(target, SDL_GetTicks(), topx+width-size, topy);
	se.Draw(target, SDL_GetTicks(), topx+width-size, topy+height-size);
	sw.Draw(target, SDL_GetTicks(), topx, topy+height-size);
}

static void DrawRectWhite(SDL_Renderer* target, int topx, int topy, int height, int width) {
	std::string name = "ui_rect_white_";
	DrawRect(target, topx, topy, height, width, name);
}

static void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width) {
	std::string name = "ui_rect_yellow_";
	DrawRect(target, topx, topy, height, width, name);
}

bool OpenDialogbox(int x, int y, std::string& name, const std::string& header) {
	DialogBox d(x, y, name, header);
	RunGameState(d);
	if (d.IsUpdated()) {
		name = d.GetName();
		return true;
	}
	return false;
}

DialogBox::DialogBox(int x, int y, const std::string& name, const std::string& header) : header(header) {
	this->x = x;
	this->y = y;
	SetName(name);
	setButtonFont(&globalData.spriteHolder->GetDataHolder(), headerLabel, header.c_str());
	setButtonFont(&globalData.spriteHolder->GetDataHolder(), enterLabel, _("Enter to accept"));
	setButtonFont(&globalData.spriteHolder->GetDataHolder(), cancelLabel, _("Esc to cancel"));
}


DialogBox::~DialogBox() {
}

bool DialogBox::IsActive() {
	return isActive;
}


void DialogBox::Draw(SDL_Renderer* target) {
	DrawBackground(globalData.screen);
	this->x = globalData.xsize/2-300;
	this->y = globalData.ysize/2-100;
	DrawRectYellow(target, x, y, 200, 600);
	headerLabel.Draw(target, x+300, y+20, sago::SagoTextField::Alignment::center);
	enterLabel.Draw(target, x+150, y+140, sago::SagoTextField::Alignment::center);
	cancelLabel.Draw(target, x+450, y+140, sago::SagoTextField::Alignment::center);
	DrawRectWhite(target, x+26, y+64, 54, 600-2*26);
	NFont_Write(target, x+40, y+76,rk->GetString());
	std::string strHolder = rk->GetString();
	strHolder.erase((int)rk->CharsBeforeCursor());

	if (((SDL_GetTicks()/600)%2)==1) {
		NFont_Write(target, x+40+globalData.standard_blue_font.getWidth( strHolder),y+76,"|");
	}
}

void DialogBox::ProcessInput(const SDL_Event& event, bool& processed) {
	if (event.type == SDL_TEXTINPUT) {
		if ((rk->ReadKey(event))&&(globalData.SoundEnabled)&&(!globalData.NoSound)) {
			Mix_PlayChannel(1, globalData.typingChunk.get(), 0);
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
			if ((rk->ReadKey(event))&&(globalData.SoundEnabled)&&(!globalData.NoSound)) {
				Mix_PlayChannel(1, globalData.typingChunk.get(), 0);
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
