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

//Some definitions
//The game is divided in frames. FALLTIME means the blocks will fall one block every FRAMELENGTH*FALLTIME millisecond
#define FRAMELENGTH 50
#define HANGTIME 40
#define FALLTIME 20
//Don't change the following, they are fundamental and later some functions are hardcoded
#define BLOCKFALL 10000
#define GARBAGE 1000000
#define CHAINPLACE 10000000

// The game uses a very special  base-10 pack system
// int  999999999999
//      YYYYYGWBHTTC
// YYYY = Chain
// G = Garbage, 1= NORMAL, 2=GRAY
// W = Waiting (A bomb is on it)
// B = 1 if the block is falling
// H = Block is hanging after garbage (Get Ready text on it)
// TT = Time (in steps) until something happens
// C = color

static bool block_isFalling(int block) {
	return (block/BLOCKFALL)%10;
}

static void block_setFalling(int& block, bool value) {
	if (value) {
		if (!block_isFalling(block)) {
			block += BLOCKFALL;
		}
	}
	else {
		if (block_isFalling(block)) {
			block -= BLOCKFALL;
		}
	}
}

#include "BlockGame.hpp"
#include "puzzlehandler.hpp"
#include "stageclearhandler.hpp"
#include <sstream>
#include <deque>

using std::string;
using std::stringstream;
using std::cerr;
using std::vector;

//Function to convert numbers to string (2 diget)
static string itoa2(int num) {
	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%02i", num);
	return buffer;
}

/**
 * This function tells how long the game may pause the rise of the blocks.
 * This decreases as the game progresses.
 * @param stops In out variable. If larger than the max allowed
 * @param ticks Ticks since the game was started
 * @param gameStartedAt Tick the game was started at.
 */
static void capMaxStops(int& stops, unsigned int ticks, unsigned int gameStartedAt) {
	if (ticks < gameStartedAt || stops == 0) {
		//Game not started or not stops, do nothing
		return;
	}
	unsigned int gameTime = gameStartedAt - ticks;

	int maxSeconds = 10-( (gameTime/1000) % 30 );
	if (maxSeconds < 3) {
		maxSeconds = 1;
	}
	int cappedStops = maxSeconds * 1000;
	if (stops > cappedStops) {
		stops = cappedStops;
	}
}

stageButton stageButtonStatus;

static std::stringstream ss; //Used for internal formatting

////////////////////////////////////////////////////////////////////////////////
//The BloackGame class represents a board, score, time etc. for a single player/
////////////////////////////////////////////////////////////////////////////////
int BlockGame::rand2() {
	nextRandomNumber = nextRandomNumber*1103515245 + 12345;
	return ((int)(nextRandomNumber/65536)) % 32768;
}

int BlockGame::firstUnusedChain() {
	bool found=false;
	int i = 0;
	while (!found) {
		if (!chainUsed[++i]) {
			found=true;
		}
		if (i>NUMBEROFCHAINS-2) {
			found=true;
		}
	}
	return i;
}

//Constructor
BlockGame::BlockGame() {
	AI_MoveSpeed=100;
	baseSpeed = 0.5;           //All other speeds are relative to this
	speed = baseSpeed;
	speedLevel = 1;
	ticks = 0;
	gameStartedAt = ticks;
	gameEndedAfter = 0;
	pushedPixelAt = gameStartedAt;
	nextGarbageNumber = 10;
	handicap=0;
	for (int i=0; i<7; i++) {
		for (int j=0; j<30; j++) {
			board[i][j] = -1;
		}
	}
	lastCounter = -1;           //To prevent the final chunk to be played when stating the program
}   //Constructor


void BlockGame::setGameSpeed(int globalSpeedLevel) {
	switch (globalSpeedLevel) {
	case 0:
		baseSpeed=0.5;
		break;
	case 1:
		baseSpeed=0.4;
		break;
	case 2:
		baseSpeed=0.3;
		break;
	case 3:
		baseSpeed=0.25;
		break;
	case 4:
		baseSpeed=0.2;
		break;
	default:
		baseSpeed=0.15;
		break;
	};
}

void BlockGame::setHandicap(int globalHandicap) {
	handicap=1000*((int)globalHandicap);
}

//Set the move speed of the AI based on the aiLevel parameter
//Also enables AI
void BlockGame::setAIlevel(int aiLevel) {
	AI_MoveSpeed=120-(20*(aiLevel-3));
};

int BlockGame::getAIlevel() const {
	return (120-AI_MoveSpeed)/20+3;
}

int BlockGame::GetScore() const {
	return score;
}

int BlockGame::GetHandicap() const {
	return handicap;
}

bool BlockGame::isGameOver() const {
	return bGameOver;
}

int BlockGame::GetGameStartedAt() const {
	return gameStartedAt;
}

int BlockGame::GetGameEndedAt() const {
	return gameEndedAfter;
}

bool BlockGame::isTimeTrial() const {
	return timetrial;
}

bool BlockGame::isStageClear() const {
	return stageClear;
}

bool BlockGame::isVsMode() const {
	return vsMode;
}

bool BlockGame::isPuzzleMode() const {
	return puzzleMode;
}

int BlockGame::GetLinesCleared() const {
	return linesCleared;
}

int BlockGame::GetStageClearLimit() const {
	return stageClearLimit;
}

int BlockGame::GetChains() const {
	return chain;
}

int BlockGame::GetPixels() const {
	return pixels;
}

int BlockGame::GetSpeedLevel() const {
	return speedLevel;
}

int BlockGame::GetTowerHeight() const {
	return TowerHeight;
}

int BlockGame::GetCursorX() const {
	return cursorx;
}

int BlockGame::GetCursorY() const {
	return cursory;
}

void BlockGame::MoveCursorTo(int x, int y) {
	cursorx = x;
	cursory = y;
}

bool BlockGame::GetIsWinner()  const {
	return hasWonTheGame;
}

void BlockGame::NewGame(const BlockGameStartInfo& s) {
	replayInfo.startInfo = s;
	replayInfo.actions.clear();
	this->recordStats = s.recordStats;
	if (s.AI) {
		recordStats = false;
	}
	NewGame(s.ticks);
	if (s.timeTrial) {
		timetrial = true;
		putStartBlocks();
	}
	if (s.stageClear) {
		if (s.level > -1) {
			stageClear = true;
			Level = s.level;
			Stats::getInstance()->addOne("PlayedStageLevel"+itoa2(s.level));
			stageClearLimit = 30+(Level%6)*10;
			baseSpeed = 0.5/((double)(Level*0.5)+1.0);
			speed = baseSpeed;
		}
	}
	if (s.puzzleMode) {
		if (s.level>-1) {
			puzzleMode = true;
			Level = s.level;
			MovesLeft = PuzzleNumberOfMovesAllowed(Level);
			for (int i=0; i<6; i++) {
				for (int j=0; j<12; j++) {
					board[i][j+1] = PuzzleGetBrick(Level,i,j);
				}
			}
			baseSpeed = 100000;
			speed = 100000;

			//Now push the blines up
			for (int i=19; i>0; i--) {
				for (int j=0; j<6; j++) {
					board[j][i] = board[j][i-1];
				}
			}
			for (int j=0; j<6; j++) {
				board[j][0] = 5;
			}
		}
		this->singlePuzzle = s.singlePuzzle;
	}
	if (s.vsMode) {
		vsMode = true;
		AI_Enabled = s.AI;
		if (recordStats) {
			Stats::getInstance()->addOne("VSgamesStarted");
		}
		if (s.vsAI) {
			setAIlevel(s.level);
			vsAI = true;
		}
		if (AI_Enabled) {
			name = "CPU";
			setAIlevel(s.level);
		}
		putStartBlocks();
	}
	if (s.startBlocks >= 0) {
		putStartBlocks(s.startBlocks);
	}
	if (s.handicap > 0) {
		setHandicap(s.handicap);
	}
	if (s.gameSpeed > 0) {
		setGameSpeed(s.gameSpeed);
	}
}

