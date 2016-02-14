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
http://blockattack.net
===========================================================================
*/

#include "common.h"
#include "global.hpp"
#include "scopeHelpers.hpp"
#include "icon.hpp"
#include "os.hpp"

#include <string.h>


#ifndef VERSION_NUMBER
#define VERSION_NUMBER "2.0.0 SNAPSHOT"
#endif

//If DEBUG is defined: AI info and FPS will be written to screen
#ifndef DEBUG
#define DEBUG 1
#endif

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
#include "physfs_stream.hpp" //To use C++ style file streams
#include "Libs/NFont.h"
//#include "ttfont.h"        //To use True Type Fonts in SDL
//#include "config.h"
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
#include "joypad.h"         //Used for joypads
#include "listFiles.h"      //Used to show files on screen
#include "stats.h"          //Saves general stats 
//#include "uploadReplay.h"   //Takes care of everything libcurl related

#include "common.h"
#include <boost/program_options.hpp>

/*******************************************************************************
* All variables and constant has been moved to mainVars.inc for the overview.  *
*******************************************************************************/
#include "mainVars.inc"

using namespace std;


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
	//We first create the folder there we will save (only on UNIX systems)
	//we call the external command "mkdir"... the user might have renamed this, but we hope he hasn't
	PHYSFS_mkdir("screenshots");
	PHYSFS_mkdir("replays");
	PHYSFS_mkdir("puzzles");
}

//Load all image files to memory
static int InitImages(sago::SagoSpriteHolder& holder) {
	bricks[0] = holder.GetSprite("blue");
	bricks[1] = holder.GetSprite("green");
	bricks[2] = holder.GetSprite("purple");
	bricks[3] = holder.GetSprite("red");
	bricks[4] = holder.GetSprite("turkish");
	bricks[5] = holder.GetSprite("yellow");
	bricks[6] = holder.GetSprite("grey");
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
	ready = holder.GetSprite("ready");
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
	dialogBox = holder.GetSprite("dialogbox");
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
	} //All sound has been loaded or not
	return 0;
} //InitImages()


static stringstream converter;

//Function to convert numbers to string (2 diget)
static string itoa2(int num) {
	converter.str(std::string());
	converter.clear();
	if (num<10) {
		converter << "0";
	}
	converter << num;
	return converter.str();
}

/*Draws a image from on a given Surface. Takes source image, destination surface and coordinates*/
void DrawIMG(sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y) {
	sprite.Draw(target, SDL_GetTicks() ,x,y);
}

void DrawIMG_Bounded(sago::SagoSprite& sprite, SDL_Renderer* target, int x, int y, int minx, int miny, int maxx, int maxy) {
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

void ResetFullscreen() {
#if defined(_WIN32)
	/*  if (bFullscreen) {
	        screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_FULLSCREEN|SDL_ANYFORMAT);
	    }
	    else {
	        screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_ANYFORMAT);
	    }
	    DrawIMG(background, screen, 0, 0);
	*/
#else
	//TODO: Find SDL2 alternative
	//SDL_WM_ToggleFullScreen(screen); //Will only work in Linux
#endif
	if (bFullscreen) {
		SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else {
		SDL_SetWindowFullscreen(sdlWindow, 0);
	}
	SDL_ShowCursor(SDL_DISABLE);
}


//The small things that are faaling when you clear something
class ABall {
private:
	double x;
	double y;
	double velocityY;
	double velocityX;
	int color;
	unsigned long int lastTime;
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
		while ((ballUsed[ballNumber])&&(ballNumber<maxNumberOfBalls)) {
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
	int x;
	int y;
	Uint8 frameNumber;
#define frameLength 80
	//How long an image in an animation should be showed
#define maxFrame 4
	//How many images there are in the animation
	unsigned long int placeTime; //Then the explosion occored
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
		while ((explosionUsed[explosionNumber])&&(explosionNumber<maxNumberOfBalls)) {
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
	int x;
	int y;
	string textt;
	unsigned long int time;
	unsigned long int placeTime; //Then the text was placed
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
#include "BlockGame.cpp"
#include "os.hpp"
#include "sago/SagoMiscSdl2.hpp"
#include "ScoresDisplay.hpp"

class BlockGameSdl : public BlockGame {
public:
	BlockGameSdl(int tx, int ty) {
		topx = tx;
		topy = ty;
	}

	void DrawImgBoard(sago::SagoSprite& img, int x, int y) const {
		DrawIMG(img, screen, x+topx, y+topy);
	}

	void DrawImgBoardBounded(sago::SagoSprite& img, int x, int y) const {
		DrawIMG_Bounded(img, screen, x+topx, y+topy, topx, topy, topx + backBoard.GetWidth(), topy + backBoard.GetHeight());
	}

	void PrintTextCenteredBoard(int x, int y, const char* text) {
		nf_button_font.draw(screen, x+topx+60, y+topy+10, NFont::CENTER, "%s", text);
	}

	int GetTopX() const {
		return topx;
	}
	int GetTopY() const {
		return topy;
	}

	void AddText(int x, int y, const std::string& text, int time) const override {
		theTextManager.addText(topx-10+x*bsize, topy+12*bsize-y*bsize, text, time);
	}

	void AddBall(int x, int y, bool right, int color) const  override {
		theBallManager.addBall(topx+40+x*bsize, topy+bsize*12-y*bsize, right, color);
	}

	void AddExplosion(int x, int y) const  override {
		theExplosionManager.addExplosion(topx-10+x*bsize, topy+bsize*12-10-y*bsize);
	}

	void PlayerWonEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, applause, 0);
	}

	void DrawEvent() const override {
		Mix_HaltChannel(1);
	}

	void BlockPopEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(0, boing, 0);
	}

	void LongChainDoneEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, applause, 0);
	}

	void TimeTrialEndEvent() const  override {
		if (!NoSound && SoundEnabled) {
			Mix_PlayChannel(1,counterFinalChunk,0);
		}
	}

	void EndlessHighscoreEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, applause, 0);
	}
