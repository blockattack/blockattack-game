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

#include "common.h"
#include "global.hpp"
#include "scopeHelpers.hpp"
#include "icon.hpp"
#include "os.hpp"

#include <string.h>
#include <fmt/core.h>

#include "version.h"

#define WITH_SDL 1

#include "sago/SagoSpriteHolder.hpp"
#include "sago/SagoTextBox.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>           //Used for srand()
#include <sstream>          //Still used by std::to_string2
#include <string>
#include "SDL.h"            //The SDL libary, used for most things
#include <SDL_mixer.h>      //Used for sound & music
#include <SDL_image.h>      //To load PNG images!
#include <physfs.h>         //Abstract file system. To use containers
#include <vector>
#include "MenuSystem.h"
#include "puzzlehandler.hpp"
#include "stageclearhandler.hpp"
#include <memory>
#include "ScoresDisplay.hpp"

#include "highscore.h"      //Stores highscores
#include "ReadKeyboard.h"   //Reads text from keyboard
#include "stats.h"          //Saves general stats
#include "replayhandler.hpp"

#include "common.h"
#include "gamecontroller.h"
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include "levelselect.hpp"

/*******************************************************************************
* All variables and constant has been moved to mainVars.inc for the overview.  *
*******************************************************************************/
#include "mainVars.inc"

GlobalData globalData;

static int InitImages(sago::SagoSpriteHolder& holder);

static void FsSearchParthMainAppend(std::vector<std::string>& paths) {
	paths.push_back((std::string)SHAREDIR+"/blockattack.data");
	paths.push_back((std::string)PHYSFS_getBaseDir()+"/blockattack.data");
	paths.push_back((std::string)PHYSFS_getBaseDir()+"/data");
}

static void FsSearchPathModAppend(std::vector<std::string>& paths, const std::vector<std::string>& modlist) {
	for (const std::string& mod : modlist) {
		std::string filename = std::string(SHAREDIR)+"/mods/"+mod+".data";
		paths.push_back(filename);
		filename = std::string(PHYSFS_getBaseDir())+"/mods/"+mod+".data";
		paths.push_back(filename);
	}
}

static void PhysFsSetSearchPath(const std::vector<std::string>& paths, const std::string& savepath) {
	for (const std::string& path : paths) {
		PHYSFS_mount(path.c_str(), "/", 0);
	}
	PHYSFS_mount(savepath.c_str(), "/", 0);
	PHYSFS_setWriteDir(savepath.c_str());
}


static void PhysFsCreateFolders() {
	//PHYSFS_mkdir("screenshots");
	PHYSFS_mkdir("replays");
	PHYSFS_mkdir("puzzles");
}

static void setGameOverFont(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({0,0,255,255});
	field.SetFontSize(60);
	field.SetOutline(3, {192,192,255,255});
	field.SetMaxWidth(280);
	field.SetText(text);
}

//Load all image files to memory
static int InitImages(sago::SagoSpriteHolder& holder) {
	bricks[0] = holder.GetSprite("block_blue");
	bricks[1] = holder.GetSprite("block_green");
	bricks[2] = holder.GetSprite("block_purple");
	bricks[3] = holder.GetSprite("block_red");
	bricks[4] = holder.GetSprite("block_turkish");
	bricks[5] = holder.GetSprite("block_yellow");
	bricks[6] = holder.GetSprite("block_grey");
	bomb = holder.GetSprite("block_bomb");
	backgroundImage = holder.GetSprite("background");
	backgroundSixteenNineImage = holder.GetSprite("background_sixteen_nine");
	globalData.bHighScore = holder.GetSprite("b_highscore");
	globalData.bBack = holder.GetSprite("b_blank");
	bForward = holder.GetSprite("b_forward");
	blackLine = holder.GetSprite("black_line");
	stageBobble = holder.GetSprite("i_stage_clear_limit");
	crossover = holder.GetSprite("crossover");
	balls[0] = holder.GetSprite("ball_blue");
	balls[1] = holder.GetSprite("ball_green");
	balls[2] = holder.GetSprite("ball_purple");
	balls[3] = holder.GetSprite("ball_red");
	balls[4] = holder.GetSprite("ball_turkish");
	balls[5] = holder.GetSprite("ball_yellow");
	balls[6] = holder.GetSprite("ball_gray");
	cursor = holder.GetSprite("cursor");
	ready = holder.GetSprite("block_ready");
	explosion[0] = holder.GetSprite("explosion0");
	explosion[1] = holder.GetSprite("explosion1");
	explosion[2] = holder.GetSprite("explosion2");
	explosion[3] = holder.GetSprite("explosion3");
	counter[0] = holder.GetSprite("counter_1");
	counter[1] = holder.GetSprite("counter_2");
	counter[2] = holder.GetSprite("counter_3");
	setGameOverFont(&holder.GetDataHolder(),tbGameOver, _("GAME OVER"));
	setGameOverFont(&holder.GetDataHolder(),tbWinner, _("WINNER"));
	setGameOverFont(&holder.GetDataHolder(),tbDraw, _("DRAW"));
	iChainFrame = holder.GetSprite("chain_frame");
	globalData.iLevelCheck = holder.GetSprite("i_level_check");
	globalData.iLevelCheckBox = holder.GetSprite("i_level_check_box");
	globalData.iLevelCheckBoxMarked = holder.GetSprite("i_level_check_box_marked");
	globalData.iCheckBoxArea = holder.GetSprite("i_check_box_area");
	boardBackBack = holder.GetSprite("board_back_back");
	garbageTL = holder.GetSprite("garbage_tl");
	garbageT = holder.GetSprite("garbage_t");
	garbageTR = holder.GetSprite("garbage_tr");
	garbageR = holder.GetSprite("garbage_r");
	garbageBR = holder.GetSprite("garbage_br");
	garbageB = holder.GetSprite("garbage_b");
	garbageBL = holder.GetSprite("garbage_bl");
	garbageL = holder.GetSprite("garbage_l");
	garbageFill = holder.GetSprite("garbage_fill");
	garbageML = holder.GetSprite("garbage_ml");
	garbageM = holder.GetSprite("garbage_m");
	garbageMR = holder.GetSprite("garbage_mr");
	garbageGM = holder.GetSprite("garbage_gm");
	garbageGML = holder.GetSprite("garbage_gml");
	garbageGMR = holder.GetSprite("garbage_gmr");
	smiley[0] = holder.GetSprite("smileys0");
	smiley[1] = holder.GetSprite("smileys1");
	smiley[2] = holder.GetSprite("smileys2");
	smiley[3] = holder.GetSprite("smileys3");
	transCover = holder.GetSprite("trans_cover");
	bExit = holder.GetSprite("b_exit");
	bSkip = holder.GetSprite("b_blank");
	globalData.bNext = holder.GetSprite("b_blank");
	bRetry = holder.GetSprite("b_blank");
	globalData.mouse = holder.GetSprite("mouse");
	backBoard = holder.GetSprite("back_board");

	sagoTextSetBlueFont(player1name);
	sagoTextSetBlueFont(player2name);
	sagoTextSetBlueFont(player1time);
	sagoTextSetBlueFont(player2time);
	sagoTextSetBlueFont(player1score);
	sagoTextSetBlueFont(player2score);
	sagoTextSetBlueFont(player1chain);
	sagoTextSetBlueFont(player2chain);
	sagoTextSetBlueFont(player1speed);
	sagoTextSetBlueFont(player2speed);


//Loads the sound if sound present
	if (!globalData.NoSound) {
		//And here the music:
		bgMusic = holder.GetDataHolder().getMusicHandler("bgmusic");
		highbeatMusic = holder.GetDataHolder().getMusicHandler("highbeat");
		//the music... we just hope it exists, else the user won't hear anything
		//Same goes for the sounds
		boing = holder.GetDataHolder().getSoundHandler("pop");
		applause = holder.GetDataHolder().getSoundHandler("applause");
		photoClick = holder.GetDataHolder().getSoundHandler("cameraclick");
		globalData.typingChunk = holder.GetDataHolder().getSoundHandler("typing");
		counterChunk = holder.GetDataHolder().getSoundHandler("counter");
		counterFinalChunk = holder.GetDataHolder().getSoundHandler("counter_final");
		Config::getInstance()->setDefault("volume_sound", "24"); //0-128
		int soundVolume = Config::getInstance()->getInt("volume_sound");
		Mix_VolumeChunk(boing.get(), soundVolume);
		Mix_VolumeChunk(applause.get(), soundVolume);
		Mix_VolumeChunk(photoClick.get(), soundVolume);
		Mix_VolumeChunk(globalData.typingChunk.get(), soundVolume);
		Mix_VolumeChunk(counterChunk.get(), soundVolume);
		Mix_VolumeChunk(counterFinalChunk.get(), soundVolume);
	} //All sound has been loaded or not
	return 0;
} //InitImages()

