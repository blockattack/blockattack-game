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

#include "common.h"
#include "global.hpp"
#include "scopeHelpers.hpp"
#include "icon.hpp"
#include "os.hpp"

#include <string.h>

#include "version.h"

#define WITH_SDL 1

#include "sago/SagoSpriteHolder.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>           //Used for srand()
#include <sstream>          //Still used by itoa2
#include <string>
#include "SDL.h"            //The SDL libary, used for most things
#include <SDL_mixer.h>      //Used for sound & music
#include <SDL_image.h>      //To load PNG images!
#include <physfs.h>         //Abstract file system. To use containers
#include "Libs/NFont.h"
#include <vector>
#include "MenuSystem.h"
#include "puzzlehandler.hpp"
#include "stageclearhandler.hpp"
#include <memory>
#include "ScoresDisplay.hpp"

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

#include "highscore.h"      //Stores highscores
#include "ReadKeyboard.h"   //Reads text from keyboard
#include "stats.h"          //Saves general stats 
#include "replayhandler.hpp"

#include "common.h"
#include "gamecontroller.h"
#include <boost/program_options.hpp>
#include <fstream>

/*******************************************************************************
* All variables and constant has been moved to mainVars.inc for the overview.  *
*******************************************************************************/
#include "mainVars.inc"

using std::string;
using std::cerr;
using std::cout;
using std::exception;
using std::vector;


static int InitImages(sago::SagoSpriteHolder& holder);

static void FsSearchParthMainAppend(std::vector<std::string>& paths) {
	paths.push_back((string)SHAREDIR+"/blockattack.data");
	paths.push_back((string)PHYSFS_getBaseDir()+"/blockattack.data");
	paths.push_back((string)PHYSFS_getBaseDir()+"/data");
}

static void PhysFsSetSearchPath(const vector<string>& paths, const string& savepath) {
	for (const string& path : paths) {
		PHYSFS_addToSearchPath(path.c_str(),1);
	}
	PHYSFS_addToSearchPath(savepath.c_str(), 1);
	PHYSFS_setWriteDir(savepath.c_str());
}


static void PhysFsCreateFolders() {
	PHYSFS_mkdir("screenshots");
	PHYSFS_mkdir("replays");
	PHYSFS_mkdir("puzzles");
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
	bHighScore = holder.GetSprite("b_highscore");
	bBack = holder.GetSprite("b_blank");
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
	iGameOver = holder.GetSprite("i_game_over");
	iWinner = holder.GetSprite("i_winner");
	iDraw = holder.GetSprite("i_draw");
	iLoser = holder.GetSprite("i_loser");
	iChainFrame = holder.GetSprite("chain_frame");
	iLevelCheck = holder.GetSprite("i_level_check");
	iLevelCheckBox = holder.GetSprite("i_level_check_box");
	iLevelCheckBoxMarked = holder.GetSprite("i_level_check_box_marked");
	iCheckBoxArea = holder.GetSprite("i_check_box_area");
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
	bNext = holder.GetSprite("b_blank");
	bRetry = holder.GetSprite("b_blank");
	mouse = holder.GetSprite("mouse");
	menuMarked = holder.GetSprite("menu_marked");
	menuUnmarked = holder.GetSprite("menu_unmarked");
	backBoard = holder.GetSprite("back_board");

	SDL_Color nf_button_color, nf_standard_blue_color, nf_standard_small_color;
	memset(&nf_button_color,0,sizeof(SDL_Color));
	nf_button_color.b = 255;
	nf_button_color.g = 255;
	nf_button_color.r = 255;
	nf_button_color.a = 255;
	nf_standard_blue_color.b = 255;
	nf_standard_blue_color.g = 0;
	nf_standard_blue_color.r = 0;
	nf_standard_blue_color.a = 255;
	nf_standard_small_color.b = 0;
	nf_standard_small_color.g = 0;
	nf_standard_small_color.r = 200;
	nf_standard_small_color.a = 255;
	nf_button_font.load(screen, holder.GetDataHolder().getFontPtr("freeserif", 24), nf_button_color);
	nf_standard_blue_font.load(screen, holder.GetDataHolder().getFontPtr("freeserif", 30), nf_standard_blue_color);
	nf_standard_small_font.load(screen, holder.GetDataHolder().getFontPtr("freeserif", 16), nf_standard_small_color);
	nf_scoreboard_font.load(screen, holder.GetDataHolder().getFontPtr("penguinattack", 20), nf_button_color);

//Loads the sound if sound present
	if (!NoSound) {
		//And here the music:
		bgMusic = holder.GetDataHolder().getMusicPtr("bgmusic");
		highbeatMusic = holder.GetDataHolder().getMusicPtr("highbeat");
		//the music... we just hope it exists, else the user won't hear anything
		//Same goes for the sounds
		boing = holder.GetDataHolder().getSoundPtr("pop");
		applause = holder.GetDataHolder().getSoundPtr("applause");
		photoClick = holder.GetDataHolder().getSoundPtr("cameraclick");
		typingChunk = holder.GetDataHolder().getSoundPtr("typing");
		counterChunk = holder.GetDataHolder().getSoundPtr("counter");
		counterFinalChunk = holder.GetDataHolder().getSoundPtr("counter_final");
		const int soundVolume = 84;  //0-128
		Mix_VolumeChunk(boing, soundVolume);
		Mix_VolumeChunk(applause, soundVolume);
		Mix_VolumeChunk(photoClick, soundVolume);
		Mix_VolumeChunk(typingChunk, soundVolume);
		Mix_VolumeChunk(counterChunk, soundVolume);
		Mix_VolumeChunk(counterFinalChunk, soundVolume);
	} //All sound has been loaded or not
	return 0;
} //InitImages()



