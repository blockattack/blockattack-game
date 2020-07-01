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
https://blockattack.net
===========================================================================
*/


#include "MenuSystem.h"
#include "common.h"
#include "global.hpp"
#include "gamecontroller.h"
#include "BlockGame.hpp"

static int oldmousex = 0;
static int oldmousey = 0;

const char* const menu_marked = "menu_marked";
const char* const menu_unmarked = "menu_unmarked";

ButtonGfx standardButton;

void ButtonGfx::setSurfaces() {
	this->xsize = globalData.spriteHolder->GetSprite(menu_marked).GetWidth();
	this->ysize = globalData.spriteHolder->GetSprite(menu_marked).GetHeight();
	if (globalData.verboseLevel) {
		std::cout << "Surfaces set, size: " << this->xsize << " , " << this->ysize << "\n";
	}
}

sago::SagoTextField* ButtonGfx::getLabel(const std::string& text) {
	const auto& theLabel = labels.find(text);
	if (theLabel != labels.end()) {
		return labels[text].get();
	}
	std::shared_ptr<sago::SagoTextField> newField = std::make_shared<sago::SagoTextField>();
	newField->SetHolder(&globalData.spriteHolder->GetDataHolder());
	newField->SetFont("freeserif");
	newField->SetFontSize(30);
	newField->SetOutline(1, {64,64,64,255});
	newField->SetText(text);
	labels[text] = newField;
	return labels[text].get();
}


Button::Button(const Button& b) : action{b.action}, label{b.label}, marked{b.marked} {
}

Button& Button::operator=(const Button& other) {
	action = other.action;
	label = other.label;
	marked = other.marked;
	return *this;
}

void Button::setLabel(const std::string& text) {
	label = text;
}

void Button::setAction(void (*action2run)(void)) {
	action = action2run;
}

void Button::doAction() {
	if (action) {
		action();
		return;
	}
	std::cerr << "Warning: button \"" << label << "\" has no action assigned!";
}

void Button::setPopOnRun(bool popOnRun) {
	this->popOnRun = popOnRun;
}

bool Button::isPopOnRun() const {
	return popOnRun;
}

static void drawToScreen(const Button& b) {
	if (b.marked) {
		globalData.spriteHolder->GetSprite(menu_marked).Draw(globalData.screen, SDL_GetTicks(), b.x, b.y);
	}
	else {
		globalData.spriteHolder->GetSprite(menu_unmarked).Draw(globalData.screen, SDL_GetTicks(), b.x, b.y);
	}

	standardButton.getLabel(b.label)->Draw(globalData.screen, b.x+standardButton.xsize/2,b.y+standardButton.ysize/2,
	                                       sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::center);
}


static bool isClicked(const Button& b, int x,int y) {
	if ( x >= b.x && y >= b.y && x<= b.x+standardButton.xsize && y <= b.y + standardButton.ysize) {
		return true;
	}
	return false;
}

void Menu::drawSelf(SDL_Renderer* target) {
	DrawBackground(target);
	for (const Button* b : buttons) {
		drawToScreen(*b);
	}
	drawToScreen(exit);
	standardButton.getLabel(title)->Draw(target, 50, 50);
}


void Menu::placeButtons() {
	int nextY = 100;
	int X = 50;
	for (Button* it : buttons) {
		X = (globalData.xsize - standardButton.xsize)/2;
		it->x = X;
		it->y = nextY;
		nextY += standardButton.ysize+10;
	}
	exit.x = X;
	exit.y = nextY;
}

void Menu::addButton(Button* b) {
	buttons.push_back(b);
	b->marked = false;
	placeButtons();
}

Menu::Menu(SDL_Renderer* screen) : buttons{std::vector<Button*>(10)} {
	this->screen = screen;
	isSubmenu = true;
	exit.setLabel( _("Back") );
}

Menu::Menu(SDL_Renderer* screen,bool submenu) : buttons{std::vector<Button*>(0)} {
	this->screen = screen;
	isSubmenu = submenu;
	if (isSubmenu) {
		exit.setLabel( _("Back") );
	}
	else {
		exit.setLabel( _("Exit") );
	}
}

