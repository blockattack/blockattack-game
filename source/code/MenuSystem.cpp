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


#include "BlockGame.hpp"
#include "MenuSystem.h"
#include "common.h"
#include "global.hpp"
#include "gamecontroller.h"
#include "menudef_themes.hpp"

static int oldmousex = 0;
static int oldmousey = 0;

void ButtonGfx::setSurfaces() {
	this->xsize = globalData.spriteHolder->GetSprite(menu_marked).GetWidth();
	this->ysize = globalData.spriteHolder->GetSprite(menu_marked).GetHeight();
	if (globalData.verboseLevel) {
		std::cout << "Surfaces set, size: " << this->xsize << " , " << this->ysize << "\n";
	}
}

sago::SagoTextField* ButtonGfx::getLabel(const std::string& text, bool marked) const {
	if (!marked) {
		const auto& theLabel = labels.find(text);
		if (theLabel != labels.end()) {
			return labels[text].get();
		}
	}
	else {
		const auto& theLabel = labels_marked.find(text);
		if (theLabel != labels_marked.end()) {
			return labels_marked[text].get();
		}
	}
	std::shared_ptr<sago::SagoTextField> newField = std::make_shared<sago::SagoTextField>();
	newField->SetHolder(&globalData.spriteHolder->GetDataHolder());
	newField->SetFont("freeserif");
	newField->SetFontSize(30);
	newField->SetOutline(1, {64,64,64,255});
	if (marked) {
		newField->SetOutline(3, {32,32,32,255});
	}
	newField->SetText(text);
	if (!marked) {
		labels[text] = newField;
		return labels[text].get();
	}
	labels_marked[text] = newField;
	return labels_marked[text].get();
}

Button::Button(const Button& b) : action{b.action}, popOnRun{b.popOnRun}, label{b.label}, marked{b.marked} {
}

Button& Button::operator=(const Button& other) {
	action = other.action;
	label = other.label;
	marked = other.marked;
	popOnRun = other.popOnRun;
	return *this;
}

void Button::setLabel(const std::string& text) {
	label = text;
	standardButton.setSurfaces();
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
		globalData.spriteHolder->GetSprite(b.standardButton.menu_marked).Draw(globalData.screen, SDL_GetTicks(), b.x, b.y, &globalData.logicalResize);
	}
	else {
		globalData.spriteHolder->GetSprite(b.standardButton.menu_unmarked).Draw(globalData.screen, SDL_GetTicks(), b.x, b.y, &globalData.logicalResize);
	}

	b.standardButton.getLabel(b.getLabel(), b.marked)->Draw(globalData.screen, b.x+b.standardButton.xsize/2,b.y+b.standardButton.ysize/2,
	        sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::center, &globalData.logicalResize);
}

void Button::drawToScreen(SDL_Renderer* target) const {
	::drawToScreen(*this);
}

void ButtonWithAdjustment::drawToScreen(SDL_Renderer* target) const {
	// Draw the main button
	if (marked) {
		globalData.spriteHolder->GetSprite(standardButton.menu_marked).Draw(globalData.screen, SDL_GetTicks(), x, y, &globalData.logicalResize);
	}
	else {
		globalData.spriteHolder->GetSprite(standardButton.menu_unmarked).Draw(globalData.screen, SDL_GetTicks(), x, y, &globalData.logicalResize);
	}

	standardButton.getLabel(getLabel(), marked)->Draw(globalData.screen, x+standardButton.xsize/2, y+standardButton.ysize/2,
	        sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::center, &globalData.logicalResize);

	// Draw left adjustment button (down arrow)
	int adjustSize = getAdjustButtonSize();
	int leftX = x - adjustSize - 10;
	int leftY = y;
	Uint32 currentTime = SDL_GetTicks();
	bool leftMarked = (currentTime - leftButtonMarkTime) < MARK_DURATION;
	if (leftMarked) {
		globalData.spriteHolder->GetSprite("menu_marked").DrawScaled(globalData.screen, SDL_GetTicks(), leftX, leftY, adjustSize, adjustSize, &globalData.logicalResize);
	}
	else {
		globalData.spriteHolder->GetSprite("menu_unmarked").DrawScaled(globalData.screen, SDL_GetTicks(), leftX, leftY, adjustSize, adjustSize, &globalData.logicalResize);
	}
	standardButton.getLabel("\u25c0", leftMarked)->Draw(globalData.screen, leftX+adjustSize/2, leftY+adjustSize/2,
	        sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::center, &globalData.logicalResize);

	// Draw right adjustment button (up arrow)
	int rightX = x + standardButton.xsize + 10;
	int rightY = y;
	bool rightMarked = (currentTime - rightButtonMarkTime) < MARK_DURATION;
	if (rightMarked) {
		globalData.spriteHolder->GetSprite("menu_marked").DrawScaled(globalData.screen, SDL_GetTicks(), rightX, rightY, adjustSize, adjustSize, &globalData.logicalResize);
	}
	else {
		globalData.spriteHolder->GetSprite("menu_unmarked").DrawScaled(globalData.screen, SDL_GetTicks(), rightX, rightY, adjustSize, adjustSize, &globalData.logicalResize);
	}
	standardButton.getLabel("\u25b6", rightMarked)->Draw(globalData.screen, rightX+adjustSize/2, rightY+adjustSize/2,
	        sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::center, &globalData.logicalResize);
}

