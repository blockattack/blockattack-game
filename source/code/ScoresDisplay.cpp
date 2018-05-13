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

#include "ScoresDisplay.hpp"
#include "global.hpp"
#include "common.h"
#include "stats.h"
#include "MenuSystem.h"

using std::string;
using std::cerr;
using std::vector;

static void setButtonFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(24);
	field.SetOutline(1, {128,128,128,255});
	field.SetText(text);
}

sago::SagoTextField* ScoresDisplay::getCachedText(const std::string& text) {
	std::shared_ptr<sago::SagoTextField> ptr = fieldCache[text];
	if (!ptr) {
		std::shared_ptr<sago::SagoTextField> newText = std::make_shared<sago::SagoTextField>();
		sagoTextSetBlueFont(*newText.get());
		newText->SetText(text);
		fieldCache[text] = newText;
	}
	return fieldCache[text].get();
}

void ScoresDisplay::Write(SDL_Renderer* target, int x, int y, const char* text) {
	getCachedText(text)->Draw(target, x, y);
}

const int numberOfPages = 3;

void ScoresDisplay::DrawBackgroundAndCalcPlacements() {
	DrawBackground(globalData.screen);
	nextX = globalData.xsize-buttonXsize-20;
	backY = globalData.ysize-buttonYsize-20;
	nextY = backY;
}

//Draws the highscores
void ScoresDisplay::DrawHighscores(int x, int y, bool endless) {
	DrawBackgroundAndCalcPlacements();
	if (endless) {
		Write(globalData.screen, x+100,y+100, _("Endless:") );
	}
	else {
		Write(globalData.screen, x+100,y+100, _("Time Trial:") );
	}
	for (int i =0; i<10; i++) {
		record r;
		if (endless) {
			r = globalData.theTopScoresEndless.getScoreNumber(i);
		}
		else {
			r = globalData.theTopScoresTimeTrial.getScoreNumber(i);
		}
		char playerScore[32];
		char playerName[32];
		snprintf(playerScore, sizeof(playerScore), "%i", r.score);
		snprintf(playerName, sizeof(playerName), "%s", r.name.c_str());
		Write(globalData.screen, x+420,y+150+i*35, playerScore);
		Write(globalData.screen, x+60,y+150+i*35, playerName);
	}
}

void ScoresDisplay::DrawStats() {
	DrawBackgroundAndCalcPlacements();
	int y = 5;
	const int y_spacing = 30;
	Write(globalData.screen, 10,y,_("Stats") );
	y+=y_spacing*2;
	Write(globalData.screen, 10,y,_("Chains") );
	for (int i=2; i<13; i++) {
		y+=y_spacing;
		Write(globalData.screen, 10,y,(std::to_string(i)+"X").c_str());
		string numberAsString = std::to_string(Stats::getInstance()->getNumberOf("chainX"+std::to_string(i)));
		Write(globalData.screen, 300,y,numberAsString.c_str());
	}
	y+=y_spacing*2;
	Write(globalData.screen, 10,y,_("Lines Pushed: ") );
	string numberAsString = std::to_string(Stats::getInstance()->getNumberOf("linesPushed"));
	Write(globalData.screen, 300,y,numberAsString.c_str());

	y+=y_spacing;
	Write(globalData.screen, 10,y, _("Puzzles solved: ") );
	numberAsString = std::to_string(Stats::getInstance()->getNumberOf("puzzlesSolved"));
	Write(globalData.screen, 300,y,numberAsString.c_str());

	y+=y_spacing*2;
	Write(globalData.screen, 10,y, _("Run time: ") );
	commonTime ct = TimeHandler::peekTime("totalTime",TimeHandler::ms2ct(SDL_GetTicks()));
	y+=y_spacing;
	Write(globalData.screen, 10, y, SPrintCF( _("Days: %i"), ct.days) );
	y+=y_spacing;
	Write(globalData.screen, 10, y, SPrintCF( _("Hours: %i"), ct.hours) );
	y+=y_spacing;
	Write(globalData.screen, 10, y, SPrintCF( _("Minutes: %i"), ct.minutes) );
	y+=y_spacing;
	Write(globalData.screen, 10, y, SPrintCF( _("Seconds: %i"), ct.seconds) );

	y-=y_spacing*4; //Four rows back
	const int x_offset3 = globalData.xsize/3+10; //Ofset for three rows
	Write(globalData.screen, x_offset3,y, _("Play time: ") );
	ct = TimeHandler::getTime("playTime");
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, SPrintCF( _("Days: %i"), ct.days) );
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, SPrintCF( _("Hours: %i"), ct.hours) );
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, SPrintCF( _("Minutes: %i"), ct.minutes) );
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, SPrintCF( _("Seconds: %i"), ct.seconds) );

	const int x_offset = globalData.xsize/2+10;
	y = 5+y_spacing*2;
	Write(globalData.screen, x_offset,y, _("VS CPU (win/loss)") );
	for (int i=0; i<7; i++) {
		y += y_spacing;
		Write(globalData.screen, x_offset,y,string("AI "+std::to_string(i+1)).c_str());
		numberAsString = std::to_string(Stats::getInstance()->getNumberOf("defeatedAI"+std::to_string(i)));
		string numberAsString2 = std::to_string(Stats::getInstance()->getNumberOf("defeatedByAI"+std::to_string(i)));
		string toPrint = numberAsString + "/" + numberAsString2;
		Write(globalData.screen, x_offset+230,y,toPrint.c_str());
	}
}

