/*
 * BlockGame.hpp (this was cut from main.cpp for the overview)
 * Copyright (C) 2005 Poul Sander
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Poul Sander
 * R�veh�jvej 36, V. 1111
 * 2800 Kgs. Lyngby
 * DENMARK
 * blockattack@poulsander.com
 */

#ifndef BLOCKGAME_HPP
#define BLOCKGAME_HPP 1

#include "stats.h"
#include "common.h"
#include "SDL.h"
#include "replay.h"


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

////////////////////////////////////////////////////////////////////////////////
//The BloackGame class represents a board, score, time etc. for a single player/
////////////////////////////////////////////////////////////////////////////////
class BlockGame {
private:
    int prevTowerHeight;
    bool bGarbageFallLeft;

    Uint32 nextGarbageNumber;
    Uint32 pushedPixelAt;
    Uint32 nrPushedPixel, nrFellDown, nrStops;
    bool garbageToBeCleared[7][30];
    Uint32 lastAImove;

    Sint16 AI_LineOffset; //how many lines have changed since command
    Uint32 hangTicks;    //How many times have hang been decreased?
    //int the two following index 0 may NOT be used (what the fuck did I meen?)
    Uint8 chainSize[NUMBEROFCHAINS]; //Contains the chains
    bool chainUsed[NUMBEROFCHAINS];   //True if the chain is used

    Uint32 nextRandomNumber;

    Uint16 rand2();
    int firstUnusedChain();

//public:
protected:
    int lastCounter;
    string strHolder;
    bool bDraw;
    bool bReplaying; //true if we are watching a replay
    #if NETWORK
    bool bNetworkPlayer; //must recieve packages from the net
    bool bDisconnected; //The player has disconnected
    #endif
    unsigned int ticks;
    Uint32 gameStartedAt;
    Uint32 gameEndedAfter;		//How long did the game last?
    int linesCleared;
    int TowerHeight;
    BlockGame *garbageTarget;
    Sint32 board[7][30];
    int stop;
    int speedLevel;
    int pixels;
    int MovesLeft;
    bool timetrial, stageClear, vsMode, puzzleMode;
    int Level; //Only used in stageClear and puzzle (not implemented)
    int stageClearLimit; //stores number of lines user must clear to win
    int topx, topy;
    int combo;
    int chain;
    int cursorx; //stores cursor position
    int cursory; // -||-
    double speed, baseSpeed; //factor for speed. Lower value = faster gameplay
    Uint32 score;
    bool bGameOver;
    bool hasWonTheGame;
    int AI_MoveSpeed;   //How often will the computer move? milliseconds
    bool AI_Enabled;

    Uint32 handicap;


    int AIlineToClear;

    short AIstatus;   //Status flags:
    //0: nothing, 2: clear tower, 3: clear horisontal, 4: clear vertical
    //1: make more lines, 5: make 2 lines, 6: make 1 line

public:
    
    char name[30];
    Replay theReplay;   //Stores the replay

    //Constructor
    BlockGame();

    //Deconstructor, never really used... game used to crash when called, cause of the way sBoard was created
    //It should work now and can be used if we want to assign more players in network games that we need to free later
    ~BlockGame();

    void setGameSpeed(Uint8 globalSpeedLevel);
    void setHandicap(Uint8 globalHandicap);
    //Set the move speed of the AI based on the aiLevel parameter
    //Also enables AI
    void setAIlevel(Uint8 aiLevel);
    Uint8 getAIlevel();

    int GetScore();
    int GetHandicap();
    bool isGameOver();
    int GetTopX();
    int GetTopY();
    unsigned int GetGameStartedAt();
    unsigned int GetGameEndedAt();
    bool isTimeTrial();
    bool isStageClear();
    bool isVsMode();
    bool isPuzzleMode();
    int GetLinesCleared();
    int GetStageClearLimit();
    int GetChains();
    int GetPixels();
    int GetSpeedLevel();
    int GetTowerHeight();
    int GetCursorX();
    int GetCursorY();
    void MoveCursorTo(int x, int y);
    bool GetIsWinner();
private:
#if NETWORK
#define garbageStackSize 10
    Uint8 garbageStack[garbageStackSize][3]; //A garbage stack with space for 10 garbage blocks. 0=x,1=y,2=type
    int garbageStackUsed;