/*Draws a image from on a given Surface. Takes source image, destination surface and coordinates*/
void DrawIMG(const sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y) {
	sprite.Draw(target, SDL_GetTicks(),x,y);
}

void DrawIMG_Bounded(const sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y, int minx, int miny, int maxx, int maxy) {
	SDL_Rect bounds;
	bounds.x = minx;
	bounds.y = miny;
	bounds.w = maxx-minx;
	bounds.h = maxy-miny;
	sprite.DrawBounded(target, SDL_GetTicks(),x,y,bounds);
}

SDL_Window* sdlWindow;

sago::SagoDataHolder dataHolder;

void ResetFullscreen() {
	Mix_HaltMusic();  //We need to reload all data in case the screen type changes. Music must be stopped before unload.
	if (globalData.bFullscreen) {
		SDL_DisplayMode dm;
		globalData.xsize = SIXTEEN_NINE_WIDTH;
		globalData.ysize = SCREEN_HIGHT;
		if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
			globalData.xsize = globalData.ysize*dm.w/(double)dm.h;
		}
		SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else {
		globalData.xsize = FOUR_THREE_WIDTH;
		globalData.ysize = SCREEN_HIGHT;
		SDL_SetWindowFullscreen(sdlWindow, 0);
	}
	if (globalData.alwaysSixteenNine || globalData.xsize > SIXTEEN_NINE_WIDTH) {
		globalData.xsize = SIXTEEN_NINE_WIDTH;
	}
	if (globalData.xsize < FOUR_THREE_WIDTH) {
		globalData.xsize = FOUR_THREE_WIDTH;
	}
	SDL_RenderSetLogicalSize(globalData.screen, globalData.xsize, globalData.ysize);
	dataHolder.invalidateAll(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder( dataHolder ) );
	globalData.spriteHolder->ReadSprites(globalData.modinfo.getModSpriteFiles());
	InitImages(*(globalData.spriteHolder.get()) );
	SDL_ShowCursor(SDL_DISABLE);
}

static bool logicalRenderer = false;

void DrawBackground(SDL_Renderer* target) {
	SDL_RenderClear(target);
	if ( (double)globalData.xsize/globalData.ysize > 1.5) {
		backgroundSixteenNineImage.DrawScaled(target, SDL_GetTicks(), 0, 0, globalData.xsize, globalData.ysize);
	}
	else {
		backgroundImage.DrawScaled(target, SDL_GetTicks(), 0, 0, globalData.xsize, globalData.ysize);
	}
}

/**
 * This function reads the mouse coordinates from a relevant event.
 * Unlike SDL_GetMouseState this works even if SDL_RenderSetLogicalSize is used
 * @param event
 * @param mousex
 * @param mousey
 */
void UpdateMouseCoordinates(const SDL_Event& event, int& mousex, int& mousey) {
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		mousex = event.button.x;
		mousey = event.button.y;
		break;
	case SDL_MOUSEMOTION:
		mousex = event.motion.x;
		mousey = event.motion.y;
		break;
	default:
		break;
	}
}

static BallManager theBallManager;

static ExplosionManager theExplosionManager;


//Here comes the Block Game object
#include "BlockGame.hpp"
#include "os.hpp"
#include "sago/SagoMiscSdl2.hpp"
#include "ScoresDisplay.hpp"

#include "BlockGameSdl.inc"
#include "sago/SagoMisc.hpp"
#include "ReplayPlayer.hpp"
#include "platform_folders.h"

std::string pathToScreenShots() {
	Config::getInstance()->setDefault("screenshot_dir", _("Block Attack - Rise of the Blocks"));
	std::string screenshot_dir = Config::getInstance()->getString("screenshot_dir");
	if (OsPathIsRelative(screenshot_dir)) {
		return sago::getPicturesFolder() + "/" + screenshot_dir;
	}
	return screenshot_dir;
}

//writeScreenShot saves the screen as a bmp file, it uses the time to get a unique filename
void writeScreenShot() {
	if (globalData.verboseLevel) {
		std::cout << "Saving screenshot" << "\n";
	}
	int rightNow = (int)time(nullptr);
	int w, h;
	SDL_GetRendererOutputSize(globalData.screen, &w, &h);
	SDL_Surface* sreenshotSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(globalData.screen, NULL, SDL_PIXELFORMAT_ARGB8888, sreenshotSurface->pixels, sreenshotSurface->pitch);
	OsCreateFolder(pathToScreenShots());
	std::string buf = pathToScreenShots() + "/screenshot"+std::to_string(rightNow)+".bmp";
	SDL_SaveBMP(sreenshotSurface, buf.c_str());
	SDL_FreeSurface(sreenshotSurface);
	if (!globalData.NoSound) {
		if (globalData.SoundEnabled) {
			Mix_PlayChannel(1, photoClick.get(), 0);
		}
	}
}

//Function to return the name of a key, to be displayed...
std::string getKeyName(SDL_Keycode key);


void RunGameState(sago::GameStateInterface& state ) {
	bool done = false;     //We are done!
	while (!done && !Config::getInstance()->isShuttingDown()) {
		state.Draw(globalData.screen);

		SDL_Delay(1);
		SDL_Event event;

		bool mustWriteScreenshot = false;

		while ( SDL_PollEvent(&event) ) {
			UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);
			if ( event.type == SDL_QUIT ) {
				Config::getInstance()->setShuttingDown(5);
				done = true;
			}

			if ( event.key.keysym.sym == SDLK_F9 ) {
				mustWriteScreenshot = true;
			}

			bool processed = false;
			state.ProcessInput(event, processed);

		}

		state.Update();

		globalData.mouse.Draw(globalData.screen, SDL_GetTicks(), globalData.mousex, globalData.mousey);
		SDL_RenderPresent(globalData.screen);
		if (mustWriteScreenshot) {
			writeScreenShot();
		}

		if (!state.IsActive()) {
			done = true;
		}
	}
}

void OpenScoresDisplay() {
	ScoresDisplay d;
	d.scoreX = buttonXsize*2;
	d.scoreY = 0;
	d.buttonXsize = buttonXsize;
	d.buttonYsize = buttonYsize;
	RunGameState(d);
}


static sago::SagoTextField* getSmallInt(size_t number) {
	static std::vector<std::shared_ptr<sago::SagoTextField> > smallFontCache;
	if (smallFontCache.size() < number+1) {
		smallFontCache.resize(number+1);
	}
	if (!smallFontCache[number]) {
		std::shared_ptr<sago::SagoTextField> newNumber = std::make_shared<sago::SagoTextField>();
		newNumber->SetHolder(&globalData.spriteHolder->GetDataHolder());
		newNumber->SetFont("freeserif");
		newNumber->SetFontSize(16);
		newNumber->SetColor({255,0,0,255});
		newNumber->SetText(std::to_string(number));
		smallFontCache[number] = newNumber;
	}
	return smallFontCache[number].get();
}

//Draws the balls and explosions
static void DrawBalls() {
	for (size_t i = 0; i< theBallManager.ballArray.size(); i++) {
		if (theBallManager.ballArray[i].inUse) {
			DrawIMG(balls[theBallManager.ballArray[i].getColor()],globalData.screen,theBallManager.ballArray[i].getX(),theBallManager.ballArray[i].getY());
		} //if used
	}
	for (size_t i = 0; i< theExplosionManager.explosionArray.size(); i++) {
		if (theExplosionManager.explosionArray[i].inUse) {
			DrawIMG(explosion[theExplosionManager.explosionArray[i].getFrame()],globalData.screen,theExplosionManager.explosionArray[i].getX(),theExplosionManager.explosionArray[i].getY());
		}
	} //for
	for (size_t i = 0; i < globalData.theTextManager.textArray.size(); ++i) {
		if (globalData.theTextManager.textArray[i].inUse) {
			int x = globalData.theTextManager.textArray[i].getX()-12;
			int y = globalData.theTextManager.textArray[i].getY()-12;
			DrawIMG(iChainFrame,globalData.screen,x,y);

			getSmallInt(globalData.theTextManager.textArray[i].getText())->Draw(globalData.screen, x+12, y+7,
			        sago::SagoTextField::Alignment::center);
		}
	}
}    //DrawBalls

template <class T> void sagoTextSetHelpFont(T& field) {
	field.SetHolder(&globalData.spriteHolder->GetDataHolder());
	field.SetFont("freeserif");
	field.SetFontSize(30);
	field.SetOutline(1, {128,128,128,255});
}

void sagoTextSetHelpFont(sago::SagoTextField& gametypeNameField) {
	sagoTextSetHelpFont<sago::SagoTextField>(gametypeNameField);
}

void sagoTextSetBlueFont(sago::SagoTextField& field) {
	field.SetHolder(&globalData.spriteHolder->GetDataHolder());
	field.SetFont("freeserif");
	field.SetFontSize(30);
	field.SetColor({0,0,255,255});
	field.SetOutline(1, {128,128,255,255});
}