bool ButtonWithAdjustment::isClickedAdjustLeft(int mx, int my) const {
	int adjustSize = getAdjustButtonSize();
	int leftX = x - adjustSize - 10;
	int leftY = y;
	if (mx >= leftX && my >= leftY && mx <= leftX + adjustSize && my <= leftY + adjustSize) {
		return true;
	}
	return false;
}

bool ButtonWithAdjustment::isClickedAdjustRight(int mx, int my) const {
	int adjustSize = getAdjustButtonSize();
	int rightX = x + standardButton.xsize + 10;
	int rightY = y;
	if (mx >= rightX && my >= rightY && mx <= rightX + adjustSize && my <= rightY + adjustSize) {
		return true;
	}
	return false;
}


static bool isClicked(const Button& b, int x,int y) {
	if ( x >= b.x && y >= b.y && x<= b.x+b.standardButton.xsize && y <= b.y + b.standardButton.ysize) {
		return true;
	}
	return false;
}

void Menu::drawSelf(SDL_Renderer* target) {
	DrawBackground(target);
	for (const Button* b : buttons) {
		b->drawToScreen(target);
	}
	exit.drawToScreen(target);
	exit.standardButton.getLabel(title, false)->Draw(target, 50, 50, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
}


void Menu::placeButtons() {
	int nextY = 100;
	int X = 50;
	for (Button* it : buttons) {
		// For all buttons, center based on the main button width to keep them aligned
		int centerX = (globalData.xsize - it->standardButton.xsize)/2;
		it->x = centerX;
		it->y = nextY;
		nextY += it->standardButton.ysize+10;
		X = it->x;
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
	buttons = std::vector<Button*>(10);
	isSubmenu = true;
	this->screen = screen;
	exit.setLabel( _("Back") );
}

Menu::Menu(SDL_Renderer* screen,bool submenu) {
	buttons = std::vector<Button*>(0);
	isSubmenu = submenu;
	this->screen = screen;
	if (isSubmenu) {
		exit.setLabel( _("Back") );
	}
	else {
		exit.setLabel( _("Exit") );
	}
}

Menu::Menu(SDL_Renderer* screen, const std::string& title, bool submenu) {
	buttons = std::vector<Button*>(0);
	isSubmenu = submenu;
	this->screen = screen;
	this->title = title;
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
	return GameControllerIsUpEvent(event);
}

bool isDownEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_DOWN) {
			return true;
		}
	}
	return GameControllerIsDownEvent(event);
}

bool isLeftEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			return true;
		}
	}
	return GameControllerIsLeftEvent(event);
}

bool isRightEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if (event.key.keysym.sym == SDLK_RIGHT) {
			return true;
		}
	}
	return GameControllerIsRightEvent(event);
}

bool isEscapeEvent(const SDL_Event& event) {
	if ( event.type == SDL_KEYDOWN ) {
		if ( event.key.keysym.sym == SDLK_ESCAPE ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B || event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK ) {
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
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A ) {
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
	fpsField.Draw(globalData.screen, globalData.xsize - 4, 4, sago::SagoTextField::Alignment::right, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
#endif
}
void Menu::ProcessInput(const SDL_Event& event, bool& processed) {
	if (GameControllerIsConnectionEvent(event)) {
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

	if (isLeftEvent(event)) {
		if (marked < (int)buttons.size()) {
			buttons.at(marked)->doLeft();
		}
		processed = true;
	}

	if (isRightEvent(event)) {
		if (marked < (int)buttons.size()) {
			buttons.at(marked)->doRight();
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
	int mousex = globalData.mousex;
	int mousey = globalData.mousey;
	globalData.logicalResize.PhysicalToLogical(globalData.mousex, globalData.mousey, mousex, mousey);

	if (abs(mousex-oldmousex)>5 || abs(mousey-oldmousey)>5) {
		for (int i=0; i< (int)buttons.size(); ++i) {
			if (isClicked(*buttons.at(i), mousex, mousey)) {
				marked = i;
			}
		}
		if (isClicked(exit, mousex, mousey)) {
			marked = buttons.size();
		}
		oldmousex = mousex;
		oldmousey = mousey;
	}

	//mouse clicked
	if ( (buttonState&SDL_BUTTON(1) )==SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;
		for (int i=0; i< (int)buttons.size(); ++i) {
			// Check for adjustment button clicks first
			if (buttons.at(i)->isClickedAdjustLeft(mousex, mousey)) {
				buttons.at(i)->doLeft();
				return;
			}
			if (buttons.at(i)->isClickedAdjustRight(mousex, mousey)) {
				buttons.at(i)->doRight();
				return;
			}
			// Then check for main button click
			if (isClicked(*buttons.at(i), mousex, mousey)) {
				buttons.at(i)->doAction();
				if (buttons.at(i)->isPopOnRun()) {
					running = false;
				}
				//Quit here to ensure that we do not continue checking buttons after we have done the action.
				return;
			}
		}
		if (isClicked(exit,  mousex, mousey)) {
			running = false;
		}
	}
}

