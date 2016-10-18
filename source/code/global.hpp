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
http://www.blockattack.net
===========================================================================
*/

#ifndef _GLOBAL_HPP
#define	_GLOBAL_HPP

#include "Libs/NFont.h"
#include <memory>
#include "sago/SagoSpriteHolder.hpp"
#include "highscore.h"
#include "sago/GameStateInterface.hpp"

void MainMenu();
void ResetFullscreen();
void RunGameState(sago::GameStateInterface& state );

enum class Gametype { SinglePlayerEndless=0, SinglePlayerTimeTrial=1, StageClear=2, Puzzle=3, SinglePlayerVs=4, TwoPlayerTimeTrial=10, TwoPlayerVs=11  };

int runGame(Gametype gametype,int level);
bool OpenDialogbox(int x, int y, std::string& name, const std::string& header);
void DrawBackground(SDL_Renderer* target);
void UpdateMouseCoordinates(const SDL_Event& event, int& mousex, int& mousey);

extern sago::SagoSprite  menuMarked;
extern sago::SagoSprite  menuUnmarked;
extern sago::SagoSprite bHighScore;
extern sago::SagoSprite bBack;
extern sago::SagoSprite bNext;
extern NFont nf_scoreboard_font;
extern NFont nf_standard_blue_font;
extern NFont nf_button_font;
extern bool MusicEnabled;			//true if background music is enabled
extern bool SoundEnabled;			//true if sound effects is enabled
extern bool bFullscreen;			//true if game is running fullscreen
extern std::string player1name;
extern std::string player2name;
extern SDL_Renderer *screen;        //The whole screen;
extern Mix_Chunk *typingChunk;
extern sago::SagoSprite mouse;
extern bool highPriority;
extern bool NoSound;	
extern int verboseLevel;
extern Highscore theTopScoresEndless;      //Stores highscores for endless
extern Highscore theTopScoresTimeTrial;    //Stores highscores for timetrial
extern std::unique_ptr<sago::SagoSpriteHolder> spriteHolder;

//The xsize and ysize are updated everytime the background is drawn
extern int xsize;
extern int ysize;

#endif	/* _GLOBAL_HPP */

