/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2017 Poul Sander

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

#ifndef _GLOBAL_HPP
#define	_GLOBAL_HPP

#include <memory>
#include "sago/SagoSpriteHolder.hpp"
#include "highscore.h"
#include "sago/GameStateInterface.hpp"
#include "TextManager.hpp"
#include "ExplosionManager.hpp"
#include "sago/SagoTextField.hpp"
#include "ModInfo.hpp"
#include "themes.hpp"

//if SHAREDIR is not used we look in current directory
#ifndef SHAREDIR
#define SHAREDIR "."
#endif

#ifndef LOCALEDIR
#define LOCALEDIR SHAREDIR"/locale"
#endif

#ifndef PACKAGE
#define PACKAGE "blockattack_roftb"
#endif

#define MODLIST_TXT "mod_list.txt"

void MainMenu();
void SafeModeMenu();
void ResetFullscreen();
void RunGameState(sago::GameStateInterface& state );

enum class Gametype { SinglePlayerEndless=0, SinglePlayerTimeTrial=1, StageClear=2, Puzzle=3, SinglePlayerVs=4, TwoPlayerTimeTrial=10, TwoPlayerVs=11, Replay=100  };

int runGame(Gametype gametype,int level);
bool OpenDialogbox(int x, int y, std::string& name, const std::string& header);
void DrawBackground(SDL_Renderer* target);
void UpdateMouseCoordinates(const SDL_Event& event, int& mousex, int& mousey);
void DrawIMG(const sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y);
void DrawIMG_Bounded(const sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y, int minx, int miny, int maxx, int maxy);

void sagoTextSetHelpFont(sago::SagoTextField& field);
void sagoTextSetBlueFont(sago::SagoTextField& field);

const int SIXTEEN_NINE_WIDTH = 1364;
const int FOUR_THREE_WIDTH = 1024;
const int SCREEN_HIGHT = 768;

const int BOARD_WIDTH = 300;
const int BOARD_HEIGHT = 600;


struct GlobalData {
	sago::SagoSprite bHighScore;
	sago::SagoSprite bBack;
	sago::SagoSprite bNext;
	sago::SagoSprite iLevelCheck;  //To the level select screen
	sago::SagoSprite iLevelCheckBox;
	sago::SagoSprite iLevelCheckBoxMarked;
	sago::SagoSprite iCheckBoxArea;
	bool MusicEnabled;  //true if background music is enabled
	bool SoundEnabled;  //true if sound effects is enabled
	bool bFullscreen;  //true if game is running fullscreen
	std::string replayArgument;  //Name of the replay to play (if given as a commandline argument)
	std::string player1name;
	std::string player2name;
	SDL_Renderer *screen = nullptr;  //The whole screen;
	sago::SoundHandler typingChunk;
	sago::SagoSprite mouse;
	bool highPriority = false;
	bool NoSound = false;
	bool alwaysSixteenNine = false;
	int verboseLevel = 0;
	std::unique_ptr<sago::SagoSpriteHolder> spriteHolder;
	std::vector<std::string> modList;
	ModInfo modinfo;
	Theme theme;

	TextManager theTextManager;

	int xsize = FOUR_THREE_WIDTH;
	int ysize = SCREEN_HIGHT;
	int mousex = 0;
	int mousey = 0;
};

#include "BallManager.hpp"

extern GlobalData globalData;

#endif	/* _GLOBAL_HPP */