private:
	//Draws all the bricks to the board (including garbage)
	void PaintBricks() const {
		for (int i=0; ((i<13)&&(i<30)); i++)
			for (int j=0; j<6; j++) {
				if ((board[j][i]%10 != -1) && (board[j][i]%10 < 7) && ((board[j][i]/1000000)%10==0)) {
					DrawImgBoardBounded(bricks[board[j][i]%10],  j*bsize, bsize*12-i*bsize-pixels);
					if ((board[j][i]/BLOCKWAIT)%10==1) {
						DrawImgBoard(bomb,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((board[j][i]/BLOCKHANG)%10==1) {
						DrawImgBoardBounded(ready,  j*bsize, bsize*12-i*bsize-pixels);
					}

				}
				if ((board[j][i]/1000000)%10==1) {
					int left, right, over, under;
					int number = board[j][i];
					if (j<1) {
						left = -1;
					}
					else {
						left = board[j-1][i];
					}
					if (j>5) {
						right = -1;
					}
					else {
						right = board[j+1][i];
					}
					if (i>28) {
						over = -1;
					}
					else {
						over = board[j][i+1];
					}
					if (i<1) {
						under = -1;
					}
					else {
						under = board[j][i-1];
					}
					if ((left == number)&&(right == number)&&(over == number)&&(under == number)) {
						DrawImgBoardBounded(garbageFill,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over == number)&&(under == number)) {
						DrawImgBoardBounded(garbageL,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over == number)&&(under == number)) {
						DrawImgBoardBounded(garbageR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right == number)&&(over != number)&&(under == number)) {
						DrawImgBoardBounded(garbageT,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right == number)&&(over == number)&&(under != number)) {
						DrawImgBoardBounded(garbageB,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over != number)&&(under == number)) {
						DrawImgBoardBounded(garbageTL,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over == number)&&(under != number)) {
						DrawImgBoardBounded(garbageBL,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over != number)&&(under == number)) {
						DrawImgBoardBounded(garbageTR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over == number)&&(under != number)) {
						DrawImgBoardBounded(garbageBR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over != number)&&(under != number)) {
						DrawImgBoardBounded(garbageMR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right == number)&&(over != number)&&(under != number)) {
						DrawImgBoardBounded(garbageM,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over != number)&&(under != number)) {
						DrawImgBoardBounded(garbageML,  j*bsize, bsize*12-i*bsize-pixels);
					}
				}
				if ((board[j][i]/1000000)%10==2) {
					if (j==0) {
						DrawImgBoardBounded(garbageGML,  j*bsize, bsize*12-i*bsize-pixels);
					}
					else if (j==5) {
						DrawImgBoardBounded(garbageGMR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					else {
						DrawImgBoardBounded(garbageGM,  j*bsize, bsize*12-i*bsize-pixels);
					}
				}
			}
		const int j = 0;

		int garbageSize=0;
		for (int i=0; i<20; i++) {
			if ((board[j][i]/1000000)%10==1) {
				int left, right, over, under;
				int number = board[j][i];
				if (j<1) {
					left = -1;
				}
				else {
					left = board[j-1][i];
				}
				if (j>5) {
					right = -1;
				}
				else {
					right = board[j+1][i];
				}
				if (i>28) {
					over = -1;
				}
				else {
					over = board[j][i+1];
				}
				if (i<1) {
					under = -1;
				}
				else {
					under = board[j][i-1];
				}
				if (((left != number)&&(right == number)&&(over != number)&&(under == number))&&(garbageSize>0)) {
					DrawImgBoardBounded(smiley[board[j][i]%4],  2*bsize, 12*bsize-i*bsize-pixels+(bsize/2)*garbageSize);
				}
				if (!((left != number)&&(right == number)&&(over == number)&&(under == number))) { //not in garbage
					garbageSize=0;
				}
				else {
					garbageSize++;
				}

			}
		}
		for (int i=0; i<6; i++) {
			if (board[i][0]!=-1) {
				DrawImgBoardBounded(transCover,  i*bsize, 12*bsize-pixels);    //Make the appering blocks transperant
			}
		}

	}
public:
	//Draws everything
	void DoPaintJob() {
		DrawIMG(boardBackBack,screen,this->GetTopX()-60,this->GetTopY()-68);

		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+148,_("Score:") );
		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+197,_("Time:") );
		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+246,_("Chain:") );
		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+295,_("Speed:") );
		DrawImgBoard(backBoard,  0, 0);

		PaintBricks();
		if (stageClear) {
			DrawImgBoard(blackLine,  0, bsize*(12+2)+bsize*(stageClearLimit-linesCleared)-pixels-1);
		}
		if (puzzleMode&&(!bGameOver)) {
			//We need to write nr. of moves left!
			strHolder = _("Moves left: ") + itoa(MovesLeft);
			nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());

		}
		if (puzzleMode && stageButtonStatus == SBpuzzleMode) {
			DrawImgBoard(bRetry, cordRetryButton.x, cordRetryButton.y);
			PrintTextCenteredBoard(cordRetryButton.x, cordRetryButton.y, _("Retry"));
			if (Level<PuzzleGetNumberOfPuzzles()-1) {
				if (hasWonTheGame) {
					DrawImgBoard(bNext,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Next"));
				}
				else {
					DrawImgBoard(bSkip,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Skip"));
				}
			}
			else {
				strHolder = "Last puzzle";
				nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());
			}
		}
		if (stageClear && stageButtonStatus == SBstageClear) {
			DrawImgBoard(bRetry, cordRetryButton.x, cordRetryButton.y);
			PrintTextCenteredBoard(cordRetryButton.x, cordRetryButton.y, _("Retry"));
			if (Level<50-1) {
				if (hasWonTheGame) {
					DrawImgBoard(bNext,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Next"));
				}
				else {
					DrawImgBoard(bSkip,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Skip"));
				}
			}
			else {
				strHolder = "Last stage";
				nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());
			}
		}

#if DEBUG
		if (AI_Enabled&&(!bGameOver)) {
			strHolder = "AI_status: " + itoa(AIstatus)+ ", "+ itoa(AIlineToClear);
			//NFont_Write(   5, 5, strHolder.c_str());
			nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());
		}
#endif
		if (!bGameOver) {
			DrawImgBoard(cursor,cursorx*bsize-4,11*bsize-cursory*bsize-pixels-4);
		}
		if (ticks<gameStartedAt) {
			int currentCounter = abs((int)ticks-(int)gameStartedAt)/1000;
			if ( (currentCounter!=lastCounter) && (SoundEnabled)&&(!NoSound)) {
				Mix_PlayChannel(1,counterChunk,0);
			}
			lastCounter = currentCounter;
			switch (currentCounter) {
			case 2:
				DrawImgBoard(counter[2],  2*bsize, 5*bsize);
				break;
			case 1:
				DrawImgBoard(counter[1],  2*bsize, 5*bsize);
				break;
			case 0:
				DrawImgBoard(counter[0],  2*bsize, 5*bsize);
				break;
			default:
				break;
			}
		}
		else {
			if (SoundEnabled&&(!NoSound)&&(timetrial)&&(ticks>gameStartedAt+10000)&&(!bGameOver)) {
				int currentCounter = (ticks-(int)gameStartedAt)/1000;
				if (currentCounter!=lastCounter) {
					if (currentCounter>115 && currentCounter<120) {
						Mix_PlayChannel(1,counterChunk,0);
					}
				}
				lastCounter = currentCounter;
			}
			else {
				if ( (0==lastCounter) && (SoundEnabled)&&(!NoSound)) {
					Mix_PlayChannel(1,counterFinalChunk,0);
				}
				lastCounter = -1;
			}
		}

		if ((bGameOver)&&(!editorMode)) {
			if (hasWonTheGame) {
				DrawImgBoard(iWinner,  0, 5*bsize);
			}
			else {
				if (bDraw) {
					DrawImgBoard(iDraw,  0, 5*bsize);
				}
				else {
					DrawImgBoard(iGameOver,  0, 5*bsize);
				}
			}
		}
	}


	void Update(int newtick) {
		BlockGame::Update(newtick);
	}