Menu::Menu(SDL_Renderer* screen, const std::string& title, bool submenu) : title{title} {
	this->screen = screen;
	buttons = std::vector<Button*>(0);
	isSubmenu = submenu;
	if (isSubmenu) {
		exit.setLabel(_("Back") );
	}
	else {
		exit.setLabel(_("Exit") );
	}
}

bool isUpEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_UP) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value < -deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isDownEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_DOWN) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value > deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isLeftEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value < -deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isRightEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_RIGHT) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value > deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isEscapeEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if ( event.key.keysym.sym == SDLK_ESCAPE ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_Y || event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK ) {
			return true;
		}
	}
	return false;
}

bool isConfirmEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A || event.cbutton.button == SDL_CONTROLLER_BUTTON_B ) {
			return true;
		}
	}
	return false;
}


bool Menu::IsActive() {
	return running;
}
void Menu::Draw(SDL_Renderer* target) {
	placeButtons();
	drawSelf(target);
#if DEBUG
	static unsigned long int Frames;
	static unsigned long int Ticks;
	static char FPS[10];
	static sago::SagoTextField fpsField;
	sagoTextSetBlueFont(fpsField);
	Frames++;
	if (SDL_GetTicks() >= Ticks + 1000) {
		if (Frames > 999) {
			Frames=999;
		}
		snprintf(FPS, sizeof(FPS), "%lu fps", Frames);
		Frames = 0;
		Ticks = SDL_GetTicks();
	}
	fpsField.SetText(FPS);
	fpsField.Draw(globalData.screen, 800, 4);
#endif
}
void Menu::ProcessInput(const SDL_Event& event, bool& processed) {
	if (isGameControllerConnectionEvent(event)) {
		UnInitGameControllers();
		InitGameControllers();
		processed = true;
	}

	if (isUpEvent(event)) {
		marked--;
		if (marked<0) {
			marked = buttons.size();    //not -1, since exit is after the last element in the list
		}
		processed = true;
	}

	if (isDownEvent(event)) {
		marked++;
		if (marked> (int)buttons.size()) {
			marked = 0;
		}
		processed = true;
	}

	if (isEscapeEvent(event) && isSubmenu) {
		running = false;
		processed = true;
	}

	if (isConfirmEvent(event)) {
		bMouseUp = false;
		if (marked < (int)buttons.size()) {
			buttons.at(marked)->doAction();
			if (buttons.at(marked)->isPopOnRun()) {
				running = false;
			}
		}
		if (marked == (int)buttons.size()) {
			running = false;
		}
		processed = true;
	}
}

void Menu::Update() {
	for (int i=0; i<(int)buttons.size(); i++) {
		buttons.at(i)->marked = (i == marked);
	}
	exit.marked = (marked == (int)buttons.size());
	Uint8 buttonState = SDL_GetMouseState(nullptr,nullptr);
	// If the mouse button is released, make bMouseUp equal true
	if ( (buttonState&SDL_BUTTON(1))==0) {
		bMouseUp=true;
	}

	if (abs(globalData.mousex-oldmousex)>5 || abs(globalData.mousey-oldmousey)>5) {
		for (int i=0; i< (int)buttons.size(); ++i) {
			if (isClicked(*buttons.at(i), globalData.mousex, globalData.mousey)) {
				marked = i;
			}
		}
		if (isClicked(exit, globalData.mousex, globalData.mousey)) {
			marked = buttons.size();
		}
		oldmousex = globalData.mousex;
		oldmousey = globalData.mousey;
	}

	//mouse clicked
	if ( (buttonState&SDL_BUTTON(1) )==SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;
		for (int i=0; i< (int)buttons.size(); ++i) {
			if (isClicked(*buttons.at(i), globalData.mousex, globalData.mousey)) {
				buttons.at(i)->doAction();
				if (buttons.at(i)->isPopOnRun()) {
					running = false;
				}
				//Quit here to ensure that we do not continue checking buttons after we have done the action.
				return;
			}
		}
		if (isClicked(exit,  globalData.mousex, globalData.mousey)) {
			running = false;
		}
	}
}

