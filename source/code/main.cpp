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
http://blockattack.sf.net
===========================================================================
*/

#include "common.h"
#include "global.hpp"

#include <string.h>


#ifndef VERSION_NUMBER
#define VERSION_NUMBER "version 1.4.2 BETA"
#endif

//If DEBUG is defined: AI info and FPS will be written to screen
#ifndef DEBUG
#define DEBUG 0
#endif

//If NETWORK id defined: enet will be used
#ifndef NETWORK
#define NETWORK 1
#endif

//Build-in level editor is still experimental!
#ifndef LEVELEDITOR
#define LEVELEDITOR 0
#endif

#define WITH_SDL 1

//Macros to convert surfaces (ffor faster drawing)
#define CONVERT(n) tmp = SDL_DisplayFormat(n); SDL_FreeSurface(n); n = tmp
#define CONVERTA(n) tmp = SDL_DisplayFormatAlpha(n); SDL_FreeSurface(n); n = tmp

#include <iostream>
#include <stdlib.h>
#include <time.h>           //Used for srand()
#include <sstream>          //Still used by itoa2
#include <string>
#include "SDL.h"            //The SDL libary, used for most things
#include <SDL_mixer.h>      //Used for sound & music
#include <SDL_image.h>      //To load PNG images!
#include <physfs.h>         //Abstract file system. To use containers
#include "physfs_stream.hpp" //To use C++ style file streams
#include "Libs/NFont.h"
//#include "ttfont.h"        //To use True Type Fonts in SDL
//#include "config.h"
#include <vector>
#include <SDL/SDL_timer.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_ttf.h>
#include "CppSdlImageHolder.hpp"
#include "MenuSystem.h"

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

//enet things
#if NETWORK
#include "enet/enet.h"
#endif
//enet things end

//#include "SFont.h"          //Used to write on screen
#include "highscore.h"      //Stores highscores
#include "ReadKeyboard.h"   //Reads text from keyboard
#include "joypad.h"         //Used for joypads
#include "listFiles.h"	    //Used to show files on screen
#include "replay.h"			//Used for replays
#include "stats.h"          //Saves general stats 
#if LEVELEDITOR
#include "editor/editorMain.hpp" //The level editor
#endif
//#include "uploadReplay.h"   //Takes care of everything libcurl related

#include "common.h"

/*******************************************************************************
* All variables and constant has been moved to mainVars.hpp for the overview.  *
*******************************************************************************/
#include "mainVars.hpp"

static void MakeBackground(int,int);

void closeAllMenus()
{
	bNewGameOpen=false;          //show sub menues
	bOptionsOpen=false;          //Show OptionsMenu (Configure and Select Puzzle)
	b1playerOpen=false;			//show submenu
	b2playersOpen=false;
	bReplayOpen = false;
#if NETWORK
	bNetworkOpen = false;
#endif
	showOptions = false;

}

SDL_Surface * IMG_Load2(const char* path)
{
	if (!PHYSFS_exists(path))
	{
		cerr << "Error: File not in blockattack.data: " << path << endl;
		return NULL; //file doesn't exist
	}

	PHYSFS_file* myfile = PHYSFS_openRead(path);

	// Get the lenght of the file
	unsigned int m_size = PHYSFS_fileLength(myfile);

	// Get the file data.
	char *m_data = new char[m_size];

	int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

	if (length_read != (int)m_size)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return NULL;
	}

	PHYSFS_close(myfile);

// And this is how you load an image from a memory buffer with SDL
	SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

	//The above might fail an return null.
	if(!rw)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return NULL;
	}

	SDL_Surface* surface = IMG_Load_RW(rw,true); //the second argument tells the function to three RWops

	return surface;
}

CppSdl::CppSdlImageHolder IMG_Load3(string path)
{
	if (!PHYSFS_exists(path.c_str()))
	{
		cerr << "Error: File not in blockattack.data: " << path << endl;
		throw exception();
	}

	PHYSFS_file* myfile = PHYSFS_openRead(path.c_str());

	// Get the lenght of the file
	unsigned int m_size = PHYSFS_fileLength(myfile);

	// Get the file data.
	char *m_data = new char[m_size];

	int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

	if (length_read != (int)m_size)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		throw exception();
	}

	PHYSFS_close(myfile);

	CppSdl::CppSdlImageHolder surface(m_data, m_size);

	return surface;
}

static void UnloadImages();
static int InitImages();

static string oldThemePath = "default";
static bool loaded = false;

void loadTheme(const string &themeName)
{
	if(loaded)
		UnloadImages();
#if defined(__unix__)
	string home = (string)getenv("HOME")+(string)"/.gamesaves/blockattack";
#elif defined(_WIN32)
	string home = (string)getMyDocumentsPath()+(string)"/My Games/blockattack";
#endif
	//Remove old theme
	PHYSFS_removeFromSearchPath(oldThemePath.c_str());
	//Look in blockattack.data
	PHYSFS_addToSearchPath(((string)SHAREDIR+"/blockattack.data").c_str(), 1);
	//Look in folder
	PHYSFS_addToSearchPath(SHAREDIR, 1);
	//Look in home folder
#if defined(__unix__) || defined(_WIN32)
	PHYSFS_addToSearchPath(home.c_str(), 1);
#endif
	if(themeName.compare(Config::getInstance()->getString("themename"))!=0)
	{
		//If this is a theme different from the saved one. Remember it!
		Config::getInstance()->setString("themename",themeName);
	}
	if(themeName.compare("default")==0 || (themeName.compare("start")==0))
	{
		InitImages();
		loaded =true;
		return; //Nothing more to do
	}
	oldThemePath = "themes/"+themeName;
	PHYSFS_addToSearchPath(oldThemePath.c_str(),0);
#if defined(__unix__) || defined(_WIN32)
	PHYSFS_addToSearchPath((home+(string)"/"+oldThemePath).c_str(), 0);
#endif
	InitImages();
	loaded = true;
}


long NFont_OpenFont(NFont *target, const char* path,int ptsize, SDL_Color color, int style=TTF_STYLE_NORMAL)
{
	if (!PHYSFS_exists(path))
	{
		cerr << "Error: File not in blockattack.data: " << path << endl;
		return -1; //file doesn't exist
	}

	if(!(TTF_WasInit()))
		TTF_Init();

	PHYSFS_file* myfile = PHYSFS_openRead(path);

	// Get the lenght of the file
	unsigned int m_size = PHYSFS_fileLength(myfile);

	// Get the file data.
	char *m_data = new char[m_size];
	int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

	if (length_read != (int)m_size)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return -3;
	}

	PHYSFS_close(myfile);

// And this is how you load from a memory buffer with SDL
	SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

	//The above might fail an return null.
	if(!rw)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return -2;
	}

	TTF_Font *font;
	font=TTF_OpenFontRW(rw, 1, ptsize);
	TTF_SetFontStyle(font,style);

	target->load(font,color);

	TTF_CloseFont(font); //Once loaded we don't care anymore!

	return 0;
}


Mix_Music * Mix_LoadMUS2(string path)
{
	if (!PHYSFS_exists(path.c_str()))
	{
		cerr << "Warning: File not in blockattack.data: " << path << endl;
		return NULL; //file doesn't exist
	}

	PHYSFS_file* myfile = PHYSFS_openRead(path.c_str());

	// Get the lenght of the file
	unsigned int m_size = PHYSFS_fileLength(myfile);

	// Get the file data.
	char *m_data = new char[m_size];

	int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

	if (length_read != (int)m_size)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return NULL;
	}

	PHYSFS_close(myfile);

// And this is how you load from a memory buffer with SDL
	SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

	//The above might fail an return null.
	if(!rw)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return NULL;
	}

	Mix_Music* ret = Mix_LoadMUS_RW(rw);

	return ret;
}


Mix_Chunk * Mix_LoadWAV2(const char* path)
{
	if (!PHYSFS_exists(path))
	{
		cerr << "Warning: File not in blockattack.data: " << path << endl;
		return NULL; //file doesn't exist
	}

	PHYSFS_file* myfile = PHYSFS_openRead(path);

	// Get the lenght of the file
	unsigned int m_size = PHYSFS_fileLength(myfile);

	// Get the file data.
	char *m_data = new char[m_size];

	int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

	if (length_read != (int)m_size)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return NULL;
	}

	PHYSFS_close(myfile);

// And this is how you load from a memory buffer with SDL
	SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

	//The above might fail an return null.
	if(!rw)
	{
		delete [] m_data;
		m_data = 0;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file!" << endl;
		return NULL;
	}

	Mix_Chunk* ret = Mix_LoadWAV_RW(rw,true); //the second argument tells the function to three RWops

	return ret;
}

//Load all image files to memory
static int InitImages()
{
	if (!((backgroundImage = IMG_Load2("gfx/background.png"))
			&& (background = IMG_Load2("gfx/blackBackGround.png"))
//            && (b1player = IMG_Load2("gfx/bOnePlayer.png"))
//            && (b2players = IMG_Load2("gfx/bTwoPlayers.png"))
//            && (bVsMode = IMG_Load2("gfx/bVsGame.png"))
//            && (bVsModeConfig = IMG_Load2("gfx/bVsGameConfig.png"))
//            && (bPuzzle = IMG_Load2("gfx/bPuzzle.png"))
//            && (bStageClear = IMG_Load2("gfx/bStageClear.png"))
//            && (bTimeTrial = IMG_Load2("gfx/bTimeTrial.png"))
			//&& (bEndless = IMG_Load2("gfx/bEndless.png"))
			&& (bOptions = IMG_Load2("gfx/bOptions.png"))
			&& (bConfigure = IMG_Load2("gfx/bConfigure.png"))
			&& (bSelectPuzzle = IMG_Load2("gfx/bSelectPuzzle.png"))
			&& (bHighScore = IMG_Load2("gfx/bHighScore.png"))
//            && (bExit = IMG_Load2("gfx/bExit.png"))
			&& (bBack = IMG_Load2("gfx/bBlank.png"))
			&& (bForward = IMG_Load2("gfx/bForward.png"))
//            && (bReplay = IMG_Load2("gfx/bReplays.png"))
//            && (bSave = IMG_Load2("gfx/bSave.png"))
//            && (bLoad = IMG_Load2("gfx/bLoad.png"))
			/*#if NETWORK
			            && (bNetwork = IMG_Load2("gfx/bNetwork.png"))
			            && (bHost = IMG_Load2("gfx/bHost.png"))
			            && (bConnect = IMG_Load2("gfx/bConnect.png"))
			#endif*/
			&& (blackLine = IMG_Load2("gfx/blackLine.png"))
			&& (stageBobble = IMG_Load2("gfx/iStageClearLimit.png"))
			&& (bricks[0] = IMG_Load2("gfx/bricks/blue.png"))
			&& (bricks[1] = IMG_Load2("gfx/bricks/green.png"))
			&& (bricks[2] = IMG_Load2("gfx/bricks/purple.png"))
			&& (bricks[3] = IMG_Load2("gfx/bricks/red.png"))
			&& (bricks[4] = IMG_Load2("gfx/bricks/turkish.png"))
			&& (bricks[5] = IMG_Load2("gfx/bricks/yellow.png"))
			&& (bricks[6] = IMG_Load2("gfx/bricks/grey.png"))
			&& (crossover = IMG_Load2("gfx/crossover.png"))
			&& (balls[0] = IMG_Load2("gfx/balls/ballBlue.png"))
			&& (balls[1] = IMG_Load2("gfx/balls/ballGreen.png"))
			&& (balls[2] = IMG_Load2("gfx/balls/ballPurple.png"))
			&& (balls[3] = IMG_Load2("gfx/balls/ballRed.png"))
			&& (balls[4] = IMG_Load2("gfx/balls/ballTurkish.png"))
			&& (balls[5] = IMG_Load2("gfx/balls/ballYellow.png"))
			&& (balls[6] = IMG_Load2("gfx/balls/ballGray.png"))
			&& (cursor[0] = IMG_Load2("gfx/animations/cursor/1.png"))
			&& (cursor[1] = IMG_Load2("gfx/animations/cursor/2.png"))
			&& (bomb[0] = IMG_Load2("gfx/animations/bomb/bomb_1.png"))
			&& (bomb[1] = IMG_Load2("gfx/animations/bomb/bomb_2.png"))
			&& (ready[0] = IMG_Load2("gfx/animations/ready/ready_1.png"))
			&& (ready[1] = IMG_Load2("gfx/animations/ready/ready_2.png"))
			&& (explosion[0] = IMG_Load2("gfx/animations/explosion/0.png"))
			&& (explosion[1] = IMG_Load2("gfx/animations/explosion/1.png"))
			&& (explosion[2] = IMG_Load2("gfx/animations/explosion/2.png"))
			&& (explosion[3] = IMG_Load2("gfx/animations/explosion/3.png"))
			&& (counter[0] = IMG_Load2("gfx/counter/1.png"))
			&& (counter[1] = IMG_Load2("gfx/counter/2.png"))
			&& (counter[2] = IMG_Load2("gfx/counter/3.png"))
			&& (backBoard = IMG_Load2("gfx/BackBoard.png")) //not used, we just test if it exists :)
			&& (iGameOver = IMG_Load2("gfx/iGameOver.png"))
			&& (iWinner = IMG_Load2("gfx/iWinner.png"))
			&& (iDraw = IMG_Load2("gfx/iDraw.png"))
			&& (iLoser = IMG_Load2("gfx/iLoser.png"))
			&& (iChainBack = IMG_Load2("gfx/chainFrame.png"))
//            && (optionsBack = IMG_Load2("gfx/options.png"))
			&& (bOn = IMG_Load2("gfx/bOn.png"))
			&& (bOff = IMG_Load2("gfx/bOff.png"))
//            && (bChange = IMG_Load2("gfx/bChange.png"))
			&& (b1024 = IMG_Load2("gfx/b1024.png"))
			&& (dialogBox = IMG_Load2("gfx/dialogbox.png"))
//	&& (fileDialogBox = IMG_Load2("gfx/fileDialogbox.png"))
			&& (iLevelCheck = IMG_Load2("gfx/iLevelCheck.png"))
			&& (iLevelCheckBox = IMG_Load2("gfx/iLevelCheckBox.png"))
			&& (iLevelCheckBoxMarked = IMG_Load2("gfx/iLevelCheckBoxMarked.png"))
			&& (iCheckBoxArea = IMG_Load2("gfx/iCheckBoxArea.png"))
			&& (boardBackBack = IMG_Load2("gfx/boardBackBack.png"))
			&& (changeButtonsBack = IMG_Load2("gfx/changeButtonsBack.png"))
			&& (garbageTL = IMG_Load2("gfx/garbage/garbageTL.png"))
			&& (garbageT = IMG_Load2("gfx/garbage/garbageT.png"))
			&& (garbageTR = IMG_Load2("gfx/garbage/garbageTR.png"))
			&& (garbageR = IMG_Load2("gfx/garbage/garbageR.png"))
			&& (garbageBR = IMG_Load2("gfx/garbage/garbageBR.png"))
			&& (garbageB = IMG_Load2("gfx/garbage/garbageB.png"))
			&& (garbageBL = IMG_Load2("gfx/garbage/garbageBL.png"))
			&& (garbageL = IMG_Load2("gfx/garbage/garbageL.png"))
			&& (garbageFill = IMG_Load2("gfx/garbage/garbageFill.png"))
			&& (garbageML = IMG_Load2("gfx/garbage/garbageML.png"))
			&& (garbageM = IMG_Load2("gfx/garbage/garbageM.png"))
			&& (garbageMR = IMG_Load2("gfx/garbage/garbageMR.png"))
			&& (garbageGM = IMG_Load2("gfx/garbage/garbageGM.png"))
			&& (garbageGML = IMG_Load2("gfx/garbage/garbageGML.png"))
			&& (garbageGMR = IMG_Load2("gfx/garbage/garbageGMR.png"))
			&& (smiley[0] = IMG_Load2("gfx/smileys/0.png"))
			&& (smiley[1] = IMG_Load2("gfx/smileys/1.png"))
			&& (smiley[2] = IMG_Load2("gfx/smileys/2.png"))
			&& (smiley[3] = IMG_Load2("gfx/smileys/3.png"))
			//new in 1.3.2
			&& (transCover = IMG_Load2("gfx/transCover.png"))
#if LEVELEDITOR
			&& (bCreateFile = IMG_Load2("gfx/editor/bCreateFile.png"))
			&& (bDeletePuzzle = IMG_Load2("gfx/editor/bDeletePuzzle.png"))
			&& (bLoadFile = IMG_Load2("gfx/editor/bLoadFile.png"))
			&& (bMoveBack = IMG_Load2("gfx/editor/bMoveBack.png"))
			&& (bMoveDown = IMG_Load2("gfx/editor/bMoveDown.png"))
			&& (bMoveForward = IMG_Load2("gfx/editor/bMoveForward.png"))
			&& (bMoveLeft = IMG_Load2("gfx/editor/bMoveLeft.png"))
			&& (bMoveRight = IMG_Load2("gfx/editor/bMoveRight.png"))
			&& (bMoveUp = IMG_Load2("gfx/editor/bMoveUp.png"))
			&& (bNewPuzzle = IMG_Load2("gfx/editor/bNewPuzzle.png"))
			&& (bSaveFileAs = IMG_Load2("gfx/editor/bSaveFileAs.png"))
			&& (bSavePuzzle = IMG_Load2("gfx/editor/bSavePuzzle.png"))
			&& (bSaveToFile = IMG_Load2("gfx/editor/bSaveToFile.png"))
			&& (bTestPuzzle = IMG_Load2("gfx/editor/bTestPuzzle.png"))
#endif
			//end new in 1.3.2
			//new in 1.4.0
			&& (bSkip = IMG_Load2("gfx/bBlank.png"))
			&& (bNext = IMG_Load2("gfx/bBlank.png"))
			&& (bRetry = IMG_Load2("gfx/bBlank.png"))
		 ))
		//if there was a problem ie. "File not found"
	{
		cerr << "Error: Failed to load image file: " << SDL_GetError() << endl;
		exit(1);
	}
	try
	{
		bNewGame = IMG_Load3("gfx/bNewGame.png");
		mouse = IMG_Load3("gfx/mouse.png");
		menuMarked = IMG_Load3("gfx/menu/marked.png");
		menuUnmarked = IMG_Load3("gfx/menu/unmarked.png");
	}
	catch (exception &e)
	{
		cerr << e.what() << endl;
		exit(1);
	}


	//Prepare for fast blittering!
	CONVERT(background);
	CONVERT(backgroundImage);
//    CONVERT(b1player);
//    CONVERT(b2players);
//    CONVERT(bVsMode);
//    CONVERT(bVsModeConfig);
//    CONVERT(bPuzzle);
//    CONVERT(bStageClear);
//    CONVERT(bTimeTrial);
	//CONVERT(bEndless);
	CONVERT(bOptions);
	CONVERTA(bConfigure);
	CONVERTA(bSelectPuzzle);
//    CONVERTA(bReplay);
//    CONVERTA(bSave);
//    CONVERTA(bLoad);
	CONVERTA(bSkip);
	CONVERTA(bRetry);
	CONVERTA(bNext);
	/*#if NETWORK
	    CONVERTA(bNetwork);
	    CONVERTA(bHost);
	    CONVERTA(bConnect);
	#endif*/
	CONVERT(bHighScore);
	CONVERTA(boardBackBack);
	CONVERT(backBoard);
	CONVERT(blackLine);
	CONVERTA(changeButtonsBack);
	CONVERTA(cursor[0]);
	CONVERTA(cursor[1]);
	CONVERTA(counter[0]);
	CONVERTA(counter[1]);
	CONVERTA(counter[2]);
//    CONVERTA(optionsBack);
//    CONVERT(bExit);
	CONVERT(bOn);
	CONVERT(bOff);
//    CONVERT(bChange);
	CONVERT(b1024);
	CONVERTA(dialogBox);
//	CONVERTA(fileDialogBox);
	CONVERTA(iLevelCheck);
	CONVERT(iLevelCheckBox);
	CONVERT(iLevelCheckBoxMarked);
	CONVERTA(iCheckBoxArea);
	for (int i = 0; i<4; i++)
	{
		CONVERTA(explosion[i]);
	}
	for (int i = 0; i<7; i++)
	{
		CONVERTA(bricks[i]);
		CONVERTA(balls[i]);
	}
	CONVERTA(crossover);
	CONVERTA(garbageTL);
	CONVERTA(garbageT);
	CONVERTA(garbageTR);
	CONVERTA(garbageR);
	CONVERTA(garbageBR);
	CONVERTA(garbageB);
	CONVERTA(garbageBL);
	CONVERTA(garbageL);
	CONVERTA(garbageFill);
	CONVERTA(garbageML);
	CONVERTA(garbageMR);
	CONVERTA(garbageM);
	CONVERTA(garbageGML);
	CONVERTA(garbageGMR);
	CONVERTA(garbageGM);
	CONVERTA(smiley[0]);
	CONVERTA(smiley[1]);
	CONVERTA(smiley[2]);
	CONVERTA(smiley[3]);
	CONVERTA(iWinner);
	CONVERTA(iDraw);
	CONVERTA(iLoser);
	CONVERTA(iChainBack);
	CONVERTA(iGameOver);
	mouse.OptimizeForBlit(true);
	bNewGame.OptimizeForBlit(true);
	CONVERTA(stageBobble);
	CONVERTA(transCover);
	//Editor:
#if LEVELEDITOR
	CONVERTA(bCreateFile);
	CONVERTA(bDeletePuzzle);
	CONVERTA(bLoadFile);
	CONVERTA(bMoveBack);
	CONVERTA(bMoveDown);
	CONVERTA(bMoveForward);
	CONVERTA(bMoveLeft);
	CONVERTA(bMoveRight);
	CONVERTA(bMoveUp);
	CONVERTA(bNewPuzzle);
	CONVERTA(bSaveFileAs);
	CONVERTA(bSavePuzzle);
	CONVERTA(bSaveToFile);
	CONVERTA(bTestPuzzle);
#endif

	SDL_Color nf_button_color, nf_standard_blue_color, nf_standard_small_color;
	memset(&nf_button_color,0,sizeof(SDL_Color));
	nf_button_color.b = 255;
	nf_button_color.g = 255;
	nf_button_color.r = 255;
	nf_standard_blue_color.b = 255;
	nf_standard_blue_color.g = 0;
	nf_standard_blue_color.r = 0;
	nf_standard_small_color.b = 0;
	nf_standard_small_color.g = 0;
	nf_standard_small_color.r = 200;
	NFont_OpenFont(&nf_button_font,"fonts/FreeSerif.ttf",24,nf_button_color);
	nf_button_font.setDest(screen);
	NFont_OpenFont(&nf_standard_blue_font,"fonts/FreeSerif.ttf",30,nf_standard_blue_color);
	nf_standard_blue_font.setDest(screen);
	NFont_OpenFont(&nf_standard_small_font,"fonts/FreeSerif.ttf",16,nf_standard_small_color);
	nf_standard_small_font.setDest(screen);
	NFont_OpenFont(&nf_scoreboard_font,"fonts/PenguinAttack.ttf",20,nf_button_color);
	nf_scoreboard_font.setDest(boardBackBack);
	nf_scoreboard_font.draw(370,148,_("Score:") );
	nf_scoreboard_font.draw(370,197,_("Time:") );
	nf_scoreboard_font.draw(370,246,_("Chain:") );
	nf_scoreboard_font.draw(370,295,_("Speed:") );
	nf_button_font.setDest(bRetry);
	nf_button_font.drawCenter(60,10,_("Retry"));
	nf_button_font.setDest(bNext);
	nf_button_font.drawCenter(60,10,_("Next"));
	nf_button_font.setDest(bSkip);
	nf_button_font.drawCenter(60,10,_("Skip"));
	nf_button_font.setDest(bBack);
	nf_button_font.drawCenter(60,10,_("Back"));
	nf_button_font.setDest(screen);


//Loads the sound if sound present
	if (!NoSound)
	{
		//And here the music:
		bgMusic = Mix_LoadMUS2("music/bgMusic.ogg");
		highbeatMusic = Mix_LoadMUS2("music/highbeat.ogg");
		//the music... we just hope it exists, else the user won't hear anything
		//Same goes for the sounds
		boing = Mix_LoadWAV2("sound/pop.ogg");
		applause = Mix_LoadWAV2("sound/applause.ogg");
		photoClick = Mix_LoadWAV2("sound/cameraclick.ogg");
		typingChunk = Mix_LoadWAV2("sound/typing.ogg");
		counterChunk = Mix_LoadWAV2("sound/counter.ogg");
		counterFinalChunk = Mix_LoadWAV2("sound/counterFinal.ogg");
	} //All sound has been loaded or not
	return 0;
} //InitImages()


