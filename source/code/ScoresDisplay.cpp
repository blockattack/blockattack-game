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
#include <fmt/core.h>


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
	getCachedText(text)->Draw(target, x, y, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
}


void ScoresDisplay::Write(SDL_Renderer* target, int x, int y, const std::string& text) {
	Write(target, x, y, text.c_str());
}

const int numberOfPages = 13;

void ScoresDisplay::DrawBackgroundAndCalcPlacements() {
	DrawBackground(globalData.screen);
	nextX = globalData.xsize-buttonXsize-20;
	backY = globalData.ysize-buttonYsize-18;
	nextY = backY;
}

//Draws the highscores
void ScoresDisplay::DrawHighscores(int x, int y, bool endless, int level = 0) {
	DrawBackgroundAndCalcPlacements();
	if (endless) {
		std::string header;
		switch (level) {
		case 1:
			header = _("Endless (Fast):");
			break;
		case 2:
			header = _("Endless (Faster):");
			break;
		case 3:
			header = _("Endless (Even faster):");
			break;
		case 4:
			header = _("Endless (Fastest):");
			break;
		case 5:
			header = _("Endless (5 blocks):");
			break;
		case 6:
			header = _("Endless (5 blocks, Fast):");
			break;
		case 7:
			header = _("Endless (5 blocks, Faster):");
			break;
		case 8:
			header = _("Endless (5 blocks, Even faster):");
			break;
		case 9:
			header = _("Endless (5 blocks, Fastest):");
			break;
		default:
			header = _("Endless:");
		};
		Write(globalData.screen, x+100,y+100, header );
	}
	else {
		std::string header = _("Time Trial:");
		if (level == 1) {
			header = _("Time Trial (5 blocks):");
		}
		Write(globalData.screen, x+100,y+100, header );
	}
	for (int i =0; i<10; i++) {
		record r;
		if (endless) {
			switch (level) {
			case 1:
				r = theTopScoresEndless1.getScoreNumber(i);
				break;
			case 2:
				r = theTopScoresEndless2.getScoreNumber(i);
				break;
			case 3:
				r = theTopScoresEndless3.getScoreNumber(i);
				break;
			case 4:
				r = theTopScoresEndless4.getScoreNumber(i);
				break;
			case 5:
				r = theTopScoresEndless0_5.getScoreNumber(i);
				break;
			case 6:
				r = theTopScoresEndless1_5.getScoreNumber(i);
				break;
			case 7:
				r = theTopScoresEndless2_5.getScoreNumber(i);
				break;
			case 8:
				r = theTopScoresEndless3_5.getScoreNumber(i);
				break;
			case 9:
				r = theTopScoresEndless4_5.getScoreNumber(i);
				break;
			default:
				r = theTopScoresEndless0.getScoreNumber(i);
			}
		}
		else {
			if (level == 1) {
				r = theTopScoresTimeTrial_5.getScoreNumber(i);
			}
			else {
				r = theTopScoresTimeTrial.getScoreNumber(i);
			}
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
		Write(globalData.screen, 10,y,std::to_string(i)+"X");
		std::string numberAsString = std::to_string(Stats::getInstance()->getNumberOf("chainX"+std::to_string(i)));
		Write(globalData.screen, 300,y,numberAsString);
	}
	y+=y_spacing*2;
	Write(globalData.screen, 10,y,_("Lines Pushed: ") );
	std::string numberAsString = std::to_string(Stats::getInstance()->getNumberOf("linesPushed"));
	Write(globalData.screen, 300,y,numberAsString);

	y+=y_spacing;
	Write(globalData.screen, 10,y, _("Puzzles solved: ") );
	numberAsString = std::to_string(Stats::getInstance()->getNumberOf("puzzlesSolved"));
	Write(globalData.screen, 300,y,numberAsString);

	y+=y_spacing*2;
	Write(globalData.screen, 10,y, _("Run time: ") );
	commonTime ct = TimeHandler::peekTime("totalTime",TimeHandler::ms2ct(SDL_GetTicks()));
	y+=y_spacing;
	Write(globalData.screen, 10, y, fmt::format( _("Days: {}"), ct.days) );
	y+=y_spacing;
	Write(globalData.screen, 10, y, fmt::format( _("Hours: {:02}"), ct.hours) );
	y+=y_spacing;
	Write(globalData.screen, 10, y, fmt::format( _("Minutes: {:02}"), ct.minutes) );
	y+=y_spacing;
	Write(globalData.screen, 10, y, fmt::format( _("Seconds: {:02}"), ct.seconds) );

	y-=y_spacing*4; //Four rows back
	const int x_offset3 = globalData.xsize/3+10; //Ofset for three rows
	Write(globalData.screen, x_offset3,y, _("Play time: ") );
	ct = TimeHandler::getTime("playTime");
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, fmt::format( _("Days: {}"), ct.days) );
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, fmt::format( _("Hours: {:02}"), ct.hours) );
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, fmt::format( _("Minutes: {:02}"), ct.minutes) );
	y+=y_spacing;
	Write(globalData.screen, x_offset3, y, fmt::format( _("Seconds: {:02}"), ct.seconds) );

	const int x_offset = globalData.xsize/2+10;
	y = 5+y_spacing*2;
	Write(globalData.screen, x_offset,y, _("VS CPU (win/loss)") );
	for (int i=0; i<7; i++) {
		y += y_spacing;
		Write(globalData.screen, x_offset,y, fmt::format("AI {}",i+1) );
		numberAsString = std::to_string(Stats::getInstance()->getNumberOf("defeatedAI"+std::to_string(i)));
		std::string numberAsString2 = std::to_string(Stats::getInstance()->getNumberOf("defeatedByAI"+std::to_string(i)));
		std::string toPrint = numberAsString + "/" + numberAsString2;
		Write(globalData.screen, x_offset+230,y,toPrint);
	}
}

ScoresDisplay::ScoresDisplay() {
}

ScoresDisplay::~ScoresDisplay() {
}

void ScoresDisplay::Draw(SDL_Renderer* target) {
	switch (page) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		//Highscores, endless
		DrawHighscores(100,100,true, page);
		break;
	case 5:
		//Highscores, Time Trial
		DrawHighscores(100,100,false);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		//Highscores, endless 5 blocks
		DrawHighscores(100,100,true, page-1);
		break;
	case 11:
		//Highscores, Time Trial 5 blocks
		DrawHighscores(100,100,false, 1);
		break;
	case 12:
	default:
		DrawStats();
	};

	const sago::SagoDataHolder* holder = &globalData.spriteHolder->GetDataHolder();
	//Draw buttons:
	globalData.bBack.Draw(globalData.screen, 0, backX, backY, &globalData.logicalResize);
	static sago::SagoTextField backLabel;
	setButtonFont(holder, backLabel, _("Back"));
	backLabel.Draw(globalData.screen, backX+60,backY+10, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
	globalData.bNext.Draw(globalData.screen, 0, nextX, nextY, &globalData.logicalResize);
	static sago::SagoTextField nextLabel;
	setButtonFont(holder, nextLabel, _("Next"));
	nextLabel.Draw(globalData.screen, nextX+60, nextY+10, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);

	//Draw page number
	std::string pageXofY = fmt::format(_("Page {} of {}"), page+1, numberOfPages);
	getCachedText(pageXofY)->Draw(globalData.screen,  globalData.xsize/2, globalData.ysize-60, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
	HelpCommonState::Draw(target);
}

void ScoresDisplay::ProcessInput(const SDL_Event& event, bool& processed) {

	UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

	if (isRightEvent(event)) {
		page++;
		if (page>=numberOfPages) {
			page = 0;
		}
		processed = true;
	}

	if (isLeftEvent(event)) {
		page--;
		if (page<0) {
			page = numberOfPages-1;
		}
		processed = true;
	}
	HelpCommonState::ProcessInput(event, processed);
}

void ScoresDisplay::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;
		int mousex;
		int mousey;
		globalData.logicalResize.PhysicalToLogical(globalData.mousex, globalData.mousey, mousex, mousey);

		//The back button:
		if ((mousex>backX) && (mousex<backX+buttonXsize) && (mousey>backY) && (mousey<backY+buttonYsize)) {
			page--;
			if (page<0) {
				page = numberOfPages-1;
			}
		}

		//The next button:
		if ((mousex>nextX) && (mousex<nextX+buttonXsize) && (mousey>nextY) && (mousey<nextY+buttonYsize)) {
			page++;
			if (page>=numberOfPages) {
				page = 0;
			}
		}
	}
	HelpCommonState::Update();
}