private:
	int topx, topy;
};




//writeScreenShot saves the screen as a bmp file, it uses the time to get a unique filename
void writeScreenShot() {
	if (verboseLevel) {
		cout << "Saving screenshot" << endl;
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


//Dialogbox
bool OpenDialogbox(int x, int y, std::string& name) {
	bool done = false;     //We are done!
	bool accept = false;   //New name is accepted! (not Cancelled)
	SDL_TextInput textInputScope;
	ReadKeyboard rk = ReadKeyboard(name.c_str());
	string strHolder;
	DrawIMG(backgroundImage,screen,0,0);
	while (!done && !Config::getInstance()->isShuttingDown()) {
		DrawIMG(dialogBox,screen,x,y);
		NFont_Write(screen, x+40,y+76,rk.GetString());
		strHolder = rk.GetString();
		strHolder.erase((int)rk.CharsBeforeCursor());

		if (((SDL_GetTicks()/600)%2)==1) {
			NFont_Write(screen, x+40+nf_standard_blue_font.getWidth( "%s", strHolder.c_str()),y+76,"|");
		}

		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				Config::getInstance()->setShuttingDown(5);
				done = true;
				accept = false;
			}

			if (event.type == SDL_TEXTINPUT) {
				if ((rk.ReadKey(event))&&(SoundEnabled)&&(!NoSound)) {
					Mix_PlayChannel(1,typingChunk,0);
				}
			}

			if ( event.type == SDL_KEYDOWN ) {
				if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) ) {
					done = true;
					accept = true;
				}
				else if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
					done = true;
					accept = false;
				}
				else {
					if ((rk.ReadKey(event))&&(SoundEnabled)&&(!NoSound)) {
						Mix_PlayChannel(1,typingChunk,0);
					}
				}
			}

		}   //while(event)

		SDL_RenderPresent(screen); //Update screen
	}   //while(!done)
	name = rk.GetString();
	bScreenLocked = false;
	showDialog = false;
	return accept;
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