//Unload images and fonts and sounds
void UnloadImages()
{
	if(verboseLevel)
		cout << "Unloading data..." << endl;
	if (!NoSound) //Only unload then it has been loaded!
	{
		Mix_HaltMusic();
		Mix_FreeMusic(bgMusic);
		Mix_FreeMusic(highbeatMusic);
		Mix_FreeChunk(boing);
		Mix_FreeChunk(applause);
		Mix_FreeChunk(photoClick);
		Mix_FreeChunk(counterChunk);
		Mix_FreeChunk(counterFinalChunk);
		Mix_FreeChunk(typingChunk);
	}
	//Free surfaces:
	//I think this will crash, at least it happend to me...
	//Chrashes no more. Caused by an undocumented double free
	SDL_FreeSurface(backgroundImage);
	SDL_FreeSurface(background);
	//SDL_FreeSurface(bNewGame);
//    SDL_FreeSurface(b1player);
//    SDL_FreeSurface(b2players);
//    SDL_FreeSurface(bVsMode);
//    SDL_FreeSurface(bVsModeConfig);
//    SDL_FreeSurface(bPuzzle);
//    SDL_FreeSurface(bStageClear);
//    SDL_FreeSurface(bTimeTrial);
	//SDL_FreeSurface(bEndless);
	SDL_FreeSurface(bOptions);
	SDL_FreeSurface(bConfigure);
	SDL_FreeSurface(bSelectPuzzle);
	SDL_FreeSurface(bHighScore);
//    SDL_FreeSurface(bReplay);
//    SDL_FreeSurface(bSave);
//    SDL_FreeSurface(bLoad);
	/*    #if NETWORK
	    SDL_FreeSurface(bNetwork);
	    SDL_FreeSurface(bHost);
	    SDL_FreeSurface(bConnect);
	    #endif
	    SDL_FreeSurface(bExit);*/
	SDL_FreeSurface(blackLine);
	SDL_FreeSurface(stageBobble);
	SDL_FreeSurface(bricks[0]);
	SDL_FreeSurface(bricks[1]);
	SDL_FreeSurface(bricks[2]);
	SDL_FreeSurface(bricks[3]);
	SDL_FreeSurface(bricks[4]);
	SDL_FreeSurface(bricks[5]);
	SDL_FreeSurface(bricks[6]);
	SDL_FreeSurface(crossover);
	SDL_FreeSurface(balls[0]);
	SDL_FreeSurface(balls[1]);
	SDL_FreeSurface(balls[2]);
	SDL_FreeSurface(balls[3]);
	SDL_FreeSurface(balls[4]);
	SDL_FreeSurface(balls[5]);
	SDL_FreeSurface(balls[6]);
	SDL_FreeSurface(cursor[0]);
	SDL_FreeSurface(cursor[1]);
	SDL_FreeSurface(backBoard); //not used, we just test if it exists :)
	SDL_FreeSurface(iGameOver);
	SDL_FreeSurface(iWinner);
	SDL_FreeSurface(iDraw);
	SDL_FreeSurface(iLoser);
	SDL_FreeSurface(iChainBack);
//    SDL_FreeSurface(optionsBack);
	SDL_FreeSurface(bOn);
	SDL_FreeSurface(bOff);
//    SDL_FreeSurface(bChange);
	SDL_FreeSurface(b1024);
	SDL_FreeSurface(dialogBox);
	//SDL_FreeSurface(fileDialogBox);
	SDL_FreeSurface(iLevelCheck);
	SDL_FreeSurface(iLevelCheckBox);
	SDL_FreeSurface(iLevelCheckBoxMarked);
	SDL_FreeSurface(iCheckBoxArea);
	SDL_FreeSurface(boardBackBack);
	SDL_FreeSurface(changeButtonsBack);
	SDL_FreeSurface(garbageTL);
	SDL_FreeSurface(garbageT);
	SDL_FreeSurface(garbageTR);
	SDL_FreeSurface(garbageR);
	SDL_FreeSurface(garbageBR);
	SDL_FreeSurface(garbageB);
	SDL_FreeSurface(garbageBL);
	SDL_FreeSurface(garbageL);
	SDL_FreeSurface(garbageFill);
	SDL_FreeSurface(garbageML);
	SDL_FreeSurface(garbageM);
	SDL_FreeSurface(garbageMR);
	SDL_FreeSurface(garbageGML);
	SDL_FreeSurface(garbageGM);
	SDL_FreeSurface(garbageGMR);
	SDL_FreeSurface(smiley[0]);
	SDL_FreeSurface(smiley[1]);
	SDL_FreeSurface(smiley[2]);
	SDL_FreeSurface(smiley[3]);
	SDL_FreeSurface(transCover);
	mouse.MakeNull();
	bNewGame.MakeNull();
}

//Function to convert numbers to string
/*string itoa(int num)
{
    stringstream converter;
    converter << num;
    return converter.str();
}*/


static stringstream converter;

//Function to convert numbers to string (2 diget)
static string itoa2(int num)
{
	converter.str(std::string());
	converter.clear();
	if(num<10)
		converter << "0";
	converter << num;
	return converter.str();
}

/*Loads all the puzzle levels*/
static int LoadPuzzleStages()
{
	//if(puzzleLoaded)
	//    return 1;
	if (!PHYSFS_exists(((string)("puzzles/"+puzzleName)).c_str()))
	{
		cerr << "Warning: File not in blockattack.data: " << ("puzzles/"+puzzleName) << endl;
		return -1; //file doesn't exist
	}
	PhysFS::ifstream inFile(((string)("puzzles/"+puzzleName)).c_str());

	inFile >> nrOfPuzzles;
	if (nrOfPuzzles>maxNrOfPuzzleStages)
		nrOfPuzzles=maxNrOfPuzzleStages;
	for (int k=0; (k<nrOfPuzzles) /*&&(!inFile.eof())*/ ; k++)
	{
		inFile >> nrOfMovesAllowed[k];
		for (int i=11; i>=0; i--)
			for (int j=0; j<6; j++)
			{
				inFile >> puzzleLevels[k][j][i];
			}
	}
	puzzleLoaded = true;
	return 0;
}

/*Draws a image from on a given Surface. Takes source image, destination surface and coordinates*/
void DrawIMG(SDL_Surface *img, SDL_Surface *target, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(img, NULL, target, &dest);
}

/*Draws a part of an image on a surface of choice*/
void DrawIMG(SDL_Surface *img, SDL_Surface * target, int x, int y, int w, int h, int x2, int y2)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_Rect dest2;
	dest2.x = x2;
	dest2.y = y2;
	dest2.w = w;
	dest2.h = h;
	SDL_BlitSurface(img, &dest2, target, &dest);
}


void NFont_Write(SDL_Surface *target,int x,int y,string text)
{
	nf_standard_blue_font.setDest(target);
	nf_standard_blue_font.draw(x,y,text.c_str());
	nf_standard_blue_font.setDest(screen);
}

void ResetFullscreen()
{
#if defined(WIN32)
	if (bFullscreen) screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_FULLSCREEN|SDL_ANYFORMAT);
	else screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_ANYFORMAT);
	DrawIMG(background, screen, 0, 0);
#else
	SDL_WM_ToggleFullScreen(screen); //Will only work in Linux
#endif
	SDL_ShowCursor(SDL_DISABLE);
}


//The small things that are faaling when you clear something
class aBall
{
private:
	double x;
	double y;
	double velocityY;
	double velocityX;
	int color;
	unsigned long int lastTime;
public:

	aBall()
	{}

	//constructor:
	aBall(int X, int Y, bool right, int coulor)
	{
		double tal = 1.0+((double)rand()/((double)RAND_MAX));
		velocityY = -tal*startVelocityY;
		lastTime = currentTime;
		x = (double)X;
		y = (double)Y;
		color = coulor;
		if (right)
			velocityX = tal*VelocityX;
		else
			velocityX = -tal*VelocityX;
	}  //constructor

	//Deconstructor
	~aBall()
	{
	}   //Deconstructor

	void update()
	{
		double timePassed = (((double)(currentTime-lastTime))/1000.0);  //time passed in seconds
		x = x+timePassed*velocityX;
		y = y+timePassed*velocityY;
		velocityY = velocityY + gravity*timePassed;
		if (y<1.0)
			velocityY=10.0;
		if ((velocityY>minVelocity) && (y>(double)(768-ballSize)) && (y<768.0))
		{
			velocityY = -0.70*velocityY;
			y = 768.0-ballSize;
		}
		lastTime = currentTime;
	}

	int getX()
	{
		return (int)x;
	}

	int getY()
	{
		return (int)y;
	}

	int getColor()
	{
		return color;
	}
};  //aBall

static const int maxNumberOfBalls = 6*12*2*2;

class ballManeger
{
public:
	aBall ballArray[maxNumberOfBalls];
	bool ballUsed[maxNumberOfBalls];
	//The old ball information is also saved so balls can be deleted!
	aBall oldBallArray[maxNumberOfBalls];
	bool oldBallUsed[maxNumberOfBalls];

	ballManeger()
	{
		for (int i=0; i<maxNumberOfBalls; i++)
		{
			ballUsed[i] = false;
			oldBallUsed[i] = false;
		}
	}

	//Adds a ball to the screen at given coordiantes, traveling right or not with color
	int addBall(int x, int y,bool right,int color)
	{
		int ballNumber = 0;
		//Find a free ball
		while ((ballUsed[ballNumber])&&(ballNumber<maxNumberOfBalls))
			ballNumber++;
		//Could not find a free ball, return -1
		if (ballNumber==maxNumberOfBalls)
			return -1;
		currentTime = SDL_GetTicks();
		ballArray[ballNumber] = aBall(x,y,right,color);
		ballUsed[ballNumber] = true;
		return 1;
	}  //addBall

	void update()
	{
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfBalls; i++)
		{

			if (ballUsed[i])
			{
				oldBallUsed[i] = true;
				oldBallArray[i] = ballArray[i];
				ballArray[i].update();
				if (ballArray[i].getY()>800 || ballArray[i].getX()>xsize || ballArray[i].getX()<-ballSize)
				{
					ballUsed[i] = false;
				}
			}
			else
			{
				oldBallUsed[i] = false;
			}
		}
	} //update


}; //theBallManeger

static ballManeger theBallManeger;

//a explosions, non moving
class anExplosion
{
private:
	int x;
	int y;
	Uint8 frameNumber;
#define frameLength 80
	//How long an image in an animation should be showed
#define maxFrame 4
	//How many images there are in the animation
	unsigned long int placeTime; //Then the explosion occored
public:

	anExplosion()
	{}

	//constructor:
	anExplosion(int X, int Y)
	{
		placeTime = currentTime;
		x = X;
		y = Y;
		frameNumber=0;
	}  //constructor

	//Deconstructor
	~anExplosion()
	{
	}   //Deconstructor

	//true if animation has played and object should be removed from the screen
	bool removeMe()
	{
		frameNumber = (currentTime-placeTime)/frameLength;
		return (!(frameNumber<maxFrame));
	}

	int getX()
	{
		return (int)x;
	}

	int getY()
	{
		return (int)y;
	}

	int getFrame()
	{
		return frameNumber;
	}
};  //nExplosion

class explosionManeger
{
public:
	anExplosion explosionArray[maxNumberOfBalls];
	bool explosionUsed[maxNumberOfBalls];
	//The old explosion information is also saved so explosions can be deleted!
	anExplosion oldExplosionArray[maxNumberOfBalls];
	bool oldExplosionUsed[maxNumberOfBalls];