/*Draws a image from on a given Surface. Takes source image, destination surface and coordinates*/
void DrawIMG(const sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y) {
	sprite.Draw(target, SDL_GetTicks() ,x,y);
}

void DrawIMG_Bounded(const sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y, int minx, int miny, int maxx, int maxy) {
	SDL_Rect bounds;
	bounds.x = minx;
	bounds.y = miny;
	bounds.w = maxx-minx;
	bounds.h = maxy-miny;
	sprite.DrawBounded(target, SDL_GetTicks(),x,y,bounds);
}


static void NFont_Write(SDL_Renderer* target, int x, int y, const string& text) {
	nf_standard_blue_font.draw(target, x, y, "%s", text.c_str());
}

static void NFont_Write(SDL_Renderer* target, int x, int y, const char* text) {
	nf_standard_blue_font.draw(target, x, y, "%s", text);
}

SDL_Window* sdlWindow;

std::unique_ptr<sago::SagoDataHolder> dataHolder;
std::unique_ptr<sago::SagoSpriteHolder> spriteHolder;

void ResetFullscreen() {
	Mix_HaltMusic();  //We need to reload all data in case the screen type changes. Music must be stopped before unload.
	if (bFullscreen) {
		SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else {
		SDL_SetWindowFullscreen(sdlWindow, 0);
	}
	dataHolder.reset(new sago::SagoDataHolder(screen));
	spriteHolder.reset(new sago::SagoSpriteHolder( *(dataHolder.get()) ) );
	InitImages(*(spriteHolder.get()) );
	SDL_ShowCursor(SDL_DISABLE);
}


//The small things that are faaling when you clear something
class ABall {
private:
	double x = 0.0;
	double y = 0.0;
	double velocityY = 0.0;
	double velocityX = 0.0;
	int color = 0;
	unsigned long int lastTime = 0;
public:

	ABall() {
	}

	//constructor:
	ABall(int X, int Y, bool right, int coulor) {
		double tal = 1.0+((double)rand()/((double)RAND_MAX));
		velocityY = -tal*startVelocityY;
		lastTime = currentTime;
		x = (double)X;
		y = (double)Y;
		color = coulor;
		if (right) {
			velocityX = tal*VelocityX;
		}
		else {
			velocityX = -tal*VelocityX;
		}
	}  //constructor

	void update() {
		double timePassed = (((double)(currentTime-lastTime))/1000.0);  //time passed in seconds
		x = x+timePassed*velocityX;
		y = y+timePassed*velocityY;
		velocityY = velocityY + gravity*timePassed;
		if (y<1.0) {
			velocityY=10.0;
		}
		if ((velocityY>minVelocity) && (y>(double)(768-ballSize)) && (y<768.0)) {
			velocityY = -0.70*velocityY;
			y = 768.0-ballSize;
		}
		lastTime = currentTime;
	}

	int getX() {
		return (int)x;
	}

	int getY() {
		return (int)y;
	}

	int getColor() {
		return color;
	}
};  //aBall

static const int maxNumberOfBalls = 6*12*2*2;

class BallManager {
public:
	ABall ballArray[maxNumberOfBalls];
	bool ballUsed[maxNumberOfBalls];

	BallManager() {
		for (int i=0; i<maxNumberOfBalls; i++) {
			ballUsed[i] = false;
		}
	}

	//Adds a ball to the screen at given coordiantes, traveling right or not with color
	int addBall(int x, int y,bool right,int color) {
		int ballNumber = 0;
		//Find a free ball
		while (ballNumber<maxNumberOfBalls && ballUsed[ballNumber]) {
			ballNumber++;
		}
		//Could not find a free ball, return -1
		if (ballNumber==maxNumberOfBalls) {
			return -1;
		}
		currentTime = SDL_GetTicks();
		ballArray[ballNumber] = ABall(x,y,right,color);
		ballUsed[ballNumber] = true;
		return 1;
	}  //addBall

	void update() {
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfBalls; i++) {

			if (ballUsed[i]) {
				ballArray[i].update();
				if (ballArray[i].getY()>800 || ballArray[i].getX()>xsize || ballArray[i].getX()<-ballSize) {
					ballUsed[i] = false;
				}
			}
		}
	} //update


}; //theBallManager

static BallManager theBallManager;

//a explosions, non moving
class AnExplosion {
private:
	int x = 0;
	int y = 0;
	Uint8 frameNumber = 0;
#define frameLength 80
	//How long an image in an animation should be showed
#define maxFrame 4
	//How many images there are in the animation
	unsigned long int placeTime = 0; //Then the explosion occored
public:

	AnExplosion() {
	}

	//constructor:
	AnExplosion(int X, int Y) {
		placeTime = currentTime;
		x = X;
		y = Y;
		frameNumber=0;
	}  //constructor

	//true if animation has played and object should be removed from the screen
	bool removeMe() {
		frameNumber = (currentTime-placeTime)/frameLength;
		return (!(frameNumber<maxFrame));
	}

	int getX() {
		return (int)x;
	}

	int getY() {
		return (int)y;
	}

	int getFrame() {
		return frameNumber;
	}
};  //nExplosion

class ExplosionManager {
public:
	AnExplosion explosionArray[maxNumberOfBalls];
	bool explosionUsed[maxNumberOfBalls];

	ExplosionManager() {
		for (int i=0; i<maxNumberOfBalls; i++) {
			explosionUsed[i] = false;
		}
	}