//Open a puzzle file
bool OpenFileDialogbox(int x, int y, char* name) {
	bool done = false;  //We are done!
	int mousex, mousey;
	ListFiles lf = ListFiles();
	string folder = (string)SHAREDIR+(string)"/puzzles";
	if (verboseLevel) {
		cout << "Looking in " << folder << endl;
	}
	lf.setDirectory(folder.c_str());
#ifdef __unix__
	string homeFolder = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/puzzles";
	lf.setDirectory2(homeFolder.c_str());
#endif
	while (!done && !Config::getInstance()->isShuttingDown()) {
		DrawIMG(backgroundImage,screen,0,0);
		DrawIMG(bForward,screen,x+460,y+420);
		nf_button_font.draw(screen, x+20+60, y+420+10, NFont::CENTER, _("Forward"));
		DrawIMG(bBack,screen,x+20,y+420);
		nf_button_font.draw(screen, x+20+60, y+420+10, NFont::CENTER, _("Back"));
		const int nrOfFiles = 10;
		for (int i=0; i<nrOfFiles; i++) {
			NFont_Write(screen, x+10,y+10+36*i,lf.getFileName(i).c_str());
		}

		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				Config::getInstance()->setShuttingDown(5);
				done = true;
			}

			if ( event.type == SDL_KEYDOWN ) {
				if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
					done = true;
				}

				if ( (event.key.keysym.sym == SDLK_RIGHT) ) {
					lf.forward();
				}

				if ( (event.key.keysym.sym == SDLK_LEFT) ) {
					lf.back();
				}
			}

		} //while(event)

		SDL_GetMouseState(&mousex,&mousey);

		// If the mouse button is released, make bMouseUp equal true
		if (!SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) {
			bMouseUp=true;
		}

		if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
			bMouseUp = false;

			//The Forward Button:
			if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) ) {
				lf.forward();
			}

			//The back button:
			if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) ) {
				lf.back();
			}

			for (int i=0; i<10; i++) {
				if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) ) {
					if (lf.fileExists(i)) {
						strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
						done=true; //The user have, clicked the purpose of this function is now complete
					}
				}
			}
		}

		mouse.Draw(screen, SDL_GetTicks(), mousex, mousey);
		SDL_RenderPresent(screen); //Update screen
	}
	return true;
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
	else if (editorMode) {
		NFont_Write(screen, theGame->GetTopX()+10,theGame->GetTopY()-34,_("Playing field") );
	}
	else if (!singlePuzzle) {
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
	if (!editorMode) {
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

	//Keeps track of background;
	SDL_GetTicks();

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
		SDL_GetTicks();


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
			if ( event.type == SDL_KEYDOWN ) {
				if ( event.key.keysym.sym == SDLK_ESCAPE ) {
					levelNr = -1;
					levelSelected = true;
				}
				if ( event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER ) {
					levelNr = selected;
					levelSelected = true;
				}
				if ( event.key.keysym.sym == SDLK_RIGHT ) {
					++selected;
					if (selected >= nrOfLevels) {
						selected = 0;
					}
				}
				if ( event.key.keysym.sym == SDLK_LEFT ) {
					--selected;
					if (selected < 0) {
						selected = nrOfLevels-1;
					}
				}
				if ( event.key.keysym.sym == SDLK_DOWN ) {
					selected+=10;
					if (selected >= nrOfLevels) {
						selected-=10;
					}
				}
				if ( event.key.keysym.sym == SDLK_UP ) {
					selected-=10;
					if (selected < 0) {
						selected+=10;
					}
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
				timeString = SPrintStringF(_("Time used: %s"), string(itoa(GetStageTime(selected)/1000/60)+" : "+itoa2((GetStageTime(selected)/1000)%60)).c_str() );
			}

			NFont_Write(screen, 200,200,scoreString.c_str());
			NFont_Write(screen, 200,250,timeString.c_str());
			string totalString = (boost::format(_("Total score: %1% in %2%:%3%"))%totalScore%(totalTime/1000/60)%((totalTime/1000)%60)).str(); //"Total score: " +itoa(totalScore) + " in " + itoa(totalTime/1000/60) + " : " + itoa2((totalTime/1000)%60);
			NFont_Write(screen, 200,600,totalString.c_str());
		}

		mouse.Draw(screen, SDL_GetTicks(), mousex, mousey);
		SDL_RenderPresent(screen); //draws it all to the screen

	}
	DrawIMG(backgroundImage, screen, 0, 0);
	return levelNr;
}

//This function will promt for the user to select another file for puzzle mode
void changePuzzleLevels() {
	char theFileName[30];
	snprintf(theFileName, sizeof(theFileName), "%s", PuzzleGetName().c_str());
	for (int i=PuzzleGetName().length(); i<30; i++) {
		theFileName[i]=' ';
	}
	theFileName[29]=0;
	if (OpenFileDialogbox(200,100,theFileName)) {
		for (int i=28; ((theFileName[i]==' ')&&(i>0)); i--) {
			theFileName[i]=0;
		}
		PuzzleSetName(theFileName);
	}

}

static BlockGameSdl* player1;
static BlockGameSdl* player2;

static bool registerEndlessHighscore = false;
static bool registerTTHighscorePlayer1 = false;
static bool registerTTHighscorePlayer2 = false;

static void StartSinglePlayerEndless() {
	//1 player - endless
	player1->NewGame(SDL_GetTicks());
	player1->putStartBlocks(time(0));
	twoPlayers =false;
	player2->SetGameOver();
	player1->name = player1name;
	player2->name = player2name;
	registerEndlessHighscore = true;
}

static void StartSinglePlayerTimeTrial() {
	player1->NewTimeTrialGame(SDL_GetTicks());
	twoPlayers =false;
	player2->SetGameOver();
	//vsMode = false;
	player1->name = player1name;
	player2->name = player2name;
	registerTTHighscorePlayer1 = true;
}

static int StartSinglePlayerPuzzle(int level) {
	int myLevel = PuzzleLevelSelect(0);
	if (myLevel == -1) {
		return 1;
	}
	player1->NewPuzzleGame(myLevel,SDL_GetTicks());
	DrawIMG(backgroundImage, screen, 0, 0);
	twoPlayers = false;
	player2->SetGameOver();
	//vsMode = true;
	player1->name = player1name;
	player2->name = player2name;
	return 0;
}


