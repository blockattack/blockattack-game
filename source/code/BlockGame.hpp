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

#ifndef BLOCKGAME_HPP
#define BLOCKGAME_HPP 1

#include "stats.h"
#include "common.h"

#define NUMBEROFCHAINS 100
#define BLOCKWAIT 100000
#define BLOCKHANG 1000

enum stageButton {SBdontShow, SBstageClear, SBpuzzleMode};

extern stageButton stageButtonStatus;

//This is the size of the blocks. They are always 50. The internal logic calculates it that way
const int bsize = 50;

/**
 * This struct defines the start conditions of the game
 */
struct BlockGameStartInfo {
	unsigned int ticks = 0;
	bool timeTrial = false;
	///True means a stage clear game will be started. level must be set too.
	bool stageClear = false;
	///True if puzzle mode. level must be set too.
	bool puzzleMode = false;
	///Single puzzle is used for the editor only.
	bool singlePuzzle = false;
	int level = 0;
	bool AI = false;
	/**
	 * True means that stats will be recorded.
	 * If AI is true then this will be overruled to false 
	 */
	bool recordStats = true;
	bool vsMode = false;
	/**
	 * Set to true if we are fighting an AI. level should be the AI level we are fighting
	 */
	bool vsAI = false;
	int startBlocks = -1;
	int handicap = 0;
	int gameSpeed = 0;
};

struct GarbageStruct {
	bool greyGarbage = false;
	int width = 0;
	int height = 0;
	void setGarbage(int w, int h, bool g = false) {
		greyGarbage = g;
		width = w;
		height = h;
	}
};

struct BlockGameAction {
	enum class Action {NONE, UPDATE, SET_DRAW, SET_WON, SET_GAME_OVER, MOVE, SWITCH, PUSH};
	Action action = Action::NONE;
	unsigned int tick = 0;
	int value1 = 0;
};

////////////////////////////////////////////////////////////////////////////////
//The BloackGame class represents a board, score, time etc. for a single player/
////////////////////////////////////////////////////////////////////////////////
class BlockGame
{
private:
	int prevTowerHeight = 0;
	bool bGarbageFallLeft = false;
	bool singlePuzzle = false;

	int nextGarbageNumber = 0;
	int pushedPixelAt = 0;
	int nrPushedPixel = 0;
	int nrFellDown = 0;
	unsigned int nrStops = 0;
	bool garbageToBeCleared[7][30];
	unsigned int lastAImove = 0;

	int AI_LineOffset = 0; //how many lines have changed since command
	int hangTicks = 0;    //How many times have hang been decreased?
	//int the two following index 0 may NOT be used (what the fuck did I meen?)
	int chainSize[NUMBEROFCHAINS]{}; //Contains the chains
	bool chainUsed[NUMBEROFCHAINS]{};   //True if the chain is used

	unsigned int nextRandomNumber = 0;
	int Level = 0; //Only used in stageClear and puzzle (not implemented)

	int rand2();
	int firstUnusedChain();

//public:
protected:
	int lastCounter = 0;
	std::string strHolder;
	bool bDraw = false;
	unsigned int ticks = 0;
	unsigned int gameStartedAt = 0;
	unsigned int gameEndedAfter = 0;		//How long did the game last?
	int linesCleared = 0;
	int TowerHeight = 0;
	int board[7][30];
	int stop = 0;
	int speedLevel = 0;
	int pixels = 0;
	int MovesLeft = 0;
	bool timetrial = false;
	bool stageClear = false;
	bool vsMode = false;
	bool puzzleMode = false;
	int stageClearLimit = 0; //stores number of lines user must clear to win
	int combo = 0;
	int chain = 0;
	int cursorx = 2; //stores cursor position
	int cursory = 3; // -||-
	int mouse_cursorx = -1;  //Stores the mouse hold cursor. -1 if nothing is selected.
	int mouse_cursory = -1;
	double speed = 0.0;
	double baseSpeed = 0.0; //factor for speed. Lower value = faster gameplay
	int score = 0;
	bool bGameOver = false;
	bool hasWonTheGame = false;
	int AI_MoveSpeed = 0;   //How often will the computer move? milliseconds
	bool AI_Enabled = false;
	bool recordStats = true;
	bool vsAI = false;  //Set to true for single player vs

	int handicap = 0;
	
	std::vector<GarbageStruct> garbageSendQueue;

	int AIlineToClear = 0;

	short AIstatus = 0;   //Status flags:
	//0: nothing, 2: clear tower, 3: clear horisontal, 4: clear vertical
	//1: make more lines, 5: make 2 lines, 6: make 1 line

public:

	std::string name;

public:
	BlockGame();
	virtual ~BlockGame() = default;

	int getAIlevel()  const;
	
	virtual void AddText(int x, int y, const std::string& text, int time) const  {}
	virtual void AddBall(int x, int y, bool right, int color) const  {}
	virtual void AddExplosion(int x, int y) const  {}
	virtual void PlayerWonEvent() const  {}
	virtual void DrawEvent() const {}
	virtual void BlockPopEvent() const  {}
	virtual void LongChainDoneEvent() const  {}
	virtual void TimeTrialEndEvent() const  {}
	virtual void EndlessHighscoreEvent() const  {}
	
