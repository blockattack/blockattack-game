/*
mainVars.hpp
Copyright (C) 2007 Poul Sander

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Poul Sander
    R�veh�jvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
*/

//main variables and constants

//Some definitions
//The game is divided in frames. FALLTIME means the blocks will fall one block every FRAMELENGTH*FALLTIME millisecond
#define FRAMELENGTH 50
#define HANGTIME 40
#define FALLTIME 20
//Don't change the following, they are fundamental and later some functions are hardcoded
#define BLOCKFALL 10000
#define BLOCKWAIT 100000
#define BLOCKHANG 1000
#define GARBAGE 1000000
#define CHAINPLACE 10000000
#define NUMBEROFCHAINS 100

//Animation lengths:
#define READYTIME 500
#define BOMBTIME 200
#define CURSORTIME 200


using namespace std;	//remove it if you dare...
char sharedir[] = SHAREDIR;

//All graphic in the game (as pointers):
SDL_Surface *background;    //Stores background
SDL_Surface *backgroundImage; //Stores the background image
int backgroundImageW, backgroundImageH; //size of background image
SDL_Surface *backBoard;     //Stores the background to the board
SDL_Surface *b1player;
SDL_Surface *b2players;
SDL_Surface *bVsMode;
SDL_Surface *bStageClear;
SDL_Surface *bPuzzle;
SDL_Surface *bNewGame;      //The New Game botton
SDL_Surface *bEndless;      //Endless button (sub to new)
SDL_Surface *bTimeTrial;    //Time trial button (sub to new)
SDL_Surface *bOptions;      //The Options botton
//new in 1.1.1
SDL_Surface *bConfigure;    //The configure button
SDL_Surface *bSelectPuzzle; //The Select Puzzle Button
SDL_Surface *bBack;         //The "Back" button
SDL_Surface *bForward;      //The "forward" button
//new in 1.1.1 end
//new in 1.1.2
SDL_Surface *iChainBack;
//new in 1.1.2 end (nota that iSmallFont has also been added)
//new in 1.2.0
SDL_Surface *bReplay;
SDL_Surface *bSave;
SDL_Surface *bLoad;
#if NETWORK
SDL_Surface *bNetwork;
SDL_Surface *bConnect;
SDL_Surface *bHost;
#endif
//new in 1.2.0 end
SDL_Surface *bHighScore;    //The High Score botton
SDL_Surface *bExit;         //The Exit botton
SDL_Surface *blackLine;		//The seperator in stage clear
SDL_Surface *stageBobble;	//The bobble instage clear
SDL_Surface *screen;        //The whole screen;
SDL_Surface *iGameOver;     //The gameOver image
SDL_Surface *iWinner;		//the "winner" image
SDL_Surface *iDraw;			//the "draw" image
SDL_Surface *iLoser;		//the "loser" image
//Animations:
SDL_Surface *cursor[2];    //The animated cursor
SDL_Surface *bomb[2];       //Bomb then the bricks should blow
SDL_Surface *ready[2];      //Before the blocks fall
SDL_Surface *explosion[4];   //Then a block explodes
//Animations end
SDL_Surface *counter[3];    //Counts down from 3
SDL_Surface *bricks[7];     //The bricks, saved in an array of pointers
SDL_Surface *crossover;     //Cross the bricks that will be cleared soon
SDL_Surface *balls[7];      //The balls (the small ones that jump around)
SDL_Surface *iBlueFont;      //Contains the blue font used
SDL_Surface *iSmallFont;	//Small font used for the chain text
SDL_Surface *topscoresBack;  //The backgound to the highscore list
SDL_Surface *optionsBack;
SDL_Surface *changeButtonsBack;
SDL_Surface *dialogBox;
//SDL_Surface *fileDialogBox;  //Manual entering of filename, new in 1.1.1, obsolute in 1.1.2
SDL_Surface *bOn;
SDL_Surface *bOff;
SDL_Surface *bChange;
SDL_Surface *b1024;
//SDL_Surface *b1280;
//SDL_Surface *b1400;
//SDL_Surface *b1600;
SDL_Surface *iLevelCheck;		//To the level select screen
SDL_Surface *iLevelCheckBox;
SDL_Surface *iCheckBoxArea;
SDL_Surface *boardBackBack;
SDL_Surface *garbageTL;			//the Garbage Blocks
SDL_Surface *garbageT;
SDL_Surface *garbageTR;
SDL_Surface *garbageR;
SDL_Surface *garbageBR;
SDL_Surface *garbageB;
SDL_Surface *garbageBL;
SDL_Surface *garbageL;
SDL_Surface *garbageFill;
SDL_Surface *garbageM;
SDL_Surface *garbageML;
SDL_Surface *garbageMR;
//new in 1.3.0 start
SDL_Surface *smiley[4];
SDL_Surface *garbageGM;
SDL_Surface *garbageGML;
SDL_Surface *garbageGMR;
//new in 1.3.0 end
//new in 1.3.2 start
SDL_Surface *transCover;        //The transperant block, covers the upcomming
//also new in 1.3.2 for the editor
#if LEVELEDITOR
SDL_Surface *bCreateFile;
SDL_Surface *bDeletePuzzle;
SDL_Surface *bLoadFile;
SDL_Surface *bMoveBack;
SDL_Surface *bMoveDown;
SDL_Surface *bMoveForward;
SDL_Surface *bMoveLeft;
SDL_Surface *bMoveRight;
SDL_Surface *bMoveUp;
SDL_Surface *bNewPuzzle;
SDL_Surface *bSaveFileAs;
SDL_Surface *bSavePuzzle;
SDL_Surface *bSaveToFile;
SDL_Surface *bTestPuzzle;
#endif
//new in 1.3.2 end
//new in 1.4.0
SDL_Surface *bTheme;
SDL_Surface *bSkip;
SDL_Surface *bRetry;
SDL_Surface *bNext;
//end new in 1.4.0
SDL_Surface *mouse;				//The mouse cursor