	explosionManeger()
	{
		for (int i=0; i<maxNumberOfBalls; i++)
		{
			explosionUsed[i] = false;
			oldExplosionUsed[i] = false;
		}
	}

	int addExplosion(int x, int y)
	{
		int explosionNumber = 0;
		while ((explosionUsed[explosionNumber])&&(explosionNumber<maxNumberOfBalls))
			explosionNumber++;
		if (explosionNumber==maxNumberOfBalls)
			return -1;
		currentTime = SDL_GetTicks();
		explosionArray[explosionNumber] = anExplosion(x,y);
		explosionUsed[explosionNumber] = true;
		return 1;
	}  //addBall

	void update()
	{
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfBalls; i++)
		{

			if (explosionUsed[i])
			{
				oldExplosionUsed[i] = true;
				oldExplosionArray[i] = explosionArray[i];
				if (explosionArray[i].removeMe())
				{
					explosionArray[i].~anExplosion();
					explosionUsed[i] = false;
				}
			}
			else
			{
				oldExplosionUsed[i] = false;
			}
		}
	} //update


}; //explosionManeger

static explosionManeger theExplosionManeger;

//text pop-up
class textMessage
{
private:
	int x;
	int y;
	char textt[10];
	unsigned long int time;
	unsigned long int placeTime; //Then the text was placed
public:

	textMessage()
	{}

	//constructor:
	textMessage(int X, int Y,const char* Text,unsigned int Time)
	{
		placeTime = currentTime;
		x = X;
		y = Y;
		strncpy(textt,Text,10);
		textt[9]=0;
		time = Time;
	}  //constructor

	//true if the text has expired
	bool removeMe()
	{
		return currentTime-placeTime>time;
	}

	int getX()
	{
		return x;
	}

	int getY()
	{
		return y;
	}

	const char* getText()
	{
		return textt;
	}
};  //text popup

class textManeger
{
public:
	textMessage textArray[maxNumberOfBalls];
	bool textUsed[maxNumberOfBalls];
	//The old text information is also saved so text can be deleted!
	textMessage oldTextArray[maxNumberOfBalls];
	bool oldTextUsed[maxNumberOfBalls];

	textManeger()
	{
		for (int i=0; i<maxNumberOfBalls; i++)
		{
			textUsed[i] = false;
			oldTextUsed[i] = false;
		}
	}

	int addText(int x, int y,string Text,unsigned int Time)
	{
		int textNumber = 0;
		while ((textNumber<maxNumberOfBalls)&&((textUsed[textNumber])||(oldTextUsed[textNumber])))
			textNumber++;
		if (textNumber==maxNumberOfBalls)
			return -1;
		currentTime = SDL_GetTicks();
		textArray[textNumber] = textMessage(x,y,Text.c_str(),Time);
		textUsed[textNumber] = true;
		return 1;
	}  //addText

	void update()
	{
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfBalls; i++)
		{

			if (textUsed[i])
			{
				if (!oldTextUsed[i])
				{
					oldTextUsed[i] = true;
					oldTextArray[i] = textMessage(textArray[i]);
				}
				if (textArray[i].removeMe())
				{
					textArray[i].~textMessage();
					textUsed[i] = false;
				}
			}
			else if (oldTextUsed[i])
			{
				oldTextUsed[i] = false;
				oldTextArray[i].~textMessage();
			}
		}
	} //update


}; //textManeger

static textManeger theTextManeger;

//Here comes the Block Game object
#include "BlockGame.hpp"
#include "BlockGame.cpp"

class BlockGameSdl : public BlockGame
{
public:
	SDL_Surface* sBoard;

	BlockGameSdl(int tx, int ty)
	{
		tmp = IMG_Load2("gfx/BackBoard.png");
		sBoard = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);
		//BlockGame::BlockGame(tx,ty);
		BlockGame::topx = tx;
		BlockGame::topy = ty;
	}
	~BlockGameSdl()
	{
		SDL_FreeSurface(sBoard);
	}
private:
	void convertSurface()
	{
		SDL_FreeSurface(sBoard);
		sBoard = SDL_DisplayFormat(backBoard);
	}
	//Draws all the bricks to the board (including garbage)
	void PaintBricks()
	{
		for (int i=0; ((i<13)&&(i<30)); i++)
			for (int j=0; j<6; j++)
			{
				if ((board[j][i]%10 != -1) && (board[j][i]%10 < 7) && ((board[j][i]/1000000)%10==0))
				{
					DrawIMG(bricks[board[j][i]%10], sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((board[j][i]/BLOCKWAIT)%10==1)
						DrawIMG(bomb[(ticks/BOMBTIME)%2], sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((board[j][i]/BLOCKHANG)%10==1)
						DrawIMG(ready[(ticks/READYTIME)%2], sBoard, j*bsize, bsize*12-i*bsize-pixels);

				}
				if ((board[j][i]/1000000)%10==1)
				{
					int left, right, over, under;
					int number = board[j][i];
					if (j<1) left = -1;
					else left = board[j-1][i];
					if (j>5) right = -1;
					else right = board[j+1][i];
					if (i>28) over = -1;
					else over = board[j][i+1];
					if (i<1) under = -1;
					else under = board[j][i-1];
					if ((left == number)&&(right == number)&&(over == number)&&(under == number))
						DrawIMG(garbageFill, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left != number)&&(right == number)&&(over == number)&&(under == number))
						DrawIMG(garbageL, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right != number)&&(over == number)&&(under == number))
						DrawIMG(garbageR, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right == number)&&(over != number)&&(under == number))
						DrawIMG(garbageT, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right == number)&&(over == number)&&(under != number))
						DrawIMG(garbageB, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left != number)&&(right == number)&&(over != number)&&(under == number))
						DrawIMG(garbageTL, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left != number)&&(right == number)&&(over == number)&&(under != number))
						DrawIMG(garbageBL, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right != number)&&(over != number)&&(under == number))
						DrawIMG(garbageTR, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right != number)&&(over == number)&&(under != number))
						DrawIMG(garbageBR, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right != number)&&(over != number)&&(under != number))
						DrawIMG(garbageMR, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left == number)&&(right == number)&&(over != number)&&(under != number))
						DrawIMG(garbageM, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					if ((left != number)&&(right == number)&&(over != number)&&(under != number))
						DrawIMG(garbageML, sBoard, j*bsize, bsize*12-i*bsize-pixels);
				}
				if ((board[j][i]/1000000)%10==2)
				{
					if (j==0)
						DrawIMG(garbageGML, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					else if (j==5)
						DrawIMG(garbageGMR, sBoard, j*bsize, bsize*12-i*bsize-pixels);
					else
						DrawIMG(garbageGM, sBoard, j*bsize, bsize*12-i*bsize-pixels);
				}
			}
		const int j = 0;

		int garbageSize=0;
		for (int i=0; i<20; i++)
		{
			if ((board[j][i]/1000000)%10==1)
			{
				int left, right, over, under;
				int number = board[j][i];
				if (j<1) left = -1;
				else left = board[j-1][i];
				if (j>5) right = -1;
				else right = board[j+1][i];
				if (i>28) over = -1;
				else over = board[j][i+1];
				if (i<1) under = -1;
				else under = board[j][i-1];
				if (((left != number)&&(right == number)&&(over != number)&&(under == number))&&(garbageSize>0))
				{
					DrawIMG(smiley[board[j][i]%4], sBoard, 2*bsize, 12*bsize-i*bsize-pixels+(bsize/2)*garbageSize);
				}
				if (!((left != number)&&(right == number)&&(over == number)&&(under == number))) //not in garbage
				{
					garbageSize=0;
				}
				else
				{
					garbageSize++;
				}

			}
		}
		for (int i=0; i<6; i++)
			if (board[i][0]!=-1)
				DrawIMG(transCover, sBoard, i*bsize, 12*bsize-pixels); //Make the appering blocks transperant

	}
	//Paints the bricks gotten from a replay/net package
	void SimplePaintBricks()
	{
		/*
		 * We will need to mark the blocks that must have a bomb, we will here need to see, what is falling
		 */
		bool bbomb[6][13]; //has a bomb on it!
		bool falling[6][13]; //this is falling
		bool getReady[6][13]; //getReady
		for (int i=0; i<6; i++)
			for (int j=0; j<13; j++)
			{
				bbomb[i][j]=false; //All false by default
				falling[i][j]=false;
				if (board[i][j]>29)
					getReady[i][j]=true;
				else
					getReady[i][j]=false;
			}
		//See that is falling
		for (int i=0; i<6; i++)
		{
			bool rowFalling = false;
			for (int j=0; j<13; j++)
			{
				if (rowFalling)
					falling[i][j]=true;
				if ((!rowFalling)&&(board[i][j]%30==-1))
					rowFalling = true;
				if ((rowFalling)&&(board[i][j]%30>6))
					rowFalling = false;
				if (getReady[i][j])
				{
					falling[i][j]=true; //getReady is the same as falling
					rowFalling = false;
				}
			}
		}
		//Now looking at rows:
		for (int i=0; i<6; i++)
		{
			int count = 0;
			int color = -1;
			for (int j=1; j<13; j++)
			{
				if ((board[i][j]%30==color)&&(!falling[i][j]))
					count++;
				else if (falling[i][j])
				{
					count = 0;
				}
				else
				{
					color=board[i][j]%30;
					count=1;
				}
				if ((count>2)&&(color>-1)&&(color)<7)
				{
					bbomb[i][j]=true;
					bbomb[i][j-1]=true;
					bbomb[i][j-2]=true;
				}
			}
		}
		//now looking for lines
		for (int i=1; i<13; i++)
		{
			int count = 0;
			int color = -1;
			for (int j=0; j<6; j++)
			{
				if ((board[j][i]%30==color)&&(!falling[j][i]))
					count++;
				else if (falling[j][i])
				{
					count = 0;
				}
				else
				{
					color=board[j][i]%30;
					count=1;
				}
				if ((count>2)&&(color>-1)&&(color<7))
				{
					bbomb[j][i]=true;
					bbomb[j-1][i]=true;
					bbomb[j-2][i]=true;
				}
			}
		}
		for (int i=0; ((i<13)&&(i<30)); i++)
			for (int j=0; j<6; j++)
			{
				if ((board[j][i]%10 != -1) && (board[j][i]%30 < 7))
				{
					DrawIMG(bricks[board[j][i]%10], sBoard, j*bsize, 12*bsize-i*bsize-pixels);
					if (bbomb[j][i])
						DrawIMG(bomb[(ticks/BOMBTIME)%2], sBoard, j*bsize, 12*bsize-i*bsize-pixels);
					if (getReady[j][i])
						DrawIMG(ready[(ticks/READYTIME)%2], sBoard, j*bsize, 12*bsize-i*bsize-pixels);
				}
				if (board[j][i]%30>6)
				{
					if (board[j][i]%30==7)
						DrawIMG(garbageR, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==9)
						DrawIMG(garbageML, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==10)
						DrawIMG(garbageMR, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==11)
						DrawIMG(garbageTR, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==12)
						DrawIMG(garbageTL, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==13)
						DrawIMG(garbageBL, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==14)
						DrawIMG(garbageBR, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==15)
						DrawIMG(garbageM, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==16)
						DrawIMG(garbageFill, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==17)
						DrawIMG(garbageT, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==18)
						DrawIMG(garbageB, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==19)
						DrawIMG(garbageL, sBoard, j*bsize, 12*bsize-i*bsize-pixels); //good
					if (board[j][i]%30==20)
						switch(j)
						{
						case 0:
							DrawIMG(garbageGML, sBoard,j*bsize, 12*bsize-i*bsize-pixels);
							break;
						case 5:
							DrawIMG(garbageGMR, sBoard,j*bsize, 12*bsize-i*bsize-pixels);
							break;
						default:
							DrawIMG(garbageGM, sBoard,j*bsize, 12*bsize-i*bsize-pixels);
						}
				}


			}

		int garbageSize=0;
		for (int i=0; i<20; i++)
		{
			if ((board[0][i]%30==12)&&(garbageSize>0))
			{
				DrawIMG(smiley[0], sBoard, 2*bsize, bsize-i*bsize-pixels+(bsize/2)*garbageSize);
			}
			if (board[0][i]%30!=19) //not in garbage
			{
				garbageSize=0;
			}
			else
			{
				garbageSize++;
			}

		}
	}
public:
	//Draws everything
	void DoPaintJob()
	{
		DrawIMG(backBoard, sBoard, 0, 0);
		nf_standard_blue_font.setDest(sBoard); //reset to screen at the end of this funciton!

		PaintBricks();
		if (stageClear) DrawIMG(blackLine, sBoard, 0, bsize*(12+2)+bsize*(stageClearLimit-linesCleared)-pixels-1);
		if (puzzleMode&&(!bGameOver))
		{
			//We need to write nr. of moves left!
			strHolder = "Moves left: " + itoa(MovesLeft);
			nf_standard_blue_font.draw(5,5,strHolder.c_str());

		}
		if(puzzleMode && stageButtonStatus == SBpuzzleMode)
		{
			DrawIMG(bRetry,sBoard, cordRetryButton.x, cordRetryButton.y);
			if(Level<nrOfPuzzles-1)
			{
				if(hasWonTheGame)
					DrawIMG(bNext,sBoard,cordNextButton.x, cordNextButton.y);
				else
					DrawIMG(bSkip,sBoard,cordNextButton.x, cordNextButton.y);
			}
			else
			{
				strHolder = "Last puzzle";
				nf_standard_blue_font.draw(5,5,strHolder.c_str());
			}
		}
		if(stageClear && stageButtonStatus == SBstageClear)
		{
			DrawIMG(bRetry,sBoard, cordRetryButton.x, cordRetryButton.y);
			if(Level<50-1)
			{
				if(hasWonTheGame)
					DrawIMG(bNext,sBoard,cordNextButton.x, cordNextButton.y);
				else
					DrawIMG(bSkip,sBoard,cordNextButton.x, cordNextButton.y);
			}
			else
			{
				strHolder = "Last stage";
				nf_standard_blue_font.draw(5,5,strHolder.c_str());
			}
		}

#if DEBUG
		if (AI_Enabled&&(!bGameOver))
		{
			strHolder = "AI_status: " + itoa(AIstatus)+ ", "+ itoa(AIlineToClear);
			//NFont_Write(sBoard,   5, 5, strHolder.c_str());
			nf_standard_blue_font.draw(5,5,strHolder.c_str());
		}
#endif
		if (!bGameOver)DrawIMG(cursor[(ticks/600)%2],sBoard,cursorx*bsize-4,11*bsize-cursory*bsize-pixels-4);
		if (ticks<gameStartedAt)
		{
			int currentCounter = abs((int)ticks-(int)gameStartedAt)/1000;
			if( (currentCounter!=lastCounter) && (SoundEnabled)&&(!NoSound))
				Mix_PlayChannel(1,counterChunk,0);
			lastCounter = currentCounter;
			switch (currentCounter)
			{
			case 2:
				DrawIMG(counter[2], sBoard, 2*bsize, 5*bsize);
				break;
			case 1:
				DrawIMG(counter[1], sBoard, 2*bsize, 5*bsize);
				break;
			case 0:
				DrawIMG(counter[0], sBoard, 2*bsize, 5*bsize);
				break;
			default:
				break;
			}
		}
		else
		{
			if(SoundEnabled&&(!NoSound)&&(timetrial)&&(ticks>gameStartedAt+10000)&&(!bGameOver))
			{
				int currentCounter = (ticks-(int)gameStartedAt)/1000;
				if(currentCounter!=lastCounter)
				{
					if(currentCounter>115 && currentCounter<120)
						Mix_PlayChannel(1,counterChunk,0);
				}
				lastCounter = currentCounter;
			}
			else
			{
				if( (0==lastCounter) && (SoundEnabled)&&(!NoSound))
				{
					Mix_PlayChannel(1,counterFinalChunk,0);
				}
				lastCounter = -1;
			}
		}

		if ((bGameOver)&&(!editorMode))
		{
			if (hasWonTheGame)
			{
				DrawIMG(iWinner, sBoard, 0, 5*bsize);
			}
			else
			{
				if (bDraw)
					DrawIMG(iDraw, sBoard, 0, 5*bsize);
				else
					DrawIMG(iGameOver, sBoard, 0, 5*bsize);
			}
		}
		nf_standard_blue_font.setDest(screen);
	}


	void Update(int newtick)
	{
		BlockGame::Update(newtick);
		DoPaintJob();
	}
};




//writeScreenShot saves the screen as a bmp file, it uses the time to get a unique filename
void writeScreenShot()
{
	if(verboseLevel)
		cout << "Saving screenshot" << endl;
	int rightNow = (int)time(NULL);
	/*#if defined(__unix__)
	    char buf[514];
	    sprintf( buf, "%s/.gamesaves/blockattack/screenshots/screenshot%i.bmp", getenv("HOME"), rightNow );
	#elif defined(__win32__)
	    char buf[MAX_PATH];
	    sprintf( buf, "%s\\My Games\\blockattack\\screenshots\\screenshot%i.bmp", (getMyDocumentsPath()).c_str(), rightNow );
	#else
	    char buf[MAX_PATH];
	    sprintf( buf, "screenshot%i.bmp", rightNow );
	#endif*/
#if defined(__unix__)
	string buf = (string)getenv("HOME")+"/.gamesaves/blockattack/screenshots/screenshot"+itoa(rightNow)+".bmp";
#elif defined(__win32__)
	string buf = getMyDocumentsPath()+"\\My Games\\blockattack\\screenshots\\screenshot"+itoa(rightNow)+".bmp";
#else
	string buf = "screenshot"+itoa(rightNow)+".bmp";
#endif
	SDL_SaveBMP( screen, buf.c_str() );
	if (!NoSound)
		if (SoundEnabled)Mix_PlayChannel(1,photoClick,0);
}

//Function to return the name of a key, to be displayed...
static string getKeyName(SDLKey key)
{
	string keyname(SDL_GetKeyName(key));
	return keyname;
}

void MakeBackground(int xsize,int ysize,BlockGame &theGame, BlockGame &theGame2);