//draws everything
void DrawEverything(int xsize, int ysize,BlockGameSdl* theGame, BlockGameSdl* theGame2) {
	SDL_ShowCursor(SDL_DISABLE);
	DrawBackground(globalData.screen);
	theGame->DoPaintJob();
	theGame2->DoPaintJob();
	std::string strHolder;
	strHolder = std::to_string(theGame->GetScore()+theGame->GetHandicap());
	player1score.SetText(strHolder);
	player1score.Draw(globalData.screen, theGame->GetTopX()+310,theGame->GetTopY()+100);
	if (theGame->GetAIenabled()) {
		player1name.SetText(_("AI"));
	}
	else if (singlePuzzle) {
		player1name.SetText(_("Playing field"));
	}
	else {
		player1name.SetText(globalData.player1name);
	}
	player1name.Draw(globalData.screen, theGame->GetTopX()+10,theGame->GetTopY()-34);
	if (theGame->isTimeTrial()) {
		int tid = (int)SDL_GetTicks()-theGame->GetGameStartedAt();
		int minutes;
		int seconds;
		if (tid>=0) {
			minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))/60/1000;
			seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))%(60*1000))/1000;
		}
		else {
			minutes = ((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))/60/1000;
			seconds = (((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))%(60*1000))/1000;
		}
		if (theGame->isGameOver()) {
			minutes=0;
		}
		if (theGame->isGameOver()) {
			seconds=0;
		}
		if (seconds>9) {
			strHolder = std::to_string(minutes)+":"+std::to_string(seconds);
		}
		else {
			strHolder = std::to_string(minutes)+":0"+std::to_string(seconds);
		}
		player1time.SetText(strHolder);
	}
	else {
		int minutes = ((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))/60/1000;
		int seconds = (((abs((int)SDL_GetTicks()-(int)theGame->GetGameStartedAt())))%(60*1000))/1000;
		if (theGame->isGameOver()) {
			minutes=(theGame->GetGameEndedAt()/1000/60)%100;
		}
		if (theGame->isGameOver()) {
			seconds=(theGame->GetGameEndedAt()/1000)%60;
		}
		if (seconds>9) {
			strHolder = std::to_string(minutes)+":"+std::to_string(seconds);
		}
		else {
			strHolder = std::to_string(minutes)+":0"+std::to_string(seconds);
		}
		player1time.SetText(strHolder);
	}
	player1time.Draw(globalData.screen, theGame->GetTopX()+310,theGame->GetTopY()+150);
	strHolder = std::to_string(theGame->GetChains());
	player1chain.SetText(strHolder);
	player1chain.Draw(globalData.screen, theGame->GetTopX()+310,theGame->GetTopY()+200);
	//drawspeedLevel:
	strHolder = std::to_string(theGame->GetSpeedLevel());
	player1speed.SetText(strHolder);
	player1speed.Draw(globalData.screen, theGame->GetTopX()+310,theGame->GetTopY()+250);
	if ((theGame->isStageClear()) &&(theGame->GetTopY()+700+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1<600+theGame->GetTopY())) {
		oldBubleX = theGame->GetTopX()+280;
		oldBubleY = theGame->GetTopY()+650+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1;
		DrawIMG(stageBobble,globalData.screen,theGame->GetTopX()+280,theGame->GetTopY()+650+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1);
	}
	//player1 finnish, player2 start
	if (true ) {
		/*
		 *If single player mode (and not VS)
		 */
		if (!twoPlayers && !theGame->isGameOver()) {
			//Blank player2's board:
			DrawIMG(backBoard,globalData.screen,theGame2->GetTopX(),theGame2->GetTopY());
			//Write a description:
			std::string gametypeName;
			std::string infostring;
			if (theGame->isTimeTrial()) {
				gametypeName = _("Time Trial");
				infostring = _("Score as much as possible in 2 minutes");

			}
			else if (theGame->isStageClear()) {
				gametypeName = _("Stage Clear");
				infostring = _("You must clear a number of lines. Speed is rapidly increased.");
			}
			else if (theGame->isPuzzleMode()) {
				gametypeName = _("Puzzle");
				infostring = _("Clear the entire board with a limited number of moves.");
			}
			else {
				gametypeName = _("Endless");
				infostring = _("Score as much as possible. No time limit.");
			}
			if (infostring.length() > 0) {
				static sago::SagoTextBox infoBox;
				static sago::SagoTextField objectiveField;
				static sago::SagoTextField gametypeNameField;
				sagoTextSetHelpFont(infoBox);
				infoBox.SetMaxWidth(290);
				infoBox.SetText(infostring);
				sagoTextSetHelpFont(objectiveField);
				objectiveField.SetText(_("Objective:"));
				sagoTextSetHelpFont(gametypeNameField);
				gametypeNameField.SetText(gametypeName);
				gametypeNameField.Draw(globalData.screen, theGame2->GetTopX()+7,theGame2->GetTopY()+10);
				objectiveField.Draw(globalData.screen, theGame2->GetTopX()+7, theGame2->GetTopY()+160);
				infoBox.Draw(globalData.screen, theGame2->GetTopX()+7, theGame2->GetTopY()+160+32);
			}

			//Write the keys that are in use
			int y = theGame2->GetTopY()+400;
			std::string controldBoxText = std::string(_("Movement keys:"))+"\n"+getKeyName(keySettings[0].left)+", "+getKeyName(keySettings[0].right)+",\n"
			                              + getKeyName(keySettings[0].up)+", "+getKeyName(keySettings[0].down)+"\n"
			                              + _("Switch: ") + getKeyName(keySettings[0].change);
			if (theGame->isPuzzleMode()) {
				controldBoxText += std::string("\n") + _("Restart: ")+getKeyName(keySettings[0].push);
			}
			else {
				controldBoxText += std::string("\n") + _("Push line: ")+getKeyName(keySettings[0].push);
			}
			static sago::SagoTextBox controldBox;
			controldBox.SetHolder(&globalData.spriteHolder->GetDataHolder());
			sagoTextSetHelpFont(controldBox);
			controldBox.SetMaxWidth(290);
			controldBox.SetText(controldBoxText);
			controldBox.Draw(globalData.screen, theGame2->GetTopX()+7,y);
		}
		strHolder = std::to_string(theGame2->GetScore()+theGame2->GetHandicap());
		player2score.SetText(strHolder);
		player2score.Draw(globalData.screen, theGame2->GetTopX()+310, theGame2->GetTopY()+100);
		if (theGame2->GetAIenabled()) {
			player2name.SetText(_("AI"));
		}
		else {
			player2name.SetText(theGame2->name);
		}
		player2name.Draw(globalData.screen, theGame2->GetTopX()+10,theGame2->GetTopY()-34);
		if (theGame2->isTimeTrial()) {
			int tid = (int)SDL_GetTicks()-theGame2->GetGameStartedAt();
			int minutes;
			int seconds;
			if (tid>=0) {
				minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))/60/1000;
				seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))%(60*1000))/1000;
			}
			else {
				minutes = ((abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))/60/1000;
				seconds = (((abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())))%(60*1000))/1000;
			}
			if (theGame2->isGameOver()) {
				minutes=0;
			}
			if (theGame2->isGameOver()) {
				seconds=0;
			}
			if (seconds>9) {
				strHolder = std::to_string(minutes)+":"+std::to_string(seconds);
			}
			else {
				strHolder = std::to_string(minutes)+":0"+std::to_string(seconds);
			}
		}
		else {
			int minutes = (abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt()))/60/1000;
			int seconds = (abs((int)SDL_GetTicks()-(int)theGame2->GetGameStartedAt())%(60*1000))/1000;
			if (theGame2->isGameOver()) {
				minutes=(theGame2->GetGameEndedAt()/1000/60)%100;
			}
			if (theGame2->isGameOver()) {
				seconds=(theGame2->GetGameEndedAt()/1000)%60;
			}
			if (seconds>9) {
				strHolder = std::to_string(minutes)+":"+std::to_string(seconds);
			}
			else {
				strHolder = std::to_string(minutes)+":0"+std::to_string(seconds);
			}
		}
		player2time.SetText(strHolder);
		player2time.Draw(globalData.screen, theGame2->GetTopX()+310,theGame2->GetTopY()+150);
		strHolder = std::to_string(theGame2->GetChains());
		player2chain.SetText(strHolder);
		player2chain.Draw(globalData.screen, theGame2->GetTopX()+310,theGame2->GetTopY()+200);
		strHolder = std::to_string(theGame2->GetSpeedLevel());
		player2speed.SetText(strHolder);
		player2speed.Draw(globalData.screen, theGame2->GetTopX()+310,theGame2->GetTopY()+250);
	}
	//player2 finnish


	//draw exit
	bExit.Draw(globalData.screen,SDL_GetTicks(), xsize-bExitOffset, ysize-bExitOffset);
	DrawBalls();

