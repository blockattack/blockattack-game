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

//Make sure it is only included once
#ifndef _MAINVARS_HPP
#define	_MAINVARS_HPP

//main variables and constants


//Animation lengths:
#define READYTIME 500
#define BOMBTIME 200
#define CURSORTIME 200


using namespace std;	//remove it if you dare...
const char sharedir[] = SHAREDIR;

//All graphic in the game (as pointers):
static SDL_Surface *background;    //Stores background
SDL_Surface *backgroundImage; //Stores the background image
static SDL_Surface *backBoard;     //Stores the background to the board
static CppSdl::CppSdlImageHolder bNewGame;      //The New Game botton
static SDL_Surface *bOptions;      //The Options botton
static SDL_Surface *bConfigure;    //The configure button
static SDL_Surface *bSelectPuzzle; //The Select Puzzle Button
static SDL_Surface *bBack;         //The "Back" button
static SDL_Surface *bForward;      //The "forward" button
static SDL_Surface *iChainBack;
#if NETWORK
//static SDL_Surface *bNetwork;
//static SDL_Surface *bConnect;
//static SDL_Surface *bHost;
#endif
static SDL_Surface *bHighScore;    //The High Score botton
static SDL_Surface *blackLine;		//The seperator in stage clear
static SDL_Surface *stageBobble;	//The bobble instage clear
SDL_Surface *screen;        //The whole screen;
static SDL_Surface *iGameOver;     //The gameOver image
static SDL_Surface *iWinner;		//the "winner" image
static SDL_Surface *iDraw;			//the "draw" image
static SDL_Surface *iLoser;		//the "loser" image
//Animations:
static SDL_Surface *cursor[2];    //The animated cursor
static SDL_Surface *bomb[2];       //Bomb then the bricks should blow
static SDL_Surface *ready[2];      //Before the blocks fall
static SDL_Surface *explosion[4];   //Then a block explodes
//Animations end
static SDL_Surface *counter[3];    //Counts down from 3
static SDL_Surface *bricks[7];     //The bricks, saved in an array of pointers
static SDL_Surface *crossover;     //Cross the bricks that will be cleared soon
static SDL_Surface *balls[7];      //The balls (the small ones that jump around)
static SDL_Surface *changeButtonsBack;
static SDL_Surface *dialogBox;
static SDL_Surface *bOn;
static SDL_Surface *bOff;
static SDL_Surface *b1024;
static SDL_Surface *iLevelCheck;		//To the level select screen
static SDL_Surface *iLevelCheckBox;
static SDL_Surface *iLevelCheckBoxMarked;
static SDL_Surface *iCheckBoxArea;
static SDL_Surface *boardBackBack;
static SDL_Surface *garbageTL;			//the Garbage Blocks
static SDL_Surface *garbageT;
static SDL_Surface *garbageTR;
static SDL_Surface *garbageR;
static SDL_Surface *garbageBR;
static SDL_Surface *garbageB;
static SDL_Surface *garbageBL;
static SDL_Surface *garbageL;
static SDL_Surface *garbageFill;
static SDL_Surface *garbageM;
static SDL_Surface *garbageML;
static SDL_Surface *garbageMR;
static SDL_Surface *smiley[4];
static SDL_Surface *garbageGM;
static SDL_Surface *garbageGML;
static SDL_Surface *garbageGMR;
static SDL_Surface *transCover;        //The transperant block, covers the upcomming
#if LEVELEDITOR
static SDL_Surface *bCreateFile;
static SDL_Surface *bDeletePuzzle;
static SDL_Surface *bLoadFile;
static SDL_Surface *bMoveBack;
static SDL_Surface *bMoveDown;
static SDL_Surface *bMoveForward;
static SDL_Surface *bMoveLeft;
static SDL_Surface *bMoveRight;
static SDL_Surface *bMoveUp;
static SDL_Surface *bNewPuzzle;
static SDL_Surface *bSaveFileAs;
static SDL_Surface *bSavePuzzle;
static SDL_Surface *bSaveToFile;
static SDL_Surface *bTestPuzzle;
#endif
static SDL_Surface *bSkip;
static SDL_Surface *bRetry;
static SDL_Surface *bNext;

CppSdl::CppSdlImageHolder menuMarked;
CppSdl::CppSdlImageHolder menuUnmarked;
CppSdl::CppSdlImageHolder mouse;