//Dialogbox
bool OpenDialogbox(int x, int y, char *name)
{
	bool done = false;     //We are done!
	bool accept = false;   //New name is accepted! (not Cancelled)
	bool repeating = false; //The key is being held (BACKSPACE)
	const int repeatDelay = 200;    //Repeating
	unsigned long time = 0;
	ReadKeyboard rk = ReadKeyboard(name);
	Uint8* keys;
	string strHolder;
	MakeBackground(xsize,ysize);
	DrawIMG(background,screen,0,0);
	while (!done && !Config::getInstance()->isShuttingDown())
	{
		DrawIMG(dialogBox,screen,x,y);
		NFont_Write(screen, x+40,y+76,rk.GetString());
		strHolder = rk.GetString();
		strHolder.erase((int)rk.CharsBeforeCursor());

		if (((SDL_GetTicks()/600)%2)==1)
			NFont_Write(screen, x+40+nf_standard_blue_font.getWidth( strHolder.c_str()),y+76,"|");

		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )
			{
				Config::getInstance()->setShuttingDown(5);
				done = true;
				accept = false;
			}

			if ( event.type == SDL_KEYDOWN )
			{
				if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) )
				{
					done = true;
					accept = true;
				}
				else if ( (event.key.keysym.sym == SDLK_ESCAPE) )
				{
					done = true;
					accept = false;
				}
				else if (!(event.key.keysym.sym == SDLK_BACKSPACE))
				{
					if ((rk.ReadKey(event.key.keysym.sym))&&(SoundEnabled)&&(!NoSound))Mix_PlayChannel(1,typingChunk,0);
				}
				else if ((event.key.keysym.sym == SDLK_BACKSPACE)&&(!repeating))
				{
					if ((rk.ReadKey(event.key.keysym.sym))&&(SoundEnabled)&&(!NoSound))Mix_PlayChannel(1,typingChunk,0);
					repeating = true;
					time=SDL_GetTicks();
				}
			}

		}	//while(event)

		if (SDL_GetTicks()>(time+repeatDelay))
		{
			time = SDL_GetTicks();
			keys = SDL_GetKeyState(NULL);
			if ( (keys[SDLK_BACKSPACE])&&(repeating) )
			{
				if ((rk.ReadKey(SDLK_BACKSPACE))&&(SoundEnabled)&&(!NoSound))Mix_PlayChannel(1,typingChunk,0);
			}
			else
				repeating = false;
		}

		SDL_Flip(screen); //Update screen
	}	//while(!done)
	strcpy(name,rk.GetString());
	bScreenLocked = false;
	showDialog = false;
	return accept;
}

//Draws the highscores
void DrawHighscores(int x, int y, bool endless)
{
	MakeBackground(xsize,ysize);
	DrawIMG(background,screen,0,0);
	if (endless) nf_standard_blue_font.draw(x+100,y+100,_("Endless:") );
	else nf_standard_blue_font.draw(x+100,y+100,_("Time Trial:") );
	for (int i =0; i<10; i++)
	{
		char playerScore[32];
		char playerName[32];
		if (endless)
		{
			sprintf(playerScore, "%i", theTopScoresEndless.getScoreNumber(i));
		}
		else
		{
			sprintf(playerScore, "%i", theTopScoresTimeTrial.getScoreNumber(i));
		}
		if (endless)
		{
			strcpy(playerName,theTopScoresEndless.getScoreName(i));
		}
		else
		{
			strcpy(playerName,theTopScoresTimeTrial.getScoreName(i));
		}
		nf_standard_blue_font.draw(x+420,y+150+i*35,playerScore);
		nf_standard_blue_font.draw(x+60,y+150+i*35,playerName);
	}
}

void DrawStats()
{
	MakeBackground(xsize,ysize);
	DrawIMG(background,screen,0,0);
	int y = 5;
	const int y_spacing = 30;
	NFont_Write(screen, 10,y,_("Stats") );
	y+=y_spacing*2;
	NFont_Write(screen, 10,y,_("Chains") );
	for(int i=2; i<13; i++)
	{
		y+=y_spacing;
		NFont_Write(screen, 10,y,(itoa(i)+"X").c_str());
		string numberAsString = itoa(Stats::getInstance()->getNumberOf("chainX"+itoa(i)));
		NFont_Write(screen, 300,y,numberAsString.c_str());
	}
	y+=y_spacing*2;
	NFont_Write(screen, 10,y,_("Lines Pushed: ") );
	string numberAsString = itoa(Stats::getInstance()->getNumberOf("linesPushed"));
	NFont_Write(screen, 300,y,numberAsString.c_str());

	y+=y_spacing;
	NFont_Write(screen, 10,y, _("Puzzles solved: ") );
	numberAsString = itoa(Stats::getInstance()->getNumberOf("puzzlesSolved"));
	NFont_Write(screen, 300,y,numberAsString.c_str());

	y+=y_spacing*2;
	NFont_Write(screen, 10,y, _("Run time: ") );
	commonTime ct = TimeHandler::peekTime("totalTime",TimeHandler::ms2ct(SDL_GetTicks()));
	y+=y_spacing;
	NFont_Write(screen, 10,y,((string)( _("Days: ")+itoa(ct.days))).c_str());
	y+=y_spacing;
	NFont_Write(screen, 10,y,((string)( _("Hours: ")+itoa(ct.hours))).c_str());
	y+=y_spacing;
	NFont_Write(screen, 10,y,((string)( _("Minutes: ")+itoa(ct.minutes))).c_str());
	y+=y_spacing;
	NFont_Write(screen, 10,y,((string)( _("Seconds: ")+itoa(ct.seconds))).c_str());

	y-=y_spacing*4; //Four rows back
	const int x_offset3 = xsize/3+10; //Ofset for three rows
	NFont_Write(screen, x_offset3,y, _("Play time: ") );
	ct = TimeHandler::getTime("playTime");
	y+=y_spacing;
	NFont_Write(screen, x_offset3,y,((string)( _("Days: ")+itoa(ct.days))).c_str());
	y+=y_spacing;
	NFont_Write(screen, x_offset3,y,((string)( _("Hours: ")+itoa(ct.hours))).c_str());
	y+=y_spacing;
	NFont_Write(screen, x_offset3,y,((string)( _("Minutes: ")+itoa(ct.minutes))).c_str());
	y+=y_spacing;
	NFont_Write(screen, x_offset3,y,((string)( _("Seconds: ")+itoa(ct.seconds))).c_str());

	const int x_offset = xsize/2+10;
	y = 5+y_spacing*2;
	NFont_Write(screen, x_offset,y, _("VS CPU (win/loss)") );
	for(int i=0; i<7; i++)
	{
		y += y_spacing;
		NFont_Write(screen, x_offset,y,string("AI "+itoa(i+1)).c_str());
		numberAsString = itoa(Stats::getInstance()->getNumberOf("defeatedAI"+itoa(i)));
		string numberAsString2 = itoa(Stats::getInstance()->getNumberOf("defeatedByAI"+itoa(i)));
		string toPrint = numberAsString + "/" + numberAsString2;
		NFont_Write(screen, x_offset+230,y,toPrint.c_str());
	}
}

void OpenScoresDisplay()
{
	int mousex,mousey;
	bool done = false;     //We are done!
	int page = 0;
	const int numberOfPages = 3;
	//button coodinates:
	const int scoreX = buttonXsize*2;
	const int scoreY = 0;
	const int backX = 20;
	const int backY = ysize-buttonYsize-20;
	const int nextX = xsize-buttonXsize-20;
	const int nextY = backY;
	while (!done && !Config::getInstance()->isShuttingDown())
	{
		switch(page)
		{
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

		//Draw buttons:
		DrawIMG(bHighScore,screen,scoreX,scoreY);
		DrawIMG(bBack,screen,backX,backY);
		DrawIMG(bNext,screen,nextX,nextY);

		//Draw page number
		string pageXofY = (format(_("Page %1% of %2%") )%(page+1)%numberOfPages).str();
		NFont_Write(screen, xsize/2-nf_standard_blue_font.getWidth( pageXofY.c_str())/2,ysize-60,pageXofY.c_str());

		SDL_Delay(10);
		SDL_Event event;

		SDL_GetMouseState(&mousex,&mousey);

		while ( SDL_PollEvent(&event) )
		{


			if ( event.type == SDL_QUIT )
			{
				Config::getInstance()->setShuttingDown(5);
				done = true;
			}

			if ( event.type == SDL_KEYDOWN )
			{
				if( (event.key.keysym.sym == SDLK_RIGHT))
				{
					page++;
					if(page>=numberOfPages)
						page = 0;
				}
				else if( (event.key.keysym.sym == SDLK_LEFT))
				{
					page--;
					if(page<0)
						page = numberOfPages-1;
				}
				else
					done = true;

				if ( event.key.keysym.sym == SDLK_F9 )
				{
					writeScreenShot();
				}

				if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) )
				{
					done = true;
				}
				else if ( (event.key.keysym.sym == SDLK_ESCAPE) )
				{
					done = true;
				}
			}

		}	//while(event)

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
		{
			bMouseUp=true;
		}

		if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
		{
			bMouseUp = false;

			//The Score button:
			if((mousex>scoreX) && (mousex<scoreX+buttonXsize) && (mousey>scoreY) && (mousey<scoreY+buttonYsize))
				done =true;

			//The back button:
			if((mousex>backX) && (mousex<backX+buttonXsize) && (mousey>backY) && (mousey<backY+buttonYsize))
			{
				page--;
				if(page<0)
					page = numberOfPages-1;
			}

			//The next button:
			if((mousex>nextX) && (mousex<nextX+buttonXsize) && (mousey>nextY) && (mousey<nextY+buttonYsize))
			{
				page++;
				if(page>=numberOfPages)
					page = 0;
			}
		}

		//DrawIMG(mouse,screen,mousex,mousey);
		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen); //Update screen
	}


}


//Open a puzzle file
bool OpenFileDialogbox(int x, int y, char *name)
{
	bool done = false;	//We are done!
	int mousex, mousey;
	ListFiles lf = ListFiles();
	string folder = (string)SHAREDIR+(string)"/puzzles";
	if(verboseLevel)
		cout << "Looking in " << folder << endl;
	lf.setDirectory(folder.c_str());
#ifdef __unix__
	string homeFolder = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/puzzles";
	lf.setDirectory2(homeFolder.c_str());
#endif
	MakeBackground(xsize,ysize);
	DrawIMG(background,screen,0,0);
	DrawIMG(bForward,background,x+460,y+420);
	DrawIMG(bBack,background,x+20,y+420);
	while (!done && !Config::getInstance()->isShuttingDown())
	{
		DrawIMG(background,screen,0,0);
		const int nrOfFiles = 10;
		DrawIMG(changeButtonsBack,screen,x,y);
		for (int i=0; i<nrOfFiles; i++)
		{
			NFont_Write(screen, x+10,y+10+36*i,lf.getFileName(i).c_str());
		}

		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )
			{
				Config::getInstance()->setShuttingDown(5);
				done = true;
			}

			if ( event.type == SDL_KEYDOWN )
			{
				if ( (event.key.keysym.sym == SDLK_ESCAPE) )
				{
					done = true;
				}

				if ( (event.key.keysym.sym == SDLK_RIGHT) )
				{
					lf.forward();
				}

				if ( (event.key.keysym.sym == SDLK_LEFT) )
				{
					lf.back();
				}
			}

		} //while(event)

		SDL_GetMouseState(&mousex,&mousey);

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
		{
			bMouseUp=true;
		}

		if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
		{
			bMouseUp = false;

			//The Forward Button:
			if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
			{
				lf.forward();
			}

			//The back button:
			if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
			{
				lf.back();
			}

			for (int i=0; i<10; i++)
			{
				if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) )
				{
					if (lf.fileExists(i))
					{
						strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
						done=true; //The user have, clicked the purpose of this function is now complete
					}
				}
			}
		}

		//DrawIMG(mouse,screen,mousex,mousey);
		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen); //Update screen
	}
	return true;
}

//Slelect a theme
bool SelectThemeDialogbox(int x, int y, char *name)
{
	bool done = false;	//We are done!
	int mousex, mousey;
	ListFiles lf = ListFiles();
	string folder = (string)SHAREDIR+(string)"/themes";
	if(verboseLevel)
		cout << "Looking in " << folder << endl;
	lf.setDirectory(folder.c_str());
#ifdef __unix__
	string homeFolder = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/themes";
	lf.setDirectory2(homeFolder.c_str());
#endif
	MakeBackground(xsize,ysize);
	DrawIMG(background,screen,0,0);
	DrawIMG(bForward,background,x+460,y+420);
	DrawIMG(bBack,background,x+20,y+420);
	while (!done && !Config::getInstance()->isShuttingDown())
	{
		DrawIMG(background,screen,0,0);
		const int nrOfFiles = 10;
		DrawIMG(changeButtonsBack,screen,x,y);
		for (int i=0; i<nrOfFiles; i++)
		{
			NFont_Write(screen, x+10,y+10+36*i,lf.getFileName(i).c_str());
		}

		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )
			{
				Config::getInstance()->setShuttingDown(5);
				done = true;
			}

			if ( event.type == SDL_KEYDOWN )
			{
				if ( (event.key.keysym.sym == SDLK_ESCAPE) )
				{
					done = true;
				}

				if ( (event.key.keysym.sym == SDLK_RIGHT) )
				{
					lf.forward();
				}

				if ( (event.key.keysym.sym == SDLK_LEFT) )
				{
					lf.back();
				}
			}

		} //while(event)

		SDL_GetMouseState(&mousex,&mousey);

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
		{
			bMouseUp=true;
		}

		if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
		{
			bMouseUp = false;

			//The Forward Button:
			if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
			{
				lf.forward();
			}

			//The back button:
			if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
			{
				lf.back();
			}

			for (int i=0; i<10; i++)
			{
				if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) )
				{
					if (lf.fileExists(i))
					{
						strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
						loadTheme(lf.getFileName(i));
						done=true; //The user have, clicked the purpose of this function is now complete
					}
				}
			}
		}

		//DrawIMG(mouse,screen,mousex,mousey);
		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen); //Update screen
	}
	return true;
}

//Open a saved replay
bool OpenReplayDialogbox(int x, int y, char *name)
{
	bool done = false;	//We are done!
	int mousex, mousey;
	ListFiles lf = ListFiles();
	if(verboseLevel)
		cout << "Ready to set directory!" << endl;
#ifdef __unix__
	string directory = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/replays";
#elif WIN32
	string directory = getMyDocumentsPath()+(string)"/My Games/blockattack/replays";
#else
	string directory = "./replays";
#endif
	lf.setDirectory(directory);
	if(verboseLevel)
		cout << "Directory sat" << endl;
	MakeBackground(xsize,ysize);
	DrawIMG(background,screen,0,0);
	DrawIMG(bForward,background,x+460,y+420);
	DrawIMG(bBack,background,x+20,y+420);
	while (!done && !Config::getInstance()->isShuttingDown())
	{
		DrawIMG(background,screen,0,0);
		const int nrOfFiles = 10;
		DrawIMG(changeButtonsBack,screen,x,y);
		for (int i=0; i<nrOfFiles; i++)
		{
			NFont_Write(screen, x+10,y+10+36*i,lf.getFileName(i).c_str());
		}

		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )
			{
				Config::getInstance()->setShuttingDown(5);
				done = true;
				return false;
			}

			if ( event.type == SDL_KEYDOWN )
			{
				if ( (event.key.keysym.sym == SDLK_ESCAPE) )
				{
					done = true;
					return false;
				}

				if ( (event.key.keysym.sym == SDLK_RIGHT) )
				{
					lf.forward();
				}

				if ( (event.key.keysym.sym == SDLK_LEFT) )
				{
					lf.back();
				}
			}

		} //while(event)

		SDL_GetMouseState(&mousex,&mousey);

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
		{
			bMouseUp=true;
		}

		if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
		{
			bMouseUp = false;

			//The Forward Button:
			if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
			{
				lf.forward();
			}

			//The back button:
			if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
			{
				lf.back();
			}

			for (int i=0; i<10; i++)
			{
				if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) )
				{
					if (lf.fileExists(i))
					{
						strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
						done=true; //The user have, clicked the purpose of this function is now complete
						return true;
					}
				}
			}
		}

		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen); //Update screen
	}
	return false;
}

//Draws the balls and explosions
static void DrawBalls()
{
	for (int i = 0; i< maxNumberOfBalls; i++)
	{
		if (theBallManeger.ballUsed[i])
		{
			DrawIMG(balls[theBallManeger.ballArray[i].getColor()],screen,theBallManeger.ballArray[i].getX(),theBallManeger.ballArray[i].getY());
		} //if used
		if (theExplosionManeger.explosionUsed[i])
		{
			DrawIMG(explosion[theExplosionManeger.explosionArray[i].getFrame()],screen,theExplosionManeger.explosionArray[i].getX(),theExplosionManeger.explosionArray[i].getY());
		}
		if (theTextManeger.textUsed[i])
		{
			//cout << "Printing text: " << theTextManeger.textArray[i].getText() << endl;
			int x = theTextManeger.textArray[i].getX()-12;
			int y = theTextManeger.textArray[i].getY()-12;
			DrawIMG(iChainBack,screen,x,y);
			nf_standard_small_font.drawCenter(x+12,y+7,theTextManeger.textArray[i].getText());
		}
	} //for
}    //DrawBalls

//Removes the old balls
void UndrawBalls()
{
	for (int i = 0; i< maxNumberOfBalls; i++)
	{
		if (theBallManeger.oldBallUsed[i])
		{
			DrawIMG(background,screen,theBallManeger.oldBallArray[i].getX(),theBallManeger.oldBallArray[i].getY(),ballSize,ballSize,theBallManeger.oldBallArray[i].getX(),theBallManeger.oldBallArray[i].getY());
		} //if used
		if (theExplosionManeger.oldExplosionUsed[i])
		{
			DrawIMG(background,screen,theExplosionManeger.oldExplosionArray[i].getX(),theExplosionManeger.oldExplosionArray[i].getY(),70,120,theExplosionManeger.oldExplosionArray[i].getX(),theExplosionManeger.oldExplosionArray[i].getY());
		}
		if (theTextManeger.oldTextUsed[i])
		{
			int x = theTextManeger.oldTextArray[i].getX()-12;
			int y = theTextManeger.oldTextArray[i].getY()-12;
			DrawIMG(background,screen,x,y,25,25,x,y);
		}
	} //for
}   //UndrawBalls

