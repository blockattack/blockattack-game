/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2012 Poul Sander

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
http://blockattack.net
===========================================================================
*/


#include "MenuSystem.h"
#include "common.h"
#include "global.hpp"

int mousex;
int mousey;

using namespace std;


ButtonGfx standardButton;

void ButtonGfx::setSurfaces(sago::SagoSprite& marked, sago::SagoSprite& unmarked) {
	this->marked = marked;
	this->unmarked = unmarked;
	xsize=(marked).GetWidth();
	ysize=(marked).GetHeight();
	if (verboseLevel) {
		cout << "Surfaces set, size: " <<xsize << " , " << ysize << endl;
	}
}

Button::Button() {
	gfx = &standardButton;
	label = "";
	marked = false;
	action = nullptr;
	popOnRun = false;
}

Button::~Button() {
}

Button::Button(const Button& b) {
	gfx = b.gfx;
	label = b.label;
	marked = b.marked;
	action = b.action;
	popOnRun = false;
}

void Button::setLabel(const string& text) {
	label = text;
}

void Button::setAction(void (*action2run)(Button*)) {
	action = action2run;
}

bool Button::isClicked(int x,int y) {
	if ( x >= this->x && y >= this->y && x<= this->x+gfx->xsize && y <= this->y + gfx->ysize) {
		return true;
	}
	return false;
}

void Button::doAction() {
	if (action) {
		action(this);
		return;
	}
	cerr << "Warning: button \"" << label << "\" has no action assigned!";
}

void Button::drawToScreen() {
#if DEBUG
	//cout << "Painting button: " << label << " at: " << x << "," << y << endl;
#endif
	if (marked) {
		gfx->marked.Draw(screen, SDL_GetTicks(), x, y);
	}
	else {
		gfx->unmarked.Draw(screen, SDL_GetTicks(), x, y);
	}
	gfx->thefont->draw(screen, x+gfx->xsize/2,y+gfx->ysize/2-gfx->thefont->getHeight("%s", label.c_str())/2, NFont::CENTER, "%s", label.c_str());
}

void Button::setPopOnRun(bool popOnRun) {
	this->popOnRun = popOnRun;
}

bool Button::isPopOnRun() const {
	return popOnRun;
}

void Button::setGfx(ButtonGfx* gfx) {
	this->gfx = gfx;
}

int Button::getHeight() {
	return this->gfx->ysize;
}

void Menu::drawSelf() {
	SDL_RenderClear(screen);
	backgroundImage.Draw(screen, SDL_GetTicks(), 0, 0);
	vector<Button*>::iterator it;
	for (it = buttons.begin(); it < buttons.end(); it++) {
		(*it)->drawToScreen();
	}
	exit.drawToScreen();
	standardButton.thefont->draw(screen, 50, 50, "%s", title.c_str());
	mouse.Draw(screen, SDL_GetTicks(), mousex, mousey);
}

void Menu::performClick(int x,int y) {
	vector<Button*>::iterator it;
	for (it = buttons.begin(); it < buttons.end(); it++) {
		Button* b = (*it);
		if (b->isClicked(x,y)) {
			b->doAction();
		}
		if (b->isPopOnRun()) {
			running = false;
		}
	}
	if (exit.isClicked(x,y)) {
		running = false;
	}
}

void Menu::placeButtons() {
	int nextY = 100;
	const int X = 50;
	vector<Button*>::iterator it;
	for (it = buttons.begin(); it < buttons.end(); it++) {
		(*it)->x = X;
		(*it)->y = nextY;
		nextY += (*it)->getHeight()+10;
	}
	exit.x = X;
	exit.y = nextY;
}

void Menu::addButton(Button* b) {
	buttons.push_back(b);
	b->marked = false;
	placeButtons();
}

Menu::Menu(SDL_Renderer* screen) {
	this->screen = screen;
	buttons = vector<Button*>(10);
	isSubmenu = true;
	exit.setLabel( _("Back") );
}

Menu::Menu(SDL_Renderer* screen,bool submenu) {
	this->screen = screen;
	buttons = vector<Button*>(0);
	isSubmenu = submenu;
	if (isSubmenu) {
		exit.setLabel( _("Back") );
	}
	else {
		exit.setLabel( _("Exit") );
	}
}

Menu::Menu(SDL_Renderer* screen, const string& title, bool submenu) {
	this->screen = screen;
	buttons = vector<Button*>(0);
	isSubmenu = submenu;
	this->title = title;
	if (isSubmenu) {
		exit.setLabel(_("Back") );
	}
	else {
		exit.setLabel(_("Exit") );
	}
}

void Menu::run() {
	running = true;
	bool bMouseUp = false;
	long oldmousex = mousex;
	long oldmousey = mousey;
	while (running && !Config::getInstance()->isShuttingDown()) {
		if (!(highPriority)) {
			SDL_Delay(10);
		}


		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				Config::getInstance()->setShuttingDown(5);
				running = false;
			}

			if ( event.type == SDL_KEYDOWN ) {
				if ( event.key.keysym.sym == SDLK_ESCAPE ) {
					running = false;
				}

				if (event.key.keysym.sym == SDLK_UP) {
					marked--;
					if (marked<0) {
						marked = buttons.size();    //not -1, since exit is after the last element in the list
					}
				}

				if (event.key.keysym.sym == SDLK_DOWN) {
					marked++;
					if (marked>buttons.size()) {
						marked = 0;
					}
				}

				if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER ) {
					if (marked < buttons.size()) {
						buttons.at(marked)->doAction();
						if (buttons.at(marked)->isPopOnRun()) {
							running = false;
						}
					}
					if (marked == buttons.size()) {
						running = false;
					}
				}
			}


		}

		for (int i=0; i<buttons.size(); i++) {
			buttons.at(i)->marked = (i == marked);
		}
		exit.marked = (marked == buttons.size());
		Uint8 buttonState = SDL_GetMouseState(&mousex,&mousey);
		// If the mouse button is released, make bMouseUp equal true
		if ( (buttonState&SDL_BUTTON(1))==0) {
			bMouseUp=true;
		}

		if (abs(mousex-oldmousex)>5 || abs(mousey-oldmousey)>5) {
			for (int i=0; i< buttons.size(); ++i) {
				if (buttons.at(i)->isClicked(mousex,mousey)) {
					marked = i;
				}
			}
			if (exit.isClicked(mousex,mousey)) {
				marked = buttons.size();
			}
			oldmousex = mousex;
			oldmousey = mousey;
		}

		//mouse clicked
		if ( (buttonState&SDL_BUTTON(1) )==SDL_BUTTON(1) && bMouseUp) {
			bMouseUp = false;
			for (int i=0; i< buttons.size(); ++i) {
				if (buttons.at(i)->isClicked(mousex,mousey)) {
					buttons.at(i)->doAction();
					if (buttons.at(i)->isPopOnRun()) {
						running = false;
					}
					mousex = 0;
				}
			}
			if (exit.isClicked(mousex,mousey)) {
				running = false;
			}
		}

		drawSelf();
		SDL_RenderPresent(screen);
	}
}
