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

//Make sure it is only included once
#ifndef _MAINVARS_HPP
#define	_MAINVARS_HPP

#include "sago/SagoSprite.hpp"
#include "BlockGame.hpp"


//main variables and constants


//Animation lengths:
#define READYTIME 500
#define BOMBTIME 200
#define CURSORTIME 200

#ifndef SHAREDIR
#define SHAREDIR "."
#endif

const char sharedir[] = SHAREDIR;

//All graphic in the game (as pointers):
sago::SagoSprite backgroundImage; //Stores the background image
static sago::SagoSprite backBoard;     //Stores the background to the board
sago::SagoSprite bBack;         //The "Back" button
static sago::SagoSprite bForward;      //The "forward" button
#if NETWORK
//static sago::SagoSprite bNetwork;
//static sago::SagoSprite bConnect;
//static sago::SagoSprite bHost;
#endif
sago::SagoSprite bHighScore;    //The High Score botton
static sago::SagoSprite blackLine;		//The seperator in stage clear
static sago::SagoSprite stageBobble;	//The bobble instage clear
SDL_Renderer *screen;        //The whole screen;
static sago::SagoSprite iGameOver;     //The gameOver image
static sago::SagoSprite iWinner;		//the "winner" image
static sago::SagoSprite iDraw;			//the "draw" image
static sago::SagoSprite iLoser;		//the "loser" image
static sago::SagoSprite iChainFrame;
//Animations:
static sago::SagoSprite cursor;    //The animated cursor
static sago::SagoSprite bomb;
static sago::SagoSprite ready;      //Before the blocks fall
static sago::SagoSprite explosion[4];   //Then a block explodes
//Animations end
static sago::SagoSprite counter[3];    //Counts down from 3
static sago::SagoSprite bricks[7];     //The bricks, saved in an array of pointers
static sago::SagoSprite crossover;     //Cross the bricks that will be cleared soon
static sago::SagoSprite balls[7];      //The balls (the small ones that jump around)
sago::SagoSprite dialogBox;
static sago::SagoSprite iLevelCheck;		//To the level select screen
static sago::SagoSprite iLevelCheckBox;
static sago::SagoSprite iLevelCheckBoxMarked;
static sago::SagoSprite iCheckBoxArea;
static sago::SagoSprite boardBackBack;
static sago::SagoSprite garbageTL;			//the Garbage Blocks
static sago::SagoSprite garbageT;
static sago::SagoSprite garbageTR;
static sago::SagoSprite garbageR;
static sago::SagoSprite garbageBR;
static sago::SagoSprite garbageB;
static sago::SagoSprite garbageBL;
static sago::SagoSprite garbageL;
static sago::SagoSprite garbageFill;
static sago::SagoSprite garbageM;
static sago::SagoSprite garbageML;
static sago::SagoSprite garbageMR;
static sago::SagoSprite smiley[4];
static sago::SagoSprite garbageGM;
static sago::SagoSprite garbageGML;
static sago::SagoSprite garbageGMR;
static sago::SagoSprite transCover;        //The transperant block, covers the upcomming
static sago::SagoSprite bSkip;
static sago::SagoSprite bRetry;
sago::SagoSprite bNext;

sago::SagoSprite menuMarked;
sago::SagoSprite menuUnmarked;
sago::SagoSprite mouse;

NFont nf_button_font;        //Font used for buttons!
NFont nf_scoreboard_font;
NFont nf_standard_blue_font;      //Font used instead of the old blue SFont
static NFont nf_standard_small_font;

static Mix_Music *bgMusic;         //backgroundMusic
static Mix_Music *highbeatMusic;   //Background music with higher beat
static Mix_Chunk *boing;           //boing sound when clearing
static Mix_Chunk *applause;        //Applause, then the player is good
static Mix_Chunk *photoClick;      //clickSound
Mix_Chunk *typingChunk;          //When writing
static Mix_Chunk *counterChunk;         //When counting down
static Mix_Chunk *counterFinalChunk;

Highscore theTopScoresEndless;      //Stores highscores for endless
Highscore theTopScoresTimeTrial;    //Stores highscores for timetrial

static bool bMouseUp;              //true if the mouse(1) is unpressed
static bool bMouseUp2;             //true if the mouse(2) is unpressed
#if NETWORK
static bool bNetworkOpen;			//Show the network menu
#endif
bool NoSound;				//if true, absolutely no sound will be played, can be set from the commandline
//prevents crash on systems without a soundcard
bool MusicEnabled;			//true if background music is enabled
bool SoundEnabled;			//true if sound effects is enabled
static bool bNearDeathPrev;                    //Near death status last time checked.
bool bFullscreen;			//true if game is running fullscreen
static bool puzzleLoaded;          //true if the puzzle levels have been loaded
static bool drawBalls;             //if true balls are drawed to the screen, this might lower framerate too much
bool highPriority;

static bool editorMode = false;
static bool editorModeTest = false;

//other ball constants:
const double gravity = 200.8; //acceleration
const double startVelocityY = 50.0;
const double VelocityX = 50.0;
const int ballSize = 16;
const double minVelocity = 200.0;

//global settings (reset everytime the game starts)
static Uint8 player1Speed=0;
static Uint8 player2Speed=0;
static bool player1AI=false;		//Is AI enabled?
static bool player2AI=false;		//Is AI enabled for player 2 (opponent in single player)
static Uint8 player1AIlevel=3;		//What level is AI? 0 min, 6 max
static Uint8 player2AIlevel=3;
static const Uint8 AIlevels=7;		//7 possible levels: 0..6
static Uint8 player1handicap=0;
static Uint8 player2handicap=0;

unsigned long int currentTime;      //contains the current time, so we don't call SDL_GetTickets() too often...

static int xsize = 1024;
static int ysize = 768;

//Stores the players names (way to long, but at least no buffer overflows (max length is 16 for display reasons))
std::string player1name;
std::string player2name;

bool twoPlayers;    //True if two players are playing

//Old Stage Clear Buble
static int oldBubleX, oldBubleY;

static bool singlePuzzle = false; //if true we are just in a little 300x600 window
static int singlePuzzleNr = 0;
static std::string singlePuzzleFile;

#if DEBUG
//frame counter (fps)
unsigned long int Frames, Ticks;
char FPS[10];
#endif

int verboseLevel = 0;

//keySetup
int player1keys, player2keys;
bool mouseplay1=false;  //The mouse works on the play field
bool mouseplay2=false;  //Same for player2
bool joyplay1=false;    //Player one uses the joypad
bool joyplay2=false;    //Player two uses the joypad

//Stores the controls
struct control
{
	SDL_Keycode up;
	SDL_Keycode down;
	SDL_Keycode left;
	SDL_Keycode right;
	SDL_Keycode change;
	SDL_Keycode push;
};

control keySettings[3];	//array to hold the controls (default and two custom)

#define KEYMENU_MAXWITH 4
#define KEYMENU_MAXDEPTH 7


static const int buttonXsize = 120;
static const int buttonYsize = 40;

struct ButtonCords
{
	int x;
	int y;
	int xsize;
	int ysize;
};

ButtonCords cordNextButton =
{
	cordNextButton.x = 3*bsize+(3*bsize-buttonXsize)/2,
	cordNextButton.y = 10*bsize,
	cordNextButton.xsize = buttonXsize,
	cordNextButton.ysize = buttonYsize
};

ButtonCords cordRetryButton =
{
	cordRetryButton.x = (3*bsize-buttonXsize)/2,
	cordRetryButton.y = 10*bsize,
	cordRetryButton.xsize = buttonXsize,
	cordRetryButton.ysize = buttonYsize
};

#endif