	int addExplosion(int x, int y) {
		int explosionNumber = 0;
		while ( explosionNumber<maxNumberOfBalls && explosionUsed[explosionNumber]) {
			explosionNumber++;
		}
		if (explosionNumber==maxNumberOfBalls) {
			return -1;
		}
		currentTime = SDL_GetTicks();
		explosionArray[explosionNumber] = AnExplosion(x,y);
		explosionUsed[explosionNumber] = true;
		return 1;
	}  //addBall

	void update() {
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfBalls; i++) {

			if (explosionUsed[i]) {
				if (explosionArray[i].removeMe()) {
					explosionUsed[i] = false;
				}
			}
		}
	} //update


}; //explosionManager

static ExplosionManager theExplosionManager;

//text pop-up
class TextMessage {
private:
	int x = 0;
	int y = 0;
	string textt;
	unsigned long int time = 0;
	unsigned long int placeTime = 0; //Then the text was placed
public:

	TextMessage() {
	}

	//constructor:
	TextMessage(int X, int Y,const char* Text,unsigned int Time) {
		placeTime = currentTime;
		x = X;
		y = Y;
		textt = Text;
		time = Time;
	}  //constructor

	//true if the text has expired
	bool removeMe() {
		return currentTime-placeTime>time;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	const char* getText() {
		return textt.c_str();
	}
};  //text popup

class TextManager {
public:
	TextMessage textArray[maxNumberOfBalls];
	bool textUsed[maxNumberOfBalls];

	TextManager() {
		for (int i=0; i<maxNumberOfBalls; i++) {
			textUsed[i] = false;
		}
	}

	int addText(int x, int y,string Text,unsigned int Time) {
		int textNumber = 0;
		while (textNumber<maxNumberOfBalls && textUsed[textNumber]) {
			textNumber++;
		}
		if (textNumber==maxNumberOfBalls) {
			return -1;
		}
		currentTime = SDL_GetTicks();
		textArray[textNumber] = TextMessage(x,y,Text.c_str(),Time);
		textUsed[textNumber] = true;
		return 1;
	}  //addText

	void update() {
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfBalls; i++) {

			if (textUsed[i]) {
				if (textArray[i].removeMe()) {
					textUsed[i] = false;
				}
			}
		}
	} //update


}; //textManager

static TextManager theTextManager;

//Here comes the Block Game object
#include "BlockGame.hpp"
#include "os.hpp"
#include "sago/SagoMiscSdl2.hpp"
#include "ScoresDisplay.hpp"

#include "BlockGameSdl.inc"
#include "sago/SagoMisc.hpp"