//draws everything
void DrawEverything(int xsize, int ysize,BlockGameSdl *theGame, BlockGameSdl *theGame2)
{
	SDL_ShowCursor(SDL_DISABLE);
	//draw background:
	if (forceredraw != 1)
	{

		UndrawBalls();
		DrawIMG(background,screen,oldMousex,oldMousey,32,32,oldMousex,oldMousey);
		DrawIMG(background,screen,oldBubleX,oldBubleY,140,50,oldBubleX,oldBubleY);


		DrawIMG(background,screen,350,200,120,200,350,200);
		DrawIMG(background,screen,830,200,120,200,830,200);
		DrawIMG(background,screen,800,0,140,50,800,0);

		DrawIMG(background,screen,50,60,300,50,50,60);
		DrawIMG(background,screen,510,60,300,50,510,60);
	}
	else
		DrawIMG(background,screen,0,0);
	//draw bottons (should be moves and drawn directly to background once)
	/*if (!editorMode)
	    #if NETWORK
	    if (!networkActive) //We don't show the menu while running server or connected to a server
	    #else
	        if(true)
	    #endif
	    {
	        //Here we draw the menu
	        bNewGame.PaintTo(screen,0,0);
	        DrawIMG(bOptions, screen, buttonXsize,0);
	        DrawIMG(bHighScore, screen, 2*buttonXsize,0);
	        DrawIMG(bReplay,screen,3*buttonXsize,0);
	    }
	    else
	    { //If network is active
	        DrawIMG(bBack, screen, 0, 0); //Display a disconnect button
	    }
	if (!editorMode)
	    DrawIMG(bExit, screen, xsize-120,ysize-120);*/
	//DrawIMG(boardBackBack,screen,theGame->GetTopX()-60,theGame->GetTopY()-68);
	DrawIMG(theGame->sBoard,screen,theGame->GetTopX(),theGame->GetTopY());
	string strHolder;
	strHolder = itoa(theGame->GetScore()+theGame->GetHandicap());
	NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+100,strHolder.c_str());
	if (theGame->GetAIenabled())
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,_("AI") );
	else if (editorMode)
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,_("Playing field") );
	else if (!singlePuzzle)
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,player1name);
	if (theGame->isTimeTrial())
	{
		int tid = (int)SDL_GetTicks()-theGame->GetGameStartedAt();
		int minutes;
		int seconds;
		if (tid>=0)
		{
			minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))/60/1000;
			seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))%(60*1000))/1000;
		}
		else
		{
			minutes = ((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))/60/1000;
			seconds = (((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))%(60*1000))/1000;
		}
		if (theGame->isGameOver()) minutes=0;
		if (theGame->isGameOver()) seconds=0;
		if (seconds>9)
			strHolder = itoa(minutes)+":"+itoa(seconds);
		else strHolder = itoa(minutes)+":0"+itoa(seconds);
		//if ((SoundEnabled)&&(!NoSound)&&(tid>0)&&(seconds<5)&&(minutes == 0)&&(seconds>1)&&(!(Mix_Playing(6)))) Mix_PlayChannel(6,heartBeat,0);
		NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+150,strHolder.c_str());
	}
	else
	{
		int minutes = ((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))/60/1000;
		int seconds = (((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))%(60*1000))/1000;
		if (theGame->isGameOver()) minutes=(theGame->GetGameEndedAt()/1000/60)%100;
		if (theGame->isGameOver()) seconds=(theGame->GetGameEndedAt()/1000)%60;
		if (seconds>9)
			strHolder = itoa(minutes)+":"+itoa(seconds);
		else
			strHolder = itoa(minutes)+":0"+itoa(seconds);
		NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+150,strHolder.c_str());
	}
	strHolder = itoa(theGame->GetChains());
	NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+200,strHolder.c_str());
	//drawspeedLevel:
	strHolder = itoa(theGame->GetSpeedLevel());
	NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+250,strHolder.c_str());
	if ((theGame->isStageClear()) &&(theGame->GetTopY()+700+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1<600+theGame->GetTopY()))
	{
		oldBubleX = theGame->GetTopX()+280;
		oldBubleY = theGame->GetTopY()+650+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1;
		DrawIMG(stageBobble,screen,theGame->GetTopX()+280,theGame->GetTopY()+650+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1);
	}
	//player1 finnish, player2 start
	//DrawIMG(boardBackBack,screen,theGame2->GetTopX()-60,theGame2->GetTopY()-68);
	if (!editorMode)
	{
		/*
		 *If single player mode (and not VS)
		 */
		if(!twoPlayers && !theGame->isGameOver())
		{
			//Blank player2's board:
			DrawIMG(backBoard,screen,theGame2->GetTopX(),theGame2->GetTopY());
			//Write a description:
			if(theGame->isTimeTrial())
			{
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+10,"Time Trial");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160,"Objective:");
				string infostring = _("Score as much as \npossible in 2 minutes");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160+32,infostring);
			}
			else if(theGame->isStageClear())
			{
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+10,"Stage Clear");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160,"Objective:");
				string infostring = _("You must clear a \nnumber of lines.\nSpeed is rapidly \nincreased.");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160+32,infostring);
			}
			else if(theGame->isPuzzleMode())
			{
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+10,"Puzzle");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160,"Objective:");
				string infostring = _("Clear the entire board \nwith a limited number \nof moves.");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160+32,infostring);
			}
			else
			{
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+10,"Endless");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160,"Objective:");
				string infostring = _("Score as much as \npossible. No time limit.");
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160+32,infostring);
			}

			//Write the keys that are in use
			int y = theGame2->GetTopY()+400;
			NFont_Write(screen, theGame2->GetTopX()+7,y,_("Movement keys:") );
			NFont_Write(screen, theGame2->GetTopX()+7,y+40,(getKeyName(keySettings[0].left)+", "+getKeyName(keySettings[0].right)+"," ).c_str() );
			NFont_Write(screen, theGame2->GetTopX()+7,y+76,(getKeyName(keySettings[0].up)+", "+getKeyName(keySettings[0].down)).c_str() );
			NFont_Write(screen, theGame2->GetTopX()+7,y+120,( _("Switch: ")+getKeyName(keySettings[0].change) ).c_str() );
			if(theGame->isPuzzleMode())
				NFont_Write(screen, theGame2->GetTopX()+7,y+160,( _("Restart: ")+getKeyName(keySettings[0].push) ).c_str() );
			else
				NFont_Write(screen, theGame2->GetTopX()+7,y+160,( _("Push line: ")+getKeyName(keySettings[0].push) ).c_str() );
		}
		else
			DrawIMG(theGame2->sBoard,screen,theGame2->GetTopX(),theGame2->GetTopY());
		strHolder = itoa(theGame2->GetScore()+theGame2->GetHandicap());
		NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+100,strHolder.c_str());
		if (theGame2->GetAIenabled())
			NFont_Write(screen, theGame2->GetTopX()+10,theGame2->GetTopY()-34,_("AI") );
		else
			NFont_Write(screen, theGame2->GetTopX()+10,theGame2->GetTopY()-34,theGame2->name);
		if (theGame2->isTimeTrial())
		{
			int tid = (int)SDL_GetTicks()-theGame2->GetGameStartedAt();
			int minutes;
			int seconds;
			if (tid>=0)
			{
				minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))/60/1000;
				seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))%(60*1000))/1000;
			}
			else
			{
				minutes = ((abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))/60/1000;
				seconds = (((abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))%(60*1000))/1000;
			}
			if (theGame2->isGameOver()) minutes=0;
			if (theGame2->isGameOver()) seconds=0;
			if (seconds>9)
				strHolder = itoa(minutes)+":"+itoa(seconds);
			else
				strHolder = itoa(minutes)+":0"+itoa(seconds);
			//if ((SoundEnabled)&&(!NoSound)&&(tid>0)&&(seconds<5)&&(minutes == 0)&&(seconds>1)&&(!(Mix_Playing(6)))) Mix_PlayChannel(6,heartBeat,0);
			NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+150,strHolder.c_str());
		}
		else
		{
			int minutes = (abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt()))/60/1000;
			int seconds = (abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())%(60*1000))/1000;
			if (theGame2->isGameOver()) minutes=(theGame2->GetGameEndedAt()/1000/60)%100;
			if (theGame2->isGameOver()) seconds=(theGame2->GetGameEndedAt()/1000)%60;
			if (seconds>9)
				strHolder = itoa(minutes)+":"+itoa(seconds);
			else
				strHolder = itoa(minutes)+":0"+itoa(seconds);
			NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+150,strHolder.c_str());
		}
		strHolder = itoa(theGame2->GetChains());
		NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+200,strHolder.c_str());
		strHolder = itoa(theGame2->GetSpeedLevel());
		NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+250,strHolder.c_str());
	}
	//player2 finnish


	DrawBalls();

#if DEBUG
	Frames++;
	if (SDL_GetTicks() >= Ticks + 1000)
	{
		if (Frames > 999) Frames=999;
		sprintf(FPS, "%lu fps", Frames);
		Frames = 0;
		Ticks = SDL_GetTicks();
	}

	//NFont_Write(screen, 800,4,FPS);
	nf_standard_blue_font.draw(800,4,FPS);
#endif

	//SDL_Flip(screen); Update screen is now called outside DrawEvrything, bacause the mouse needs to be painted

}

//Generates the standard background
static void MakeBackground(int xsize,int ysize)
{
	int w = backgroundImage->w;
	int h = backgroundImage->h;
	for(int i=0; i*w<xsize; i++)
		for(int j=0; j*h<ysize; j++)
			DrawIMG(backgroundImage,background,i*w,j*h);
	standardBackground = true;
}

//Generates the background with red board backs
static void MakeBackground(int xsize,int ysize,BlockGame *theGame, BlockGame *theGame2)
{
	MakeBackground(xsize,ysize);
	DrawIMG(boardBackBack,background,theGame->GetTopX()-60,theGame->GetTopY()-68);
	DrawIMG(boardBackBack,background,theGame2->GetTopX()-60,theGame2->GetTopY()-68);
	standardBackground = false;
}

static void MakeBackground(int xsize, int ysize, BlockGame *theGame)
{
	MakeBackground(xsize,ysize);
	DrawIMG(boardBackBack,background,theGame->GetTopX()-60,theGame->GetTopY()-68);
	standardBackground = false;
}


//The function that allows the player to choose PuzzleLevel
int PuzzleLevelSelect(int Type)
{
	const int xplace = 200;
	const int yplace = 300;
	int levelNr, mousex, mousey, oldmousex, oldmousey;
	bool levelSelected = false;
	bool tempBool;
	int nrOfLevels;
	Uint32 tempUInt32;
	Uint32 totalScore = 0;
	Uint32 totalTime = 0;
	int selected = 0;

	//Loads the levels, if they havn't been loaded:
	if(Type == 0)
		LoadPuzzleStages();

	//Keeps track of background;
	SDL_GetTicks();

	MakeBackground(xsize,ysize);
	if(Type == 0)
	{
		ifstream puzzleFile(puzzleSavePath.c_str(),ios::binary);
		if (puzzleFile)
		{
			for (int i=0; (i<nrOfPuzzles)&&(!puzzleFile.eof()); i++)
			{
				puzzleFile.read(reinterpret_cast<char*>(&tempBool),sizeof(bool));
				puzzleCleared[i] = tempBool;
			}
			puzzleFile.close();
		}
		else
		{
			tempBool = false;
			for (int i=0; i<nrOfPuzzles; i++)
				puzzleCleared[i] = tempBool;
		}
		nrOfLevels = nrOfPuzzles;
	}
	if(Type == 1)
	{
		ifstream stageFile(stageClearSavePath.c_str(),ios::binary);
		if (stageFile)
		{
			for (int i = 0; i<nrOfStageLevels; i++)
			{
				stageFile.read(reinterpret_cast<char*>(&tempBool),sizeof(bool));
				stageCleared[i]=tempBool;
			}
			if(!stageFile.eof())
			{
				for(int i=0; i<nrOfStageLevels; i++)
				{
					tempUInt32 = 0;
					if(!stageFile.eof())
						stageFile.read(reinterpret_cast<char*>(&tempUInt32),sizeof(Uint32));
					stageScores[i]=tempUInt32;
					totalScore+=tempUInt32;
				}
				for(int i=0; i<nrOfStageLevels; i++)
				{
					tempUInt32 = 0;
					if(!stageFile.eof())
						stageFile.read(reinterpret_cast<char*>(&tempUInt32),sizeof(Uint32));
					stageTimes[i]=tempUInt32;
					totalTime += tempUInt32;
				}
			}
			else
			{
				for(int i=0; i<nrOfStageLevels; i++)
				{
					stageScores[i]=0;
					stageTimes[i]=0;
				}
			}
			stageFile.close();
		}
		else
		{
			for (int i=0; i<nrOfStageLevels; i++)
			{
				stageCleared[i]= false;
				stageScores[i]=0;
				stageTimes[i]=0;
			}
		}
		nrOfLevels = nrOfStageLevels;
	}

	while(!levelSelected)
	{
		SDL_GetTicks();


		DrawIMG(background, screen, 0, 0);
		DrawIMG(iCheckBoxArea,screen,xplace,yplace);
		if(Type == 0)
			NFont_Write(screen, xplace+12,yplace+2,_("Select Puzzle") );
		if(Type == 1)
			NFont_Write(screen, xplace+12,yplace+2, _("Stage Clear Level Select") );
		//Now drow the fields you click in (and a V if clicked):
		for (int i = 0; i < nrOfLevels; i++)
		{
			DrawIMG(iLevelCheckBox,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			if(i==selected) DrawIMG(iLevelCheckBoxMarked,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			if (Type == 0 && puzzleCleared.at(i)==true) DrawIMG(iLevelCheck,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			if (Type == 1 && stageCleared.at(i)==true) DrawIMG(iLevelCheck,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
		}

		SDL_Event event;
		while ( SDL_PollEvent(&event) )
			if ( event.type == SDL_KEYDOWN )
			{
				if ( event.key.keysym.sym == SDLK_ESCAPE )
				{
					levelNr = -1;
					levelSelected = true;
				}
				if ( event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER )
				{
					levelNr = selected;
					levelSelected = true;
				}
				if ( event.key.keysym.sym == SDLK_RIGHT )
				{
					++selected;
					if(selected >= nrOfLevels)
						selected = 0;
				}
				if ( event.key.keysym.sym == SDLK_LEFT )
				{
					--selected;
					if(selected < 0)
						selected = nrOfLevels-1;
				}
				if ( event.key.keysym.sym == SDLK_DOWN )
				{
					selected+=10;
					if(selected >= nrOfLevels)
						selected-=10;
				}
				if ( event.key.keysym.sym == SDLK_UP )
				{
					selected-=10;
					if(selected < 0)
						selected+=10;
				}
			}

		SDL_GetKeyState(NULL);

		SDL_GetMouseState(&mousex,&mousey);
		if(mousex != oldmousex || mousey != oldmousey)
		{
			int tmpSelected = -1;
			int j;
			for (j = 0; (tmpSelected == -1) && ( (j<nrOfLevels/10)||((j<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)) ); j++)
				if ((60+j*50<mousey-yplace)&&(mousey-yplace<j*50+92))
					tmpSelected = j*10;
			if (tmpSelected != -1)
				for (int k = 0; (( (!(nrOfLevels%10) || k<nrOfLevels-10*(j-1)) )&&(k<10)); k++)
					if ((10+k*50<mousex-xplace)&&(mousex-xplace<k*50+42))
					{
						tmpSelected +=k;
						selected = tmpSelected;
					}
		}
		oldmousey = mousey;
		oldmousex= mousex;

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
		{
			bMouseUp=true;
		}

		if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
		{
			bMouseUp = false;

			int levelClicked = -1;
			int i;
			for (i = 0; (i<nrOfLevels/10)||((i<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)); i++)
				if ((60+i*50<mousey-yplace)&&(mousey-yplace<i*50+92))
					levelClicked = i*10;
			i++;
			if (levelClicked != -1)
				for (int j = 0; ((j<nrOfStageLevels%(i*10))&&(j<10)); j++)
					if ((10+j*50<mousex-xplace)&&(mousex-xplace<j*50+42))
					{
						levelClicked +=j;
						levelSelected = true;
						levelNr = levelClicked;
					}
		}

		if(Type == 1)
		{
			string scoreString = _("Best score: 0");
			string timeString = _("Time used: -- : --");

			if(stageScores.at(selected)>0)
				scoreString = _("Best score: ")+itoa(stageScores.at(selected));
			if(stageTimes.at(selected)>0)
				timeString = _("Time used: ")+itoa(stageTimes.at(selected)/1000/60)+" : "+itoa2((stageTimes.at(selected)/1000)%60);

			NFont_Write(screen, 200,200,scoreString.c_str());
			NFont_Write(screen, 200,250,timeString.c_str());
			string totalString = (format("Total score: %1% in %2%:%3%")%totalScore%(totalTime/1000/60)%((totalTime/1000)%60)).str(); //"Total score: " +itoa(totalScore) + " in " + itoa(totalTime/1000/60) + " : " + itoa2((totalTime/1000)%60);
			NFont_Write(screen, 200,600,totalString.c_str());
		}

		//DrawIMG(mouse,screen,mousex,mousey);
		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen); //draws it all to the screen

	}
	DrawIMG(background, screen, 0, 0);
	return levelNr;
}


//The function that allows the player to choose Level number
void startVsMenu()
{
	const int xplace = 200;
	const int yplace = 100;
	int mousex, mousey;
	bool done = false;

	//Keeps track of background;
	//int nowTime=SDL_GetTicks();

	MakeBackground(xsize,ysize);
	NFont_Write(background, 360,650, _("Press ESC to accept") );
	DrawIMG(bBack,background,xsize/2-120/2,600);
	do
	{
		//nowTime=SDL_GetTicks();
		DrawIMG(background, screen, 0, 0);
		DrawIMG(changeButtonsBack,screen,xplace,yplace);
		NFont_Write(screen, xplace+50,yplace+20,"Player 1");
		NFont_Write(screen, xplace+300+50,yplace+20,"Player 2");
		NFont_Write(screen, xplace+50,yplace+70,"Speed:");
		NFont_Write(screen, xplace+50+300,yplace+70,"Speed:");
		for (int i=0; i<5; i++)
		{
			char levelS[2]; //level string;
			levelS[0]='1'+i;
			levelS[1]=0;
			NFont_Write(screen, xplace+50+i*40,yplace+110,levelS);
			DrawIMG(iLevelCheckBox,screen,xplace+50+i*40,yplace+150);
			if (player1Speed==i)
				DrawIMG(iLevelCheck,screen,xplace+50+i*40,yplace+150);
		}
		for (int i=0; i<5; i++)
		{
			char levelS[2]; //level string;
			levelS[0]='1'+i;
			levelS[1]=0;
			NFont_Write(screen, xplace+300+50+i*40,yplace+110,levelS);
			DrawIMG(iLevelCheckBox,screen,xplace+300+50+i*40,yplace+150);
			if (player2Speed==i)
				DrawIMG(iLevelCheck,screen,xplace+300+50+i*40,yplace+150);
		}
		NFont_Write(screen, xplace+50,yplace+200,"AI: ");
		DrawIMG(iLevelCheckBox,screen,xplace+50+70,yplace+200);
		if (player1AI)
			DrawIMG(iLevelCheck,screen,xplace+50+70,yplace+200);
		NFont_Write(screen, xplace+50,yplace+250,"TT Handicap: ");
		NFont_Write(screen, xplace+50+300,yplace+200,"AI: ");
		DrawIMG(iLevelCheckBox,screen,xplace+50+70+300,yplace+200);
		if (player2AI)
			DrawIMG(iLevelCheck,screen,xplace+50+70+300,yplace+200);
		NFont_Write(screen, xplace+50+300,yplace+250,"TT Handicap: ");
		for (int i=0; i<5; i++)
		{
			char levelS[2]; //level string;
			levelS[0]='1'+i;
			levelS[1]=0;
			NFont_Write(screen, xplace+50+i*40,yplace+290,levelS);
			DrawIMG(iLevelCheckBox,screen,xplace+50+i*40,yplace+330);
			if (player1handicap==i)
				DrawIMG(iLevelCheck,screen,xplace+50+i*40,yplace+330);
		}
		for (int i=0; i<5; i++)
		{
			char levelS[2]; //level string;
			levelS[0]='1'+i;
			levelS[1]=0;
			NFont_Write(screen, xplace+50+i*40+300,yplace+290,levelS);
			DrawIMG(iLevelCheckBox,screen,xplace+50+i*40+300,yplace+330);
			if (player2handicap==i)
				DrawIMG(iLevelCheck,screen,xplace+50+i*40+300,yplace+330);
		}

		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_KEYDOWN )
			{
				if ( event.key.keysym.sym == SDLK_ESCAPE )
				{
					done = true;
				}
				if ( event.key.keysym.sym == SDLK_RETURN )
				{
					done = true;
				}
				if ( event.key.keysym.sym == SDLK_KP_ENTER )
				{
					done = true;
				}
			}
		}

		SDL_GetKeyState(NULL);

		SDL_GetMouseState(&mousex,&mousey);

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
		{
			bMouseUp=true;
		}

		if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
		{
			bMouseUp = false;

			if ((mousex>xplace+50+70)&&(mousey>yplace+200)&&(mousex<xplace+50+70+30)&&(mousey<yplace+200+30))
				player1AI=!player1AI;
			if ((mousex>xplace+50+70+300)&&(mousey>yplace+200)&&(mousex<xplace+50+70+30+300)&&(mousey<yplace+200+30))
				player2AI=!player2AI;
			for (int i=0; i<5; i++)
			{
				if ((mousex>xplace+50+i*40)&&(mousex<xplace+50+i*40+30)&&(mousey>yplace+150)&&(mousey<yplace+150+30))
					player1Speed=i;
			}
			for (int i=0; i<5; i++)
			{
				if ((mousex>xplace+50+i*40+300)&&(mousex<xplace+50+i*40+30+300)&&(mousey>yplace+150)&&(mousey<yplace+150+30))
					player2Speed=i;
			}
			for (int i=0; i<5; i++)
			{
				if ((mousex>xplace+50+i*40)&&(mousex<xplace+50+i*40+30)&&(mousey>yplace+330)&&(mousey<yplace+330+30))
					player1handicap=i;
			}
			for (int i=0; i<5; i++)
			{
				if ((mousex>xplace+50+i*40+300)&&(mousex<xplace+50+i*40+30+300)&&(mousey>yplace+330)&&(mousey<yplace+330+30))
					player2handicap=i;
			}
			if ((mousex>xsize/2-120/2)&&(mousex<xsize/2+120/2)&&(mousey>600)&&(mousey<640))
				done = true;
		}

		//DrawIMG(mouse,screen,mousex,mousey);
		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen); //draws it all to the screen
		SDL_Delay(10);

	}
	while (!done && !Config::getInstance()->isShuttingDown());
	DrawIMG(background, screen, 0, 0);
}