    void emptyGarbageStack();
    bool pushGarbage(Uint8 width, Uint8 height, Uint8 type);
public:
    bool popGarbage(Uint8 *width, Uint8 *height, Uint8 *type);
private:

#endif


public:
    //Instead of creating new object new game is called, to prevent memory leaks
    void NewGame(int tx, int ty, unsigned int ticks);
    void NewTimeTrialGame(int x,int y, unsigned int ticks);
    //Starts a new stage game, takes level as input!
    void NewStageGame(int level, int tx, int ty,unsigned int ticks);
    void NewPuzzleGame(int level, int tx, int ty, unsigned int ticks);
    //Replay the current level
    void retryLevel(unsigned int ticks);
    //Play the next level
    void nextLevel(unsigned int ticks);
    //Starts new Vs Game (two Player)
    void NewVsGame(int tx, int ty, BlockGame *target,unsigned int ticks);
    //Starts new Vs Game (two Player)
    void NewVsGame(int tx, int ty, BlockGame *target, bool AI,unsigned int ticks);
private:
    //Go in Demonstration mode, no movement
    void Demonstration(bool toggle);
public:
    //We want to play the replay (must have been loaded beforehand)
    void playReplay(int tx, int ty, unsigned int ticks);
#if NETWORK
    //network play
    void playNetwork(int tx, int ty,unsigned int ticks);
#endif
    //Prints "winner" and ends game
    void setPlayerWon();
    //void SetGameOver();

    #if NETWORK
    //Sets disconnected:
    void setDisconnect();
    #endif
    //Prints "draw" and ends the game
    void setDraw();
    //Function to get a boardpackage
    boardPackage getPackage();
    //Takes a package and sets the board like it
    void setBoard(boardPackage bp);
private:
    //Test if LineNr is an empty line, returns false otherwise.
    bool LineEmpty(int lineNr);
    //Test if the entire board is empty (used for Puzzles)
    bool BoardEmpty();
    //Anything that the user can't move? In that case Game Over cannot occur
    bool hasStaticContent();
    void putStartBlocks();
public:
    void putStartBlocks(Uint32 n);
private:
    //decreases hang for all hanging blocks and wait for waiting blocks
    void ReduceStuff();
public:
    //Creates garbage using a given wide and height
    bool CreateGarbage(int wide, int height);
    //Creates garbage using a given wide and height
    bool CreateGreyGarbage();
private:
    //Clears garbage, must take one the lower left corner!
    int GarbageClearer(int x, int y, int number, bool aLineToClear, int chain);
    //Marks garbage that must be cleared
    int GarbageMarker(int x, int y);
    int FirstGarbageMarker(int x, int y);
    //Clear Blocks if 3 or more is alligned (naive implemented)
    void ClearBlocks();
public:
    //prints "Game Over" and ends game
    void SetGameOver();
    bool GetAIenabled();
private:
    //Moves all peaces a spot down if possible
    int FallBlock(int x, int y, int number);
    //Makes all Garbage fall one spot
    void GarbageFall();
    //Makes the blocks fall (it doesn't test time, this must be done before hand)
    void FallDown();
public:
    //Moves the cursor, receaves N,S,E or W as a char an moves as desired
    void MoveCursor(char way);
    //switches the two blocks at the cursor position, unless game over
    void SwitchAtCursor();
    //Generates a new line and moves the field one block up (restart puzzle mode)
    void PushLine();
private:
    //Pushes a single pixel, so it appears to scrool
    void PushPixels();
    //See how high the tower is, saved in integer TowerHeight
    void FindTowerHeight();
///////////////////////////////////////////////////////////////////////////
/////////////////////////// AI starts here! ///////////////////////////////
///////////////////////////////////////////////////////////////////////////
    //First the helpet functions:
    int nrOfType(int line, int type);
    int AIcolorToClear;
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
    bool firstLineCreated;
    void AI_Move();
//////////////////////////////////////////////////////////////////////////
///////////////////////////// AI ends here! //////////////////////////////
//////////////////////////////////////////////////////////////////////////

private:

    //Updates evrything, if not called nothing happends
    void Update();
public:
    void Update(int newtick);
};
////////////////////////////////////////////////////////////////////////////////
///////////////////////// BlockAttack class end ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif
