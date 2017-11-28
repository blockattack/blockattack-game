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

#include "levelselect.hpp"
#include "SDL.h"
#include "common.h"
#include "global.hpp"
#include "puzzlehandler.hpp"
#include "stageclearhandler.hpp"
#include "MenuSystem.h"


using std::string;
using std::cerr;
using std::cout;
using std::exception;
using std::vector;

static bool bMouseUp;              //true if the mouse(1) is unpressed

static void NFont_Write(SDL_Renderer* target, int x, int y, const std::string& text) {
	globalData.standard_blue_font.draw(target, x, y, text);
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

	//Loads the levels, if they havn't been loaded:
	if (Type == 0) {
		LoadPuzzleStages();
	}
	if (Type == 0) {
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
		globalData.iCheckBoxArea.Draw(globalData.screen,ticks,xplace,yplace);
		if (Type == 0) {
			NFont_Write(globalData.screen, xplace+12,yplace+2,_("Select Puzzle") );
		}
		if (Type == 1) {
			NFont_Write(globalData.screen, xplace+12,yplace+2, _("Stage Clear Level Select") );
		}
		//Now drow the fields you click in (and a V if clicked):
		for (int i = 0; i < nrOfLevels; i++) {
			globalData.iLevelCheckBox.Draw(globalData.screen, ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			if (i==selected) {
				globalData.iLevelCheckBoxMarked.Draw(globalData.screen, ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			}
			if (Type == 0 && PuzzleIsCleared(i)) {
				globalData.iLevelCheck.Draw(globalData.screen,ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			}
			if (Type == 1 && IsStageCleared(i)) {
				globalData.iLevelCheck.Draw(globalData.screen, ticks, xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			}
		}

		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);

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

		if (globalData.mousex != oldmousex || globalData.mousey != oldmousey) {
			int tmpSelected = -1;
			int j;
			for (j = 0; (tmpSelected == -1) && ( (j<nrOfLevels/10)||((j<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)) ); j++) {
				if ((60+j*50<globalData.mousey-yplace)&&(globalData.mousey-yplace<j*50+92)) {
					tmpSelected = j*10;
				}
			}
			if (tmpSelected != -1) {
				for (int k = 0; (( (!(nrOfLevels%10) || k<nrOfLevels-10*(j-1)) )&&(k<10)); k++) {
					if ((10+k*50<globalData.mousex-xplace)&&(globalData.mousex-xplace<k*50+42)) {
						tmpSelected +=k;
						selected = tmpSelected;
					}
				}
			}
		}
		oldmousey = globalData.mousey;
		oldmousex= globalData.mousex;

		// If the mouse button is released, make bMouseUp equal true
		if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
			bMouseUp=true;
		}

		if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
			bMouseUp = false;

			int levelClicked = -1;
			int i;
			for (i = 0; (i<nrOfLevels/10)||((i<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)); i++)
				if ((60+i*50<globalData.mousey-yplace)&&(globalData.mousey-yplace<i*50+92)) {
					levelClicked = i*10;
				}
			i++;
			if (levelClicked != -1)
				for (int j = 0; ((j<nrOfStageLevels%(i*10))&&(j<10)); j++)
					if ((10+j*50<globalData.mousex-xplace)&&(globalData.mousex-xplace<j*50+42)) {
						levelClicked +=j;
						levelSelected = true;
						levelNr = levelClicked;
					}
		}

		if (Type == 1) {
			string scoreString = SPrintStringF(_("Best score: %i"), GetStageScores(selected)) ;
			string timeString = SPrintStringF(_("Time used: %s"),"-- : --");

			if (GetStageTime(selected)>0) {
				timeString = SPrintStringF(_("Time used: %d : %02d"), GetStageTime(selected)/1000/60, (GetStageTime(selected)/1000)%60);
			}

			NFont_Write(globalData.screen, 200,200,scoreString.c_str());
			NFont_Write(globalData.screen, 200,250,timeString.c_str());
			string totalString = SPrintStringF(_("Total score: %i in %i:%02i"), totalScore, totalTime/1000/60, ((totalTime/1000)%60) );
			NFont_Write(globalData.screen, 200,600,totalString.c_str());
		}

		globalData.mouse.Draw(globalData.screen, SDL_GetTicks(), globalData.mousex, globalData.mousey);
		SDL_RenderPresent(globalData.screen); //draws it all to the screen

	}
	DrawBackground(globalData.screen);
	return levelNr;
}