//This function will promt for the user to select another file for puzzle mode
void changePuzzleLevels()
{
	char theFileName[30];
	strcpy(theFileName,puzzleName.c_str());
	for (int i=puzzleName.length(); i<30; i++)
		theFileName[i]=' ';
	theFileName[29]=0;
	if (OpenFileDialogbox(200,100,theFileName))
	{
		for (int i=28; ((theFileName[i]==' ')&&(i>0)); i--)
			theFileName[i]=0;
		puzzleName = theFileName;
#if defined(__unix__)
		string home = getenv("HOME");
		puzzleSavePath = home+"/.gamesaves/blockattack/"+puzzleName+".save";
#elif defined(_WIN32)
		string home = getMyDocumentsPath();
		if (&home!=NULL)
		{
			puzzleSavePath = home+"/My Games/blockattack/"+puzzleName+".save";
		}
		else
		{
			puzzleSavePath = puzzleName+".save";
		}
#else
		puzzleSavePath = puzzleName+".save";
#endif
	}

}

#if NETWORK
#include "NetworkThing.hpp"
//#include "MenuSystem.h"
NetworkThing nt;
#endif

static BlockGameSdl *player1;
static BlockGameSdl *player2;

static void StartSinglePlayerEndless()
{
	//1 player - endless
	player1->NewGame(50,100,SDL_GetTicks());
	player1->putStartBlocks(time(0));
	bNewGameOpen = false;
	b1playerOpen = false;
	twoPlayers =false;
	player2->SetGameOver();
	showGame = true;
	strcpy(player1->name, player1name);
	strcpy(player2->name, player2name);
}

static void StartSinglePlayerTimeTrial()
{
	player1->NewTimeTrialGame(50,100,SDL_GetTicks());
	closeAllMenus();
	twoPlayers =false;
	player2->SetGameOver();
	showGame = true;
	//vsMode = false;
	strcpy(player1->name, player1name);
	strcpy(player2->name, player2name);
}

static int StartSinglePlayerPuzzle(int level)
{
	int myLevel = PuzzleLevelSelect(0);
	if(myLevel == -1)
		return 1;
	player1->NewPuzzleGame(myLevel,50,100,SDL_GetTicks());
	MakeBackground(xsize,ysize,player1,player2);
	DrawIMG(background, screen, 0, 0);
	closeAllMenus();
	twoPlayers = false;
	player2->SetGameOver();
	showGame = true;
	//vsMode = true;
	strcpy(player1->name, player1name);
	strcpy(player2->name, player2name);
	return 0;
}


static void StarTwoPlayerTimeTrial()
{
	player1->NewTimeTrialGame(50,100,SDL_GetTicks());
	player2->NewTimeTrialGame(xsize-500,100,SDL_GetTicks());
	int theTime = time(0);
	player1->putStartBlocks(theTime);
	player2->putStartBlocks(theTime);
	closeAllMenus();
	twoPlayers = true;
	player1->setGameSpeed(player1Speed);
	player2->setGameSpeed(player2Speed);
	player1->setHandicap(player1handicap);
	player2->setHandicap(player2handicap);
	if(player1AI)
		player1->setAIlevel(player1AIlevel);
	if(player2AI)
		player2->setAIlevel(player2AIlevel);
	strcpy(player1->name, player1name);
	strcpy(player2->name, player2name);
}

static void StartTwoPlayerVs()
{
	//2 player - VsMode
	player1->NewVsGame(50,100,player2,SDL_GetTicks());
	player2->NewVsGame(xsize-500,100,player1,SDL_GetTicks());
	bNewGameOpen = false;
	//vsMode = true;
	twoPlayers = true;
	b2playersOpen = false;
	player1->setGameSpeed(player1Speed);
	player2->setGameSpeed(player2Speed);
	player1->setHandicap(player1handicap);
	player2->setHandicap(player2handicap);
	if(player1AI)
		player1->setAIlevel(player1AIlevel);
	if(player2AI)
		player2->setAIlevel(player2AIlevel);
	int theTime = time(0);
	player1->putStartBlocks(theTime);
	player2->putStartBlocks(theTime);
	strcpy(player1->name, player1name);
	strcpy(player2->name, player2name);
}

static void StartReplay(string filename)
{
	Replay r1,r2;
	r1.loadReplay(filename);
	player1->playReplay(50,100,SDL_GetTicks(),r1);
	r2.loadReplay2(filename);
	player2->playReplay(xsize-500,100,SDL_GetTicks(),r2);
}

static void StartHostServer()
{
	player1->SetGameOver();
	player2->SetGameOver();
	nt.startServer();
}

static void StartJoinServer()
{
	player1->SetGameOver();
	player2->SetGameOver();
	string server = Config::getInstance()->getString("address0");
	nt.connectToServer(server.substr(0,server.find(" ")));
}


//The main function, quite big... too big
int main(int argc, char *argv[])
{
	//We first create the folder there we will save (only on UNIX systems)
	//we call the external command "mkdir"... the user might have renamed this, but we hope he hasn't
#if defined(__unix__)
	//Compiler warns about unused result. The users envisonment should normally give the user all the information he needs
	if(system("mkdir -p ~/.gamesaves/blockattack/screenshots"))
		cerr << "mkdir error creating ~/.gamesaves/blockattack/screenshots" << endl;
	if(system("mkdir -p ~/.gamesaves/blockattack/replays"))
		cerr << "mkdir error creating ~/.gamesaves/blockattack/replays" << endl;
	if(system("mkdir -p ~/.gamesaves/blockattack/puzzles"))
		cerr << "mkdir error creating ~/.gamesaves/blockattack/puzzles" << endl;
#elif defined(_WIN32)
	//Now for Windows NT/2k/xp/2k3 etc.
	string tempA = getMyDocumentsPath()+"\\My Games";
	CreateDirectory(tempA.c_str(),NULL);
	tempA = getMyDocumentsPath()+"\\My Games\\blockattack";
	CreateDirectory(tempA.c_str(),NULL);
	tempA = getMyDocumentsPath()+"\\My Games\\blockattack\\replays";
	CreateDirectory(tempA.c_str(),NULL);
	tempA = getMyDocumentsPath()+"\\My Games\\blockattack\\screenshots";
	CreateDirectory(tempA.c_str(),NULL);
#endif
	highPriority = false;	//if true the game will take most resources, but increase framerate.
	bFullscreen = false;
	//Set default Config variables:
	Config::getInstance()->setDefault("themename","default");
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
	if (argc > 1)
	{
		int argumentNr = 1;
		forceredraw = 2;
		while (argc>argumentNr)
		{
			const char helpString[] = "--help";
			const char priorityString[] = "-priority";
			const char forceRedrawString[] = "-forceredraw";
			const char forcepartdrawString[] = "-forcepartdraw";
			const char singlePuzzleString[] = "-SP";
			const char noSoundAtAll[] = "-nosound";
			const char IntegratedEditor[] = "-editor";
			const char selectTheme[] = "-theme";
			const char verbose[] = "-v";
			if (!(strncmp(argv[argumentNr],helpString,6)))
			{
				cout << "Block Attack Help" << endl << "--help  " << _("Displays this message") <<
					 endl << "-priority  " << _("Starts game in high priority") << endl <<
					 "-forceredraw  " << _("Redraw the whole screen every frame, prevents garbage") << endl <<
					 "-forcepartdraw  " << _("Only draw what is changed, sometimes cause garbage") << endl <<
					 "-nosound  " << _("No sound will be played at all, and sound hardware will not be loaded (use this if game crashes because of sound)") << endl <<
					 "-theme <" << _("THEMENAME") << ">  " << _("Changes to the theme <THEMENAME> on startup") << endl <<
					 "-editor  " << _("Starts the build-in editor (not yet integrated)") << endl;
#ifdef WIN32
				system("Pause");
#endif
				return 0;
			}
			if (!(strncmp(argv[argumentNr],priorityString,9)))
			{
				if(verboseLevel)
					cout << "Priority mode" << endl;
				highPriority = true;
			}
			if (!(strncmp(argv[argumentNr],forceRedrawString,12)))
			{
				forceredraw = 1;
			}
			if (!(strncmp(argv[argumentNr],forcepartdrawString,14)))
			{
				forceredraw = 2;
			}
			if (!(strncmp(argv[argumentNr],singlePuzzleString,3)))
			{
				singlePuzzle = true; //We will just have one puzzle
				if (argv[argumentNr+1][1]!=0)
					singlePuzzleNr = (argv[argumentNr+1][1]-'0')+(argv[argumentNr+1][0]-'0')*10;
				else
					singlePuzzleNr = (argv[argumentNr+1][0]-'0');
				singlePuzzleFile = argv[argumentNr+2];
				argumentNr+=2;
				if(verboseLevel)
					cout << "SinglePuzzleMode, File: " << singlePuzzleFile << " and Level: " << singlePuzzleNr << endl;
			}
			if (!(strncmp(argv[argumentNr],noSoundAtAll,8)))
			{
				NoSound = true;
			}
			if (!(strncmp(argv[argumentNr],IntegratedEditor,7)))
			{
#if LEVELEDITOR
				editorMode = true;
				if(verboseLevel)
					cout << "Integrated Puzzle Editor Activated" << endl;
#else
				cout << "Integrated Puzzle Editor was disabled at compile time" << endl;
				return -1;
#endif
			}
			if(!(strncmp(argv[argumentNr],selectTheme,6)))
			{
				argumentNr++; //Go to themename (the next argument)
				if(verboseLevel)
					cout << "Theme set to \"" << argv[argumentNr] << '"' << endl;
				Config::getInstance()->setString("themename",argv[argumentNr]);
			}
			if(!(strcmp(argv[argumentNr],verbose)))
			{
				verboseLevel++;
			}
			argumentNr++;
		}   //while
	}   //if

	SoundEnabled = true;
	MusicEnabled = true;
	showOptions = false;
	b1playerOpen = false;
	b2playersOpen = false;
	bReplayOpen = false;
	bScreenLocked = false;
	twoPlayers = false;	//true if two players splitscreen
	theTopScoresEndless = Highscore(1);
	theTopScoresTimeTrial = Highscore(2);
	drawBalls = true;
	puzzleLoaded = false;

	theBallManeger = ballManeger();
	theExplosionManeger = explosionManeger();

//We now set the paths were we are saving, we are using the keyword __unix__ . I hope that all UNIX systems has a home folder
#if defined(__unix__)
	string home = getenv("HOME");
	string optionsPath = home+"/.gamesaves/blockattack/options.dat";
#elif defined(_WIN32)
	string home = getMyDocumentsPath();
	string optionsPath;
	if (&home!=NULL) //Null if no APPDATA dir exists (win 9x)
		optionsPath = home+"/My Games/blockattack/options.dat";
	else
		optionsPath = "options.dat";
#else
	string optionsPath = "options.dat";
#endif

#if defined(__unix__)
	stageClearSavePath = home+"/.gamesaves/blockattack/stageClear.SCsave";
	puzzleSavePath = home+"/.gamesaves/blockattack/puzzle.levels.save";
#elif defined(_WIN32)
	if (&home!=NULL)
	{
		stageClearSavePath = home+"/My Games/blockattack/stageClear.SCsave";
		puzzleSavePath = home+"/My Games/blockattack/puzzle.levels.save";
	}
	else
	{
		stageClearSavePath = "stageClear.SCsave";
		puzzleSavePath = "puzzle.levels.save";
	}
#else
	stageClearSavePath = "stageClear.SCsave";
	puzzleSavePath = "puzzle.levels.save";
#endif
	puzzleName="puzzle.levels";

	//Init SDL
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		cerr << "Unable to init SDL: " << SDL_GetError() << endl;
		exit(1);
	}
	atexit(SDL_Quit);		//quits SDL when the game stops for some reason (like you hit exit or Esc)

	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

	Joypad_init();    //Prepare the joysticks
	Joypad joypad1 = Joypad();    //Creates a joypad
	Joypad joypad2 = Joypad();    //Creates a joypad

	theTextManeger = textManeger();

	//Open Audio
	if (!NoSound) //If sound has not been disabled, then load the sound system
		if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0)
		{
			cerr << "Warning: Couldn't set 44100 Hz 16-bit audio - Reason: " << SDL_GetError() << endl
				 << "Sound will be disabled!" << endl;
			NoSound = true; //Tries to stop all sound from playing/loading
		}

	SDL_WM_SetCaption("Block Attack - Rise of the Blocks", NULL); //Sets title line

	if(verboseLevel)
	{
		//Copyright notice:
		cout << "Block Attack - Rise of the Blocks (" << VERSION_NUMBER << ")" << endl << "http://blockattack.sf.net" << endl << "Copyright 2004-2011 Poul Sander" << endl <<
			 "A SDL based game (see www.libsdl.org)" << endl <<
			 "The game is availeble under the GPL, see COPYING for details." << endl;
#if defined(_WIN32)
		cout << "Windows build" << endl;
#elif defined(__linux__)
		cout << "Linux build" <<  endl;
#elif defined(__unix__)
		cout << "Unix build" <<  endl;
#else
		cout << "Alternative build" << endl;
#endif
		cout << "-------------------------------------------" << endl;
	}


	keySettings[0].up= SDLK_UP;
	keySettings[0].down = SDLK_DOWN;
	keySettings[0].left = SDLK_LEFT;
	keySettings[0].right = SDLK_RIGHT;
	keySettings[0].change = SDLK_RCTRL;
	keySettings[0].push = SDLK_RSHIFT;

	keySettings[2].up= SDLK_w;
	keySettings[2].down = SDLK_s;
	keySettings[2].left = SDLK_a;
	keySettings[2].right = SDLK_d;
	keySettings[2].change = SDLK_LCTRL;
	keySettings[2].push = SDLK_LSHIFT;

	player1keys=0;
	player2keys=2;

	strcpy(player1name, "Player 1                    \0");
	strcpy(player2name, "Player 2                    \0");

	Config *configSettings = Config::getInstance();
	//configSettings->setString("aNumber"," A string");
	//configSettings->save();
	if(configSettings->exists("fullscreen")) //Test if an configFile exists
	{
		bFullscreen = (bool)configSettings->getInt("fullscreen");
		MusicEnabled = (bool)configSettings->getInt("musicenabled");
		SoundEnabled = (bool)configSettings->getInt("soundenabled");
		mouseplay1 = (bool)configSettings->getInt("mouseplay1");
		mouseplay2 = (bool)configSettings->getInt("mouseplay2");
		joyplay1 = (bool)configSettings->getInt("joypad1");
		joyplay2 = (bool)configSettings->getInt("joypad2");

		if(configSettings->exists("player1keyup")) keySettings[0].up = (SDLKey)configSettings->getInt("player1keyup");
		if(configSettings->exists("player1keydown")) keySettings[0].down = (SDLKey)configSettings->getInt("player1keydown");
		if(configSettings->exists("player1keyleft")) keySettings[0].left = (SDLKey)configSettings->getInt("player1keyleft");
		if(configSettings->exists("player1keyright")) keySettings[0].right = (SDLKey)configSettings->getInt("player1keyright");
		if(configSettings->exists("player1keychange")) keySettings[0].change = (SDLKey)configSettings->getInt("player1keychange");
		if(configSettings->exists("player1keypush")) keySettings[0].push = (SDLKey)configSettings->getInt("player1keypush");

		if(configSettings->exists("player2keyup")) keySettings[2].up = (SDLKey)configSettings->getInt("player2keyup");
		if(configSettings->exists("player2keydown")) keySettings[2].down = (SDLKey)configSettings->getInt("player2keydown");
		if(configSettings->exists("player2keyleft")) keySettings[2].left = (SDLKey)configSettings->getInt("player2keyleft");
		if(configSettings->exists("player2keyright")) keySettings[2].right = (SDLKey)configSettings->getInt("player2keyright");
		if(configSettings->exists("player2keychange")) keySettings[2].change = (SDLKey)configSettings->getInt("player2keychange");
		if(configSettings->exists("player2keypush")) keySettings[2].push = (SDLKey)configSettings->getInt("player2keypush");
		if(configSettings->exists("player1name"))
			strncpy(player1name,(configSettings->getString("player1name")).c_str(),28);
		if(configSettings->exists("player2name"))
			strncpy(player2name,(configSettings->getString("player2name")).c_str(),28);
		if(verboseLevel)
			cout << "Data loaded from config file" << endl;
	}
	else
	{
		//Reads options from file:
		ifstream optionsFile(optionsPath.c_str(), ios::binary);
		if (optionsFile)
		{
			//reads data: xsize,ysize,fullescreen, player1keys, player2keys, MusicEnabled, SoundEnabled,player1name,player2name
			optionsFile.read(reinterpret_cast<char*>(&xsize), sizeof(int));
			optionsFile.read(reinterpret_cast<char*>(&ysize), sizeof(int));
			optionsFile.read(reinterpret_cast<char*>(&bFullscreen), sizeof(bool));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[0].up), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[0].down), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[0].left), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[0].right), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[0].change), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[0].push), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[2].up), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[2].down), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[2].left), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[2].right), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[2].change), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&keySettings[2].push), sizeof(SDLKey));
			optionsFile.read(reinterpret_cast<char*>(&MusicEnabled), sizeof(bool));
			optionsFile.read(reinterpret_cast<char*>(&SoundEnabled), sizeof(bool));
			optionsFile.read(player1name, 30*sizeof(char));
			optionsFile.read(player2name, 30*sizeof(char));
			//mouseplay?
			if (!optionsFile.eof())
			{
				optionsFile.read(reinterpret_cast<char*>(&mouseplay1), sizeof(bool));
				optionsFile.read(reinterpret_cast<char*>(&mouseplay2), sizeof(bool));
				optionsFile.read(reinterpret_cast<char*>(&joyplay1),sizeof(bool));
				optionsFile.read(reinterpret_cast<char*>(&joyplay2),sizeof(bool));
			}
			optionsFile.close();
			if(verboseLevel)
				cout << "Data loaded from oldstyle options file" << endl;
		}
		else
		{
			if(verboseLevel)
				cout << "Unable to load options file, using default values" << endl;
		}
	}