static void StarTwoPlayerTimeTrial() {
	player1->NewTimeTrialGame(SDL_GetTicks());
	player2->NewTimeTrialGame(SDL_GetTicks());
	int theTime = time(0);
	player1->putStartBlocks(theTime);
	player2->putStartBlocks(theTime);
	twoPlayers = true;
	player1->setGameSpeed(player1Speed);
	player2->setGameSpeed(player2Speed);
	player1->setHandicap(player1handicap);
	player2->setHandicap(player2handicap);
	registerTTHighscorePlayer1 = true;
	registerTTHighscorePlayer2 = true;
	if (player1AI) {
		player1->setAIlevel(player1AIlevel);
		registerTTHighscorePlayer1 = false;
	}
	if (player2AI) {
		player2->setAIlevel(player2AIlevel);
		registerTTHighscorePlayer2 = false;
	}
	player1->name = player1name;
	player2->name = player2name;
}

static void StartTwoPlayerVs() {
	//2 player - VsMode
	player1->NewVsGame(player2,SDL_GetTicks());
	player2->NewVsGame(player1,SDL_GetTicks());
	//vsMode = true;
	twoPlayers = true;
	player1->setGameSpeed(player1Speed);
	player2->setGameSpeed(player2Speed);
	player1->setHandicap(player1handicap);
	player2->setHandicap(player2handicap);
	if (player1AI) {
		player1->setAIlevel(player1AIlevel);
	}
	if (player2AI) {
		player2->setAIlevel(player2AIlevel);
	}
	int theTime = time(0);
	player1->putStartBlocks(theTime);
	player2->putStartBlocks(theTime);
	player1->name = player1name;
	player2->name = player2name;
}