#if DEBUG
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

static BlockGameSdl* player1;
static BlockGameSdl* player2;

static bool registerEndlessHighscore = false;
static bool registerTTHighscorePlayer1 = false;
static bool registerTTHighscorePlayer2 = false;
static bool saveReplay = false;

/**
 * startSpeed is a value from 0 to 4
 * */
static void StartSinglePlayerEndless(int startSpeed) {
	//1 player - endless
	BlockGameStartInfo startInfo;
	startInfo.ticks = SDL_GetTicks();
	startInfo.startBlocks = startInfo.ticks;
	startInfo.gameSpeed = startSpeed;
	player1->NewGame(startInfo);
	twoPlayers =false;
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	a.tick = startInfo.ticks;
	player2->DoAction(a);
	player1->name = globalData.player1name;
	player2->name = globalData.player2name;
	registerEndlessHighscore = true;
}

static void StartSinglePlayerTimeTrial() {
	BlockGameStartInfo startInfo;
	startInfo.ticks = SDL_GetTicks();
	startInfo.timeTrial = true;
	player1->NewGame(startInfo);
	twoPlayers =false;
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	a.tick = startInfo.ticks;
	player2->DoAction(a);
	//vsMode = false;
	player1->name = globalData.player1name;
	player2->name = globalData.player2name;
	registerTTHighscorePlayer1 = true;
	saveReplay = true;
}

static int StartSinglePlayerPuzzle() {
	BlockGameStartInfo startInfo;
	startInfo.ticks = SDL_GetTicks();
	startInfo.puzzleMode = true;
	startInfo.level = PuzzleLevelSelect(0);
	if (startInfo.level == -1) {
		return 1;
	}
	player1->NewGame(startInfo);
	DrawBackground(globalData.screen);
	twoPlayers = false;
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	a.tick = startInfo.ticks;
	player2->DoAction(a);
	player1->name = globalData.player1name;
	player2->name = globalData.player2name;
	return 0;
}


static void StarTwoPlayerTimeTrial() {
	BlockGameStartInfo startInfo;
	startInfo.ticks = SDL_GetTicks();
	startInfo.timeTrial = true;
	BlockGameStartInfo startInfo2 = startInfo;
	registerTTHighscorePlayer1 = true;
	registerTTHighscorePlayer2 = true;
	if (player1AI) {
		startInfo.AI = true;
		startInfo.level = player1AIlevel;
		registerTTHighscorePlayer1 = false;
	}
	if (player2AI) {
		startInfo2.AI = true;
		startInfo2.level = player2AIlevel;
		registerTTHighscorePlayer2 = false;
	}
	startInfo.gameSpeed = player1Speed;
	startInfo2.gameSpeed = player2Speed;
	startInfo.handicap = player1handicap;
	startInfo2.handicap = player2handicap;
	player1->NewGame(startInfo);
	player2->NewGame(startInfo2);
	twoPlayers = true;
	player1->name = globalData.player1name;
	player2->name = globalData.player2name;
}

static void StartTwoPlayerVs() {
	//2 player - VsMode
	BlockGameStartInfo startInfo;
	startInfo.ticks = SDL_GetTicks();
	startInfo.vsMode = true;
	startInfo.startBlocks = startInfo.ticks;
	BlockGameStartInfo startInfo2 = startInfo;
	if (player1AI) {
		startInfo.AI = true;
		startInfo.level = player1AIlevel;
	}
	if (player2AI) {
		startInfo2.AI = true;
		startInfo2.level = player2AIlevel;
	}
	startInfo.gameSpeed = player1Speed;
	startInfo2.gameSpeed = player2Speed;
	startInfo.handicap = player1handicap;
	startInfo2.handicap = player2handicap;
	player1->NewGame(startInfo);
	player2->NewGame(startInfo2);
	twoPlayers = true;
	player1->name = globalData.player1name;
	player2->name = globalData.player2name;
}

static void MoveBlockGameSdls( BlockGameSdl& game1, BlockGameSdl& game2 ) {
	game1.SetTopXY(globalData.xsize/2-440, globalData.ysize/2-284);
	game2.SetTopXY(globalData.xsize/2+40, globalData.ysize/2-284);
}

struct globalConfig {
	std::string savepath;
	std::vector<std::string> search_paths;
	std::string puzzleName;
	bool allowResize = true;
	bool autoScale = true;
	bool softwareRenderer = false;
};

static void ParseArguments(int argc, char* argv[], globalConfig& conf) {
	int consoleWidth = boost::program_options::options_description::m_default_line_length;
	const char* columnsEnv = getenv("COLUMNS"); // Allows using "COLUMNS=300 help2man" for generating the man page without bad line breaks.
	if (columnsEnv) {
		consoleWidth = sago::StrToLong(columnsEnv);
	}
	const char* commandname = "blockattack";
	if (argv[0]) {
		//NULL on Windows
		commandname = argv[0];
	}
	boost::program_options::options_description desc("Options", consoleWidth);
	desc.add_options()
	("help,h", "Displays this message")
	("version", "Display the version information")
	("config,c", boost::program_options::value<std::vector<std::string> >(), "Read a config file with the values. Can be given multiple times")
	("nosound", "Disables the sound. Can be used if sound errors prevents you from starting")
	("priority", "Causes the game to not sleep between frames.")
	("software-renderer", "Asks SDL2 to use software renderer")
	("verbose-basic", "Enables basic verbose messages")
	("verbose-game-controller", "Enables verbose messages regarding controllers")
	("print-search-path", "Prints the search path and quits")
	("no-auto-scale", "Do not automatically auto scale")
	("always-sixteen-nine", "Use 16:9 format even in Window mode")
	("puzzle-level-file", boost::program_options::value<std::string>(), "Sets the default puzzle file to load")
	("puzzle-single-level", boost::program_options::value<int>(), "Start the specific puzzle level directly")
#ifdef REPLAY_IMPLEMENTED
	("play-replay", boost::program_options::value<std::string>(), "Start a replay")
#endif
	("bind-text-domain", boost::program_options::value<std::string>(), fmt::format("Overwrites the bind text domain used for finding translations. "
	        "Default: \"{}\"", LOCALEDIR).c_str())
	("homepath", boost::program_options::value<std::string>(), fmt::format("Set the home folder where settings are saved. The directory will be created if it does not exist."
	        " Default: \"{}\"", getPathToSaveFiles()).c_str())
	("mod,m", boost::program_options::value<std::vector<std::string> >(), "Loads a mod. Later mods have preference")

	;
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "\n";
		std::cerr << desc << "\n";
		throw;
	}
	if (vm.count("config")) {
		std::vector<std::string> config_filenames = vm["config"].as<std::vector<std::string> >();
		for ( const std::string& s : config_filenames) {
			std::ifstream config_file(s);
			store(parse_config_file(config_file, desc), vm);
			notify(vm);
		}
	}
	if (vm.count("bind-text-domain")) {
		std::string s = vm["bind-text-domain"].as<std::string>();
		bindtextdomain (PACKAGE, s.c_str());
	}
	if (vm.count("homepath")) {
		std::string s = vm["homepath"].as<std::string>();
		setPathToSaveFiles(s);
		conf.savepath = getPathToSaveFiles();
	}
	if (vm.count("help")) {
		std::cout << fmt::format("Block Attack - Rise of the blocks {}\n\n"
		                         "Block Attack - Rise of the Blocks is a puzzle/blockfall game inspired by Tetris Attack for the SNES.\n\n"
		                         "{}\n\n", VERSION_NUMBER, "www.blockattack.net");
		std::cout << "Usage: "<< commandname << " [OPTION]..." << "\n";
		std::cout << desc << "\n";
		std::cout << "Examples:" << "\n";
		std::cout << "\tblockattack          \tStart the game normally" << "\n";
		std::cout << "\tblockattack --nosound\tStart the game without sound. Can be used if sound problems prevents the game from starting" << "\n";
		std::cout << "\tblockattack --puzzle-level-file puzzle.levels --puzzle-single-level 3\tStart the game with the default puzzles in level 3" << "\n";
		std::cout << "\tblockattack --bind-text-domain /dev/null\t Disables translations" << "\n";
		std::cout << "\n";
		std::cout << "Report bugs to the issue tracker here: <https://github.com/blockattack/blockattack-game/issues>" << "\n";
		exit(0);
	}
	if (vm.count("version")) {
		std::cout << "blockattack " << VERSION_NUMBER << "\n";
		std::cout << "\n";
		std::cout << "Copyright (C) 2005-2022 Poul Sander" << "\n";
		std::cout << "License GPLv2+: GNU GPL version 2 <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html> or later <http://gnu.org/licenses/gpl.html>" << "\n";
		std::cout << "This is free software: you are free to change and redistribute it." << "\n";
		std::cout << "There is NO WARRANTY, to the extent permitted by law." << "\n";
		exit(0);
	}
	if (vm.count("nosound")) {
		globalData.NoSound = true;
	}
	if (vm.count("priority")) {
		globalData.highPriority = true;
	}
	if (vm.count("software-renderer")) {
		conf.softwareRenderer = true;
	}
	if (vm.count("verbose-basic")) {
		globalData.verboseLevel++;
	}
	if (vm.count("verbose-game-controller")) {
		GameControllerSetVerbose(true);
	}
	if (vm.count("print-search-path")) {
		for (const std::string& s : conf.search_paths) {
			std::cout << s << "\n";
		}
		std::cout << conf.savepath << "\n";
		exit(0);
	}
	if (vm.count("puzzle-single-level")) {
		singlePuzzle = true;
		singlePuzzleNr = vm["puzzle-single-level"].as<int>();
	}
	if (vm.count("no-auto-scale")) {
		conf.autoScale = false;
	}
	if (vm.count("always-sixteen-nine")) {
		globalData.alwaysSixteenNine = true;
	}
	if (vm.count("puzzle-level-file")) {
		conf.puzzleName = vm["puzzle-level-file"].as<std::string>();
	}
	if (vm.count("play-replay")) {
		globalData.replayArgument = vm["play-replay"].as<std::string>();
	}
	if (vm.count("mod")) {
		globalData.modList = vm["mod"].as<std::vector<std::string> >();
	}

}