#if NETWORK
	Config::getInstance()->setDefault("portv4","42200");
	strcpy(serverAddress, "192.168.0.2                 \0");
	if(configSettings->exists("address0"))
	{
		strcpy(serverAddress, "                            \0");
		strncpy(serverAddress,configSettings->getString("address0").c_str(),sizeof(serverAddress)-1);
	}
#endif

	if (singlePuzzle)
	{
		xsize=300;
		ysize=600;
	}


	//Open video
	if ((bFullscreen)&&(!singlePuzzle)) screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_FULLSCREEN|SDL_ANYFORMAT);
	else screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_ANYFORMAT);

	if ( screen == NULL )
	{
		cerr << "Unable to set " << xsize << "x" << ysize << " video: " << SDL_GetError() << endl;
		exit(1);
	}

	//Init the file system abstraction layer
	PHYSFS_init(argv[0]);
	//Load default theme
	loadTheme(Config::getInstance()->getString("themename"));
	//Now sets the icon:
	SDL_Surface *icon = IMG_Load2("gfx/icon.png");
	SDL_WM_SetIcon(icon,NULL);

	if(verboseLevel)
		cout << "Images loaded" << endl;

	//InitMenues();

	BlockGameSdl theGame = BlockGameSdl(50,100);			//creates game objects
	BlockGameSdl theGame2 = BlockGameSdl(xsize-500,100);
	player1 = &theGame;
	player2 = &theGame2;
	/*if (singlePuzzle)
	{
	    theGame.GetTopY()=0;
	    theGame.GetTopX()=0;
	    theGame2.GetTopY()=10000;
	    theGame2.GetTopX()=10000;
	}*/
	theGame.DoPaintJob();			//Makes sure what there is something to paint
	theGame2.DoPaintJob();
	theGame.SetGameOver();		//sets the game over in the beginning
	theGame2.SetGameOver();


	//Takes names from file instead
	strcpy(theGame.name, player1name);
	strcpy(theGame2.name, player2name);

#if NETWORK
	//NetworkThing nt = NetworkThing();
	nt.setBGpointers(&theGame,&theGame2);
#endif

	if (singlePuzzle)
	{
		LoadPuzzleStages();
		theGame.NewPuzzleGame(singlePuzzleNr,0,0,SDL_GetTicks());
		showGame = true;
	}
	//Draws everything to screen
	if (!editorMode)
		MakeBackground(xsize,ysize,&theGame,&theGame2);
	else
		MakeBackground(xsize,ysize,&theGame);
	DrawIMG(background, screen, 0, 0);
	DrawEverything(xsize,ysize,&theGame,&theGame2);
	SDL_Flip(screen);
	//game loop
	MainMenu();



	//Saves options
	if (!editorMode)
	{
		configSettings->setInt("fullscreen",(int)bFullscreen);
		configSettings->setInt("musicenabled",(int)MusicEnabled);
		configSettings->setInt("soundenabled",(int)SoundEnabled);
		configSettings->setInt("mouseplay1",(int)mouseplay1);
		configSettings->setInt("mouseplay2",(int)mouseplay2);
		configSettings->setInt("joypad1",(int)joyplay1);
		configSettings->setInt("joypad2",(int)joyplay2);

		configSettings->setInt("player1keyup",(int)keySettings[0].up);
		configSettings->setInt("player1keydown",(int)keySettings[0].down);
		configSettings->setInt("player1keyleft",(int)keySettings[0].left);
		configSettings->setInt("player1keyright",(int)keySettings[0].right);
		configSettings->setInt("player1keychange",(int)keySettings[0].change);
		configSettings->setInt("player1keypush",(int)keySettings[0].push);

		configSettings->setInt("player2keyup",(int)keySettings[2].up);
		configSettings->setInt("player2keydown",(int)keySettings[2].down);
		configSettings->setInt("player2keyleft",(int)keySettings[2].left);
		configSettings->setInt("player2keyright",(int)keySettings[2].right);
		configSettings->setInt("player2keychange",(int)keySettings[2].change);
		configSettings->setInt("player2keypush",(int)keySettings[2].push);

		configSettings->setString("player1name",player1name);
		configSettings->setString("player2name",player2name);
		configSettings->save();
	}

	//calculate uptime:
	//int hours, mins, secs,
	commonTime ct = TimeHandler::ms2ct(SDL_GetTicks());

	//cout << "Block Attack - Rise of the Blocks ran for: " << ct.hours << " hours " << ct.minutes << " mins and " << ct.seconds << " secs" << endl;
	if(verboseLevel)
		cout << boost::format("Block Attack - Rise of the Blocks ran for: %1% hours %2% mins and %3% secs") % ct.hours % ct.minutes % ct.seconds << endl;

	ct = TimeHandler::addTime("totalTime",ct);
	if(verboseLevel)
		cout << "Total run time is now: " << ct.days << " days " << ct.hours << " hours " << ct.minutes << " mins and " << ct.seconds << " secs" << endl;

	Stats::getInstance()->save();

	Config::getInstance()->save();

	//Frees memory from music and fonts
	//This is done after writing of configurations and stats since it often crashes the program :(
	UnloadImages();

	//Close file system Apstraction layer!
	PHYSFS_deinit();
	return 0;
}