SDL_Surface *tmp;				//a temporary surface to use DisplayFormat


SFont_Font *fBlueFont;      //Stores the blue font (SFont)
SFont_Font *fSmallFont;		//Stores the small font (SFont)
//TTFont ttfont;           //Stores the TTF font (TTFSDL)

Mix_Music *bgMusic;         //backgroundMusic
Mix_Chunk *boing;           //boing sound when clearing
Mix_Chunk *timesUp;         //whistle when times up
Mix_Chunk *applause;        //Applause, then the player is good
Mix_Chunk *photoClick;      //clickSound
Mix_Chunk *heartBeat;		//heart beat
Mix_Chunk *typingChunk;          //When writing
Mix_Chunk *counterChunk;         //When counting down

Highscore theTopScoresEndless;      //Stores highscores for endless
Highscore theTopScoresTimeTrial;    //Stores highscores for timetrial

bool bMouseUp;              //true if the mouse(1) is unpressed
bool bMouseUp2;             //true if the mouse(2) is unpressed
bool bNewGameOpen;          //show sub menues
bool bOptionsOpen;          //Show OptionsMenu (Configure and Select Puzzle)
bool b1playerOpen;			//show submenu
bool b2playersOpen;			//show submenu
bool bReplayOpen;			//Show replay menu
#if NETWORK
bool bNetworkOpen;			//Show the network menu
#endif
bool showHighscores;        //true if highscores is displayed
bool showEndless;           //if true show endless highscores else timetrial
bool showGame;              //the game is active don't show highscores/options
bool showOptions;           //true if options is open
bool bScreenLocked;			//Don't take input or allow any mouse interaction! Used for dialogbox and warningbox
bool showDialog;
//new in 1.3.1
bool NoSound;				//if true, absolutely no sound will be played, can be set from the commandline
//prevents crash on systems without a soundcard
//end new 1.3.1
bool MusicEnabled;			//true if background music is enabled
bool SoundEnabled;			//true if sound effects is enabled
bool bFullscreen;			//true if game is running fullscreen
bool puzzleLoaded;          //true if the puzzle levels have been loaded
bool drawBalls;             //if true balls are drawed to the screen, this might lower framerate too much
bool standardBackground;

