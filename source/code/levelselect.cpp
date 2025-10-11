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
https://blockattack.net
===========================================================================
*/

#include "levelselect.hpp"
#include "SDL.h"
#include "common.h"
#include "global.hpp"
#include "puzzlehandler.hpp"
#include "stageclearhandler.hpp"
#include "MenuSystem.h"
#include <fmt/core.h>


static bool bMouseUp;              //true if the mouse(1) is unpressed

static std::map<std::string, std::shared_ptr<sago::SagoTextField> > fieldCache;

static sago::SagoTextField* getCachedText(const std::string& text) {
	std::shared_ptr<sago::SagoTextField> ptr = fieldCache[text];
	if (!ptr) {
		std::shared_ptr<sago::SagoTextField> newText = std::make_shared<sago::SagoTextField>();
		sagoTextSetBlueFont(*newText.get());
		newText->SetText(text);
		fieldCache[text] = newText;
	}
	return fieldCache[text].get();
}

static void Write(SDL_Renderer* target, int x, int y, const char* text) {
	getCachedText(text)->Draw(target, x, y, sago::SagoTextField::Alignment::left, sago::SagoTextField::VerticalAlignment::top, &globalData.logicalResize);
}

//The function that allows the player to choose PuzzleLevel
int PuzzleLevelSelect(int Type) {
	const int xplace = 200;
	const int yplace = 300;
	int levelNr = 0;
	int oldmousex = 0;
	int oldmousey = 0;
	bool levelSelected = false;
	int nrOfLevels = 0;
	Uint32 totalScore = 0;
	Uint32 totalTime = 0;
	int selected = 0;
	fieldCache.clear();

	//Loads the levels, if they havn't been loaded:
	if (Type == 0) {
		LoadPuzzleStages();
		nrOfLevels = PuzzleGetNumberOfPuzzles();
	}
	if (Type == 1) {
		LoadStageClearStages();
		totalScore = GetTotalScore();
		totalTime = GetTotalTime();
		nrOfLevels = GetNrOfLevels();
	}

	while (!levelSelected) {
		SDL_Delay(1);
		auto ticks = SDL_GetTicks();
		DrawBackground(globalData.screen);
		int mousex;
		int mousey;
		globalData.logicalResize.PhysicalToLogical(globalData.mousex, globalData.mousey, mousex, mousey);
		globalData.iCheckBoxArea.Draw(globalData.screen,ticks,xplace,yplace, &globalData.logicalResize);
		if (Type == 0) {
			Write(globalData.screen, xplace+12,yplace+2,_("Select Puzzle") );
		}
		if (Type == 1) {
			Write(globalData.screen, xplace+12,yplace+2, _("Stage Clear Level Select") );
		}
		//Now drow the fields you click in (and a V if clicked):
		for (int i = 0; i < nrOfLevels; i++) {
			globalData.iLevelCheckBox.Draw(globalData.screen, ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50, &globalData.logicalResize);
			if (i==selected) {
				globalData.iLevelCheckBoxMarked.Draw(globalData.screen, ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50, &globalData.logicalResize);
			}
			if (Type == 0 && PuzzleIsCleared(i)) {
				globalData.iLevelCheck.Draw(globalData.screen,ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50, &globalData.logicalResize);
			}
			if (Type == 1) {
				if (GetStageScores(i) >= GetStageParScore(i)) {
					globalData.yellow_star.Draw(globalData.screen, ticks, xplace+10+(i%10)*50+2, yplace+60+(i/10)*50+2, &globalData.logicalResize);
				}
				if (IsStageCleared(i)) {
					globalData.iLevelCheck.Draw(globalData.screen, ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50, &globalData.logicalResize);
				}
			}
		}

		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);
			globalData.logicalResize.PhysicalToLogical(globalData.mousex, globalData.mousey, mousex, mousey);

			if ( event.type == SDL_QUIT ) {
				Config::getInstance()->setShuttingDown(5);
				levelNr = -1;
				levelSelected = true;
			}
			if (isEscapeEvent(event)) {
				levelNr = -1;
				levelSelected = true;
			}
			if (isConfirmEvent(event)) {
				levelNr = selected;
				levelSelected = true;
			}
			if (isRightEvent(event)) {
				++selected;
				if (selected >= nrOfLevels) {
					selected = 0;
				}
			}
			if (isLeftEvent(event)) {
				--selected;
				if (selected < 0) {
					selected = nrOfLevels-1;
				}
			}
			if (isDownEvent(event)) {
				selected+=10;
				if (selected >= nrOfLevels) {
					selected-=10;
				}
			}
			if (isUpEvent(event)) {
				selected-=10;
				if (selected < 0) {
					selected+=10;
				}
			}
		}

		SDL_GetKeyboardState(nullptr);

		if (mousex != oldmousex || mousey != oldmousey) {
			int tmpSelected = -1;
			int j;
			for (j = 0; (tmpSelected == -1) && ( (j<nrOfLevels/10)||((j<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)) ); j++) {
				if ((60+j*50<mousey-yplace)&&(mousey-yplace<j*50+92)) {
					tmpSelected = j*10;
				}
			}
			if (tmpSelected != -1) {
				for (int k = 0; (( (!(nrOfLevels%10) || k<nrOfLevels-10*(j-1)) )&&(k<10)); k++) {
					if ((10+k*50<mousex-xplace)&&(mousex-xplace<k*50+42)) {
						tmpSelected +=k;
						selected = tmpSelected;
					}
				}
			}
		}
		oldmousey = mousey;
		oldmousex= mousex;

		// If the mouse button is released, make bMouseUp equal true
		if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
			bMouseUp=true;
		}

		if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
			bMouseUp = false;

			int levelClicked = -1;
			int i;
			for (i = 0; (i<nrOfLevels/10)||((i<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)); i++)
				if ((60+i*50<mousey-yplace)&&(mousey-yplace<i*50+92)) {
					levelClicked = i*10;
				}
			i++;
			if (levelClicked != -1)
				for (int j = 0; ((j<nrOfStageLevels%(i*10))&&(j<10)); j++)
					if ((10+j*50<mousex-xplace)&&(mousex-xplace<j*50+42)) {
						levelClicked +=j;
						levelSelected = true;
						levelNr = levelClicked;
					}
		}

		if (Type == 1) {
			std::string scoreString = fmt::format(_("Best score: {}"), GetStageScores(selected)) ;
			std::string timeString = fmt::format(_("Time used: {}"),"-- : --");
			std::string parScoreString = fmt::format(_("Par score: {}"), GetStageParScore(selected));

			if (GetStageTime(selected)>0) {
				timeString = fmt::format(_("Time used: {} : {:02}"), GetStageTime(selected)/1000/60, (GetStageTime(selected)/1000)%60);
			}

			Write(globalData.screen, 200,200,scoreString.c_str());
			Write(globalData.screen,  500, 200, parScoreString.c_str());
			Write(globalData.screen, 200,250,timeString.c_str());
			std::string totalString = fmt::format(_("Total score: {} in {}:{:02}"), totalScore, totalTime/1000/60, ((totalTime/1000)%60) );
			Write(globalData.screen, 200,600,totalString.c_str());
		}

		globalData.mouse.Draw(globalData.screen, SDL_GetTicks(), globalData.mousex, globalData.mousey);
		SDL_RenderPresent(globalData.screen); //draws it all to the screen

	}
	DrawBackground(globalData.screen);
	return levelNr;
}