static SDL_Surface *tmp;				//a temporary surface to use DisplayFormat

static NFont nf_button_font;        //Font used for buttons!
NFont nf_scoreboard_font;
static NFont nf_standard_blue_font;      //Font used instead of the old blue SFont
static NFont nf_standard_small_font;

static Mix_Music *bgMusic;         //backgroundMusic
static Mix_Music *highbeatMusic;   //Background music with higher beat
static Mix_Chunk *boing;           //boing sound when clearing
static Mix_Chunk *applause;        //Applause, then the player is good
static Mix_Chunk *photoClick;      //clickSound
static Mix_Chunk *typingChunk;          //When writing
static Mix_Chunk *counterChunk;         //When counting down
static Mix_Chunk *counterFinalChunk;

Highscore theTopScoresEndless;      //Stores highscores for endless
Highscore theTopScoresTimeTrial;    //Stores highscores for timetrial

static bool bMouseUp;              //true if the mouse(1) is unpressed
static bool bMouseUp2;             //true if the mouse(2) is unpressed
#if NETWORK
static bool bNetworkOpen;			//Show the network menu
#endif
static bool bScreenLocked;			//Don't take input or allow any mouse interaction! Used for dialogbox and warningbox
static bool showDialog;
static bool NoSound;				//if true, absolutely no sound will be played, can be set from the commandline
//prevents crash on systems without a soundcard
bool MusicEnabled;			//true if background music is enabled
bool SoundEnabled;			//true if sound effects is enabled
static bool bNearDeathPrev;                    //Near death status last time checked.
bool bFullscreen;			//true if game is running fullscreen
static bool puzzleLoaded;          //true if the puzzle levels have been loaded
static bool drawBalls;             //if true balls are drawed to the screen, this might lower framerate too much
static bool standardBackground;
bool highPriority;

static bool editorMode = false;
static bool editorModeTest = false;

//Things for network play:
#if NETWORK
static bool networkPlay;
static bool networkActive;
//sockets here
#define SERVERPORT 41780
#define CLIENTPORT 41781


static char serverAddress[30];
#endif

//other ball constants:
static const double gravity = 200.8; //acceleration
static const double startVelocityY = 50.0;
static const double VelocityX = 50.0;
static const int ballSize = 16;
static const double minVelocity = 200.0;

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
static const int bsize = 50;

//Stores the players names (way to long, but at least no buffer overflows (max length is 16 for display reasons))
char player1name[30];
char player2name[30];

//paths
static string stageClearSavePath;
static string puzzleSavePath;
static string puzzleName;              //The filename of

static const int nrOfStageLevels = 50;		//number of stages in stage Clear
static const int maxNrOfPuzzleStages = 50; //Maximum number of puzzle stages
vector<bool> stageCleared(nrOfStageLevels);		//vector that tells if a stage is cleared
vector<Uint32> stageTimes(nrOfStageLevels);             //For statistical puposes
vector<Uint32> stageScores(nrOfStageLevels);            //--||--
vector<bool> puzzleCleared(maxNrOfPuzzleStages); //vector that tells if puzzle cleared
vector<int> nrOfMovesAllowed(maxNrOfPuzzleStages);  //Moves to clear
int puzzleLevels[maxNrOfPuzzleStages][6][12]; //Contains board layout;
int nrOfPuzzles;    //How many are there actually?
bool twoPlayers;    //True if two players are playing

//Old mouse position:
static int oldMousex, oldMousey;
//Old Stage Clear Buble
static int oldBubleX, oldBubleY;

//bool doublebuf = false; //if true, screen is double buffered
static char forceredraw; //If 1: always redraw, if 2: rarely redraw

static bool singlePuzzle = false; //if true we are just in a little 300x600 window
static int singlePuzzleNr = 0;
static string singlePuzzleFile;

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
	SDLKey up;
	SDLKey down;
	SDLKey left;
	SDLKey right;
	SDLKey change;
	SDLKey push;
};

control keySettings[3];	//array to hold the controls (default and two custom)

#define KEYMENU_MAXWITH 4
#define KEYMENU_MAXDEPTH 7

enum stageButton {SBdontShow, SBstageClear, SBpuzzleMode};

static stageButton stageButtonStatus = SBdontShow;

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