int runGame(int gametype, int level)
{
	Uint8 *keys;
	int mousex, mousey;   //Mouse coordinates
	showOptions = false;
	b1playerOpen = false;
	b2playersOpen = false;
	bReplayOpen = false;
	bScreenLocked = false;
	theTopScoresEndless = Highscore(1);
	theTopScoresTimeTrial = Highscore(2);
	drawBalls = true;
	puzzleLoaded = false;
	bool weWhereConnected = false;
	bool bNearDeath;                        //Play music faster or louder while tru

	//Things used for repeating keystrokes:
	bool repeatingS[2] = {false,false};
	bool repeatingW[2] = {false,false};
	bool repeatingN[2] = {false,false};
	bool repeatingE[2] = {false,false};
	const int startRepeat = 200;
	const int repeatDelay = 100;    //Repeating
	unsigned long timeHeldP1N = 0;
	unsigned long timeHeldP1S = 0;
	unsigned long timeHeldP1E = 0;
	unsigned long timeHeldP1W = 0;
	unsigned long timeHeldP2N = 0;
	unsigned long timeHeldP2S = 0;
	unsigned long timeHeldP2E = 0;
	unsigned long timeHeldP2W = 0;
	unsigned long timesRepeatedP1N = 0;
	unsigned long timesRepeatedP1S = 0;
	unsigned long timesRepeatedP1E = 0;
	unsigned long timesRepeatedP1W = 0;
	unsigned long timesRepeatedP2N = 0;
	unsigned long timesRepeatedP2S = 0;
	unsigned long timesRepeatedP2E = 0;
	unsigned long timesRepeatedP2W = 0;

	theBallManeger = ballManeger();
	theExplosionManeger = explosionManeger();
	BlockGameSdl theGame = BlockGameSdl(50,100);			//creates game objects
	BlockGameSdl theGame2 = BlockGameSdl(xsize-500,100);
	player1 = &theGame;
	player2 = &theGame2;
	theGame.DoPaintJob();			//Makes sure what there is something to paint
	theGame2.DoPaintJob();
	theGame.SetGameOver();		//sets the game over in the beginning
	theGame2.SetGameOver();

	//Takes names from file instead
	strcpy(theGame.name, player1name);
	strcpy(theGame2.name, player2name);

	Joypad joypad1 = Joypad();    //Creates a joypad
	Joypad joypad2 = Joypad();    //Creates a joypad

#if NETWORK
	//NetworkThing nt = NetworkThing();
	nt.setBGpointers(&theGame,&theGame2);
#endif

	if (singlePuzzle)
	{
		LoadPuzzleStages();
		theGame.NewPuzzleGame(singlePuzzleNr,0,0,SDL_GetTicks());
		showGame = true;
	}
	//Draws everything to screen
	if (!editorMode)
		MakeBackground(xsize,ysize,&theGame,&theGame2);
	else
		MakeBackground(xsize,ysize,&theGame);
	/*DrawIMG(background, screen, 0, 0);
	DrawEverything(xsize,ysize,&theGame,&theGame2);
	SDL_Flip(screen);*/
	//game loop
	int done = 0;
	if(verboseLevel)
		cout << "Starting game loop" << endl;


	bool mustsetupgame = true;

	while (done == 0)
	{
		if(mustsetupgame)
		{
			switch(gametype)
			{
			case 1:
				StartSinglePlayerTimeTrial();
				break;
			case 2:
			{
				int myLevel = PuzzleLevelSelect(1);
				if(myLevel == -1)
					return 1;
				theGame.NewStageGame(myLevel,50,100,SDL_GetTicks());
				MakeBackground(xsize,ysize,&theGame,&theGame2);
				DrawIMG(background, screen, 0, 0);
				closeAllMenus();
				twoPlayers =false;
				theGame2.SetGameOver();
				showGame = true;
				strcpy(theGame.name, player1name);
				strcpy(theGame2.name, player2name);
			}
			break;
			case 3:
				if(StartSinglePlayerPuzzle(level))
					return 1;
				break;
			case 4:
			{
				//1 player - Vs mode
				bNewGameOpen = false;
				b1playerOpen = false;
				int theAIlevel = level; //startSingleVs();
				theGame.NewVsGame(50,100,&theGame2,SDL_GetTicks());
				theGame2.NewVsGame(xsize-500,100,&theGame,SDL_GetTicks());
				MakeBackground(xsize,ysize,&theGame,&theGame2);
				DrawIMG(background, screen, 0, 0);
				twoPlayers = true; //Single player, but AI plays
				showGame = true;
				theGame2.setAIlevel((Uint8)theAIlevel);
				int theTime = time(0);
				theGame.putStartBlocks(theTime);
				theGame2.putStartBlocks(theTime);
				strcpy(theGame.name, player1name);
				strcpy(theGame2.name, player2name);
			}
			break;
			case 10:
				StarTwoPlayerTimeTrial();
				break;
			case 11:
				StartTwoPlayerVs();
				break;
			case 12:
				StartHostServer();
				break;
			case 13:
				StartJoinServer();
				break;
			case 0:
			default:
				StartSinglePlayerEndless();
				break;
			};
			mustsetupgame = false;
			DrawIMG(background, screen, 0, 0);
			DrawEverything(xsize,ysize,&theGame,&theGame2);
			SDL_Flip(screen);
		}

		if (!(highPriority)) SDL_Delay(10);

		if ((standardBackground)&&(!editorMode))
		{
			MakeBackground(xsize,ysize,&theGame,&theGame2);
			DrawIMG(background, screen, 0, 0);
		}

		if ((standardBackground)&&(editorMode))
		{
			DrawIMG(backgroundImage, screen, 0, 0);
			MakeBackground(xsize,ysize,&theGame);
			DrawIMG(background, screen, 0, 0);
		}

		//updates the balls and explosions:
		theBallManeger.update();
		theExplosionManeger.update();
		theTextManeger.update();

#if NETWORK
		if (nt.isConnected())
		{
			nt.updateNetwork();
			networkActive = true;
			if (!nt.isConnectedToPeer())
				DrawIMG(background, screen, 0, 0);
		}
		else
			networkActive = false;
		if (nt.isConnectedToPeer())
		{
			networkPlay=true;
			if (!weWhereConnected) //We have just connected
			{
				theGame.NewVsGame(50,100,&theGame2,SDL_GetTicks());
				theGame.putStartBlocks(nt.theSeed);
				theGame2.playNetwork(xsize-500,100,SDL_GetTicks());
				nt.theGameHasStarted();
				DrawIMG(background, screen, 0, 0);
			}
			weWhereConnected = true;
		}
		else
		{
			networkPlay=false;
			weWhereConnected = false;
		}
#endif

		if (!bScreenLocked)
		{
			SDL_Event event;

			while ( SDL_PollEvent(&event) )
			{
				if ( event.type == SDL_QUIT )
				{
					Config::getInstance()->setShuttingDown(5);
					done = 1;
				}

				if ( event.type == SDL_KEYDOWN )
				{
					if ( event.key.keysym.sym == SDLK_ESCAPE || ( event.key.keysym.sym == SDLK_RETURN && theGame.isGameOver() ) )
					{
						if (showOptions)
						{
							showOptions = false;
						}
						else
							done=1;
						DrawIMG(background, screen, 0, 0);

					}
					if ((!editorMode)&&(!editorModeTest)&&(!theGame.GetAIenabled()))
					{
						//player1:
						if ( event.key.keysym.sym == keySettings[player1keys].up )
						{
							theGame.MoveCursor('N');
							repeatingN[0]=true;
							timeHeldP1N=SDL_GetTicks();
							timesRepeatedP1N=0;
						}
						if ( event.key.keysym.sym == keySettings[player1keys].down )
						{
							theGame.MoveCursor('S');
							repeatingS[0]=true;
							timeHeldP1S=SDL_GetTicks();
							timesRepeatedP1S=0;
						}
						if ( (event.key.keysym.sym == keySettings[player1keys].left) && (showGame) )
						{
							theGame.MoveCursor('W');
							repeatingW[0]=true;
							timeHeldP1W=SDL_GetTicks();
							timesRepeatedP1W=0;
						}
						if ( (event.key.keysym.sym == keySettings[player1keys].right) && (showGame) )
						{
							theGame.MoveCursor('E');
							repeatingE[0]=true;
							timeHeldP1E=SDL_GetTicks();
							timesRepeatedP1E=0;
						}
						if ( event.key.keysym.sym == keySettings[player1keys].push )
						{
							theGame.PushLine();
						}
						if ( event.key.keysym.sym == keySettings[player1keys].change )
						{
							theGame.SwitchAtCursor();
						}
					}
					if (!editorMode && !theGame2.GetAIenabled())
					{
						//player2:
						if ( event.key.keysym.sym == keySettings[player2keys].up )
						{
							theGame2.MoveCursor('N');
							repeatingN[1]=true;
							timeHeldP2N=SDL_GetTicks();
							timesRepeatedP2N=0;
						}
						if ( event.key.keysym.sym == keySettings[player2keys].down )
						{
							theGame2.MoveCursor('S');
							repeatingS[1]=true;
							timeHeldP2S=SDL_GetTicks();
							timesRepeatedP2S=0;
						}
						if ( (event.key.keysym.sym == keySettings[player2keys].left) && (showGame) )
						{
							theGame2.MoveCursor('W');
							repeatingW[1]=true;
							timeHeldP2W=SDL_GetTicks();
							timesRepeatedP2W=0;
						}
						if ( (event.key.keysym.sym == keySettings[player2keys].right) && (showGame) )
						{
							theGame2.MoveCursor('E');
							repeatingE[1]=true;
							timeHeldP2E=SDL_GetTicks();
							timesRepeatedP2E=0;
						}
						if ( event.key.keysym.sym == keySettings[player2keys].push )
						{
							theGame2.PushLine();
						}
						if ( event.key.keysym.sym == keySettings[player2keys].change )
						{
							theGame2.SwitchAtCursor();
						}
					}
					//common:
					if ((!singlePuzzle)&&(!editorMode))
					{
						if ( event.key.keysym.sym == SDLK_F2 )
						{
							/*#if NETWORK
							if ((!showOptions)&&(!networkActive)){
							#else
							if ((!showOptions)){
							#endif
							    StartSinglePlayerEndless();
							}
							 */
							mustsetupgame = true;
						}
						if ( event.key.keysym.sym == SDLK_F10 )
						{
							StartReplay("/home/poul/.gamesaves/blockattack/quicksave");
						}
						if ( event.key.keysym.sym == SDLK_F9 )
						{
							writeScreenShot();
						}
						if ( event.key.keysym.sym == SDLK_F5 )
						{
							if(theGame.isGameOver() && theGame2.isGameOver())
							{
								string filename = "/home/poul/.gamesaves/blockattack/quicksave";
								if(!twoPlayers)
									theGame.theReplay.saveReplay(filename);
								else
									theGame.theReplay.saveReplay(filename,theGame2.theReplay);
							}
						}
						if ( event.key.keysym.sym == SDLK_F11 )
						{
							/*This is the test place, place function to test here*/

							StartReplay("/home/poul/.gamesaves/blockattack/bestTT");

							//theGame.CreateGreyGarbage();
							//char mitNavn[30];
							//SelectThemeDialogbox(300,400,mitNavn);
							//MainMenu();
							//OpenScoresDisplay();
						} //F11
					}
					if ( event.key.keysym.sym == SDLK_F12 )
					{
						done=1;
					}
				}
			} //while event PollEvent - read keys

			/**********************************************************************
			**************************** Repeating start **************************
			**********************************************************************/

			keys = SDL_GetKeyState(NULL);
//Also the joysticks:
//Repeating not implemented

//Player 1 start
			if (!(keys[keySettings[player1keys].up]))
				repeatingN[0]=false;
			while ((repeatingN[0])&&(keys[keySettings[player1keys].up])&&(SDL_GetTicks()>timeHeldP1N+timesRepeatedP1N*repeatDelay+startRepeat))
			{
				theGame.MoveCursor('N');
				timesRepeatedP1N++;
			}

			if (!(keys[keySettings[player1keys].down]))
				repeatingS[0]=false;
			while ((repeatingS[0])&&(keys[keySettings[player1keys].down])&&(SDL_GetTicks()>timeHeldP1S+timesRepeatedP1S*repeatDelay+startRepeat))
			{
				theGame.MoveCursor('S');
				timesRepeatedP1S++;
			}

			if (!(keys[keySettings[player1keys].left]))
				repeatingW[0]=false;
			while ((repeatingW[0])&&(keys[keySettings[player1keys].left])&&(SDL_GetTicks()>timeHeldP1W+timesRepeatedP1W*repeatDelay+startRepeat))
			{
				timesRepeatedP1W++;
				theGame.MoveCursor('W');
			}

			if (!(keys[keySettings[player1keys].right]))
				repeatingE[0]=false;
			while ((repeatingE[0])&&(keys[keySettings[player1keys].right])&&(SDL_GetTicks()>timeHeldP1E+timesRepeatedP1E*repeatDelay+startRepeat))
			{
				timesRepeatedP1E++;
				theGame.MoveCursor('E');
			}

//Player 1 end

//Player 2 start
			if (!(keys[keySettings[player2keys].up]))
				repeatingN[1]=false;
			while ((repeatingN[1])&&(keys[keySettings[player2keys].up])&&(SDL_GetTicks()>timeHeldP2N+timesRepeatedP2N*repeatDelay+startRepeat))
			{
				theGame2.MoveCursor('N');
				timesRepeatedP2N++;
			}

			if (!(keys[keySettings[player2keys].down]))
				repeatingS[1]=false;
			while ((repeatingS[1])&&(keys[keySettings[player2keys].down])&&(SDL_GetTicks()>timeHeldP2S+timesRepeatedP2S*repeatDelay+startRepeat))
			{
				theGame2.MoveCursor('S');
				timesRepeatedP2S++;
			}

			if (!(keys[keySettings[player2keys].left]))
				repeatingW[1]=false;
			while ((repeatingW[1])&&(keys[keySettings[player2keys].left])&&(SDL_GetTicks()>timeHeldP2W+timesRepeatedP2W*repeatDelay+startRepeat))
			{
				theGame2.MoveCursor('W');
				timesRepeatedP2W++;
			}

			if (!(keys[keySettings[player2keys].right]))
				repeatingE[1]=false;
			while ((repeatingE[1])&&(keys[keySettings[player2keys].right])&&(SDL_GetTicks()>timeHeldP2E+timesRepeatedP2E*repeatDelay+startRepeat))
			{
				theGame2.MoveCursor('E');
				timesRepeatedP2E++;
			}

//Player 2 end

			/**********************************************************************
			**************************** Repeating end ****************************
			**********************************************************************/

			/**********************************************************************
			***************************** Joypad start ****************************
			**********************************************************************/

			//Gameplay
			if (joyplay1||joyplay2)
			{
				if (joypad1.working && !theGame.GetAIenabled())
				{
					if (joyplay1)
					{
						joypad1.update();
						if (joypad1.up)
						{
							theGame.MoveCursor('N');
							repeatingN[0]=true;
							timeHeldP1N=SDL_GetTicks();
							timesRepeatedP1N=0;
						}
						if (joypad1.down)
						{
							theGame.MoveCursor('S');
							repeatingS[0]=true;
							timeHeldP1S=SDL_GetTicks();
							timesRepeatedP1S=0;
						}
						if (joypad1.left)
						{
							theGame.MoveCursor('W');
							repeatingW[0]=true;
							timeHeldP1W=SDL_GetTicks();
							timesRepeatedP1W=0;
						}
						if (joypad1.right)
						{
							theGame.MoveCursor('E');
							repeatingE[0]=true;
							timeHeldP1E=SDL_GetTicks();
							timesRepeatedP1E=0;
						}
						if (joypad1.but1)
							theGame.SwitchAtCursor();
						if (joypad1.but2)
							theGame.PushLine();
					}
					else
					{
						joypad1.update();
						if (joypad1.up)
						{
							theGame2.MoveCursor('N');
							repeatingN[1]=true;
							timeHeldP2N=SDL_GetTicks();
							timesRepeatedP2N=0;
						}
						if (joypad1.down)
						{
							theGame2.MoveCursor('S');
							repeatingS[1]=true;
							timeHeldP2S=SDL_GetTicks();
							timesRepeatedP2S=0;
						}
						if (joypad1.left)
						{
							theGame2.MoveCursor('W');
							repeatingW[1]=true;
							timeHeldP2W=SDL_GetTicks();
							timesRepeatedP2W=0;
						}
						if (joypad1.right)
						{
							theGame2.MoveCursor('E');
							repeatingE[1]=true;
							timeHeldP2E=SDL_GetTicks();
							timesRepeatedP2E=0;
						}
						if (joypad1.but1)
							theGame2.SwitchAtCursor();
						if (joypad1.but2)
							theGame2.PushLine();
					}
				}
				if (joypad2.working && !theGame2.GetAIenabled())
				{
					if (!joyplay2)
					{
						joypad2.update();
						if (joypad2.up)
						{
							theGame.MoveCursor('N');
							repeatingN[0]=true;
							timeHeldP1N=SDL_GetTicks();
							timesRepeatedP1N=0;
						}
						if (joypad2.down)
						{
							theGame.MoveCursor('S');
							repeatingS[0]=true;
							timeHeldP1S=SDL_GetTicks();
							timesRepeatedP1S=0;
						}
						if (joypad2.left)
						{
							theGame.MoveCursor('W');
							repeatingW[0]=true;
							timeHeldP1W=SDL_GetTicks();
							timesRepeatedP1W=0;
						}
						if (joypad2.right)
						{
							theGame.MoveCursor('E');
							repeatingE[0]=true;
							timeHeldP1E=SDL_GetTicks();
							timesRepeatedP1E=0;
						}
						if (joypad2.but1)
							theGame.SwitchAtCursor();
						if (joypad2.but2)
							theGame.PushLine();
					}
					else
					{
						joypad2.update();
						if (joypad2.up)
						{
							theGame2.MoveCursor('N');
							repeatingN[1]=true;
							timeHeldP2N=SDL_GetTicks();
							timesRepeatedP2N=0;
						}
						if (joypad2.down)
						{
							theGame2.MoveCursor('S');
							repeatingS[1]=true;
							timeHeldP2S=SDL_GetTicks();
							timesRepeatedP2S=0;
						}
						if (joypad2.left)
						{
							theGame2.MoveCursor('W');
							repeatingW[1]=true;
							timeHeldP2W=SDL_GetTicks();
							timesRepeatedP2W=0;
						}
						if (joypad2.right)
						{
							theGame2.MoveCursor('E');
							repeatingE[1]=true;
							timeHeldP2E=SDL_GetTicks();
							timesRepeatedP2E=0;
						}
						if (joypad2.but1)
							theGame2.SwitchAtCursor();
						if (joypad2.but2)
							theGame2.PushLine();
					}
				}
			}

			/**********************************************************************
			***************************** Joypad end ******************************
			**********************************************************************/


			keys = SDL_GetKeyState(NULL);

			SDL_GetMouseState(&mousex,&mousey);

			/********************************************************************
			**************** Here comes mouse play ******************************
			********************************************************************/

			if ((mouseplay1)&&( ( (!editorMode)&&(!theGame.GetAIenabled()) ) ||(editorModeTest))) //player 1
				if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600))
				{
					int yLine, xLine;
					yLine = ((100+600)-(mousey-100+theGame.GetPixels()))/50;
					xLine = (mousex-50+25)/50;
					yLine-=2;
					xLine-=1;
					if ((yLine>10)&&(theGame.GetTowerHeight()<12))
						yLine=10;
					if (((theGame.GetPixels()==50)||(theGame.GetPixels()==0)) && (yLine>11))
						yLine=11;
					if (yLine<0)
						yLine=0;
					if (xLine<0)
						xLine=0;
					if (xLine>4)
						xLine=4;
					theGame.MoveCursorTo(xLine,yLine);
				}

			if ((mouseplay2)&&(!editorMode)&&(!theGame2.GetAIenabled())) //player 2
				if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600))
				{
					int yLine, xLine;
					yLine = ((100+600)-(mousey-100+theGame2.GetPixels()))/50;
					xLine = (mousex-(xsize-500)+25)/50;
					yLine-=2;
					xLine-=1;
					if ((yLine>10)&&(theGame2.GetTowerHeight()<12))
						yLine=10;
					if (((theGame2.GetPixels()==50)||(theGame2.GetPixels()==0)) && (yLine>11))
						yLine=11;
					if (yLine<0)
						yLine=0;
					if (xLine<0)
						xLine=0;
					if (xLine>4)
						xLine=4;
					theGame2.MoveCursorTo(xLine,yLine);
				}

			/********************************************************************
			**************** Here ends mouse play *******************************
			********************************************************************/

			// If the mouse button is released, make bMouseUp equal true
			if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
			{
				bMouseUp=true;
			}

			// If the mouse button 2 is released, make bMouseUp2 equal true
			if ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))!=SDL_BUTTON(3))
			{
				bMouseUp2=true;
			}

			if ((!singlePuzzle)&&(!editorMode))
			{
				//read mouse events
				if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
				{
					bMouseUp = false;
					DrawIMG(background, screen, 0, 0);


					/********************************************************************
					**************** Here comes mouse play ******************************
					********************************************************************/
					if ((!showOptions))
					{
						if (mouseplay1 && !theGame.GetAIenabled()) //player 1
							if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600))
							{
								theGame.SwitchAtCursor();
							}
						if (mouseplay2 && !theGame2.GetAIenabled()) //player 2
							if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600))
							{
								theGame2.SwitchAtCursor();
							}
					}
					/********************************************************************
					**************** Here ends mouse play *******************************
					********************************************************************/

					if(stageButtonStatus != SBdontShow && (mousex > theGame.GetTopX()+cordNextButton.x)
							&&(mousex < theGame.GetTopX()+cordNextButton.x+cordNextButton.xsize)
							&&(mousey > theGame.GetTopY()+cordNextButton.y)&&(mousey < theGame.GetTopY()+cordNextButton.y+cordNextButton.ysize))
					{
						//Clicked the next button after a stage clear or puzzle
						theGame.nextLevel(SDL_GetTicks());
					}
					if(stageButtonStatus != SBdontShow && (mousex > theGame.GetTopX()+cordRetryButton .x)
							&&(mousex < theGame.GetTopX()+cordRetryButton.x+cordRetryButton.xsize)
							&&(mousey > theGame.GetTopY()+cordRetryButton.y)&&(mousey < theGame.GetTopY()+cordRetryButton.y+cordRetryButton.ysize))
					{
						//Clicked the retry button
						theGame.retryLevel(SDL_GetTicks());
					}


					//cout << "Mouse x: " << mousex << ", mouse y: " << mousey << endl;
				}

				//Mouse button 2:
				if ((SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(3))==SDL_BUTTON(3) && bMouseUp2)
				{
					bMouseUp2=false; //The button is pressed
					/********************************************************************
					**************** Here comes mouse play ******************************
					********************************************************************/
					if (!showOptions)
					{
						if (mouseplay1 && !theGame.GetAIenabled()) //player 1
							if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600))
							{
								theGame.PushLine();
							}
						if (mouseplay2 && !theGame2.GetAIenabled()) //player 2
							if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600))
							{
								theGame2.PushLine();
							}
					}
					/********************************************************************
					**************** Here ends mouse play *******************************
					********************************************************************/
				}
			} //if !singlePuzzle
			else
			{

			}
		} //if !bScreenBocked;


		//Sees if music is stopped and if music is enabled
		if ((!NoSound)&&(!Mix_PlayingMusic())&&(MusicEnabled)&&(!bNearDeath))
		{
			// then starts playing it.
			Mix_VolumeMusic(MIX_MAX_VOLUME);
			Mix_PlayMusic(bgMusic, -1); //music loop
		}

		if(bNearDeath!=bNearDeathPrev)
		{
			if(bNearDeath)
			{
				if(!NoSound &&(MusicEnabled))
				{
					Mix_VolumeMusic(MIX_MAX_VOLUME);
					Mix_PlayMusic(highbeatMusic, 1);
				}
			}
			else
			{
				if(!NoSound &&(MusicEnabled))
				{
					Mix_VolumeMusic(MIX_MAX_VOLUME);
					Mix_PlayMusic(bgMusic, -1);
				}
			}
		}

		bNearDeathPrev = bNearDeath;


		//set bNearDeath to false theGame*.Update() will change to true as needed
		bNearDeath = theGame.IsNearDeath() || theGame2.IsNearDeath();
		//Updates the objects
		theGame.Update(SDL_GetTicks());
		theGame2.Update(SDL_GetTicks());

//see if anyone has won (two players only)
#if NETWORK
		if (!networkPlay)
#endif
			if (twoPlayers)
			{
				lastNrOfPlayers = 2;
				if ((theGame.isGameOver()) && (theGame2.isGameOver()))
				{
					if (theGame.GetScore()+theGame.GetHandicap()>theGame2.GetScore()+theGame2.GetHandicap())
						theGame.setPlayerWon();
					else if (theGame.GetScore()+theGame.GetHandicap()<theGame2.GetScore()+theGame2.GetHandicap())
						theGame2.setPlayerWon();
					else
					{
						theGame.setDraw();
						theGame2.setDraw();
					}
					//twoPlayers = false;
				}
				if ((theGame.isGameOver()) && (!theGame2.isGameOver()))
				{
					theGame2.setPlayerWon();
					//twoPlayers = false;
				}
				if ((!theGame.isGameOver()) && (theGame2.isGameOver()))
				{
					theGame.setPlayerWon();
					//twoPlayers = false;
				}
			}

		//Once evrything has been checked, update graphics
		DrawEverything(xsize,ysize,&theGame,&theGame2);
		SDL_GetMouseState(&mousex,&mousey);
		//Remember mouse placement
		oldMousex = mousex;
		oldMousey = mousey;
		//Draw the mouse:
		mouse.PaintTo(screen,mousex,mousey);
		SDL_Flip(screen);
	} //game loop
	return 0;
}