	void DoAction (const BlockGameAction& action);
	
	int GetScore() const;
	int GetHandicap() const;
	bool isGameOver() const;
	int GetGameStartedAt() const;
	int GetGameEndedAt() const;
	bool isTimeTrial() const;
	bool isStageClear() const;
	bool isVsMode() const;
	bool isPuzzleMode() const;
	int GetLinesCleared() const;
	int GetStageClearLimit() const;
	int GetChains() const;
	int GetPixels() const;
	int GetSpeedLevel() const;
	int GetTowerHeight() const;
	int GetCursorX() const;
	int GetCursorY() const;
	void MoveCursorTo(int x, int y);
	void GetMouseCursor(bool& pressed, int& x, int&y) const;
	void MouseDown(int x, int y);  //Send then the mouse is pressed 
	void MouseMove(int x);  //Send then the mouse moves
	void MouseUp();  //Send then the mouse goes up
	bool GetIsWinner() const;
    bool isSinglePuzzle() const;
    int getLevel() const;
	bool GetAIenabled() const;
	bool IsNearDeath() const;
	void NewGame(const BlockGameStartInfo &s);
	//Creates garbage using a given wide and height
	bool CreateGarbage(int wide, int height);
	//Creates garbage using a given wide and height
	bool CreateGreyGarbage();
	void PopSendGarbage(std::vector<GarbageStruct>& poppedData);
private:
	void NewGame(unsigned int ticks);
	//Test if LineNr is an empty line, returns false otherwise.
	bool LineEmpty(int lineNr) const;
	//Test if the entire board is empty (used for Puzzles)
	bool BoardEmpty() const;
	//Anything that the user can't move? In that case Game Over cannot occur
	bool hasStaticContent() const;
	void putStartBlocks();
	void putStartBlocks(int n);
	//decreases hang for all hanging blocks and wait for waiting blocks
	void ReduceStuff();
	void setGameSpeed(int globalSpeedLevel);
	void setHandicap(int globalHandicap);
	//Clears garbage, must take one the lower left corner!
	int GarbageClearer(int x, int y, int number, bool aLineToClear, int chain);
	//Marks garbage that must be cleared
	int GarbageMarker(int x, int y);
	int FirstGarbageMarker(int x, int y);
	//Clear Blocks if 3 or more is alligned (naive implemented)
	void ClearBlocks();
	//Moves all peaces a spot down if possible
	int FallBlock(int x, int y, int number);
	//Makes all Garbage fall one spot
	void GarbageFall();
	//Makes the blocks fall (it doesn't test time, this must be done before hand)
	void FallDown();
	//Pushes a single pixel, so it appears to scrool
	void PushPixels();
	//See how high the tower is, saved in integer TowerHeight
	void FindTowerHeight();
	//Generates a new line and moves the field one block up (restart puzzle mode)
	void PushLine();
	//prints "Game Over" and ends game
	void SetGameOver();
	//Moves the cursor, receaves N,S,E or W as a char an moves as desired
	void MoveCursor(char way);
	//switches the two blocks at the cursor position, unless game over
	void SwitchAtCursor();
///////////////////////////////////////////////////////////////////////////
/////////////////////////// AI starts here! ///////////////////////////////
///////////////////////////////////////////////////////////////////////////
	//First the helpet functions:
	int nrOfType(int line, int type);
	int AIcolorToClear = 0;
	//See if a combo can be made in this line
	int horiInLine(int line);
	bool horiClearPossible();
	//the Line Has Unmoveable Objects witch might stall the AI
	bool lineHasGarbage(int line);
	//Types 0..6 in line
	int nrOfRealTypes(int line);
	//See if there is a tower
	bool ThereIsATower();
	double firstInLine1(int line);
	//returns the first coordinate of the block of type
	double firstInLine(int line, int type);
	//There in the line shall we move
	int closestTo(int line, int place);
	//The AI will remove a tower
	void AI_ClearTower();
	//The AI will try to clear block horisontally
	void AI_ClearHori();
	//Test if vertical clear is possible
	bool veriClearPossible();
	//There in the line shall we move
	int closestTo(int line, int type, int place);
	//The AI will try to clear blocks vertically
	void AI_ClearVertical();
	bool firstLineCreated = 0;
	void AI_Move();
//////////////////////////////////////////////////////////////////
///////////////////////////// AI ends here! //////////////////////
//////////////////////////////////////////////////////////////////
	//Set the move speed of the AI based on the aiLevel parameter
	void setAIlevel(int aiLevel);
	void PushLineInternal();
	//Prints "winner" and ends game
	void setPlayerWon();
	//Prints "draw" and ends the game
	void setDraw();
	//Updates evrything, if not called nothing happends
	void Update();
	void UpdateInternal(unsigned int newtick);
};

//Play the next level
void nextLevel(BlockGame& g, unsigned int ticks);
//Replay the current level
void retryLevel(BlockGame& g, unsigned int ticks);

#endif