//Instead of creating new object new game is called, to prevent memory leaks
void BlockGame::NewGame( unsigned int ticks) {
	this->ticks = ticks;
	stageButtonStatus = SBdontShow;
	nrFellDown = 0;
	nrPushedPixel = 0;
	nrStops = 0;
	cursorx = 2;
	cursory = 3;
	stop = 0;
	pixels = 0;
	score = 0;
	bGameOver = false;
	bDraw = false;
	timetrial = false;
	stageClear = false;
	linesCleared = 0;
	hasWonTheGame = false;
	vsMode = false;
	puzzleMode = false;
	combo=0;
	chain=0;
	AI_Enabled = false;
	baseSpeed= 0.5;
	speed = baseSpeed;
	speedLevel = 1;
	gameStartedAt = ticks+3000;
	pushedPixelAt = gameStartedAt;
	nextGarbageNumber = 10;
	handicap=0;
	for (int i=0; i<7; i++) {
		for (int j=0; j<30; j++) {
			board[i][j] = -1;
		}
	}
	for (int i=0; i<NUMBEROFCHAINS; i++) {
		chainUsed[i]=false;
		chainSize[i] = 0;
	}
	lastAImove = ticks+3000;
}   //NewGame

void BlockGame::FinalizeBlockGameInfo() {
	replayInfo.extra.name = name;
	replayInfo.extra.score = score;
	replayInfo.extra.seconds = gameEndedAfter;
}

//Prints "winner" and ends game
void BlockGame::setPlayerWon() {
	if (!bGameOver || !hasWonTheGame) {
		gameEndedAfter = ticks-gameStartedAt; //We game ends now!
		if (recordStats) {
			TimeHandler::addTime("playTime",TimeHandler::ms2ct(gameEndedAfter));
		}
		bGameOver = true;
		PlayerWonEvent();
		if (recordStats) {
			Stats::getInstance()->addOne("totalWins");
			if (vsAI) {
				//We have defeated an AI
				Stats::getInstance()->addOne("defeatedAI"+std::to_string(getAIlevel()));
			}
		}
		if (AI_Enabled && vsAI) {
			//The AI have defeated a human player
			Stats::getInstance()->addOne("defeatedByAI"+std::to_string(getAIlevel()));
		}
		FinalizeBlockGameInfo();
	}
	hasWonTheGame = true;
}

//Prints "draw" and ends the game
void BlockGame::setDraw() {
	bGameOver = true;
	if (recordStats) {
		TimeHandler::addTime("playTime",TimeHandler::ms2ct(gameEndedAfter));
	}
	hasWonTheGame = false;
	bDraw = true;
	DrawEvent();
	if (recordStats) {
		Stats::getInstance()->addOne("totalDraws");
	}
	FinalizeBlockGameInfo();
}


//Test if LineNr is an empty line, returns false otherwise.
bool BlockGame::LineEmpty(int lineNr) const {
	bool empty = true;
	for (int i = 0; i <7; i++) {
		if (board[i][lineNr] != -1) {
			empty = false;
		}
	}
	return empty;
}

//Test if the entire board is empty (used for Puzzles)
bool BlockGame::BoardEmpty() const {
	bool empty = true;
	for (int i=0; i<6; i++) {
		for (int j=1; j<13; j++) {
			if (board[i][j] != -1) {
				empty = false;
			}
		}
	}
	return empty;
}

//Anything that the user can't move? In that case Game Over cannot occur
bool BlockGame::hasStaticContent() const {
	for (int i=0; i<6; i++) {
		for (int j=1; j<13; j++) {
			if (board[i][j] >= 10000000) {  //Higher than this means combos (garbage is static, but the stack is static but nothing to do about it)
				return true;    //They are static
			}
		}
	}
	return false;                   //Return false if no static object found
}

void BlockGame::putStartBlocks() {
	putStartBlocks(time(0));
}

void BlockGame::putStartBlocks(int n) {
	for (int i=0; i<7; i++) {
		for (int j=0; j<30; j++) {
			board[i][j] = -1;
		}
	}
	nextRandomNumber = n;
	int choice = rand2()%3; //Pick a random layout
	switch (choice) {
	case 0:
		//row 0:
		board[0][0]=1;
		board[1][0]=0;
		board[2][0]=4;
		board[3][0]=3;
		board[4][0]=3;
		board[5][0]=5;
		//row 1:
		board[0][1]=1;
		board[1][1]=4;
		board[2][1]=2;
		board[3][1]=0;
		board[4][1]=4;
		board[5][1]=5;
		//row 2:
		board[0][2]=2;
		board[1][2]=3;
		board[2][2]=0;
		board[3][2]=4;
		board[4][2]=1;
		board[5][2]=1;
		//row 3:
		board[0][3]=3;
		board[1][3]=2;
		board[2][3]=3;
		board[3][3]=1;
		board[4][3]=0;
		board[5][3]=4;
		//row 4:
		board[0][4]=2;
		board[1][4]=3;
		board[2][4]=3;
		board[3][4]=1;
		board[4][4]=4;
		board[5][4]=0;
		//row 5:
		board[0][5]=-1;
		board[1][5]=5;
		board[2][5]=5;
		board[3][5]=-1;
		board[4][5]=1;
		board[5][5]=-1;
		break;
	case 1:
		//row 0:
		board[0][0]=3;
		board[1][0]=5;
		board[2][0]=0;
		board[3][0]=0;
		board[4][0]=4;
		board[5][0]=2;
		//row 1:
		board[0][1]=3;
		board[1][1]=5;
		board[2][1]=-1;
		board[3][1]=5;
		board[4][1]=4;
		board[5][1]=2;
		//row 2:
		board[0][2]=2;
		board[1][2]=-1;
		board[2][2]=-1;
		board[3][2]=4;
		board[4][2]=0;
		board[5][2]=3;
		//row 3:
		board[0][3]=2;
		board[5][3]=3;
		break;
	default:
		//row 0:
		board[0][0]=4;
		board[1][0]=5;
		board[2][0]=2;
		board[3][0]=0;
		board[4][0]=1;
		board[5][0]=5;
		//row 1:
		board[0][1]=4;
		board[1][1]=5;
		board[2][1]=2;
		board[3][1]=1;
		board[4][1]=0;
		board[5][1]=2;
		//row 2:
		board[0][2]=2;
		board[1][2]=4;
		board[2][2]=-1;
		board[3][2]=0;
		board[4][2]=1;
		board[5][2]=5;
		//row 3:
		board[0][3]=4;
		board[1][3]=2;
		board[2][3]=-1;
		board[3][3]=1;
		board[4][3]=0;
		board[5][3]=2;
		//row 4:
		board[0][4]=4;
		board[1][4]=2;
		board[2][4]=-1;
		board[3][4]=0;
		board[4][4]=1;
		board[5][4]=-1;
		break;
	};
}