//writeScreenShot saves the screen as a bmp file, it uses the time to get a unique filename
void writeScreenShot() {
	if (verboseLevel) {
		cout << "Saving screenshot" << "\n";
	}
	int rightNow = (int)time(nullptr);
	string buf = getPathToSaveFiles() + "/screenshots/screenshot"+itoa(rightNow)+".bmp";
	SDL_Surface* sreenshotSurface = SDL_CreateRGBSurface(0, 1024, 768, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(screen, NULL, SDL_PIXELFORMAT_ARGB8888, sreenshotSurface->pixels, sreenshotSurface->pitch);
	SDL_SaveBMP(sreenshotSurface, buf.c_str());
	SDL_FreeSurface(sreenshotSurface);
	if (!NoSound) {
		if (SoundEnabled) {
			Mix_PlayChannel(1,photoClick,0);
		}
	}
}

//Function to return the name of a key, to be displayed...
static string getKeyName(SDL_Keycode key) {
	string keyname(SDL_GetKeyName(key));
	return keyname;
}


void RunGameState(sago::GameStateInterface& state ) {
	int mousex,mousey;
	bool done = false;     //We are done!
	while (!done && !Config::getInstance()->isShuttingDown()) {
		state.Draw(screen);

		SDL_Delay(1);
		SDL_Event event;

		SDL_GetMouseState(&mousex,&mousey);
		bool mustWriteScreenshot = false;

		while ( SDL_PollEvent(&event) ) {
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

		mouse.Draw(screen, SDL_GetTicks(), mousex, mousey);
		SDL_RenderPresent(screen);
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
	d.backX = 20;
	d.backY = ysize-buttonYsize-20;
	d.nextX = xsize-buttonXsize-20;
	d.nextY = d.backY;
	d.xsize = xsize;
	d.ysize = ysize;
	d.buttonXsize = buttonXsize;
	d.buttonYsize = buttonYsize;
	RunGameState(d);
}


//Draws the balls and explosions
static void DrawBalls() {
	for (int i = 0; i< maxNumberOfBalls; i++) {
		if (theBallManager.ballUsed[i]) {
			DrawIMG(balls[theBallManager.ballArray[i].getColor()],screen,theBallManager.ballArray[i].getX(),theBallManager.ballArray[i].getY());
		} //if used
		if (theExplosionManager.explosionUsed[i]) {
			DrawIMG(explosion[theExplosionManager.explosionArray[i].getFrame()],screen,theExplosionManager.explosionArray[i].getX(),theExplosionManager.explosionArray[i].getY());
		}
		if (theTextManager.textUsed[i]) {
			int x = theTextManager.textArray[i].getX()-12;
			int y = theTextManager.textArray[i].getY()-12;
			DrawIMG(iChainFrame,screen,x,y);

			nf_standard_small_font.draw(screen, x+12,y+7, NFont::CENTER, "%s",theTextManager.textArray[i].getText());
		}
	} //for
}    //DrawBalls


//draws everything
void DrawEverything(int xsize, int ysize,BlockGameSdl* theGame, BlockGameSdl* theGame2) {
	SDL_ShowCursor(SDL_DISABLE);
	DrawIMG(backgroundImage,screen,0,0);
	theGame->DoPaintJob();
	theGame2->DoPaintJob();
	string strHolder;
	strHolder = itoa(theGame->GetScore()+theGame->GetHandicap());
	NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+100,strHolder.c_str());
	if (theGame->GetAIenabled()) {
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,_("AI") );
	}
	else if (editorMode || singlePuzzle) {
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,_("Playing field") );
	}
	else {
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,player1name);
	}
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
			strHolder = itoa(minutes)+":"+itoa(seconds);
		}
		else {
			strHolder = itoa(minutes)+":0"+itoa(seconds);
		}
		//if ((SoundEnabled)&&(!NoSound)&&(tid>0)&&(seconds<5)&&(minutes == 0)&&(seconds>1)&&(!(Mix_Playing(6)))) Mix_PlayChannel(6,heartBeat,0);
		NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+150,strHolder.c_str());
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
			strHolder = itoa(minutes)+":"+itoa(seconds);
		}
		else {
			strHolder = itoa(minutes)+":0"+itoa(seconds);
		}
		NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+150,strHolder.c_str());
	}
	strHolder = itoa(theGame->GetChains());
	NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+200,strHolder.c_str());
	//drawspeedLevel:
	strHolder = itoa(theGame->GetSpeedLevel());
	NFont_Write(screen, theGame->GetTopX()+310,theGame->GetTopY()+250,strHolder.c_str());
	if ((theGame->isStageClear()) &&(theGame->GetTopY()+700+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1<600+theGame->GetTopY())) {
		oldBubleX = theGame->GetTopX()+280;
		oldBubleY = theGame->GetTopY()+650+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1;
		DrawIMG(stageBobble,screen,theGame->GetTopX()+280,theGame->GetTopY()+650+50*(theGame->GetStageClearLimit()-theGame->GetLinesCleared())-theGame->GetPixels()-1);
	}
	//player1 finnish, player2 start
	//DrawIMG(boardBackBack,screen,theGame2->GetTopX()-60,theGame2->GetTopY()-68);
	if (!editorMode /*&& !singlePuzzle*/ ) {
		/*
		 *If single player mode (and not VS)
		 */
		if (!twoPlayers && !theGame->isGameOver()) {
			//Blank player2's board:
			DrawIMG(backBoard,screen,theGame2->GetTopX(),theGame2->GetTopY());
			//Write a description:
			string gametypeName;
			string infostring;
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
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+10, gametypeName);
				NFont_Write(screen, theGame2->GetTopX()+7,theGame2->GetTopY()+160, _("Objective:"));
				nf_standard_blue_font.drawBox(screen, { static_cast<float>(theGame2->GetTopX()+7),static_cast<float>(theGame2->GetTopY()+160+32), 280, 200}, "%s", infostring.c_str());
			}

			//Write the keys that are in use
			int y = theGame2->GetTopY()+400;
			NFont_Write(screen, theGame2->GetTopX()+7,y,_("Movement keys:") );
			NFont_Write(screen, theGame2->GetTopX()+7,y+40,(getKeyName(keySettings[0].left)+", "+getKeyName(keySettings[0].right)+"," ).c_str() );
			NFont_Write(screen, theGame2->GetTopX()+7,y+76,(getKeyName(keySettings[0].up)+", "+getKeyName(keySettings[0].down)).c_str() );
			NFont_Write(screen, theGame2->GetTopX()+7,y+120,( _("Switch: ")+getKeyName(keySettings[0].change) ).c_str() );
			if (theGame->isPuzzleMode()) {
				NFont_Write(screen, theGame2->GetTopX()+7,y+160,( _("Restart: ")+getKeyName(keySettings[0].push) ).c_str() );
			}
			else {
				NFont_Write(screen, theGame2->GetTopX()+7,y+160,( _("Push line: ")+getKeyName(keySettings[0].push) ).c_str() );
			}

		}
		strHolder = itoa(theGame2->GetScore()+theGame2->GetHandicap());
		NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+100,strHolder.c_str());
		if (theGame2->GetAIenabled()) {
			NFont_Write(screen, theGame2->GetTopX()+10,theGame2->GetTopY()-34,_("AI") );
		}
		else {
			NFont_Write(screen, theGame2->GetTopX()+10,theGame2->GetTopY()-34,theGame2->name);
		}
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
				strHolder = itoa(minutes)+":"+itoa(seconds);
			}
			else {
				strHolder = itoa(minutes)+":0"+itoa(seconds);
			}
			//if ((SoundEnabled)&&(!NoSound)&&(tid>0)&&(seconds<5)&&(minutes == 0)&&(seconds>1)&&(!(Mix_Playing(6)))) Mix_PlayChannel(6,heartBeat,0);
			NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+150,strHolder.c_str());
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
				strHolder = itoa(minutes)+":"+itoa(seconds);
			}
			else {
				strHolder = itoa(minutes)+":0"+itoa(seconds);
			}
			NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+150,strHolder.c_str());
		}
		strHolder = itoa(theGame2->GetChains());
		NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+200,strHolder.c_str());
		strHolder = itoa(theGame2->GetSpeedLevel());
		NFont_Write(screen, theGame2->GetTopX()+310,theGame2->GetTopY()+250,strHolder.c_str());
	}
	//player2 finnish


	//draw exit
	bExit.Draw(screen,SDL_GetTicks(), xsize-bExitOffset, ysize-bExitOffset);
	DrawBalls();

#if DEBUG
	Frames++;
	if (SDL_GetTicks() >= Ticks + 1000) {
		if (Frames > 999) {
			Frames=999;
		}
		snprintf(FPS, sizeof(FPS), "%lu fps", Frames);
		Frames = 0;
		Ticks = SDL_GetTicks();
	}

	nf_standard_blue_font.draw(screen, 800, 4, "%s", FPS);
#endif
}

//The function that allows the player to choose PuzzleLevel
int PuzzleLevelSelect(int Type) {
	const int xplace = 200;
	const int yplace = 300;
	int levelNr = 0;
	int mousex, mousey;
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
		DrawIMG(backgroundImage, screen, 0, 0);
		DrawIMG(iCheckBoxArea,screen,xplace,yplace);
		if (Type == 0) {
			NFont_Write(screen, xplace+12,yplace+2,_("Select Puzzle") );
		}
		if (Type == 1) {
			NFont_Write(screen, xplace+12,yplace+2, _("Stage Clear Level Select") );
		}
		//Now drow the fields you click in (and a V if clicked):
		for (int i = 0; i < nrOfLevels; i++) {
			DrawIMG(iLevelCheckBox,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			if (i==selected) {
				DrawIMG(iLevelCheckBoxMarked,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			}
			if (Type == 0 && PuzzleIsCleared(i)) {
				DrawIMG(iLevelCheck,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			}
			if (Type == 1 && IsStageCleared(i)) {
				DrawIMG(iLevelCheck,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
			}
		}

		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
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

		SDL_GetMouseState(&mousex,&mousey);
		if (mousex != oldmousex || mousey != oldmousey) {
			int tmpSelected = -1;
			int j;
			for (j = 0; (tmpSelected == -1) && ( (j<nrOfLevels/10)||((j<nrOfLevels/10+1)&&(nrOfLevels%10 != 0)) ); j++)
				if ((60+j*50<mousey-yplace)&&(mousey-yplace<j*50+92)) {
					tmpSelected = j*10;
				}
			if (tmpSelected != -1)
				for (int k = 0; (( (!(nrOfLevels%10) || k<nrOfLevels-10*(j-1)) )&&(k<10)); k++)
					if ((10+k*50<mousex-xplace)&&(mousex-xplace<k*50+42)) {
						tmpSelected +=k;
						selected = tmpSelected;
					}
		}
		oldmousey = mousey;
		oldmousex= mousex;

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) {
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
			string scoreString = SPrintStringF(_("Best score: %i"), GetStageScores(selected)) ;
			string timeString = SPrintStringF(_("Time used: %s"),"-- : --");

			if (GetStageTime(selected)>0) {
				timeString = SPrintStringF(_("Time used: %d : %02d"), GetStageTime(selected)/1000/60, (GetStageTime(selected)/1000)%60);
			}

			NFont_Write(screen, 200,200,scoreString.c_str());
			NFont_Write(screen, 200,250,timeString.c_str());
			string totalString = (boost::format(_("Total score: %1% in %2%:%3%"))%totalScore%(totalTime/1000/60)%((totalTime/1000)%60)).str();
			NFont_Write(screen, 200,600,totalString.c_str());
		}

		mouse.Draw(screen, SDL_GetTicks(), mousex, mousey);
		SDL_RenderPresent(screen); //draws it all to the screen

	}
	DrawIMG(backgroundImage, screen, 0, 0);
	return levelNr;
}

static BlockGameSdl* player1;
static BlockGameSdl* player2;

static bool registerEndlessHighscore = false;
static bool registerTTHighscorePlayer1 = false;
static bool registerTTHighscorePlayer2 = false;
static bool saveReplay = false;

static void StartSinglePlayerEndless() {
	//1 player - endless
	BlockGameStartInfo startInfo;
	startInfo.ticks = SDL_GetTicks();
	startInfo.startBlocks = startInfo.ticks;
	player1->NewGame(startInfo);
	twoPlayers =false;
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	a.tick = startInfo.ticks;
	player2->DoAction(a);
	player1->name = player1name;
	player2->name = player2name;
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
	player1->name = player1name;
	player2->name = player2name;
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
	DrawIMG(backgroundImage, screen, 0, 0);
	twoPlayers = false;
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	a.tick = startInfo.ticks;
	player2->DoAction(a);
	//vsMode = true;
	player1->name = player1name;
	player2->name = player2name;
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
	player1->name = player1name;
	player2->name = player2name;
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
	//vsMode = true;
	twoPlayers = true;
	player1->name = player1name;
	player2->name = player2name;
}

#ifdef ALLOW_RESIZE
static void ClampResolution(SDL_Window* win) {
	int w = 0;
	int h = 0;
	bool changed = false;
	SDL_GetWindowSize(win, &w, &h);
	if (w < 800) {
		w = 800;
		changed = true;
	}
	if (w > 1024) {
		w = 1024;
		changed = true;
	}
	if (h < 600) {
		h = 600;
		changed = true;
	}
	if (h > 768) {
		h = 768;
		changed = true;
	}
	if (changed) {
		SDL_SetWindowSize(win, w, h);
		cerr << "Changed to " << w << "," << h << "\n"; 
	}
}
#endif

struct globalConfig {
	string savepath;
	vector<string> search_paths;
	string puzzleName;
	bool allowResize = false;
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
	("config,c", boost::program_options::value<vector<string> >(), "Read a config file with the values. Can be given multiple times")
	("nosound", "Disables the sound. Can be used if sound errors prevents you from starting")
	("priority", "Causes the game to not sleep between frames.")
	("verbose-basic", "Enables basic verbose messages")
	("verbose-game-controller", "Enables verbose messages regarding controllers")
	("print-search-path", "Prints the search path and quits")
	("allow-resize", "Allow experimental windows resize support")
	("puzzle-level-file", boost::program_options::value<string>(), "Sets the default puzzle file to load")
	("puzzle-single-level", boost::program_options::value<int>(), "Start the specific puzzle level directly")
	("bind-text-domain", boost::program_options::value<string>(), SPrintStringF("Overwrites the bind text domain used for finding translations. "
			"Default: \"%s\"", LOCALEDIR).c_str())
	("homepath", boost::program_options::value<string>(), SPrintStringF("Set the home folder where settings are saved. The directory will be created if it does not exist."
			" Default: \"%s\"", getPathToSaveFiles().c_str()).c_str())

	;
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
	}
	catch (exception& e) {
		cerr << e.what() << "\n";
		cerr << desc << "\n";
		throw;
	}
	if (vm.count("config")) {
		vector<string> config_filenames = vm["config"].as<vector<string> >();
		for ( const string& s : config_filenames) {
			std::ifstream config_file(s);
			store(parse_config_file(config_file, desc), vm);
			notify(vm);
		}
	}
	if (vm.count("bind-text-domain")) {
		string s = vm["bind-text-domain"].as<string>();
		bindtextdomain (PACKAGE, s.c_str());
	}
	if (vm.count("homepath")) {
		string s = vm["homepath"].as<string>();
		setPathToSaveFiles(s);
		conf.savepath = getPathToSaveFiles();
	}
	if (vm.count("help")) {
		cout << SPrintStringF("Block Attack - Rise of the blocks %s\n\n"
							  "Block Attack - Rise of the Blocks is a puzzle/blockfall game inspired by Tetris Attack for the SNES.\n\n"
							  "%s\n\n", VERSION_NUMBER, "www.blockattack.net");
		cout << "Usage: "<< commandname << " [OPTION]..." << "\n";
		cout << desc << "\n";
		cout << "Examples:" << "\n";
		cout << "\tblockattack          \tStart the game normally" << "\n";
		cout << "\tblockattack --nosound\tStart the game without sound. Can be used if sound problems prevents the game from starting" << "\n";
		cout << "\n";
		cout << "Report bugs to the issue tracker here: <https://github.com/blockattack/blockattack-game/issues>" << "\n";
		exit(0);
	}
	if (vm.count("version")) {
		cout << "blockattack " << VERSION_NUMBER << "\n";
		cout << "\n";
		cout << "Copyright (C) 2005-2016 Poul Sander" << "\n";
		cout << "License GPLv2+: GNU GPL version 2 <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html> or later <http://gnu.org/licenses/gpl.html>" << "\n";
		cout << "This is free software: you are free to change and redistribute it." << "\n";
		cout << "There is NO WARRANTY, to the extent permitted by law." << "\n";
		exit(0);
	}
	if (vm.count("nosound")) {
		NoSound = true;
	}
	if (vm.count("priority")) {
		highPriority = true;
	}
	if (vm.count("verbose-basic")) {
		verboseLevel++;
	}
	if (vm.count("verbose-game-controller")) {
		GameControllerSetVerbose(true);
	}
	if (vm.count("print-search-path")) {
		for (const string& s : conf.search_paths) {
			cout << s << "\n";
		}
		cout << conf.savepath << "\n";
		exit(0);
	}
	if (vm.count("puzzle-single-level")) {
		singlePuzzle = true;
		singlePuzzleNr = vm["puzzle-single-level"].as<int>();
	}
	if(vm.count("allow-resize")) {
		conf.allowResize = true;
	}
	
	if (vm.count("puzzle-level-file")) {
		conf.puzzleName = vm["puzzle-level-file"].as<string>();
	}
		
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
		highPriority = false;   //if true the game will take most resources, but increase framerate.
		bFullscreen = false;
		//Set default Config variables:
		setlocale (LC_ALL, "");
		bindtextdomain (PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(PACKAGE, "utf-8");
		textdomain (PACKAGE);
		ParseArguments(argc, argv, config);
		OsCreateSaveFolder();
		PhysFsSetSearchPath(config.search_paths, config.savepath);
		//Os create folders must be after the paramters because they can change the home folder
		PhysFsCreateFolders();
		SoundEnabled = true;
		MusicEnabled = true;
		twoPlayers = false; //true if two players splitscreen
		theTopScoresEndless = Highscore("endless");
		theTopScoresTimeTrial = Highscore("timetrial");
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
			cerr << "Warning: Game controller failed to initialize. Reason: " << SDL_GetError() << "\n";
		}
		InitGameControllers();
		TTF_Init();
		atexit(SDL_Quit);       //quits SDL when the game stops for some reason (like you hit exit or Esc)
		theTextManager = TextManager();

		//Open Audio
		if (!NoSound) {
			//If sound has not been disabled, then load the sound system
			if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
				cerr << "Warning: Couldn't set 44100 Hz 16-bit audio - Reason: " << SDL_GetError() << "\n"
				     << "Sound will be disabled!" << "\n";
				NoSound = true; //Tries to stop all sound from playing/loading
			}
		}


		if (verboseLevel) {
			//Copyright notice:
			cout << "Block Attack - Rise of the Blocks (" << VERSION_NUMBER << ")" << "\n" << "http://www.blockattack.net" << "\n" << "Copyright 2004-2016 Poul Sander" << "\n" <<
			     "A SDL2 based game (see www.libsdl.org)" << "\n" <<
			     "The game is available under the GPL, see COPYING for details." << "\n";
			cout << "-------------------------------------------" << "\n";
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

		player1name = _("Player 1");
		player2name = _("Player 2");

		Config* configSettings = Config::getInstance();
		//configSettings->setString("aNumber"," A string");
		//configSettings->save();
		if (configSettings->exists("fullscreen")) { //Test if an configFile exists
			bFullscreen = (bool)configSettings->getInt("fullscreen");
			MusicEnabled = (bool)configSettings->getInt("musicenabled");
			SoundEnabled = (bool)configSettings->getInt("soundenabled");

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
				player1name = configSettings->getString("player1name");
			}
			if (configSettings->exists("player2name")) {
				player2name = configSettings->getString("player2name");
			}
			if (verboseLevel) {
				cout << "Data loaded from config file" << "\n";
			}
		}
		else {
			if (verboseLevel) {
				cout << "Unable to load options file, using default values" << "\n";
			}
		}

		// "Block Attack - Rise of the Blocks"
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
		//Open video
		int createWindowParams = 0; //SDL_WINDOW_RESIZABLE;
		if (config.allowResize) {
			createWindowParams |= SDL_WINDOW_RESIZABLE;
		}
		sdlWindow = SDL_CreateWindow("Block Attack - Rise of the Blocks " VERSION_NUMBER,
		                             SDL_WINDOWPOS_UNDEFINED,
		                             SDL_WINDOWPOS_UNDEFINED,
		                             xsize, ysize,
		                             createWindowParams );
		dieOnNullptr(sdlWindow, "Unable to create window");
		SDL_Renderer* renderer = SDL_CreateRenderer(sdlWindow, -1, 0);
		dieOnNullptr(renderer, "Unable to create render");
		if (config.allowResize) {
			SDL_RenderSetLogicalSize(renderer, xsize, ysize);
		}
		screen = renderer;
		ResetFullscreen();
		SetSDLIcon(sdlWindow);

		if (verboseLevel) {
			cout << "Images loaded" << "\n";
		}

		BlockGameSdl theGame = BlockGameSdl(50,100);            //creates game objects
		BlockGameSdl theGame2 = BlockGameSdl(xsize-500,100);
		player1 = &theGame;
		player2 = &theGame2;
		
		BlockGameAction a;
		a.action = BlockGameAction::Action::SET_GAME_OVER;
		theGame.DoAction(a);
		theGame2.DoAction(a);


		//Takes names from file instead
		theGame.name = player1name;
		theGame2.name = player2name;

		if (singlePuzzle) {
			LoadPuzzleStages();
			BlockGameStartInfo s;
			s.puzzleMode = true;
			s.level = singlePuzzleNr;
			s.singlePuzzle = true;
			theGame.NewGame(s);
		}
		SDL_RenderClear(screen);
		DrawIMG(backgroundImage, screen, 0, 0);
		DrawEverything(xsize,ysize,&theGame,&theGame2);
		SDL_RenderPresent(screen);
		if (singlePuzzle) {
			runGame(Gametype::Puzzle, singlePuzzleNr);
		}
		else {
			//game loop
			MainMenu();
		}



		//Saves options
		if (!editorMode) {
			configSettings->setInt("fullscreen",(int)bFullscreen);
			configSettings->setInt("musicenabled",(int)MusicEnabled);
			configSettings->setInt("soundenabled",(int)SoundEnabled);

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

			configSettings->setString("player1name",player1name);
			configSettings->setString("player2name",player2name);
			configSettings->save();
		}

		//calculate uptime:
		//int hours, mins, secs,
		commonTime ct = TimeHandler::ms2ct(SDL_GetTicks());

		if (verboseLevel) {
			cout << boost::format("Block Attack - Rise of the Blocks ran for: %1% hours %2% mins and %3% secs") % ct.hours % ct.minutes % ct.seconds << "\n";
		}

		ct = TimeHandler::addTime("totalTime",ct);
		if (verboseLevel) {
			cout << "Total run time is now: " << ct.days << " days " << ct.hours << " hours " << ct.minutes << " mins and " << ct.seconds << " secs" << "\n";
		}

		Stats::getInstance()->save();

		Config::getInstance()->save();



	}
	catch (exception& e) {
		sago::SagoFatalError(e.what());
	}
	//Close file system Apstraction layer!
	PHYSFS_deinit();
	return 0;
}