ScoresDisplay::ScoresDisplay() {
}

ScoresDisplay::~ScoresDisplay() {
}

bool ScoresDisplay::IsActive() {
	return isActive;
}

void ScoresDisplay::Draw(SDL_Renderer*) {
	switch (page) {
	case 0:
		//Highscores, endless
		DrawHighscores(100,100,true);
		break;
	case 1:
		//Highscores, Time Trial
		DrawHighscores(100,100,false);
		break;
	case 2:
	default:
		DrawStats();
	};

	const sago::SagoDataHolder* holder = &globalData.spriteHolder->GetDataHolder();
	//Draw buttons:
	globalData.bHighScore.Draw(globalData.screen, 0, scoreX,scoreY);
	globalData.bBack.Draw(globalData.screen, 0, backX, backY);
	static sago::SagoTextField backLabel;
	setButtonFont(holder, backLabel, _("Back"));
	backLabel.Draw(globalData.screen, backX+60,backY+10, sago::SagoTextField::Alignment::center);
	globalData.bNext.Draw(globalData.screen, 0, nextX, nextY);
	static sago::SagoTextField nextLabel;
	setButtonFont(holder, nextLabel, _("Next"));
	nextLabel.Draw(globalData.screen, nextX+60, nextY+10, sago::SagoTextField::Alignment::center);

	//Draw page number
	string pageXofY = SPrintStringF(_("Page %i of %i"), page+1, numberOfPages);
	getCachedText(pageXofY)->Draw(globalData.screen,  globalData.xsize/2, globalData.ysize-60, sago::SagoTextField::Alignment::center);
}

void ScoresDisplay::ProcessInput(const SDL_Event& event, bool& processed) {

	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isLeftEvent(event)) {
		page++;
		if (page>=numberOfPages) {
			page = 0;
		}
		processed = true;
	}

	if (isRightEvent(event)) {
		page--;
		if (page<0) {
			page = numberOfPages-1;
		}
		processed = true;
	}

	if (isConfirmEvent(event) || isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void ScoresDisplay::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;

		//The Score button:
		if ((globalData.mousex>scoreX) && (globalData.mousex<scoreX+buttonXsize) && (globalData.mousey>scoreY) && (globalData.mousey<scoreY+buttonYsize)) {
			isActive = false;
		}

		//The back button:
		if ((globalData.mousex>backX) && (globalData.mousex<backX+buttonXsize) && (globalData.mousey>backY) && (globalData.mousey<backY+buttonYsize)) {
			page--;
			if (page<0) {
				page = numberOfPages-1;
			}
		}

		//The next button:
		if ((globalData.mousex>nextX) && (globalData.mousex<nextX+buttonXsize) && (globalData.mousey>nextY) && (globalData.mousey<nextY+buttonYsize)) {
			page++;
			if (page>=numberOfPages) {
				page = 0;
			}
		}
	}
}