//The main function, quite big... too big
//Warning: the arguments to main must be "int argc, char* argv[]" NO CONST! or SDL_main will fail to find it
int main(int argc, char* argv[]) {
	try {
		//Init the file system abstraction layer
		PHYSFS_init(argv[0]);
		vector<string> search_paths;
		FsSearchParthMainAppend(search_paths);
		string savepath = getPathToSaveFiles();
		highPriority = false;   //if true the game will take most resources, but increase framerate.
		bFullscreen = false;
		//Set default Config variables:
		setlocale (LC_ALL, "");
		bindtextdomain (PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(PACKAGE, "utf-8");
		textdomain (PACKAGE);
		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
		("help,h", "Displays this message")
		("nosound", "Disables the sound. Can be used if sound errors prevents you from starting")
		("priority", "Causes the game to not sleep between frames.")
		("verbose-basic", "Enables basic verbose messages")
		("print-search-path", "Prints the search path and quits")
		("bind-text-domain", boost::program_options::value<string>(), SPrintStringF("Overwrites the bind text domain used for finding translations. "
		        "Default: \"%s\"", LOCALEDIR).c_str())
		("homepath", boost::program_options::value<string>(), SPrintStringF("Set the home folder where settings are saved. The directory must exsist."
		        " Default: \"%s\"", getPathToSaveFiles().c_str()).c_str())

		;
		boost::program_options::variables_map vm;
		try {
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
			boost::program_options::notify(vm);
		}
		catch (exception& e) {
			cerr << e.what() << endl;
			cerr << desc << endl;
			throw;
		}
		if (vm.count("bind-text-domain")) {
			string s = vm["bind-text-domain"].as<string>();
			bindtextdomain (PACKAGE, s.c_str());
		}
		if (vm.count("homepath")) {
			string s = vm["homepath"].as<string>();
			setPathToSaveFiles(s);
			savepath = getPathToSaveFiles();
		}
		if (vm.count("help")) {
			cout << SPrintStringF("Block Attack - Rise of the blocks %s\n"
			                      "%s\n", VERSION_NUMBER, "www.blockattack.net");
			cout << desc << endl;
			cout << "The game is available under the GPL, see COPYING for details." << endl;
			return 1;
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
		if (vm.count("print-search-path")) {
			for (const string& s : search_paths) {
				cout << s << endl;
			}
			cout << savepath << endl;
			return 0;
		}
		//Os create folders must be after the paramters because they can change the home folder
		PhysFsCreateFolders();

		SoundEnabled = true;
		MusicEnabled = true;
		bScreenLocked = false;
		twoPlayers = false; //true if two players splitscreen
		theTopScoresEndless = Highscore(1);
		theTopScoresTimeTrial = Highscore(2);
		drawBalls = true;
		puzzleLoaded = false;

		theBallManager = BallManager();
		theExplosionManager = ExplosionManager();

		PuzzleSetName("puzzle.levels");

		//Init SDL
		if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
			sago::SagoFatalErrorF("Unable to init SDL: %s", SDL_GetError());
		}
		TTF_Init();
		atexit(SDL_Quit);       //quits SDL when the game stops for some reason (like you hit exit or Esc)

		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

		Joypad_init();    //Prepare the joysticks
		Joypad joypad1 = Joypad();    //Creates a joypad
		Joypad joypad2 = Joypad();    //Creates a joypad

		theTextManager = TextManager();

		//Open Audio
		if (!NoSound) {
			//If sound has not been disabled, then load the sound system
			if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
				cerr << "Warning: Couldn't set 44100 Hz 16-bit audio - Reason: " << SDL_GetError() << endl
				     << "Sound will be disabled!" << endl;
				NoSound = true; //Tries to stop all sound from playing/loading
			}
		}


		if (verboseLevel) {
			//Copyright notice:
			cout << "Block Attack - Rise of the Blocks (" << VERSION_NUMBER << ")" << endl << "http://www.blockattack.net" << endl << "Copyright 2004-2016 Poul Sander" << endl <<
			     "A SDL2 based game (see www.libsdl.org)" << endl <<
			     "The game is available under the GPL, see COPYING for details." << endl;
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

		player1name = _("Player 1");
		player2name = _("Player 2");

		Config* configSettings = Config::getInstance();
		//configSettings->setString("aNumber"," A string");
		//configSettings->save();
		if (configSettings->exists("fullscreen")) { //Test if an configFile exists
			bFullscreen = (bool)configSettings->getInt("fullscreen");
			MusicEnabled = (bool)configSettings->getInt("musicenabled");
			SoundEnabled = (bool)configSettings->getInt("soundenabled");
			mouseplay1 = (bool)configSettings->getInt("mouseplay1");
			mouseplay2 = (bool)configSettings->getInt("mouseplay2");
			joyplay1 = (bool)configSettings->getInt("joypad1");
			joyplay2 = (bool)configSettings->getInt("joypad2");

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
				cout << "Data loaded from config file" << endl;
			}
		}
		else {
			if (verboseLevel) {
				cout << "Unable to load options file, using default values" << endl;
			}
		}

		if (singlePuzzle) {
			xsize=300;
			ysize=600;
		}


		// "Block Attack - Rise of the Blocks"
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		//Open video
		int createWindowParams = 0; //SDL_WINDOW_RESIZABLE;
		if ((bFullscreen)&&(!singlePuzzle)) {
			createWindowParams |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		sdlWindow = SDL_CreateWindow("Block Attack - Rise of the Blocks",
		                             SDL_WINDOWPOS_UNDEFINED,
		                             SDL_WINDOWPOS_UNDEFINED,
		                             xsize, ysize,
		                             createWindowParams );
		dieOnNullptr(sdlWindow, "Unable to create window");
		SDL_Renderer* renderer = SDL_CreateRenderer(sdlWindow, -1, 0);
		dieOnNullptr(renderer, "Unable to create render");
		//SDL_RenderSetLogicalSize(renderer, xsize, ysize);
		screen = renderer;

		PhysFsSetSearchPath(search_paths, savepath);
		sago::SagoDataHolder d(renderer);
		d.setVerbose(false);
		sago::SagoSpriteHolder spriteholder(d);
		InitImages(spriteholder);
		SetSDLIcon(sdlWindow);

		if (verboseLevel) {
			cout << "Images loaded" << endl;
		}

		BlockGameSdl theGame = BlockGameSdl(50,100);            //creates game objects
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
		theGame.SetGameOver();      //sets the game over in the beginning
		theGame2.SetGameOver();


		//Takes names from file instead
		theGame.name = player1name;
		theGame2.name = player2name;

		if (singlePuzzle) {
			LoadPuzzleStages();
			theGame.NewPuzzleGame(singlePuzzleNr, SDL_GetTicks());
		}
		SDL_RenderClear(screen);
		DrawIMG(backgroundImage, screen, 0, 0);
		DrawEverything(xsize,ysize,&theGame,&theGame2);
		SDL_RenderPresent(screen);
		//game loop
		MainMenu();



		//Saves options
		if (!editorMode) {
			configSettings->setInt("fullscreen",(int)bFullscreen);
			configSettings->setInt("musicenabled",(int)MusicEnabled);
			configSettings->setInt("soundenabled",(int)SoundEnabled);
			configSettings->setInt("mouseplay1",(int)mouseplay1);
			configSettings->setInt("mouseplay2",(int)mouseplay2);
			configSettings->setInt("joypad1",(int)joyplay1);
			configSettings->setInt("joypad2",(int)joyplay2);

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
			cout << boost::format("Block Attack - Rise of the Blocks ran for: %1% hours %2% mins and %3% secs") % ct.hours % ct.minutes % ct.seconds << endl;
		}

		ct = TimeHandler::addTime("totalTime",ct);
		if (verboseLevel) {
			cout << "Total run time is now: " << ct.days << " days " << ct.hours << " hours " << ct.minutes << " mins and " << ct.seconds << " secs" << endl;
		}

		Stats::getInstance()->save();

		Config::getInstance()->save();
		
		//Close file system Apstraction layer!
		PHYSFS_deinit();

	}
	catch (exception& e) {
		sago::SagoFatalError(e.what());
	}
	return 0;
}


int runGame(int gametype, int level) {
	int mousex, mousey;   //Mouse coordinates
	bScreenLocked = false;
	theTopScoresEndless = Highscore(1);
	theTopScoresTimeTrial = Highscore(2);
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
	theGame.SetGameOver();      //sets the game over in the beginning
	theGame2.SetGameOver();

	//Takes names from file instead
	theGame.name = player1name;
	theGame2.name = player2name;

	Joypad joypad1 = Joypad();    //Creates a joypad
	Joypad joypad2 = Joypad();    //Creates a joypad

	if (singlePuzzle) {
		LoadPuzzleStages();
		theGame.NewPuzzleGame(singlePuzzleNr, SDL_GetTicks());
	}
	//game loop
	int done = 0;
	if (verboseLevel) {
		cout << "Starting game loop" << endl;
	}


	bool mustsetupgame = true;


	while (done == 0) {
		if (mustsetupgame) {
			registerEndlessHighscore = false;
			registerTTHighscorePlayer1 = false;
			registerTTHighscorePlayer2 = false;
			switch (gametype) {
			case 1:
				StartSinglePlayerTimeTrial();
				break;
			case 2: {
				int myLevel = PuzzleLevelSelect(1);
				if (myLevel == -1) {
					return 1;
				}
				theGame.NewStageGame(myLevel,SDL_GetTicks());
				DrawIMG(backgroundImage, screen, 0, 0);
				twoPlayers =false;
				theGame2.SetGameOver();
				theGame.name = player1name;
				theGame2.name = player2name;
			}
			break;
			case 3:
				if (StartSinglePlayerPuzzle(level)) {
					return 1;
				}
				break;
			case 4: {
				//1 player - Vs mode
				int theAIlevel = level; //startSingleVs();
				theGame.NewVsGame(&theGame2, SDL_GetTicks());
				theGame2.NewVsGame(&theGame, SDL_GetTicks());
				DrawIMG(backgroundImage, screen, 0, 0);
				twoPlayers = true; //Single player, but AI plays
				theGame2.setAIlevel((Uint8)theAIlevel);
				int theTime = time(0);
				theGame.putStartBlocks(theTime);
				theGame2.putStartBlocks(theTime);
				theGame.name = player1name;
				theGame2.name = player2name;
			}
			break;
			case 10:
				StarTwoPlayerTimeTrial();
				break;
			case 11:
				StartTwoPlayerVs();
				break;
			case 0:
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

		//updates the balls and explosions:
		theBallManager.update();
		theExplosionManager.update();
		theTextManager.update();

		bool mustWriteScreenshot = false;

		if (!bScreenLocked) {
			SDL_Event event;

			while ( SDL_PollEvent(&event) ) {
				if ( event.type == SDL_QUIT ) {
					Config::getInstance()->setShuttingDown(5);
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
							theGame.MoveCursor('N');
						}
						if ( event.key.keysym.sym == keySettings[player1keys].down ) {
							theGame.MoveCursor('S');
						}
						if ( (event.key.keysym.sym == keySettings[player1keys].left) ) {
							theGame.MoveCursor('W');
						}
						if ( (event.key.keysym.sym == keySettings[player1keys].right) ) {
							theGame.MoveCursor('E');
						}
						if ( event.key.keysym.sym == keySettings[player1keys].push ) {
							theGame.PushLine();
						}
						if ( event.key.keysym.sym == keySettings[player1keys].change ) {
							theGame.SwitchAtCursor();
						}
					}
					if (!editorMode && !theGame2.GetAIenabled()) {
						//player2:
						if ( event.key.keysym.sym == keySettings[player2keys].up ) {
							theGame2.MoveCursor('N');
						}
						if ( event.key.keysym.sym == keySettings[player2keys].down ) {
							theGame2.MoveCursor('S');
						}
						if ( (event.key.keysym.sym == keySettings[player2keys].left) ) {
							theGame2.MoveCursor('W');
						}
						if ( (event.key.keysym.sym == keySettings[player2keys].right) ) {
							theGame2.MoveCursor('E');
						}
						if ( event.key.keysym.sym == keySettings[player2keys].push ) {
							theGame2.PushLine();
						}
						if ( event.key.keysym.sym == keySettings[player2keys].change ) {
							theGame2.SwitchAtCursor();
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
			} //while event PollEvent - read keys

			/**********************************************************************
			***************************** Joypad start ****************************
			**********************************************************************/

			//Gameplay
			if (joyplay1||joyplay2) {
				if (joypad1.working && !theGame.GetAIenabled()) {
					if (joyplay1) {
						joypad1.update();
						if (joypad1.up) {
							theGame.MoveCursor('N');
						}
						if (joypad1.down) {
							theGame.MoveCursor('S');
						}
						if (joypad1.left) {
							theGame.MoveCursor('W');
						}
						if (joypad1.right) {
							theGame.MoveCursor('E');
						}
						if (joypad1.but1) {
							theGame.SwitchAtCursor();
						}
						if (joypad1.but2) {
							theGame.PushLine();
						}
					}
					else {
						joypad1.update();
						if (joypad1.up) {
							theGame2.MoveCursor('N');
						}
						if (joypad1.down) {
							theGame2.MoveCursor('S');
						}
						if (joypad1.left) {
							theGame2.MoveCursor('W');
						}
						if (joypad1.right) {
							theGame2.MoveCursor('E');
						}
						if (joypad1.but1) {
							theGame2.SwitchAtCursor();
						}
						if (joypad1.but2) {
							theGame2.PushLine();
						}
					}
				}
				if (joypad2.working && !theGame2.GetAIenabled()) {
					if (!joyplay2) {
						joypad2.update();
						if (joypad2.up) {
							theGame.MoveCursor('N');
						}
						if (joypad2.down) {
							theGame.MoveCursor('S');
						}
						if (joypad2.left) {
							theGame.MoveCursor('W');
						}
						if (joypad2.right) {
							theGame.MoveCursor('E');
						}
						if (joypad2.but1) {
							theGame.SwitchAtCursor();
						}
						if (joypad2.but2) {
							theGame.PushLine();
						}
					}
					else {
						joypad2.update();
						if (joypad2.up) {
							theGame2.MoveCursor('N');
						}
						if (joypad2.down) {
							theGame2.MoveCursor('S');
						}
						if (joypad2.left) {
							theGame2.MoveCursor('W');
						}
						if (joypad2.right) {
							theGame2.MoveCursor('E');
						}
						if (joypad2.but1) {
							theGame2.SwitchAtCursor();
						}
						if (joypad2.but2) {
							theGame2.PushLine();
						}
					}
				}
			}

			/**********************************************************************
			***************************** Joypad end ******************************
			**********************************************************************/

			SDL_GetMouseState(&mousex,&mousey);

			/********************************************************************
			**************** Here comes mouse play ******************************
			********************************************************************/

			if ((mouseplay1)&&( ( (!editorMode)&&(!theGame.GetAIenabled()) ) ||(editorModeTest))) //player 1
				if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600)) {
					int yLine, xLine;
					yLine = ((100+600)-(mousey-100+theGame.GetPixels()))/50;
					xLine = (mousex-50+25)/50;
					yLine-=2;
					xLine-=1;
					if ((yLine>10)&&(theGame.GetTowerHeight()<12)) {
						yLine=10;
					}
					if (((theGame.GetPixels()==50)||(theGame.GetPixels()==0)) && (yLine>11)) {
						yLine=11;
					}
					if (yLine<0) {
						yLine=0;
					}
					if (xLine<0) {
						xLine=0;
					}
					if (xLine>4) {
						xLine=4;
					}
					theGame.MoveCursorTo(xLine,yLine);
				}

			if ((mouseplay2)&&(!editorMode)&&(!theGame2.GetAIenabled())) //player 2
				if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600)) {
					int yLine, xLine;
					yLine = ((100+600)-(mousey-100+theGame2.GetPixels()))/50;
					xLine = (mousex-(xsize-500)+25)/50;
					yLine-=2;
					xLine-=1;
					if ((yLine>10)&&(theGame2.GetTowerHeight()<12)) {
						yLine=10;
					}
					if (((theGame2.GetPixels()==50)||(theGame2.GetPixels()==0)) && (yLine>11)) {
						yLine=11;
					}
					if (yLine<0) {
						yLine=0;
					}
					if (xLine<0) {
						xLine=0;
					}
					if (xLine>4) {
						xLine=4;
					}
					theGame2.MoveCursorTo(xLine,yLine);
				}

			/********************************************************************
			**************** Here ends mouse play *******************************
			********************************************************************/

			// If the mouse button is released, make bMouseUp equal true
			if (!SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) {
				bMouseUp=true;
			}

			// If the mouse button 2 is released, make bMouseUp2 equal true
			if ((SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(3))!=SDL_BUTTON(3)) {
				bMouseUp2=true;
			}

			if ((!singlePuzzle)&&(!editorMode)) {
				//read mouse events
				if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
					bMouseUp = false;
					DrawIMG(backgroundImage, screen, 0, 0);


					/********************************************************************
					**************** Here comes mouse play ******************************
					********************************************************************/
					{
						if (mouseplay1 && !theGame.GetAIenabled()) //player 1
							if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600)) {
								theGame.SwitchAtCursor();
							}
						if (mouseplay2 && !theGame2.GetAIenabled()) //player 2
							if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600)) {
								theGame2.SwitchAtCursor();
							}
					}
					/********************************************************************
					**************** Here ends mouse play *******************************
					********************************************************************/

					if (stageButtonStatus != SBdontShow && (mousex > theGame.GetTopX()+cordNextButton.x)
					        &&(mousex < theGame.GetTopX()+cordNextButton.x+cordNextButton.xsize)
					        &&(mousey > theGame.GetTopY()+cordNextButton.y)&&(mousey < theGame.GetTopY()+cordNextButton.y+cordNextButton.ysize)) {
						//Clicked the next button after a stage clear or puzzle
						theGame.nextLevel(SDL_GetTicks());
					}
					if (stageButtonStatus != SBdontShow && (mousex > theGame.GetTopX()+cordRetryButton .x)
					        &&(mousex < theGame.GetTopX()+cordRetryButton.x+cordRetryButton.xsize)
					        &&(mousey > theGame.GetTopY()+cordRetryButton.y)&&(mousey < theGame.GetTopY()+cordRetryButton.y+cordRetryButton.ysize)) {
						//Clicked the retry button
						theGame.retryLevel(SDL_GetTicks());
					}


					//cout << "Mouse x: " << mousex << ", mouse y: " << mousey << endl;
				}

				//Mouse button 2:
				if ((SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(3))==SDL_BUTTON(3) && bMouseUp2) {
					bMouseUp2=false; //The button is pressed
					/********************************************************************
					**************** Here comes mouse play ******************************
					********************************************************************/

					if (mouseplay1 && !theGame.GetAIenabled()) {
						//player 1
						if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600)) {
							theGame.PushLine();
						}
					}
					if (mouseplay2 && !theGame2.GetAIenabled()) {
						//player 2
						if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600)) {
							theGame2.PushLine();
						}
					}
					/********************************************************************
					**************** Here ends mouse play *******************************
					********************************************************************/
				}
			} //if !singlePuzzle
			else {

			}
		} //if !bScreenBocked;


		//Sees if music is stopped and if music is enabled
		if ((!NoSound)&&(!Mix_PlayingMusic())&&(MusicEnabled)&&(!bNearDeath)) {
			// then starts playing it.
			Mix_VolumeMusic(MIX_MAX_VOLUME);
			Mix_PlayMusic(bgMusic, -1); //music loop
		}

		if (bNearDeath!=bNearDeathPrev) {
			if (bNearDeath) {
				if (!NoSound &&(MusicEnabled)) {
					Mix_VolumeMusic(MIX_MAX_VOLUME);
					Mix_PlayMusic(highbeatMusic, 1);
				}
			}
			else {
				if (!NoSound &&(MusicEnabled)) {
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
			if (twoPlayers) {
				if ((theGame.isGameOver()) && (theGame2.isGameOver())) {
					if (theGame.GetScore()+theGame.GetHandicap()>theGame2.GetScore()+theGame2.GetHandicap()) {
						theGame.setPlayerWon();
					}
					else if (theGame.GetScore()+theGame.GetHandicap()<theGame2.GetScore()+theGame2.GetHandicap()) {
						theGame2.setPlayerWon();
					}
					else {
						theGame.setDraw();
						theGame2.setDraw();
					}
					//twoPlayers = false;
				}
				if ((theGame.isGameOver()) && (!theGame2.isGameOver())) {
					theGame2.setPlayerWon();
					//twoPlayers = false;
				}
				if ((!theGame.isGameOver()) && (theGame2.isGameOver())) {
					theGame.setPlayerWon();
					//twoPlayers = false;
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