//Physfs 2.0.z does not have PHYSFS_unmount
#if (PHYSFS_VER_MAJOR <= 2) && (PHYSFS_VER_MINOR < 1)
#define PHYSFS_unmount PHYSFS_removeFromSearchPath
#endif

static void writeStateFile(const char* executable, const char* basedir) {
	std::string path = getPathToStateFiles();
	OsCreateFolder(path);
	std::ofstream stateFile;
	stateFile.open (path+"/game.txt");
	if (executable) {
		stateFile << "executable " << executable << "\n";
	}
	stateFile << "basedir " << basedir << "\n";
	stateFile << "SHAREDIR " << SHAREDIR << "\n";
}

//Warning: the arguments to main must be "int argc, char* argv[]" NO CONST! or SDL_main will fail to find it
int main(int argc, char* argv[]) {
	try {
		//Init the file system abstraction layer
		PHYSFS_init(argv[0]);
		globalConfig config;
		config.puzzleName = "puzzle.levels";
		FsSearchParthMainAppend(config.search_paths);
		config.savepath = getPathToSaveFiles();
		globalData.highPriority = false;   //if true the game will take most resources, but increase framerate.
		globalData.bFullscreen = false;
		//Set default Config variables:
		setlocale (LC_ALL, "");
		bindtextdomain (PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(PACKAGE, "utf-8");
		textdomain (PACKAGE);
		ParseArguments(argc, argv, config);
		OsCreateSaveFolder();
		writeStateFile(argv[0], PHYSFS_getBaseDir());
		PhysFsSetSearchPath(config.search_paths, config.savepath);
		/*if (globalData.modList.empty() && sago::FileExists(MODLIST_TXT))  {
		    std::string modString = sago::GetFileContent(MODLIST_TXT);
		    boost::split(globalData.modList, modString, boost::is_any_of(","));
		}*/
		globalData.modinfo.InitModList(globalData.modList);
		if (sago::FileExists(MODLIST_TXT)) {
			globalData.modinfo.ParseModFile(sago::GetFileContent(MODLIST_TXT));
			globalData.modList = globalData.modinfo.getModList();
		}
		std::cout << "Mod list: ";
		for (const std::string& s : globalData.modList) {
			std::cout << s << ",";
		}
		std::cout <<  "\n";
		if (globalData.modList.size()>0) {
			PHYSFS_unmount(config.savepath.c_str());
			FsSearchPathModAppend(config.search_paths, globalData.modList);
			PhysFsSetSearchPath(config.search_paths, config.savepath);
		}
		//Os create folders must be after the parameters because they can change the home folder
		PhysFsCreateFolders();
		bool gameShutdownProperly = true;
		if (sago::FileExists("gameRunning")) {
			gameShutdownProperly = false;
		}
		sago::WriteFileContent("gameRunning", "Started");
		globalData.SoundEnabled = true;
		globalData.MusicEnabled = true;
		twoPlayers = false; //true if two players splitscreen
		drawBalls = true;
		puzzleLoaded = false;
		theBallManager = BallManager();
		theExplosionManager = ExplosionManager();
		PuzzleSetName(config.puzzleName);
		//Init SDL
		if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
			sago::SagoFatalErrorF("Unable to init SDL: %s", SDL_GetError());
		}
		if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER ) != 0) {
			std::cerr << "Warning: Game controller failed to initialize. Reason: " << SDL_GetError() << "\n";
		}
		InitGameControllers();
		TTF_Init();
		atexit(SDL_Quit);       //quits SDL when the game stops for some reason (like you hit exit or Esc)
		globalData.theTextManager = TextManager();

		//Open Audio
		if (!globalData.NoSound) {
			//If sound has not been disabled, then load the sound system
			if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
				std::cerr << "Warning: Couldn't set 44100 Hz 16-bit audio - Reason: " << SDL_GetError() << "\n"
				          << "Sound will be disabled!" << "\n";
				globalData.NoSound = true; //Tries to stop all sound from playing/loading
			}
		}
		Config::getInstance()->setDefault("volume_music", "20"); //0-128


		if (globalData.verboseLevel) {
			//Copyright notice:
			std::cout << "Block Attack - Rise of the Blocks (" << VERSION_NUMBER << ")" << "\n" << "http://www.blockattack.net" << "\n" << "Copyright 2004-2022 Poul Sander" << "\n" <<
			          "A SDL2 based game (see www.libsdl.org)" << "\n" <<
			          "The game is available under the GPL, see COPYING for details." << "\n";
			std::cout << "-------------------------------------------" << "\n";
		}


		keySettings[player1keys].up= SDLK_UP;
		keySettings[player1keys].down = SDLK_DOWN;
		keySettings[player1keys].left = SDLK_LEFT;
		keySettings[player1keys].right = SDLK_RIGHT;
		keySettings[player1keys].change = SDLK_RCTRL;
		keySettings[player1keys].push = SDLK_RSHIFT;

		keySettings[player2keys].up= SDLK_w;
		keySettings[player2keys].down = SDLK_s;
		keySettings[player2keys].left = SDLK_a;
		keySettings[player2keys].right = SDLK_d;
		keySettings[player2keys].change = SDLK_LCTRL;
		keySettings[player2keys].push = SDLK_LSHIFT;

		globalData.player1name = defaultPlayerName();
		globalData.player2name = _("Player 2");

		Config* configSettings = Config::getInstance();
		//configSettings->setString("aNumber"," A string");
		//configSettings->save();
		int screenHeight = 768;
		if (configSettings->exists("fullscreen")) { //Test if an configFile exists
			globalData.bFullscreen = (bool)configSettings->getInt("fullscreen");
			globalData.MusicEnabled = (bool)configSettings->getInt("musicenabled");
			globalData.SoundEnabled = (bool)configSettings->getInt("soundenabled");

			if (configSettings->exists("sdl2_player1keyup")) {
				keySettings[0].up = (SDL_Keycode)configSettings->getInt("sdl2_player1keyup");
			}
			if (configSettings->exists("sdl2_player1keydown")) {
				keySettings[0].down = (SDL_Keycode)configSettings->getInt("sdl2_player1keydown");
			}
			if (configSettings->exists("sdl2_player1keyleft")) {
				keySettings[0].left = (SDL_Keycode)configSettings->getInt("sdl2_player1keyleft");
			}
			if (configSettings->exists("sdl2_player1keyright")) {
				keySettings[0].right = (SDL_Keycode)configSettings->getInt("sdl2_player1keyright");
			}
			if (configSettings->exists("sdl2_player1keychange")) {
				keySettings[0].change = (SDL_Keycode)configSettings->getInt("sdl2_player1keychange");
			}
			if (configSettings->exists("sdl2_player1keypush")) {
				keySettings[0].push = (SDL_Keycode)configSettings->getInt("sdl2_player1keypush");
			}

			if (configSettings->exists("sdl2_player2keyup")) {
				keySettings[2].up = (SDL_Keycode)configSettings->getInt("sdl2_player2keyup");
			}
			if (configSettings->exists("sdl2_player2keydown")) {
				keySettings[2].down = (SDL_Keycode)configSettings->getInt("sdl2_player2keydown");
			}
			if (configSettings->exists("sdl2_player2keyleft")) {
				keySettings[2].left = (SDL_Keycode)configSettings->getInt("sdl2_player2keyleft");
			}
			if (configSettings->exists("sdl2_player2keyright")) {
				keySettings[2].right = (SDL_Keycode)configSettings->getInt("sdl2_player2keyright");
			}
			if (configSettings->exists("sdl2_player2keychange")) {
				keySettings[2].change = (SDL_Keycode)configSettings->getInt("sdl2_player2keychange");
			}
			if (configSettings->exists("sdl2_player2keypush")) {
				keySettings[2].push = (SDL_Keycode)configSettings->getInt("sdl2_player2keypush");
			}
			if (configSettings->exists("player1name")) {
				globalData.player1name = configSettings->getString("player1name");
			}
			if (configSettings->exists("player2name")) {
				globalData.player2name = configSettings->getString("player2name");
			}
			if (configSettings->exists("xsize")) {
				globalData.xsize = configSettings->getInt("xsize");
			}
			if (configSettings->exists("ysize")) {
				globalData.ysize = configSettings->getInt("ysize");
			}
			if (configSettings->exists("screenHeight")) {
				screenHeight = configSettings->getInt("screenHeight");
			}
			if (globalData.verboseLevel) {
				std::cout << "Data loaded from config file" << "\n";
			}
		}
		else {
			if (globalData.verboseLevel) {
				std::cout << "Unable to load options file, using default values" << "\n";
			}
		}
		if (configSettings->getInt("always-software")) {
			config.softwareRenderer = true;
		}
		if (!gameShutdownProperly) {
			std::cerr << "Game not shotdown. Using software renderer.\n";
			config.softwareRenderer = true;
		}

		// "Block Attack - Rise of the Blocks"
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
		//Open video
		int createWindowParams = 0;
		if (config.allowResize) {
			createWindowParams |= SDL_WINDOW_RESIZABLE;
		}
		globalData.xsize = FOUR_THREE_WIDTH;
		if (globalData.alwaysSixteenNine) {
			globalData.xsize = SIXTEEN_NINE_WIDTH;
		}
		globalData.ysize = SCREEN_HIGHT;
		sdlWindow = SDL_CreateWindow("Block Attack - Rise of the Blocks " VERSION_NUMBER,
		                             SDL_WINDOWPOS_UNDEFINED,
		                             SDL_WINDOWPOS_UNDEFINED,
		                             (screenHeight)*globalData.xsize/globalData.ysize, screenHeight,
		                             createWindowParams );
		dieOnNullptr(sdlWindow, "Unable to create window");
		int rendererFlags = 0;
		if (config.softwareRenderer) {
			rendererFlags |= SDL_RENDERER_SOFTWARE;
		}
		SDL_Renderer* renderer = SDL_CreateRenderer(sdlWindow, -1, rendererFlags);
		dieOnNullptr(renderer, "Unable to create render");
		if (config.autoScale) {
			SDL_RenderSetLogicalSize(renderer, globalData.xsize, globalData.ysize);
			logicalRenderer = true;
		}
		if (globalData.verboseLevel) {
			SDL_RendererInfo info;
			SDL_GetRendererInfo(renderer, &info);
			std::cout << "Renderer: " << info.name << "\n";
		}
		globalData.screen = renderer;
		ResetFullscreen();
		SetSDLIcon(sdlWindow);

		if (globalData.verboseLevel) {
			std::cout << "Images loaded" << "\n";
		}

		BlockGameSdl theGame = BlockGameSdl(globalData.xsize/2-426, 100, &globalData.spriteHolder->GetDataHolder());            //creates game objects
		BlockGameSdl theGame2 = BlockGameSdl(globalData.xsize/2+40, 100, &globalData.spriteHolder->GetDataHolder());
		player1 = &theGame;
		player2 = &theGame2;

		BlockGameAction a;
		a.action = BlockGameAction::Action::SET_GAME_OVER;
		theGame.DoAction(a);
		theGame2.DoAction(a);


		//Takes names from file instead
		theGame.name = globalData.player1name;
		theGame2.name = globalData.player2name;

		if (singlePuzzle) {
			LoadPuzzleStages();
			BlockGameStartInfo s;
			s.puzzleMode = true;
			s.level = singlePuzzleNr;
			s.singlePuzzle = true;
			theGame.NewGame(s);
		}
		DrawBackground(globalData.screen);
		MoveBlockGameSdls(theGame, theGame2);
		DrawEverything(globalData.xsize, globalData.ysize, &theGame, &theGame2);
		SDL_RenderPresent(globalData.screen);
		if (singlePuzzle) {
			runGame(Gametype::Puzzle, singlePuzzleNr);
		}
		else if (globalData.replayArgument.length()) {
			ReplayPlayer rp;
			RunGameState(rp);
		}
		else {
			if (!gameShutdownProperly) {
				SafeModeMenu();
			}
			//game loop
			MainMenu();
		}



		//Saves options
		if (true) {
			configSettings->setInt("fullscreen",(int)globalData.bFullscreen);
			configSettings->setInt("musicenabled",(int)globalData.MusicEnabled);
			configSettings->setInt("soundenabled",(int)globalData.SoundEnabled);

			configSettings->setInt("sdl2_player1keyup",(int)keySettings[0].up);
			configSettings->setInt("sdl2_player1keydown",(int)keySettings[0].down);
			configSettings->setInt("sdl2_player1keyleft",(int)keySettings[0].left);
			configSettings->setInt("sdl2_player1keyright",(int)keySettings[0].right);
			configSettings->setInt("sdl2_player1keychange",(int)keySettings[0].change);
			configSettings->setInt("sdl2_player1keypush",(int)keySettings[0].push);

			configSettings->setInt("sdl2_player2keyup",(int)keySettings[2].up);
			configSettings->setInt("sdl2_player2keydown",(int)keySettings[2].down);
			configSettings->setInt("sdl2_player2keyleft",(int)keySettings[2].left);
			configSettings->setInt("sdl2_player2keyright",(int)keySettings[2].right);
			configSettings->setInt("sdl2_player2keychange",(int)keySettings[2].change);
			configSettings->setInt("sdl2_player2keypush",(int)keySettings[2].push);

			configSettings->setString("player1name", globalData.player1name);
			configSettings->setString("player2name", globalData.player2name);
			if (!globalData.bFullscreen) {
				//Store physical height of window
				int height = 0;
				SDL_GetWindowSize(sdlWindow, nullptr, &height);
				configSettings->setInt("screenHeight", height);
			}
			configSettings->save();
		}

		//calculate uptime:
		//int hours, mins, secs,
		commonTime ct = TimeHandler::ms2ct(SDL_GetTicks());

		if (globalData.verboseLevel) {
			std::cout << fmt::format("Block Attack - Rise of the Blocks ran for: {} hours {:02} minutes and {:02} seconds", ct.hours, ct.minutes, ct.seconds) << "\n";
		}

		/*std::string modListString;
		if (globalData.modList.size()>0) {
		    modListString = globalData.modList.at(0);
		    for (size_t i = 1; i < globalData.modList.size(); ++i) {
		        modListString += std::string(",")+globalData.modList[i];
		    }
		}*/
		ct = TimeHandler::addTime("totalTime",ct);
		if (globalData.verboseLevel) {
			std::cout << fmt::format("Total run time is now: {} days, {} hours, {:02} minutes and {:02} seconds\n", ct.days, ct.hours, ct.minutes, ct.seconds);
			//std::cout << "Mods loaded: " << modListString << "\n";
		}
		//sago::WriteFileContent(MODLIST_TXT, modListString);
		Stats::getInstance()->save();
		Config::getInstance()->save();
	}
	catch (std::exception& e) {
		sago::SagoFatalError(e.what());
	}
	PHYSFS_delete("gameRunning");
	//Close file system Apstraction layer!
	PHYSFS_deinit();
	return 0;
}

