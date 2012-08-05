/* 
 * File:   global.hpp
 * Author: poul
 *
 * Created on 5. juni 2011, 18:26
 */

#ifndef _GLOBAL_HPP
#define	_GLOBAL_HPP

#include "CppSdl/CppSdlImageHolder.hpp"
#include "Libs/NFont.h"

void MainMenu();
void ConfigureMenu();
void ResetFullscreen();

extern CppSdl::CppSdlImageHolder menuMarked;
extern CppSdl::CppSdlImageHolder menuUnmarked;
extern NFont nf_scoreboard_font;
extern bool MusicEnabled;			//true if background music is enabled
extern bool SoundEnabled;			//true if sound effects is enabled
extern bool bFullscreen;			//true if game is running fullscreen
extern char player1name[30];
extern char player2name[30];
extern SDL_Surface *screen;        //The whole screen;

#endif	/* _GLOBAL_HPP */