//decreases hang for all hanging blocks and wait for waiting blocks
void BlockGame::ReduceStuff() {
	int howMuchHang = (ticks - FRAMELENGTH*hangTicks)/FRAMELENGTH;
	if (howMuchHang>0) {
		for (int i=0; i<7; i++) {
			for (int j=0; j<30; j++) {
				if ((board[i][j]/BLOCKHANG)%10==1) {
					int hangNumber = (board[i][j]/10)%100;
					if (hangNumber<=howMuchHang) {
						board[i][j]-=BLOCKHANG;
						board[i][j]-=hangNumber*10;
					}
					else {
						board[i][j]-=10*howMuchHang;
					}
				}
				if ((board[i][j]/BLOCKWAIT)%10==1) {
					int hangNumber = (board[i][j]/10)%100;
					if (hangNumber<=howMuchHang) {
						//The blocks must be cleared
						board[i][j]-=hangNumber*10;
					}
					else {
						board[i][j]-=10*howMuchHang;
					}
				}
			}
		}
	}
	hangTicks+=howMuchHang;
}

//Creates garbage using a given wide and height
bool BlockGame::CreateGarbage(int wide, int height) {
	{
		if (wide>6) {
			wide = 6;
		}
		if (height>12) {
			height = 12;
		}
		int startPosition = 12;
		while ( startPosition <= 29 && !LineEmpty(startPosition)) {
			startPosition++;
		}
		if (startPosition >= 29) {
			return false;    //failed to place blocks
		}
		if (29-startPosition<height) {
			return false;    //not enough space
		}
		int start = 0;
		int end = 6;
		if (bGarbageFallLeft) {
			start=0;
			end=start+wide;
			bGarbageFallLeft = false;
		}
		else {
			start=6-wide;
			end = 6;
			bGarbageFallLeft = true;
		}
		for (int i = startPosition; i <startPosition+height; i++) {
			for (int j = start; j < end; j++) {
				board[j][i] = 1000000+nextGarbageNumber;
			}
		}
		nextGarbageNumber++;
		if (nextGarbageNumber>999999) {
			nextGarbageNumber = 10;
		}
		return true;
	}
	return false;
}

//Creates garbage using a given wide and height
bool BlockGame::CreateGreyGarbage() {
	int startPosition = 12;
	while ( startPosition <= 29 && !LineEmpty(startPosition) ) {
		startPosition++;
	}
	if (startPosition >= 29) {
		return false;    //failed to place blocks
	}
	int start = 0;
	int end = 6;
	for (int i = startPosition; i <startPosition+1; i++) {
		for (int j = start; j < end; j++) {
			board[j][i] = 2*1000000+nextGarbageNumber;
		}
	}
	nextGarbageNumber++;
	if (nextGarbageNumber>999999) {
		nextGarbageNumber = 10;
	}
	return true;
}


//Clears garbage, must take one the lower left corner!
int BlockGame::GarbageClearer(int x, int y, int number, bool aLineToClear, int chain) {
	if ((board[x][y])%1000000 != number) {
		return -1;
	}
	if (aLineToClear) {
		board[x][y] = this->rand2() % 6;
		board[x][y] += 10*HANGTIME+BLOCKHANG+CHAINPLACE*chain;
	}
	garbageToBeCleared[x][y] = false;
	GarbageClearer(x+1, y, number, aLineToClear, chain);
	GarbageClearer(x, y+1, number, false, chain);
	return 1;
}

//Marks garbage that must be cleared
int BlockGame::GarbageMarker(int x, int y) {
	if ((x>6)||(x<0)||(y<0)||(y>29)) {
		return -1;
	}
	if (((board[x][y])/1000000 == 1)&&(garbageToBeCleared[x][y] == false)) {
		garbageToBeCleared[x][y] = true;
		//Float fill
		GarbageMarker(x-1, y);
		GarbageMarker(x+1, y);
		GarbageMarker(x, y-1);
		GarbageMarker(x, y+1);
	}
	return 1;
}

int BlockGame::FirstGarbageMarker(int x, int y) {
	if ((x>6)||(x<0)||(y<0)||(y>29)) {
		return -1;
	}
	if (((board[x][y])/1000000 == 2)&&(garbageToBeCleared[x][y] == false)) {
		for (int i=0; i<6; i++) {
			garbageToBeCleared[i][y] = true;
		}
	}
	else if (((board[x][y])/1000000 == 1)&&(garbageToBeCleared[x][y] == false)) {
		garbageToBeCleared[x][y] = true;
		//Float fill
		GarbageMarker(x-1, y);
		GarbageMarker(x+1, y);
		GarbageMarker(x, y-1);
		GarbageMarker(x, y+1);
	}
	return 1;
}

//Clear Blocks if 3 or more is alligned (naive implemented)
void BlockGame::ClearBlocks() {

	bool toBeCleared[7][30]; //true if blok must be removed

	int previus=-1; //the last block checked
	int combo=0;
	for (int i=0; i<30; i++)
		for (int j=0; j<7; j++) {
			toBeCleared[j][i] = false;
			garbageToBeCleared[j][i] = false;
		}
	for (int i=0; i<7; i++) {
		bool faaling = false;
		for (int j=0; j<30; j++) {
			if ((faaling)&&(board[i][j]>-1)&&(board[i][j]%10000000<7)) {
				block_setFalling(board[i][j], true);
			}
			if ((!faaling)&&((board[i][j]/BLOCKFALL)%10==1)) {
				block_setFalling(board[i][j], false);
			}
			if (!((board[i][j]>-1)&&(board[i][j]%10000000<7))) {
				faaling=true;
			}
			if (((board[i][j]/1000000)%10==1)||((board[i][j]/1000000)%10==2)||((board[i][j]/BLOCKHANG)%10==1)||((board[i][j]/BLOCKWAIT)%10==1)) {
				faaling = false;
			}
		}
	}


	for (int j=0; j<7; j++) {
		previus = -1;
		combo=0;

		for (int i=1; i<30; i++) {
			if ((board[j][i]>-1)&&(board[j][i]%10000000<7)) {
				if (board[j][i]%10000000 == previus) {
					combo++;
				}
				else {
					if (combo>2) {
						for (int k = i-combo; k<i; k++) {
							toBeCleared[j][k] = true;
						}
					}
					combo=1;
					previus = board[j][i]%10000000;
				}
			} //if board
			else {
				if (combo>2) {
					for (int k = i-combo; k<i; k++) {
						toBeCleared[j][k] = true;
					}
				}
				combo = 0;
				previus = -1;
			}

		} //for i
	} //for j

	chain = 0;
	for (int i=0; i<6; i++) {
		for (int j=0; j<30; j++) {
			//Clears blocks marked for clearing
			int temp=board[i][j];
			if (1==((temp/BLOCKWAIT)%10)) {
				if (((temp/10)%100)==0) {
					if (chainSize[chain]<chainSize[board[i][j]/10000000]) {
						chain = board[i][j]/10000000;
					}

					AddBall(i, j, true, board[i][j]%10);
					AddBall(i, j, false, board[i][j]%10);
					AddExplosion(i, j);
					board[i][j]=-2;
				}
			}
		}
	}
	for (int i=0; i<7; i++) {
		bool setChain=false;
		for (int j=0; j<30; j++) {
			if (board[i][j]==-1) {
				setChain=false;
			}
			if (board[i][j]==-2) {
				board[i][j]=-1;
				setChain=true;
				BlockPopEvent();
			}
			if (board[i][j]!=-1) {
				if ((setChain)&&((board[i][j]/GARBAGE)%10!=1)&&((board[i][j]/GARBAGE)%10!=2)) {
					board[i][j]=((board[i][j]%CHAINPLACE)+CHAINPLACE*chain);
				}
			}
		}
	}
	int startvalue;
	if (pixels == 0) {
		startvalue=1;
	}
	else {
		startvalue=0;
	}
	for (int i=startvalue; i<30; i++) {
		previus=-1;
		combo=0;
		for (int j=0; j<7; j++) {
			if (((board[j][i]>-1)&&(board[j][i]%10000000<7))) {
				if (board[j][i]%10000000 == previus) {
					combo++;
				}
				else {
					if (combo>2) {
						for (int k = j-combo; k<j; k++) {
							toBeCleared[k][i] = true;
						}
					}
					combo=1;
					previus = board[j][i]%10000000;
				}
			} //if board
			else {
				if (combo>2) {
					for (int k = j-combo; k<j; k++) {
						toBeCleared[k][i] = true;
					}
				}
				combo = 0;
				previus = -1;
			}

		} //for j
	} //for i

	combo = 0;
	chain = 0;
	int grey = 0;
	for (int i=0; i<30; i++) {
		for (int j=0; j<6; j++) {
			if (toBeCleared[j][i]) {
				//see if any garbage is around:
				FirstGarbageMarker(j-1, i);
				FirstGarbageMarker(j+1, i);
				FirstGarbageMarker(j, i-1);
				FirstGarbageMarker(j, i+1);
				//that is checked now :-)
				if (board[j][i]%10000000==6) {
					grey++;
				}
				if ((vsMode) && (grey>2) && (board[j][i]%10000000==6)) {
					GarbageStruct s;
					s.setGarbage(6, 1, true);
					this->garbageSendQueue.push_back(s);
				}
				if ((board[j][i]>-1)&&(board[j][i]%10000000<7)) {
					board[j][i]+=BLOCKWAIT+10*FALLTIME;
				}

				if (chainSize[board[j][i]/10000000]>chainSize[chain]) {
					chain=board[j][i]/10000000;
				}
				combo++;
				stop+=140*combo;
				score +=10;
				if (combo>3) {
					score+=3*combo;    //More points if more cleared simontanously
				}
			}
		}
	}
	score+=chainSize[chain]*100;
	if (chain==0) {
		chain=firstUnusedChain();
		chainSize[chain]=0;
		chainUsed[chain]=true;
	}
	chainSize[chain]++;
	for (int i=0; i<30; i++) {
		for (int j=0; j<6; j++) {
			if (toBeCleared[j][i]) {
				board[j][i]=(board[j][i]%10000000)+chain*10000000;
			}
		}
	}

	{
		//This is here we add text to screen!
		bool dead = false;
		for (int i=29; i>=0; i--) {
			for (int j=0; j<6; j++) {
				if (toBeCleared[j][i]) {
					if (!dead) {
						dead=true;
						string tempS = std::to_string(chainSize[chain]);
						if (chainSize[chain]>1) {
							AddText(j, i, tempS, 1000);
						}
					}
				}
			}
		}
	} //This was there text was added

	if (vsMode) {
		GarbageStruct s;
		switch (combo) {
		case 0:
		case 1:
		case 2:
		case 3:
			break;
		case 4:
			s.setGarbage(3,1);
			break;
		case 5:
			s.setGarbage(4,1);
			break;
		case 6:
			s.setGarbage(5,1);
			break;
		case 7:
			s.setGarbage(6,1);
			break;
		case 8:
			s.setGarbage(4,1);
			s.setGarbage(4,1);
			break;
		case 9:
			s.setGarbage(5,1);
			s.setGarbage(4,1);
			break;
		case 10:
			s.setGarbage(5,1);
			s.setGarbage(5,1);
			break;
		case 11:
			s.setGarbage(6,1);
			s.setGarbage(5,1);
			break;
		case 12:
			s.setGarbage(6,1);
			s.setGarbage(6,1);
			break;
		case 13:
			s.setGarbage(5,1);
			s.setGarbage(5,1);
			s.setGarbage(4,1);
			break;
		default:
			s.setGarbage(5,1);
			s.setGarbage(5,1);
			s.setGarbage(4,1);
			break;
		}
		if (s.width > 0) {
			garbageSendQueue.push_back(s);
		}
	}
	for (int i=0; i<30; i++) {
		for (int j=0; j<6; j++) {
			if (garbageToBeCleared[j][i]) {
				GarbageClearer(j, i, board[j][i]%1000000, true, chain); //Clears the blocks and all blocks connected to it.
			}
		}
	}

	chain=0;

	//Break chains (if a block is stable it is resetted to (chain == 0)):
	for (int i=0; i<7; i++) {
		bool faaling = false;  //In the beginning we are NOT falling
		for (int j=0; j<30; j++) {
			if ((faaling)&&(board[i][j]>-1)&&(board[i][j]<7)) {
				block_setFalling(board[i][j], true);
			}
			if (!faaling) {
				block_setFalling(board[i][j], false);
			}
			if ((!faaling)&&(board[i][j]>0)&&(board[i][j]/10000000!=0)&&((board[i][j]/BLOCKWAIT)%10!=1)&&((board[i][j]/BLOCKHANG)%10!=1)) {
				if (chainSize[board[i][j]/10000000]>chainSize[chain]) {
					chain=board[i][j]/10000000;
				}
				board[i][j]=board[i][j]%10000000;
			}
			if (!((board[i][j]>-1)&&(board[i][j]<7))) {
				faaling=true;
			}
			if (((board[i][j]/1000000)%10==1)||((board[i][j]/BLOCKHANG)%10==1)||((board[i][j]/BLOCKWAIT)%10==1)) {
				faaling = false;
			}
		}
	}

	//Calculate chain
	chain=0;
	for (int i=0; i<6; i++) {
		for (int j=0; j<30; j++) {
			if (chainSize[board[i][j]/10000000]>chain) {
				chain=chainSize[board[i][j]/10000000];
			}
		}
	}

	//Make space in table for more things
	if (chain==0) {
		for (int i=0; i<NUMBEROFCHAINS; i++) {
			if (chainUsed[i]==true) {
				if ((vsMode)&&(chainSize[i]>1)) {
					GarbageStruct s;
					s.setGarbage(6,chainSize[i]-1);
					this->garbageSendQueue.push_back(s);
				}
				if (chainSize[i]>4) {
					LongChainDoneEvent();
				}
				if (chainSize[i]>1 && !puzzleMode && recordStats) {
					Stats::getInstance()->addOne((string)"chainX"+std::to_string(chainSize[i]));
				}
				chainUsed[i]=false;
			}
		}
	}
} //ClearBlocks