//new in 1.3.2
bool editorMode = false;
bool editorModeTest = false;
//end new 1.3.2

//Things for network play:
#if NETWORK
bool networkPlay;
bool networkActive;
Uint8 nrOfNetworkPlayers;  //Up to 4 players (inkl. self)
bool playerAlive[4];
//sockets here
#define SERVERPORT 41780
#define CLIENTPORT 41781


char serverAddress[30];
#endif

//should be automatically disabled if framerate to low (isn't implemented and wont be soon/ever):
//const int ballsFpsEnable = 30;     //If framerate higher -> enable balls
//const int ballsFpsDisable = 10;    //If framerate lower -> disable balls

//other ball constants:
const double gravity = 200.8; //acceleration
const double startVelocityY = 50.0;
const double VelocityX = 50.0;
const int ballSize = 16;
const double minVelocity = 200.0;

//global settings (reset everytime the game starts)
Uint8 player1Speed=0;
Uint8 player2Speed=0;
bool player1AI=false;		//Is AI enabled?
bool player2AI=false;		//Is AI enabled for player 2 (opponent in single player)
Uint8 player1AIlevel=3;		//What level is AI? 0 min, 6 max
Uint8 player2AIlevel=3;
const Uint8 AIlevels=7;		//7 possible levels: 0..6
Uint8 player1handicap=0;
Uint8 player2handicap=0;

unsigned long int currentTime;      //contains the current time, so we don't call SDL_GetTickets() too often...

int xsize;

//Stores the players names (way to long, but at least no buffer overflows (max length is 16 for display reasons))
char player1name[30];
char player2name[30];

//paths
string stageClearSavePath;
string puzzleSavePath;
string puzzleName;              //The filename of

const int nrOfStageLevels = 50;		//number of stages in stage Clear
const int maxNrOfPuzzleStages = 50; //Maximum number of puzzle stages
vector<bool> stageCleared(nrOfStageLevels);		//vector that tells if a stage is cleared
vector<Uint32> stageTimes(nrOfStageLevels);             //For statistical puposes
vector<Uint32> stageScores(nrOfStageLevels);            //--||--
vector<bool> puzzleCleared(maxNrOfPuzzleStages); //vector that tells if puzzle cleared
vector<int> nrOfMovesAllowed(maxNrOfPuzzleStages);  //Moves to clear
int puzzleLevels[maxNrOfPuzzleStages][6][12]; //Contains board layout;
int nrOfPuzzles;    //How many are there actually?

//Old mouse position:
int oldMousex, oldMousey;
//Old Stage Clear Buble
int oldBubleX, oldBubleY;

//bool doublebuf = false; //if true, screen is double buffered
char forceredraw; //If 1: always redraw, if 2: rarely redraw

bool singlePuzzle = false; //if true we are just in a little 300x600 window
int singlePuzzleNr = 0;
string singlePuzzleFile;

#if DEBUG
//frame counter (fps)
unsigned long int Frames, Ticks;
char FPS[10];
#endif

int lastNrOfPlayers; //1 if 1 player and 2 if vs. mode

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

enum stageButton {SBdontShow, SBstageClear, SBpuzzleMode};

stageButton stageButtonStatus = SBdontShow;

struct ButtonCords
{
    int x;
    int y;
    int xsize;
    int ysize;
};

ButtonCords cordNextButton = {
cordNextButton.x = 150+(150-120)/2,
cordNextButton.y = 500,
cordNextButton.xsize = 120,
cordNextButton.ysize = 40};

ButtonCords cordRetryButton = {
cordRetryButton.x = (150-120)/2,
cordRetryButton.y = 500,
cordRetryButton.xsize = 120,
cordRetryButton.ysize = 40
};