int runGame(Gametype gametype, int level) {
	int mousex, mousey;   //Mouse coordinates
	theTopScoresEndless = Highscore("endless");
	theTopScoresTimeTrial = Highscore("timetrial");
	drawBalls = true;
	puzzleLoaded = false;
	bool bNearDeath = false;                        //Play music faster or louder while tru

	theBallManager = BallManager();
	theExplosionManager = ExplosionManager();
	BlockGameSdl theGame = BlockGameSdl(50,100);            //creates game objects
	BlockGameSdl theGame2 = BlockGameSdl(xsize-500,100);
	player1 = &theGame;
	player2 = &theGame2;
	theGame.DoPaintJob();           //Makes sure what there is something to paint
	theGame2.DoPaintJob();
	BlockGameAction a;
	a.action = BlockGameAction::Action::SET_GAME_OVER;
	theGame.DoAction(a);
	theGame2.DoAction(a);

	//Takes names from file instead
	theGame.name = player1name;
	theGame2.name = player2name;

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
	if (verboseLevel) {
		cout << "Starting game loop" << "\n";
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
					DrawIMG(backgroundImage, screen, 0, 0);
					twoPlayers =false;
					BlockGameAction a;
					a.action = BlockGameAction::Action::SET_GAME_OVER;
					theGame2.DoAction(a);
					theGame.name = player1name;
					theGame2.name = player2name;
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
				DrawIMG(backgroundImage, screen, 0, 0);
				twoPlayers = true; //Single player, but AI plays
				theGame.name = player1name;
				theGame2.name = player2name;
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
				StartSinglePlayerEndless();
				break;
			};
			mustsetupgame = false;
			DrawIMG(backgroundImage, screen, 0, 0);
			DrawEverything(xsize,ysize,&theGame,&theGame2);
			SDL_RenderPresent(screen);
		}

		if (!(highPriority)) {
			SDL_Delay(1);
		}

		SDL_RenderClear(screen);
		DrawIMG(backgroundImage, screen, 0, 0);

		//updates the balls and explosions:g
		theBallManager.update();
		theExplosionManager.update();
		theTextManager.update();

		bool mustWriteScreenshot = false;

		BlockGameAction a;
		a.action = BlockGameAction::Action::NONE;
		a.tick = SDL_GetTicks();
		if (true) {
			SDL_Event event;

			while ( SDL_PollEvent(&event) ) {
				if ( event.type == SDL_QUIT ) {
					Config::getInstance()->setShuttingDown(5);
					done = 1;
				}
#if ALLOW_RESIZE
				if (event.type == SDL_WINDOWEVENT ) {
					if (event.window.event == SDL_WINDOWEVENT_RESIZED ) {
						cout << "Resized\n";
						ClampResolution(sdlWindow);
					}
				}
#endif
				
				if (theGame.isGameOver() && isEscapeEvent(event)) {
					done = 1;
				}

				if ( event.type == SDL_KEYDOWN ) {
					if ( event.key.keysym.sym == SDLK_ESCAPE || ( event.key.keysym.sym == SDLK_RETURN && theGame.isGameOver() ) ) {
						done=1;
						DrawIMG(backgroundImage, screen, 0, 0);

					}
					if ((!editorMode)&&(!editorModeTest)&&(!theGame.GetAIenabled())) {
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
					if (!editorMode && !theGame2.GetAIenabled()) {
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
					if ((!singlePuzzle)&&(!editorMode)) {
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

			SDL_GetMouseState(&mousex,&mousey);

			// If the mouse button is released, make bMouseUp equal true
			if (!SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) {
				bMouseUp=true;
			}

			// If the mouse button 2 is released, make bMouseUp2 equal true
			if ((SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(3))!=SDL_BUTTON(3)) {
				bMouseUp2=true;
			}

			if (!editorMode ) {
				//read mouse events
				if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
					//This is the mouse events
					bMouseUp = false;
					DrawIMG(backgroundImage, screen, 0, 0);

					if (stageButtonStatus != SBdontShow && (mousex > theGame.GetTopX()+cordNextButton.x)
					        &&(mousex < theGame.GetTopX()+cordNextButton.x+cordNextButton.xsize)
					        &&(mousey > theGame.GetTopY()+cordNextButton.y)&&(mousey < theGame.GetTopY()+cordNextButton.y+cordNextButton.ysize)) {
						//Clicked the next button after a stage clear or puzzle
						nextLevel(theGame, SDL_GetTicks());

					}
					if (stageButtonStatus != SBdontShow && (mousex > theGame.GetTopX()+cordRetryButton .x)
					        &&(mousex < theGame.GetTopX()+cordRetryButton.x+cordRetryButton.xsize)
					        &&(mousey > theGame.GetTopY()+cordRetryButton.y)&&(mousey < theGame.GetTopY()+cordRetryButton.y+cordRetryButton.ysize)) {
						//Clicked the retry button
						retryLevel(theGame, SDL_GetTicks());
					}

					if (mousex > xsize-bExitOffset && mousex < xsize-bExitOffset+bExitSize &&
					        mousey > ysize-bExitOffset && mousey < ysize-bExitOffset+bExitSize) {
						done = 1;
					}
					//cout << "Mouse x: " << mousex << ", mouse y: " << mousey << "\n";
				}

				//Mouse button 2:
				if ((SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(3))==SDL_BUTTON(3) && bMouseUp2) {
					bMouseUp2=false; //The button is pressed
				}
			}
		} //if !bScreenBocked;


		//Sees if music is stopped and if music is enabled
		if ((!NoSound)&&(!Mix_PlayingMusic())&&(MusicEnabled)&&(!bNearDeath)) {
			// then starts playing it.
			Mix_PlayMusic(bgMusic, -1); //music loop
			Mix_VolumeMusic((MIX_MAX_VOLUME*3)/10);
		}

		if (bNearDeath!=bNearDeathPrev) {
			if (bNearDeath) {
				if (!NoSound &&(MusicEnabled)) {
					Mix_PlayMusic(highbeatMusic, 1);
					Mix_VolumeMusic((MIX_MAX_VOLUME*5)/10);
				}
			}
			else {
				if (!NoSound &&(MusicEnabled)) {
					Mix_PlayMusic(bgMusic, -1);
					Mix_VolumeMusic((MIX_MAX_VOLUME*3)/10);
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
				if ((theGame.isGameOver()) && (theGame2.isGameOver())) {
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
				vector<GarbageStruct> gs;
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
			registerEndlessHighscore = false;
			theTopScoresEndless.addScore(theGame.name, theGame.GetScore());
			theGame.EndlessHighscoreEvent();
		}
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

		//Once evrything has been checked, update graphics
		DrawEverything(xsize,ysize,&theGame,&theGame2);
		SDL_GetMouseState(&mousex,&mousey);
		//Draw the mouse:
		mouse.Draw(screen, SDL_GetTicks(), mousex, mousey);
		SDL_RenderPresent(screen);
		if (mustWriteScreenshot) {
			writeScreenShot();
		}
	} //game loop
	return 0;
}
