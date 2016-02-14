/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DialogBox.cpp
 * Author: poul
 * 
 * Created on 14. februar 2016, 14:56
 */

#include "DialogBox.hpp"
#include "global.hpp"
#include "common.h"
#include "ReadKeyboard.h"

static void NFont_Write(SDL_Renderer* target, int x, int y, const std::string& text) {
	nf_standard_blue_font.draw(target, x, y, "%s", text.c_str());
}

#if 0
bool OpenDialogbox(int x, int y, std::string& name) {
	bool done = false;     //We are done!
	bool accept = false;   //New name is accepted! (not Cancelled)
	SDL_TextInput textInputScope;
	ReadKeyboard rk = ReadKeyboard(name.c_str());
	std::string strHolder;
	backgroundImage.Draw(screen, SDL_GetTicks(), 0, 0);
	while (!done && !Config::getInstance()->isShuttingDown()) {
		dialogBox.Draw(screen, SDL_GetTicks(), x, y);
		NFont_Write(screen, x+40,y+76,rk.GetString());
		strHolder = rk.GetString();
		strHolder.erase((int)rk.CharsBeforeCursor());

		if (((SDL_GetTicks()/600)%2)==1) {
			NFont_Write(screen, x+40+nf_standard_blue_font.getWidth( "%s", strHolder.c_str()),y+76,"|");
		}

		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				Config::getInstance()->setShuttingDown(5);
				done = true;
				accept = false;
			}

			

		}   //while(event)

		SDL_RenderPresent(screen); //Update screen
	}   //while(!done)
	name = rk.GetString();
	return accept;
}
#endif

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