int runGame(Gametype gametype, int level) {
	Highscore theTopScoresTimeTrial = Highscore("timetrial", 0.5);
	drawBalls = true;
	puzzleLoaded = false;
	bool bNearDeath = false;  //Play music faster or louder while tru

	theBallManager = BallManager();
	theExplosionManager = ExplosionManager();
	BlockGameSdl theGame = BlockGameSdl(globalData.xsize/2-426, 100, &globalData.spriteHolder->GetDataHolder());  //creates game objects
	BlockGameSdl theGame2 = BlockGameSdl(globalData.xsize/2+4, 100, &globalData.spriteHolder->GetDataHolder());
	player1 = &theGame;
	player2 = &theGame2;
	theGame.DoPaintJob();  //Makes sure what there is something to paint
	theGame2.DoPaintJob();
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	theGame.DoAction(a);
	theGame2.DoAction(a);

	//Takes names from file instead
	theGame.name = globalData.player1name;
	theGame2.name = globalData.player2name;

	bool mustsetupgame = true;

	if (singlePuzzle) {
		LoadPuzzleStages();
		BlockGameStartInfo s;
		s.puzzleMode = true;
		s.level = singlePuzzleNr;
		s.singlePuzzle = true;
		theGame.NewGame(s);
		mustsetupgame = false;
	}
	//game loop
	int done = 0;
	if (globalData.verboseLevel) {
		std::cout << "Starting game loop" << "\n";
	}


	while (done == 0) {
		if (mustsetupgame) {
			registerEndlessHighscore = false;
			registerTTHighscorePlayer1 = false;
			registerTTHighscorePlayer2 = false;
			switch (gametype) {
			case Gametype::SinglePlayerTimeTrial:
				StartSinglePlayerTimeTrial();
				break;
			case Gametype::StageClear: {
				int myLevel = PuzzleLevelSelect(1);
				if (myLevel == -1) {
					return 1;
				}
				BlockGameStartInfo s;
				s.ticks = SDL_GetTicks();
				s.stageClear = true;
				s.level = myLevel;
				theGame.NewGame(s);
				DrawBackground(globalData.screen);
				twoPlayers =false;
				BlockGameAction a;
				a.action = BlockGameAction::Action::SET_GAME_OVER;
				theGame2.DoAction(a);
				theGame.name = globalData.player1name;
				theGame2.name = globalData.player2name;
			}
			break;
			case Gametype::Puzzle:
				if (StartSinglePlayerPuzzle()) {
					return 1;
				}
				break;
			case Gametype::SinglePlayerVs: {
				//1 player - Vs mode
				int theAIlevel = level; //startSingleVs();
				BlockGameStartInfo startInfo;
				startInfo.ticks = SDL_GetTicks();
				startInfo.vsMode = true;
				startInfo.vsAI = true;
				startInfo.level = theAIlevel;
				theGame.NewGame(startInfo);
				startInfo.AI = true;
				theGame2.NewGame(startInfo);
				DrawBackground(globalData.screen);
				twoPlayers = true; //Single player, but AI plays
				theGame.name = globalData.player1name;
				theGame2.name = globalData.player2name;
			}
			break;
			case Gametype::TwoPlayerTimeTrial:
				StarTwoPlayerTimeTrial();
				break;
			case Gametype::TwoPlayerVs:
				StartTwoPlayerVs();
				break;
			case Gametype::SinglePlayerEndless:
			default:
				StartSinglePlayerEndless(level);
			};
			mustsetupgame = false;
			DrawBackground(globalData.screen);
			MoveBlockGameSdls(theGame, theGame2);
			DrawEverything(globalData.xsize, globalData.ysize, &theGame, &theGame2);
			SDL_RenderPresent(globalData.screen);
		}

		if (!(globalData.highPriority)) {
			SDL_Delay(1);
		}

		DrawBackground(globalData.screen);
		//updates the balls and explosions:g
		theBallManager.update();
		theExplosionManager.update();
		globalData.theTextManager.update();

		bool mustWriteScreenshot = false;

		BlockGameAction a;
		a.tick = SDL_GetTicks();
		if (true) {
			SDL_Event event;

			while ( SDL_PollEvent(&event) ) {
				UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);
				if ( event.type == SDL_QUIT ) {
					Config::getInstance()->setShuttingDown(5);
					done = 1;
				}

				if (theGame.isGameOver() && isEscapeEvent(event)) {
					done = 1;
				}

				if (event.type == SDL_CONTROLLERBUTTONDOWN) {
					if ( event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK ) {
						done=1;
						DrawBackground(globalData.screen);
					}
				}

				if ( event.type == SDL_KEYDOWN ) {
					if ( event.key.keysym.sym == SDLK_ESCAPE || ( event.key.keysym.sym == SDLK_RETURN && theGame.isGameOver() ) ) {
						done=1;
						DrawBackground(globalData.screen);
					}
					if (!theGame.GetAIenabled()) {
						//player1:
						if ( event.key.keysym.sym == keySettings[player1keys].up ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'N';
							theGame.DoAction(a);
						}
						if ( event.key.keysym.sym == keySettings[player1keys].down ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'S';
							theGame.DoAction(a);
						}
						if ( (event.key.keysym.sym == keySettings[player1keys].left) ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'W';
							theGame.DoAction(a);
						}
						if ( (event.key.keysym.sym == keySettings[player1keys].right) ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'E';
							theGame.DoAction(a);
						}
						if ( event.key.keysym.sym == keySettings[player1keys].push ) {
							a.action = BlockGameAction::Action::PUSH;
							theGame.DoAction(a);
						}
						if ( event.key.keysym.sym == keySettings[player1keys].change ) {
							a.action = BlockGameAction::Action::SWITCH;
							theGame.DoAction(a);
						}
					}
					if (!theGame2.GetAIenabled()) {
						//player2:
						if ( event.key.keysym.sym == keySettings[player2keys].up ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'N';
							theGame2.DoAction(a);
						}
						if ( event.key.keysym.sym == keySettings[player2keys].down ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'S';
							theGame2.DoAction(a);
						}
						if ( (event.key.keysym.sym == keySettings[player2keys].left) ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'W';
							theGame2.DoAction(a);
						}
						if ( (event.key.keysym.sym == keySettings[player2keys].right) ) {
							a.action = BlockGameAction::Action::MOVE;
							a.way = 'E';
							theGame2.DoAction(a);
						}
						if ( event.key.keysym.sym == keySettings[player2keys].push ) {
							a.action = BlockGameAction::Action::PUSH;
							theGame2.DoAction(a);
						}
						if ( event.key.keysym.sym == keySettings[player2keys].change ) {
							a.action = BlockGameAction::Action::SWITCH;
							theGame2.DoAction(a);
						}
					}
					//common:
					if (!singlePuzzle) {
						if ( event.key.keysym.sym == SDLK_F2 ) {
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
						if ( event.key.keysym.sym == SDLK_F10 ) {
							//StartReplay("/home/poul/.gamesaves/blockattack/quicksave");
						}
						if ( event.key.keysym.sym == SDLK_F9 ) {
							mustWriteScreenshot = true;
						}
						if ( event.key.keysym.sym == SDLK_F5 ) {
						}
						if ( event.key.keysym.sym == SDLK_F11 ) {
						} //F11
					}
					if ( event.key.keysym.sym == SDLK_F12 ) {
						done=1;
					}
				}

				if (isPlayerUpEvent(1, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'N';
					theGame.DoAction(a);
				}
				if (isPlayerDownEvent(1, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'S';
					theGame.DoAction(a);
				}
				if (isPlayerLeftEvent(1, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'W';
					theGame.DoAction(a);
				}
				if (isPlayerRightEvent (1, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'E';
					theGame.DoAction(a);
				}
				if (isPlayerSwitchEvent(1, event)) {
					a.action = BlockGameAction::Action::SWITCH;
					theGame.DoAction(a);
				}
				if (isPlayerPushEvent(1, event)) {
					a.action = BlockGameAction::Action::PUSH;
					theGame.DoAction(a);
				}
				if (isPlayerUpEvent(2, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'N';
					theGame2.DoAction(a);
				}
				if (isPlayerDownEvent(2, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'S';
					theGame2.DoAction(a);
				}
				if (isPlayerLeftEvent(2, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'W';
					theGame2.DoAction(a);
				}
				if (isPlayerRightEvent (2, event)) {
					a.action = BlockGameAction::Action::MOVE;
					a.way = 'E';
					theGame2.DoAction(a);
				}
				if (isPlayerSwitchEvent(2, event)) {
					a.action = BlockGameAction::Action::SWITCH;
					theGame2.DoAction(a);
				}
				if (isPlayerPushEvent(2, event)) {
					a.action = BlockGameAction::Action::PUSH;
					theGame2.DoAction(a);
				}
				static int mouseDownX = 0;
				static int mouseDownY = 0;
				if (event.type == SDL_MOUSEBUTTONDOWN) {
					if (event.button.button == SDL_BUTTON_LEFT) {
						bool pressed = false;
						int x = 0;
						int y = 0;
						theGame.GetBrickCoordinateFromMouse(pressed, event.button.x, event.button.y, x, y);
						if (pressed) {
							a.action = BlockGameAction::Action::MOUSE_DOWN;
							a.x = x;
							a.y = y;
							theGame.DoAction(a);
						}
						theGame2.GetBrickCoordinateFromMouse(pressed, event.button.x, event.button.y, x, y);
						if (pressed) {
							a.action = BlockGameAction::Action::MOUSE_DOWN;
							a.x = x;
							a.y = y;
							theGame2.DoAction(a);
						}
						mouseDownX = event.button.x;
						mouseDownY = event.button.y;
					}
					if (event.button.button == SDL_BUTTON_RIGHT) {
						bool pressed = false;
						int x = 0;
						int y = 0;
						theGame.GetBrickCoordinateFromMouse(pressed, event.button.x, event.button.y, x, y);
						if (pressed) {
							a.action = BlockGameAction::Action::PUSH;
							theGame.DoAction(a);
						}
						theGame2.GetBrickCoordinateFromMouse(pressed, event.button.x, event.button.y, x, y);
						if (pressed) {
							a.action = BlockGameAction::Action::PUSH;
							theGame2.DoAction(a);
						}
					}
				}
				if (event.type == SDL_MOUSEBUTTONUP) {
					if (event.button.button == SDL_BUTTON_LEFT) {
						int x = event.button.x;
						int y = event.button.y;
						a.action = BlockGameAction::Action::MOUSE_UP;
						theGame.DoAction(a);
						theGame2.DoAction(a);
						if (theGame.IsInTheBoard(x,y) && theGame.IsUnderTheBoard(mouseDownX, mouseDownY)) {
							a.action = BlockGameAction::Action::PUSH;
							theGame.DoAction(a);
						}
						if (theGame2.IsInTheBoard(x,y) && theGame2.IsUnderTheBoard(mouseDownX, mouseDownY)) {
							a.action = BlockGameAction::Action::PUSH;
							theGame2.DoAction(a);
						}
					}
				}
				if (event.type == SDL_MOUSEMOTION) {
					//cout << "Moved" << "\n";
					bool pressed = false;
					int x = 0;
					int y = 0;
					theGame.GetMouseCursor(pressed, x, y);
					if (pressed) {
						int mx = 0;
						int my = 0;
						theGame.GetBrickCoordinateFromMouse(pressed, event.motion.x, event.motion.y, mx, my);
						if (pressed) {
							if (mx != x) {
								a.action = BlockGameAction::Action::MOUSE_MOVE;
								a.x = mx;
								theGame.DoAction(a);
							}
						}
					}
					theGame2.GetMouseCursor(pressed, x, y);
					if (pressed) {
						int mx = 0;
						int my = 0;
						theGame2.GetBrickCoordinateFromMouse(pressed, event.motion.x, event.motion.y, mx, my);
						if (pressed) {
							if (mx != x) {
								a.action = BlockGameAction::Action::MOUSE_MOVE;
								a.x = mx;
								theGame2.DoAction(a);
							}
						}
					}

				}
			} //while event PollEvent - read keys

			// If the mouse button is released, make bMouseUp equal true
			if (! (SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
				bMouseUp=true;
			}

			// If the mouse button 2 is released, make bMouseUp2 equal true
			if ((SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(3))!=SDL_BUTTON(3)) {
				bMouseUp2=true;
			}

			if (true ) {
				//read mouse events
				if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
					//This is the mouse events
					bMouseUp = false;
					DrawBackground(globalData.screen);

					if (stageButtonStatus != SBdontShow && (globalData.mousex > theGame.GetTopX()+cordNextButton.x)
					        && (globalData.mousex < theGame.GetTopX()+cordNextButton.x+cordNextButton.xsize)
					        && (globalData.mousey > theGame.GetTopY()+cordNextButton.y)
					        && (globalData.mousey < theGame.GetTopY()+cordNextButton.y+cordNextButton.ysize)) {
						//Clicked the next button after a stage clear or puzzle
						nextLevel(theGame, SDL_GetTicks());
					}

					if (stageButtonStatus != SBdontShow && (globalData.mousex > theGame.GetTopX()+cordRetryButton .x)
					        &&(globalData.mousex < theGame.GetTopX()+cordRetryButton.x+cordRetryButton.xsize)
					        &&(globalData.mousey > theGame.GetTopY()+cordRetryButton.y)
					        &&(globalData.mousey < theGame.GetTopY()+cordRetryButton.y+cordRetryButton.ysize)) {
						//Clicked the retry button
						retryLevel(theGame, SDL_GetTicks());
					}

					if (globalData.mousex > globalData.xsize-bExitOffset && globalData.mousex < globalData.xsize-bExitOffset+bExitSize &&
					        globalData.mousey > globalData.ysize-bExitOffset && globalData.mousey < globalData.ysize-bExitOffset+bExitSize) {
						done = 1;
					}
				}

				//Mouse button 2:
				if ((SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(3))==SDL_BUTTON(3) && bMouseUp2) {
					bMouseUp2=false; //The button is pressed
				}
			}
		} //if !bScreenBocked;


		//Sees if music is stopped and if music is enabled
		if ((!globalData.NoSound)&&(!Mix_PlayingMusic())&&(globalData.MusicEnabled)&&(!bNearDeath)) {
			// then starts playing it.
			Mix_PlayMusic(bgMusic.get(), -1); //music loop
			int musicVolume = Config::getInstance()->getInt("volume_music");
			Mix_VolumeMusic(musicVolume);
		}

		if (bNearDeath!=bNearDeathPrev) {
			int musicVolume = Config::getInstance()->getInt("volume_music");
			if (bNearDeath) {
				if (!globalData.NoSound &&(globalData.MusicEnabled)) {
					Mix_PlayMusic(highbeatMusic.get(), 1);
					Mix_VolumeMusic(musicVolume);
				}
			}
			else {
				if (!globalData.NoSound &&(globalData.MusicEnabled)) {
					Mix_PlayMusic(bgMusic.get(), -1);
					Mix_VolumeMusic(musicVolume);
				}
			}
		}

		bNearDeathPrev = bNearDeath;


		//set bNearDeath to false theGame*.Update() will change to true as needed
		bNearDeath = theGame.IsNearDeath() || theGame2.IsNearDeath();
		//Updates the objects
		a.action = BlockGameAction::Action::UPDATE;
		theGame.DoAction(a);
		theGame2.DoAction(a);

//see if anyone has won (two players only)
#if NETWORK
		if (!networkPlay)
#endif
			if (twoPlayers) {
				if (theGame.isGameOver() && theGame2.isGameOver() && !theGame.GetIsWinner() && !theGame2.GetIsWinner()  ) {
					if (theGame.GetScore()+theGame.GetHandicap()>theGame2.GetScore()+theGame2.GetHandicap()) {
						BlockGameAction a;
						a.action = BlockGameAction::Action::SET_WON;
						theGame.DoAction(a);
					}
					else if (theGame.GetScore()+theGame.GetHandicap()<theGame2.GetScore()+theGame2.GetHandicap()) {
						BlockGameAction a;
						a.action = BlockGameAction::Action::SET_WON;
						theGame2.DoAction(a);
					}
					else {
						BlockGameAction a;
						a.action = BlockGameAction::Action::SET_DRAW;
						theGame.DoAction(a);
						theGame2.DoAction(a);
					}
					//twoPlayers = false;
				}
				if ((theGame.isGameOver()) && (!theGame2.isGameOver())) {
					BlockGameAction a;
					a.action = BlockGameAction::Action::SET_WON;
					theGame2.DoAction(a);
				}
				if ((!theGame.isGameOver()) && (theGame2.isGameOver())) {
					BlockGameAction a;
					a.action = BlockGameAction::Action::SET_WON;
					theGame.DoAction(a);
				}
				std::vector<GarbageStruct> gs;
				theGame.PopSendGarbage(gs);
				for (const GarbageStruct& g : gs ) {
					BlockGameAction a;
					a.action = BlockGameAction::Action::PUSH_GARBAGE;
					a.garbage.push_back(g);
					theGame2.DoAction(a);
				}
				gs.clear();
				theGame2.PopSendGarbage(gs);
				for (const GarbageStruct& g : gs ) {
					BlockGameAction a;
					a.action = BlockGameAction::Action::PUSH_GARBAGE;
					a.garbage.push_back(g);
					theGame.DoAction(a);
				}
			}

		if (theGame.isGameOver() && registerTTHighscorePlayer1) {
			registerTTHighscorePlayer1 = false;
			theTopScoresTimeTrial.addScore(theGame.name, theGame.GetScore());
		}
		if (theGame2.isGameOver() && registerTTHighscorePlayer2) {
			registerTTHighscorePlayer2 = false;
			theTopScoresTimeTrial.addScore(theGame2.name, theGame2.GetScore());
		}
		if (theGame.isGameOver() && registerEndlessHighscore) {
			Highscore theTopScoresEndless = Highscore("endless", theGame.GetBaseSpeed());
			registerEndlessHighscore = false;
			theTopScoresEndless.addScore(theGame.name, theGame.GetScore());
			theGame.EndlessHighscoreEvent();
		}
#ifdef REPLAY_IMPLEMENTED
		if (theGame.isGameOver() && saveReplay) {
			if (twoPlayers && theGame2.isGameOver()) {
				saveReplay = false;
				SaveReplay(theGame.GetBlockGameInfo(), theGame2.GetBlockGameInfo());
			}
			if (!twoPlayers) {
				saveReplay = false;
				SaveReplay(theGame.GetBlockGameInfo());
			}
		}
#endif

		//Once evrything has been checked, update graphics
		MoveBlockGameSdls(theGame, theGame2);
		DrawEverything(globalData.xsize, globalData.ysize, &theGame, &theGame2);
		//Draw the mouse:
		globalData.mouse.Draw(globalData.screen, SDL_GetTicks(), globalData.mousex, globalData.mousey);
		SDL_RenderPresent(globalData.screen);
		if (mustWriteScreenshot) {
			writeScreenShot();
		}
	} //game loop
	return 0;
}