//prints "Game Over" and ends game
void BlockGame::SetGameOver() {
	if (!bGameOver) {
		gameEndedAfter = ticks-gameStartedAt; //We game ends now!
		if (recordStats) {
			TimeHandler::addTime("playTime",TimeHandler::ms2ct(gameEndedAfter));
		}
		FinalizeBlockGameInfo();
	}
	bGameOver = true;
	if (stageClear) {
		stageButtonStatus = SBstageClear;
	}
}

bool BlockGame::GetAIenabled() const {
	return AI_Enabled;
}


//Moves all peaces a spot down if possible
int BlockGame::FallBlock(int x, int y, int number) {
	if (y == 0) {
		return -1;
	}
	if (x>0) {
		if (board[x-1][y] == number) {
			return -1;
		}
	}
	int i=x;
	bool canFall = true;
	//checks a line of a garbage block and see if something is under it
	while ((board[i][y] == number)&&(canFall)&&(i<6)) {
		if (board[i][y-1] != -1) {
			canFall = false;
		}
		i++;
	}
	if (canFall) {
		//cout << "Now falling" << "\n";
		for (int j = x; j<i; j++) {
			board[j][y-1] = board[j][y];
			board[j][y] = -1;
		}
	}
	return 0;
}   //FallBlock


//Makes all Garbage fall one spot
void BlockGame::GarbageFall() {
	for (int i=0; i<30; i++) {
		for (int j=0; j<7; j++) {
			if ((((board[j][i]/1000000)%10) == 1)||(((board[j][i]/1000000)%10) == 2)) {
				FallBlock(j, i, board[j][i]);
			}
		}
	}
}

//Makes the blocks fall (it doesn't test time, this must be done before hand)
void BlockGame::FallDown() {
	bool falling =false;        //nothing is moving unless proven otherwise
	for (int i=0; i<29; i++) {
		for (int j=0; j<6; j++) {
			if ((board[j][i]==-1) && (board[j][i+1]!=-1) && (board[j][i+1]%BLOCKFALL<7)) {
				board[j][i] = board[j][i+1];
				board[j][i+1] = -1;
				falling = true;              //something is moving!
			}
			if ((board[j][i]/BLOCKWAIT)%10==1) {
				falling=true;
			}
		}
	}
	if (!falling) { //If nothing is falling
		if ((puzzleMode)&&(!bGameOver)&&(MovesLeft==0)&&(!(BoardEmpty()))) {
			//Puzzle not won
			SetGameOver();
			stageButtonStatus = SBpuzzleMode;
		}
	}
	GarbageFall();      //Makes the garbage fall
	nrFellDown++;      //Sets number of this fall, so we know then the next will occur
}

//Moves the cursor, receaves N,S,E or W as a char an moves as desired
void BlockGame::MoveCursor(char way) {
	if (!bGameOver) {     //If game over nothing happends
		if ((way == 'N') && ((cursory<10)||(TowerHeight>12) ||(((pixels==bsize)||(pixels==0)) && (cursory<11)))) {
			cursory++;
		}
		if ((way == 'S') && (cursory>0)) {
			cursory--;
		}
		if ((way == 'W') && (cursorx>0)) {
			cursorx--;
		}
		if ((way == 'E') && (cursorx<4)) {
			cursorx++;
		}
	}
}

//switches the two blocks at the cursor position, unless game over
void BlockGame::SwitchAtCursor() {
	if (bGameOver) {
		return;
	}
	ClearBlocks();  //Ensure that everything that floats are marked as floating
	if ((board[cursorx][cursory+1]<7) && (board[cursorx+1][cursory+1]<7) && ((!puzzleMode)||(MovesLeft>0)) && (gameStartedAt<ticks)) {
		int temp = board[cursorx][cursory+1];
		board[cursorx][cursory+1] = board[cursorx+1][cursory+1];
		board[cursorx+1][cursory+1] = temp;
	}
	if ((puzzleMode)&&(gameStartedAt<ticks)&&(MovesLeft>0)) {
		MovesLeft--;
	}
}

void BlockGame::PushLine() {
	PushLineInternal();
}

//Generates a new line and moves the field one block up (restart puzzle mode)
void BlockGame::PushLineInternal() {
	//If not game over, not high tower and not puzzle mode
	if ((!bGameOver) && TowerHeight<13 && (!puzzleMode) && (gameStartedAt<ticks)&&(chain==0)) {
		for (int i=19; i>0; i--) {
			for (int j=0; j<6; j++) {
				board[j][i] = board[j][i-1];
			}
		}
		for (int j=0; j<6; j++) {
			board[j][0] = rand2() % 4;
			if (j > 0) {
				if (board[j][0] == board[j-1][0]) {
					board[j][0] = rand2() % 6;
				}
			}
			if (board[j][0] == board[j][1]) {
				board[j][0] = rand2() % 6;
			}
			if (board[j][0] == board[j][1]) {
				board[j][0] = rand2() % 6;
			}
			while ((j>0)&&(board[j][0]==board[j-1][0])) {
				board[j][0] = rand2() % 6;
			}
		}
		score+=1;
		MoveCursor('N'); //Workaround for this being done registred too
		if (vsMode) {
			if (rand2()%6==1) {
				board[rand2()%6][0]=6;
			}
		}
		pixels = 0;
		stop=0;
		pushedPixelAt = ticks;
		linesCleared++;
		AI_LineOffset++;
		nrPushedPixel=(int)((double)(pushedPixelAt-gameStartedAt)/(1000.0*speed));

		if (recordStats) {
			Stats::getInstance()->addOne("linesPushed");
		}
	} //if !bGameOver

	//Restart Puzzle mode
	if (puzzleMode && !bGameOver) {
		//Reloads level
		MovesLeft = PuzzleNumberOfMovesAllowed(Level);
		for (int i=0; i<6; i++) {
			for (int j=0; j<12; j++) {
				board[i][j+1] = PuzzleGetBrick(Level,i,j);
			}
		}
		score=0;
		bGameOver=false;
	}

	if ((TowerHeight>12) && (!puzzleMode)&&(!bGameOver)&&(chain==0)) {
		SetGameOver();
	}


}//PushLine

//Pushes a single pixel, so it appears to scrool
void BlockGame::PushPixels() {
	nrPushedPixel++;
	if ((pixels < bsize) && TowerHeight<13) {
		pixels++;
	}
	else {
		PushLineInternal();
	}
	if (pixels>bsize) {
		pixels=0;
	}
}


//See how high the tower is, saved in integer TowerHeight
void BlockGame::FindTowerHeight() {
	/*
	 * Old implementation, used until I find the bug in the other.
	 * This function has a bug in stage clear! if an empty line appears.
	 */
	prevTowerHeight = TowerHeight;
	bool found = false;
	TowerHeight = 0;
	while (!found) {
		found = true;
		for (int j=0; j<6; j++)
			if (board[j][TowerHeight] != -1) {
				found = false;
			}
		TowerHeight++;
	}
	TowerHeight--;
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////// AI starts here! ///////////////////////////////
///////////////////////////////////////////////////////////////////////////
//First the helpet functions:
int BlockGame::nrOfType(int line, int type) {
	// cout << "Start_ nrOfType" << "\n";
	int counter = 0;
	for (int i=0; i<6; i++)
		if (board[i][line]==type) {
			counter++;
		}
	return counter;
}


//See if a combo can be made in this line
int BlockGame::horiInLine(int line) {
	//cout << "Start_ hori in line" << "\n";
	int nrOfType[7] = {0, 0, 0, 0, 0, 0, 0};
	int iTemp;
	int max = 0;
	for (int i=0; i<6; i++) {
		iTemp = board[i][line];
		if ((iTemp>-1)&&(iTemp<7)) {
			nrOfType[iTemp]++;
		}
	}
	for (int j=0; j<7; j++) {
		if (nrOfType[j]>max) {
			max = nrOfType[j];
			AIcolorToClear = j;
		}
	}
	return max;
}

bool BlockGame::horiClearPossible() {
	//cout << "Start_ horiclear possible" << "\n";
	int i=13;
	bool solutionFound = false;
	do {
		if (horiInLine(i)>2) {
			AI_LineOffset = 0;
			AIlineToClear = i;
			solutionFound = true;
		}
		i--;
	}
	while ((!solutionFound)&&(i>0));
	return solutionFound;
}

//the Line Has Unmoveable Objects witch might stall the AI
bool BlockGame::lineHasGarbage(int line) {
	for (int i=0; i<6; i++) {
		if (board[i][line]>1000000) {
			return true;
		}
	}
	return false;
}

//Types 0..6 in line
int BlockGame::nrOfRealTypes(int line) {
	//cout << "Start_ nrOfReal" << "\n";
	int counter = 0;
	for (int i=0; i<6; i++) {
		if ((board[i][line]>-1)&&(board[i][line]<7)) {
			counter++;
		}
	}
	return counter;
}

//See if there is a tower
bool BlockGame::ThereIsATower() {
	//cout << "Start_ there is a tower" << "\n";
	bool bThereIsATower = false; //Unless proven otherwise!
	bool topReached = false; //If we have reached the top
	int lineNumber = 0;
	bool emptySpacesFound = false;
	do {
		if ((emptySpacesFound) && (nrOfRealTypes(lineNumber)>0)&&(nrOfType(lineNumber, -1)>0)) {
			AIlineToClear = lineNumber;
			if (lineHasGarbage(lineNumber)) {
				return false;
			}
			else {
				bThereIsATower = true;
			}
		}
		else {
			emptySpacesFound=false;
		}
		if ((!emptySpacesFound)&&(nrOfType(lineNumber, -1)>0)) {
			emptySpacesFound = true;
		}
		if (lineNumber<12) {
			lineNumber++;
		}
		else {
			topReached = true;
		}
	}
	while ((!bThereIsATower)&&(!topReached));
	//if(bThereIsATower)
	//cout << "There is actually a tower" << "\n";
	return bThereIsATower;
}

double BlockGame::firstInLine1(int line) {
	if (line > 20 || line < 0) {
		cerr << "Warning: first in Line1: " << line << "\n";
		return 3.0;
	}
	for (int i=0; i<6; i++) {
		if ((board[i][line]>-1)&&(board[i][line]<7)) {
			return (double)i;
		}
	}
	return 3.0;
}

//returns the first coordinate of the block of type
double BlockGame::firstInLine(int line, int type) {
	if (line > 20 || line < 0) {
		cerr << "Warning: first in Line: " << line << "\n";
		return 3.0;
	}
	for (int i=0; i<6; i++) {
		if (board[i][line]==type) {
			return (double)i;
		}
	}
	return 3.0;
}

//There in the line shall we move
int BlockGame::closestTo(int line, int place) {
	if ((int)firstInLine1(line)>place) {
		return (int)firstInLine1(line)-1;
	}
	for (int i=place; i>=0; i--) {
		if ((board[i][line]>-1)&&(board[i][line]<7)) {
			return i;
		}
	}
	AIstatus=0;
	return place;
}

//The AI will remove a tower
void BlockGame::AI_ClearTower() {
	//   cout << "AI: ClearTower, line: " << AIlineToClear << "\n";
	int place = (int)firstInLine(AIlineToClear-1, -1); //Find an empty field to frop a brick into
	int xplace = closestTo(AIlineToClear, place); //Find the brick to drop in it
	if (cursory+1<AIlineToClear) {
		MoveCursor('N');
	}
	else if (cursory+1>AIlineToClear) {
		MoveCursor('S');
	}
	else if (cursorx<xplace) {
		MoveCursor('E');
	}
	else if (cursorx>xplace) {
		MoveCursor('W');
	}
	else {
		SwitchAtCursor();
	}
	if (!ThereIsATower()) {
		AIstatus = 0;
	}
}

//The AI will try to clear block horisontally
void BlockGame::AI_ClearHori() {
	//   cout << "AI: ClearHori";
	int lowestLine = AIlineToClear;
	for (int i=0; i<7; i++) {
		if (nrOfType(lowestLine, i)>2) {
			AIcolorToClear = i;
		}
	}
	if (cursory>lowestLine-1) {
		MoveCursor('S');
	}
	else if (cursory<lowestLine-1) {
		MoveCursor('N');
	}
	else if (nrOfType(lowestLine, AIcolorToClear)>2) {
		int left=0, right=0;
		if (board[0][lowestLine]==AIcolorToClear) {
			left++;
		}
		if (board[1][lowestLine]==AIcolorToClear) {
			left++;
		}
		if (board[2][lowestLine]==AIcolorToClear) {
			left++;
		}
		if (board[3][lowestLine]==AIcolorToClear) {
			right++;
		}
		if (board[4][lowestLine]==AIcolorToClear) {
			right++;
		}
		if (board[5][lowestLine]==AIcolorToClear) {
			right++;
		}
		int xplace = 0;
		if (left<right) {
			//   cout << ", right>left";
			int count=0;
			for (int i=0; (i<4)&&(count<1); i++) {
				if ((board[i][lowestLine]==AIcolorToClear)&&((i==0)||(board[i+1][lowestLine]!=AIcolorToClear))) {
					count++;
					xplace = i;
				}
			}
		}
		else {
			//   cout << ", left>=right";
			int count=0;
			for (int i=3; (i<=5)&&(count<1); i++) {
				if ((board[i][lowestLine]==AIcolorToClear)&&(board[i-1][lowestLine]!=AIcolorToClear)) {
					count++;
					xplace = --i;
				}
			}
		}
		//cout << ", xplace: " << xplace;
		if (cursorx<xplace) {
			MoveCursor('E');
		}
		else if (cursorx>xplace) {
			MoveCursor('W');
		}
		else if (cursorx==xplace) {
			SwitchAtCursor();
		}
		else {
			AIstatus = 0;
		}
	}
	else {
		AIstatus = 0;
	}
	//cout << "\n"; //for debugging
}

//Test if vertical clear is possible
bool BlockGame::veriClearPossible() {
	bool found=false;
	int colors[7] = {0, 0, 0, 0, 0, 0, 0};
	for (int i=12; (i>0)&&(!found); i--) {
		for (int j=0; j<7; j++) {
			if (nrOfType(i, j)==0) {
				colors[j]=0;
			}
			else if (++colors[j]>2) {
				AIcolorToClear = j;
				AIlineToClear = i;
				found=true;
			}
		}
	}
	return found;
}



//There in the line shall we move
int BlockGame::closestTo(int line, int type, int place) {
	if ((int)firstInLine(line, type)>place) {
		return (int)firstInLine(line, type)-1;
	}
	for (int i=place; i>=0; i--) {
		if (board[i][line]==type) {
			return i;
		}
	}
	AIstatus=0;
	return place;
}

//The AI will try to clear blocks vertically
void BlockGame::AI_ClearVertical() {
	// cout << "AI: ClearVeri";
	//First we find the place there we will align the bricks
	int placeToCenter = (int)(firstInLine(AIlineToClear, AIcolorToClear)/3.0+firstInLine(AIlineToClear+1, AIcolorToClear)/3.0+firstInLine(AIlineToClear+2, AIcolorToClear)/3.0);
	int unlimitedLoop=0;
	if (AIlineToClear < 0 || AIlineToClear > 20) {
		cerr << "AIlineToClear out of range: " << AIlineToClear << "\n";
		return;
	}
	if (placeToCenter<0 || placeToCenter > 5) {
		cerr << "placeToCenter out of range: " << placeToCenter << "\n";
		return;
	}
	//cout << "AI_ClearVertical: " << placeToCenter << ", " << AIlineToClear << "\n";
	while (((board[placeToCenter][AIlineToClear]>1000000)||(board[placeToCenter][AIlineToClear+1]>1000000)||(board[placeToCenter][AIlineToClear+2]>1000000))&&(unlimitedLoop<10)) {
		unlimitedLoop++;
		placeToCenter++;
		if (placeToCenter>5) {
			placeToCenter=0;
		}
	}
	if (unlimitedLoop>9) {
		AIstatus = 0;
		return;
	}
	//cout << ", ptc: " << placeToCenter << ", line: " << AIlineToClear << ", cy: " << cursory;
	if (cursory+1>AIlineToClear+2) {
		// cout << ", cursory>line+2";
		MoveCursor('S');
	}
	if (cursory+1<AIlineToClear) {
		MoveCursor('N');
	}
	bool toAlign[3]= {true, true, true};
	if (board[placeToCenter][AIlineToClear+0]==AIcolorToClear) {
		toAlign[0]=false;
	}
	if (board[placeToCenter][AIlineToClear+1]==AIcolorToClear) {
		toAlign[1]=false;
	}
	if (board[placeToCenter][AIlineToClear+2]==AIcolorToClear) {
		toAlign[2]=false;
	}
	//cout << "status: " << toAlign[0] << " " << toAlign[1] << " " << toAlign[2];
	if (cursory+1==AIlineToClear) {
		if (toAlign[0]==false) {
			MoveCursor('N');
		}
		else {
			if (cursorx>closestTo(AIlineToClear, AIcolorToClear, placeToCenter)) {
				MoveCursor('W');
			}
			else if (cursorx<closestTo(AIlineToClear, AIcolorToClear, placeToCenter)) {
				MoveCursor('E');
			}
			else {
				SwitchAtCursor();
			}
		}
	}
	else if (cursory+1==AIlineToClear+1) {
		if (toAlign[1]==false) {
			if (toAlign[2]) {
				MoveCursor('N');
			}
			else {
				MoveCursor('S');
			}
		}
		else {
			if (cursorx>closestTo(AIlineToClear+1, AIcolorToClear, placeToCenter)) {
				MoveCursor('W');
			}
			else if (cursorx<closestTo(AIlineToClear+1, AIcolorToClear, placeToCenter)) {
				MoveCursor('E');
			}
			else {
				SwitchAtCursor();
			}
		}
	}
	else if (cursory+1==AIlineToClear+2) {
		if (toAlign[2]==false) {
			MoveCursor('S');
		}
		else {
			if (cursorx>closestTo(AIlineToClear+2, AIcolorToClear, placeToCenter)) {
				MoveCursor('W');
			}
			else if (cursorx<closestTo(AIlineToClear+2, AIcolorToClear, placeToCenter)) {
				MoveCursor('E');
			}
			else {
				SwitchAtCursor();
			}
		}
	}

	if ((!toAlign[0])&&(!toAlign[1])&&(!toAlign[2])) {
		AIstatus = 0;
	}
	if ((nrOfType(AIlineToClear, AIcolorToClear)==0)||(nrOfType(AIlineToClear+1, AIcolorToClear)==0)||(nrOfType(AIlineToClear+2, AIcolorToClear)==0)) {
		AIstatus = 0;
	}
	//cout << "\n";
}


void BlockGame::AI_Move() {
	switch (AIstatus) {
	case 1:
		if (TowerHeight<8) {
			PushLine();
		}
		else {
			AIstatus = 0;
		}
		break;
	case 2:
		AI_ClearTower();
		break;
	case 3:
		AI_ClearHori();
		break;
	case 4:
		AI_ClearVertical();
		break;
	case 5:
		if (!firstLineCreated) {
			PushLine();
			firstLineCreated = true;
		}
		else {
			PushLine();
			AIstatus = 0;
		}
		break;
	case 6:
		PushLine();
		AIstatus = 0;
		break;
	default:
		if (TowerHeight<6) {
			AIstatus = 1;
		}
		else if (horiClearPossible()) {
			AIstatus = 3;
		}
		else if (veriClearPossible()) {
			AIstatus = 4;
		}
		else if (ThereIsATower()) {
			AIstatus = 2;
		}
		else {
			AIstatus = 5;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// AI ends here! //////////////////////////////
//////////////////////////////////////////////////////////////////////////


//Updates evrything, if not called nothing happends
void BlockGame::Update() {
	unsigned int nowTime = ticks; //We remember the time, so it doesn't change during this call
	capMaxStops(stop, nowTime, gameStartedAt);
	{
		FindTowerHeight();
		if ((linesCleared-TowerHeight>stageClearLimit) && (stageClear) && (!bGameOver)) {
			StageClearSetClear(Level, score, nowTime-gameStartedAt);
			setPlayerWon();
			stageButtonStatus = SBstageClear;
		}
		if ((TowerHeight>12)&&(prevTowerHeight<13)&&(!puzzleMode)) {
			stop+=1000;
		}

		while ( nowTime> nrStops*40+gameStartedAt) { //Increase stops, till we reach nowTime
			if (stop>0) {
				stop = stop-20;
				if (stop<=0) {
					nrPushedPixel=(int)((nowTime-gameStartedAt)/(1000.0*speed));
				}
			}
			if (stop<0) {
				stop = 0;
			}
			nrStops++;
		}
		//If we have static content, we don't raise at all!
		if (hasStaticContent()) {
			stop++;
		}
		if ((puzzleMode)&&(!bGameOver)&&BoardEmpty()) {
			if (!this->singlePuzzle) {
				PuzzleSetClear(Level);
				stageButtonStatus = SBpuzzleMode;
			}
			setPlayerWon();
		}

		//increse speed:
		if ((nowTime>gameStartedAt+20000*speedLevel)&&(speedLevel <99)&&(!bGameOver)) {
			speed = (baseSpeed*0.9)/((double)speedLevel*0.5);
			speedLevel++;
			nrPushedPixel=(int)((double)(nowTime-gameStartedAt)/(1000.0*speed));
		}


		//To prevent the stack from raising a lot then we stop a chain (doesn't work anymore)
		if (chain>0) {
			stop+=1;
		}
		//Raises the stack
		if ((nowTime>gameStartedAt+nrPushedPixel*1000*speed) && (!bGameOver)&&(!stop))
			while ((nowTime>gameStartedAt+nrPushedPixel*1000*speed)&&(!(puzzleMode))) {
				PushPixels();
			}
		if (!bGameOver) {
			ClearBlocks();
		}

		/*************************************************************
		Ai stuff
		 **************************************************************/
		if (bGameOver) {
			AIstatus = 0;       //Enusres that AI is resetted
		}
		else if (AI_Enabled) {
			if (lastAImove+AI_MoveSpeed<ticks) {
				AI_Move();
				lastAImove=ticks;
			}
		}

		/*************************************************************
		Ai stuff ended
		 **************************************************************/
		if ((nowTime>gameStartedAt+nrFellDown*140) && (!bGameOver)) {
			FallDown();
		}
		if ((nowTime<gameStartedAt)&&(puzzleMode)) {
			FallDown();
			nrFellDown--;
		}
		ReduceStuff();
		if ((timetrial) && (!bGameOver) && (nowTime>gameStartedAt+2*60*1000)) {
			SetGameOver();
			TimeTrialEndEvent();
		}
	}
}

bool BlockGame::IsNearDeath() const {
	if ((TowerHeight>12)&&(!puzzleMode)&&(!bGameOver)) {
		return true;
	}
	else {
		return false;
	}
}

void BlockGame::UpdateInternal(unsigned int newtick) {
	while (newtick >= ticks+10) {
		ticks+=10;
		Update();
	}
}

void BlockGame::DoAction (const BlockGameAction& action) {
	if (action.action == BlockGameAction::Action::UPDATE && action.tick < ticks+10) {
		return;  //Ignore if this is an update and not high enough
	}
	if (!replaying) {
		if (action.action == BlockGameAction::Action::UPDATE && replayInfo.actions.size() > 0 && replayInfo.actions.back().action == action.action) {
			replayInfo.actions.back() = action;
		}
		else {
			replayInfo.actions.push_back(action);
		}
	}
	if (action.action == BlockGameAction::Action::UPDATE) {
		UpdateInternal(action.tick);
	}
	if (action.action == BlockGameAction::Action::SET_DRAW) {
		setDraw();
	}
	if (action.action == BlockGameAction::Action::SET_WON) {
		setPlayerWon();
	}
	if (action.action == BlockGameAction::Action::SET_GAME_OVER) {
		SetGameOver();
	}
	if (action.action == BlockGameAction::Action::PUSH) {
		PushLine();
	}
	if (action.action == BlockGameAction::Action::MOVE) {
		char direction = action.way;
		MoveCursor(direction);
	}
	if (action.action == BlockGameAction::Action::SWITCH) {
		SwitchAtCursor();
	}
	if (action.action == BlockGameAction::Action::PUSH_GARBAGE) {
		for (const GarbageStruct& i : action.garbage) {
			if (i.greyGarbage) {
				CreateGreyGarbage();
			}
			else {
				CreateGarbage(i.width, i.height);
			}
		}
	}
	if (action.action == BlockGameAction::Action::MOUSE_DOWN) {
		MouseDown(action.x, action.y);
	}
	if (action.action == BlockGameAction::Action::MOUSE_UP) {
		MouseUp();
	}
	if (action.action == BlockGameAction::Action::MOUSE_MOVE) {
		MouseMove(action.x);
	}
}

void BlockGame::ReplayUpdate(unsigned int newReplayTick) {
	if (!replaying) {
		return;
	}
	while (replayInfo.actions.front().tick > newReplayTick-gameStartedAt && replayInfo.actions.size()>0) {
		DoAction(replayInfo.actions.front());
		std::cerr << "Replay tick: " << replayInfo.actions.front().tick << "\n";
		replayInfo.actions.pop_front();
	}
}

bool BlockGame::isSinglePuzzle() const {
	return singlePuzzle;
}

int BlockGame::getLevel() const {
	return Level;
}

void BlockGame::PopSendGarbage(std::vector<GarbageStruct>& poppedData) {
	for (const GarbageStruct& g : this->garbageSendQueue) {
		poppedData.push_back(g);
	}
	this->garbageSendQueue.clear();
}

void BlockGame::GetMouseCursor(bool& pressed, int& x, int& y) const {
	if (mouse_cursorx < 0 || mouse_cursory < 0 || mouse_cursorx >=6 || mouse_cursory > 13) {
		pressed = false;
		x = 0;
		y = 0;
		return;
	}
	pressed = true;
	x = mouse_cursorx;
	y = mouse_cursory;
}

void BlockGame::MouseDown(int x, int y) {
	if (AI_Enabled) {
		//AI may not use mouse move. It must use the controller
		return;
	}
	mouse_cursorx = x;
	mouse_cursory = y;
}

void BlockGame::MouseMove(int x) {
	if (AI_Enabled) {
		//AI may not use mouse move. It must use the controller
		return;
	}
	if (mouse_cursorx < 0) {
		return;
	}
	if (x < 0 || x >= 6) {
		return;
	}
	if (x > mouse_cursorx) {
		MoveCursorTo(mouse_cursorx, mouse_cursory);
		++mouse_cursorx;
		SwitchAtCursor();
	}
	if (x < mouse_cursorx) {
		--mouse_cursorx;
		MoveCursorTo(mouse_cursorx, mouse_cursory);
		SwitchAtCursor();
	}
}

void BlockGame::MouseUp() {
	mouse_cursorx = -1;
	mouse_cursory = -1;
}
//Play the next level
void nextLevel(BlockGame& g, unsigned int ticks) {
	BlockGameStartInfo s;
	s.ticks = ticks;
	s.level = g.getLevel()+1;
	if (g.isPuzzleMode()) {
		if (g.getLevel()<PuzzleGetNumberOfPuzzles()-1) {
			s.puzzleMode = true;
			g.NewGame(s);
		}
	}
	else if (g.isStageClear()) {
		if (g.getLevel() < 50-1) {
			s.stageClear = true;
			g.NewGame(s);
		}
	}
}

void retryLevel(BlockGame& g, unsigned int ticks) {
	BlockGameStartInfo s;
	s.ticks = ticks;
	s.level = g.getLevel();
	if (g.isPuzzleMode()) {
		s.puzzleMode = true;
		g.NewGame(s);
	}
	else if (g.isStageClear()) {
		s.stageClear = true;
		g.NewGame(s);
	}
}
