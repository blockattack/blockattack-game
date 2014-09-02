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

#include "stats.h"
#include "common.h"
#include "mainVars.hpp"


////////////////////////////////////////////////////////////////////////////////
//The BloackGame class represents a board, score, time etc. for a single player/
////////////////////////////////////////////////////////////////////////////////
class BlockGame {
private:
    int prevTowerHeight;
    bool bGarbageFallLeft;

    bool bDraw;
    bool bReplaying; //true if we are watching a replay
    #if NETWORK
    bool bNetworkPlayer; //must recieve packages from the net
    bool bDisconnected; //The player has disconnected
    #endif
    Uint32 nextGarbageNumber;
    Uint32 pushedPixelAt;
    Uint32 nrPushedPixel, nrFellDown, nrStops;
    bool garbageToBeCleared[7][30];
    Uint32 lastAImove;

    Sint16 AI_LineOffset; //how many lines have changed since command
    string strHolder;
    Uint32 hangTicks;    //How many times have hang been decreased?
    //int the two following index 0 may NOT be used (what the fuck did I meen?)
    Uint8 chainSize[NUMBEROFCHAINS]; //Contains the chains
    bool chainUsed[NUMBEROFCHAINS];   //True if the chain is used

    Uint32 nextRandomNumber;

    Uint16 rand2() {
        nextRandomNumber = nextRandomNumber*1103515245 + 12345;
        return ((Uint16)(nextRandomNumber/65536)) % 32768;
    }

    int firstUnusedChain() {
        bool found=false;
        int i = 0;
        while (!found) {
            if (!chainUsed[++i])
                found=true;
            if (i>NUMBEROFCHAINS-2)
                found=true;
        }
        return i;
    }

public:
    char name[30];
    Uint32 gameStartedAt;
    Uint32 gameEndedAfter;		//How long did the game last?
    int linesCleared;
    #if WITH_SDL
    SDL_Surface* sBoard;
    #endif
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
    Replay theReplay;   //Stores the replay

    Uint32 handicap;

    //Constructor
    BlockGame(int tx, int ty) {
        #if WITH_SDL
        tmp = IMG_Load2((char*)"gfx/BackBoard.png");
        sBoard = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
        #endif
        srand((int)time(NULL));
        nrFellDown = 0;
        nrPushedPixel = 0;
        garbageTarget = this;
        nrStops=0;
        topx = tx;
        topy = ty;
        cursorx = 2;
        cursory = 3;
        stop = 0;
        pixels = 0;
        score = 0;
        bGameOver = false;
        bDraw = false;
        bReplaying=false; //No replay by default
        #if NETWORK
        bDisconnected=false;
        bNetworkPlayer = false;
        #endif
        timetrial = false;
        stageClear = false;
        vsMode = false;
        puzzleMode = false;
        linesCleared = 0;
        AI_Enabled = false;
        AI_MoveSpeed=100;
        hasWonTheGame = false;
        combo=0;                      //counts
        chain=0;
        hangTicks = 0;
        baseSpeed = 0.5;           //All other speeds are relative to this
        speed = baseSpeed;
        speedLevel = 1;
        gameStartedAt = SDL_GetTicks();
        gameEndedAfter = 0;
        pushedPixelAt = gameStartedAt;
        nextGarbageNumber = 10;
        handicap=0;
        for (int i=0;i<7;i++)
            for (int j=0;j<30;j++) {
                board[i][j] = -1;
            }
        for (int i=0;i<NUMBEROFCHAINS;i++) {
            chainUsed[i]=false;
            chainSize[i] = 0;
        }
        theReplay = Replay();
        showGame = true;              //The game is now active
        lastCounter = -1;           //To prevent the final chunk to be played when stating the program
    }	//Constructor

    //Deconstructor, never really used... game used to crash when called, cause of the way sBoard was created
    //It should work now and can be used if we want to assign more players in network games that we need to free later
    ~BlockGame() {
        #if WITH_SDL
        SDL_FreeSurface(sBoard);
        #endif
    }

    void setGameSpeed(Uint8 globalSpeedLevel) {
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

    void setHandicap(Uint8 globalHandicap) {
        handicap=1000*((Uint32)globalHandicap);
    }

    //Set the move speed of the AI based on the aiLevel parameter
    void setAIlevel(Uint8 aiLevel) {
        AI_MoveSpeed=120-(20*(aiLevel-3));
    };
    
    Uint8 getAIlevel() {
        return (120-AI_MoveSpeed)/20+3;
    }

#if NETWORK
#define garbageStackSize 10
    Uint8 garbageStack[garbageStackSize][3]; //A garbage stack with space for 10 garbage blocks. 0=x,1=y,2=type
    int garbageStackUsed;

    void emptyGarbageStack() {
        for (int i=0;i<10;i++)
            for (int j=0;j<3;j++)
                garbageStack[i][j] = 0;
        garbageStackUsed = 0;
    }

    bool pushGarbage(Uint8 width, Uint8 height, Uint8 type) {
        if (garbageStackUsed>=garbageStackSize)
            return false;
        garbageStack[garbageStackUsed][0]=width;
        garbageStack[garbageStackUsed][1]=height;
        garbageStack[garbageStackUsed][2]=type;
        garbageStackUsed++;
        return true;
    }

    bool popGarbage(Uint8 *width, Uint8 *height, Uint8 *type) {
        if (garbageStackUsed<1)
            return false;
        garbageStackUsed--;
        *width=garbageStack[garbageStackUsed][0];
        *height=garbageStack[garbageStackUsed][1];
        *type=garbageStack[garbageStackUsed][2];
        return true;
    }

#endif

    #if WITH_SDL
    //Loads BackBoard again if surface format has changed
    void convertSurface() {
        SDL_FreeSurface(sBoard);
        sBoard = SDL_DisplayFormat(backBoard);
    }
    #endif

    //Instead of creating new object new game is called, to prevent memory leaks
    void NewGame(int tx, int ty) {
        stageButtonStatus = SBdontShow;
        bReplaying  =  false;
        #if NETWORK
        bNetworkPlayer=false;
        bDisconnected =false;
        #endif
        nrFellDown = 0;
        lastNrOfPlayers = 1; //At least one player :-)
        nrPushedPixel = 0;
        nrStops = 0;
        topx = tx;
        topy = ty;
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
        gameStartedAt = SDL_GetTicks()+3000;
        pushedPixelAt = gameStartedAt;
        nextGarbageNumber = 10;
        handicap=0;
        for (int i=0;i<7;i++)
            for (int j=0;j<30;j++) {
                board[i][j] = -1;
            }
        for (int i=0;i<NUMBEROFCHAINS;i++) {
            chainUsed[i]=false;
            chainSize[i] = 0;
        }
        lastAImove = SDL_GetTicks()+3000;
        showGame = true;
        theReplay = Replay();
    }	//NewGame

    //Starts a new stage game, takes level as input!
    void NewStageGame(int level, int tx, int ty) {
        if (level > -1) {
            NewGame(tx, ty);
            stageClear = true;
            Level = level;
            Stats::getInstance()->addOne("PlayedStageLevel"+itoa2(level));
            stageClearLimit = 30+(Level%6)*10;
            baseSpeed = 0.5/((double)(Level*0.5)+1.0);
            speed = baseSpeed;
        }
    }

    void NewPuzzleGame(int level, int tx, int ty) {
        if (level>-1) {
            NewGame(tx, ty);
            puzzleMode = true;
            Level = level;
            MovesLeft = nrOfMovesAllowed[Level];
            for (int i=0;i<6;i++)
                for (int j=0;j<12;j++) {
                    board[i][j+1] = puzzleLevels[Level][i][j];
                }
            baseSpeed = 100000;
            speed = 100000;

            //Now push the blines up
            for (int i=19;i>0;i--)
                for (int j=0;j<6;j++) {
                    board[j][i] = board[j][i-1];
                }
            for (int j=0;j<6;j++) {
                board[j][0] = rand() % 6;
                if (j > 0) {
                    if (board[j][0] == board[j-1][0])
                        board[j][0] = rand() % 6;
                }
                if (board[j][0] == board[j][1])
                    board[j][0] = 6;
                if (board[j][0] == board[j][1])
                    board[j][0] = 6;

            }
        }
    }
    
    //Replay the current level
    void retryLevel()
    {
        if(puzzleMode)
            NewPuzzleGame(Level,topx,topy);
        else
            if(stageClear)
                NewStageGame(Level,topx,topy);
    }
    
    //Play the next level
    void nextLevel()
    {
        if(puzzleMode)
        {
            if(Level<nrOfPuzzles-1)
                NewPuzzleGame(Level+1,topx,topy);
        }
        else
            if(stageClear)
            {
                if(Level<50-1)
                    NewStageGame(Level+1,topx,topy);
            }
    }

    //Starts new Vs Game (two Player)
    void NewVsGame(int tx, int ty, BlockGame *target) {
        NewGame(tx, ty);
        vsMode = true;
        putStartBlocks();
        garbageTarget = target;
        Stats::getInstance()->addOne("VSgamesStarted");
    }

    //Starts new Vs Game (two Player)
    void NewVsGame(int tx, int ty, BlockGame *target, bool AI) {
        NewGame(tx, ty);
        vsMode = true;
        AI_Enabled = AI;
        if(!AI)
            Stats::getInstance()->addOne("VSgamesStarted");
        else
            strcpy(name,"CPU\0");
        putStartBlocks();
        garbageTarget = target;
    }

    //Go in Demonstration mode, no movement
    void Demonstration(bool toggle) {
        speed=0;
        baseSpeed = 0;
    }

    //We want to play the replay (must have been loaded beforehand)
    void playReplay(int tx, int ty) {
        NewGame(tx, ty);
        gameStartedAt = SDL_GetTicks();
        bReplaying = true; //We are playing, no calculations
        #if NETWORK
        bNetworkPlayer = false; //Take input from replay file
        #endif
    }

#if NETWORK
    //network play
    void playNetwork(int tx, int ty) {
        NewGame(tx, ty);
        gameStartedAt = SDL_GetTicks();
        bReplaying = false; //We are playing, no calculations
        bNetworkPlayer = true; //Don't Take input from replay file
        emptyGarbageStack();
    }
#endif

    //Prints "winner" and ends game
    void setPlayerWon() {
        if (!bGameOver)
        {
            gameEndedAfter = SDL_GetTicks()-gameStartedAt; //We game ends now!
	    if(!AI_Enabled && !bReplaying)
		{
			TimeHandler::addTime("playTime",TimeHandler::ms2ct(gameEndedAfter));
		}
        }
        theReplay.setFinalFrame(getPackage(), 1);
        bGameOver = true;
        hasWonTheGame = true;
        showGame = false;
        if (SoundEnabled)Mix_PlayChannel(1, applause, 0);
        if(!AI_Enabled && !bReplaying)
        {
            Stats::getInstance()->addOne("totalWins");
            if(garbageTarget->AI_Enabled && !(garbageTarget->bReplaying))
            {
                //We have defeated an AI
                Stats::getInstance()->addOne("defeatedAI"+itoa(garbageTarget->getAIlevel()));
            }
        }
        if(AI_Enabled && !(garbageTarget->AI_Enabled) && garbageTarget->bReplaying==false)
        {
            //The AI have defeated a human player
            Stats::getInstance()->addOne("defeatedByAI"+itoa(getAIlevel()));
        }
    }

    //void SetGameOver();

    #if NETWORK
    //Sets disconnected:
    void setDisconnect() {
        bDisconnected = true;
        SetGameOver();
    }
    #endif
    
    //Prints "draw" and ends the game
    void setDraw() {
        bGameOver = true;
	if(!AI_Enabled && !bReplaying)
	{
		TimeHandler::addTime("playTime",TimeHandler::ms2ct(gameEndedAfter));
	}
        theReplay.setFinalFrame(getPackage(), 3);
        hasWonTheGame = false;
        bDraw = true;
        showGame = false;
        Mix_HaltChannel(1);
        if(!AI_Enabled && !bReplaying)
            Stats::getInstance()->addOne("totalDraws");
    }

    //Function to get a boardpackage
    boardPackage getPackage() {
        boardPackage bp;
        if(SDL_GetTicks()<gameStartedAt)
            bp.time = 0;
        else
            bp.time = (Uint32)(SDL_GetTicks()-gameStartedAt);
        for (int i=0;i<6;i++)
            for (int j=0;j<13;j++) {
                if (board[i][j]%10<7)
                    bp.brick[i][j]=board[i][j]%10+1;
                if ((board[i][j]/1000000)%10==1)
                {
                    //Ordinary garbage
                    int aInt=0; //N=1, S=2, W=4, E=8
                    if (i==0)
                        aInt+=4;
                    else
                        if ((board[i-1][j])!=(board[i][j]))
                            aInt+=4;
                    if (i==5)
                        aInt+=8;
                    else
                        if ((board[i+1][j])!=(board[i][j]))
                            aInt+=8;
                    if (j==0)
                        aInt+=2;
                    else
                        if ((board[i][j-1])!=(board[i][j]))
                            aInt+=2;
                    if ((board[i][j+1])!=(board[i][j]))
                        aInt+=1;
                    switch (aInt) {
                    case 8:
                        bp.brick[i][j]=8;
                        break;
                    case 7:
                        bp.brick[i][j]=10;
                        break;
                    case 11:
                        bp.brick[i][j]=11;
                        break;
                    case 9:
                        bp.brick[i][j]=12;
                        break;
                    case 5:
                        bp.brick[i][j]=13;
                        break;
                    case 6:
                        bp.brick[i][j]=14;
                        break;
                    case 10:
                        bp.brick[i][j]=15;
                        break;
                    case 3: //M
                        bp.brick[i][j]=16;
                        break;
                    case 0:
                        bp.brick[i][j]=17;
                        break;
                    case 1:
                        bp.brick[i][j]=18;
                        break;
                    case 2:
                        bp.brick[i][j]=19;
                        break;
                    case 4:
                        bp.brick[i][j]=20;
                        break;
                    default:
                        bp.brick[i][j]=0;
                    };
                    //cout << "garbage added to replay: " << aInt << endl;
                }
                if ((board[i][j]/1000000)%10==2)
                {
                    //Gray garbage
                    bp.brick[i][j]=21;
                }
                if ((board[i][j]/BLOCKHANG)%10==1) //If "get ready"
                    bp.brick[i][j]+=30;
            }
        bp.cursorX = (Uint8)cursorx;
        bp.cursorY = (Uint8)cursory;
        bp.score = (Uint32)score;
        bp.pixels = (Uint8)pixels;
        bp.chain = (Uint8)chain;
        bp.speed = (Uint8)speedLevel;
        Uint8 result = 0;
        if (bGameOver)
            result+=1;
        if (hasWonTheGame)
            result+=2;
        if (bDraw)
            result+=4;
        bp.result = (Uint8)result;
        return bp;
    }

    //Takes a package and sets the board like it
    void setBoard(boardPackage bp) {
        //gameStartedAt = SDL_GetTicks()-bp.time;
        for (int i=0;i<6;i++)
            for (int j=0;j<13;j++) {
                //if(bp.brick[i][j]/8==0)
                board[i][j]=bp.brick[i][j]-1;
                //else
                //	board[i][j]=-1;
            }
        cursorx=bp.cursorX;
        cursory=bp.cursorY;
        score=bp.score;
        pixels=bp.pixels;
        chain=bp.chain;
        speedLevel=bp.speed;
        Uint8 result = bp.result;
        bGameOver = false; //Make sure that it is initialized!
        if(result%2==1) {
            bGameOver=true;
            result-=1;
        }
        if (result%4==2) {
            hasWonTheGame = true;
            result-=2;
        }
        if (result==4) {
            bDraw = true;
        }

    }

    //Test if LineNr is an empty line, returns false otherwise.
    bool LineEmpty(int lineNr) {
        bool empty = true;
        for (int i = 0; i <7; i++)
            if (board[i][lineNr] != -1)
                empty = false;
        return empty;
    }

    //Test if the entire board is empty (used for Puzzles)
    bool BoardEmpty() {
        bool empty = true;
        for (int i=0;i<6;i++)
            for (int j=1;j<13;j++)
                if (board[i][j] != -1)
                    empty = false;
        return empty;
    }

    //Anything that the user can't move? In that case Game Over cannot occur
    bool hasStaticContent() {
        for (int i=0;i<6;i++)
            for (int j=1;j<13;j++)
                if (board[i][j] >= 10000000) 	//Higher than this means combos (garbage is static, but the stack is static but nothing to do about it)
                    return true;			//They are static
        return false;					//Return false if no static object found
    }

    /*
     * Generates some blocks so the user don't see a board without blocks
     */
    //void putStartBlocks(Uint32);

    void putStartBlocks() {
        putStartBlocks(time(0));
    }

    void putStartBlocks(Uint32 n) {
        for (int i=0;i<7;i++)
            for (int j=0;j<30;j++) {
                board[i][j] = -1;
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
    void ReduceStuff() {
        Uint32 howMuchHang = (SDL_GetTicks() - FRAMELENGTH*hangTicks)/FRAMELENGTH;
        if (howMuchHang>0) {
            for (int i=0; i<7; i++)
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
        hangTicks+=howMuchHang;
    }

    //Creates garbage using a given wide and height
    bool CreateGarbage(int wide, int height) {
#if NETWORK
        if (bNetworkPlayer) {
            pushGarbage(wide, height, 0);
        }
        else
#endif
        {
            if (wide>6) wide = 6;
            if (height>12) height = 12;
            int startPosition = 12;
            while ((!(LineEmpty(startPosition))) || (startPosition == 29))
                startPosition++;
            if (startPosition == 29) return false; //failed to place blocks
            if (29-startPosition<height) return false;	//not enough space
            int start, end;
            if (bGarbageFallLeft)
            {
                start=0;
                end=start+wide;
                bGarbageFallLeft = false;
            }
            else
            {
                start=6-wide;
                end = 6;
                bGarbageFallLeft = true;
            }
            for (int i = startPosition; i <startPosition+height; i++)
                for (int j = start; j < end; j++) {
                    board[j][i] = 1000000+nextGarbageNumber;
                }
            nextGarbageNumber++;
            if (nextGarbageNumber>999999) nextGarbageNumber = 10;
            //bGarbageFallLeft = !(bGarbageFallLeft);
            return true;
        }
    }

    //Creates garbage using a given wide and height
    bool CreateGreyGarbage() {
#if NETWORK
        if (bNetworkPlayer) {
            pushGarbage(6, 1, 1);
        }
        else
#endif
        {
            int startPosition = 12;
            while ((!(LineEmpty(startPosition))) || (startPosition == 29))
                startPosition++;
            if (startPosition == 29) return false; //failed to place blocks
            if (29-startPosition<1) return false;	//not enough space
            int start, end;
            {
                start=0;
                end=6;
            }
            for (int i = startPosition; i <startPosition+1; i++)
                for (int j = start; j < end; j++) {
                    board[j][i] = 2*1000000+nextGarbageNumber;
                }
            nextGarbageNumber++;
            if (nextGarbageNumber>999999) nextGarbageNumber = 10;
            return true;
        }
    }


    //Clears garbage, must take one the lower left corner!
    int GarbageClearer(int x, int y, int number, bool aLineToClear, int chain) {
        if ((board[x][y])%1000000 != number) return -1;
        if (aLineToClear) {
            board[x][y] = rand() % 6;
            board[x][y] += 10*HANGTIME+BLOCKHANG+CHAINPLACE*chain;
        }
        garbageToBeCleared[x][y] = false;
        GarbageClearer(x+1, y, number, aLineToClear, chain);
        GarbageClearer(x, y+1, number, false, chain);
        return 1;
    }

    //Marks garbage that must be cleared
    int GarbageMarker(int x, int y) {
        if ((x>6)||(x<0)||(y<0)||(y>29)) return -1;
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

    int FirstGarbageMarker(int x, int y) {
        if ((x>6)||(x<0)||(y<0)||(y>29)) return -1;
        if (((board[x][y])/1000000 == 2)&&(garbageToBeCleared[x][y] == false)) {
            for (int i=0;i<6;i++)
                garbageToBeCleared[i][y] = true;
        }
        else
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

    //Clear Blocks if 3 or more is alligned (naive implemented)
    void ClearBlocks() {

        bool toBeCleared[7][30]; //true if blok must be removed

        int previus=-1; //the last block checked
        int combo=0;
        for (int i=0;i<30;i++)
            for (int j=0;j<7;j++) {
                toBeCleared[j][i] = false;
                garbageToBeCleared[j][i] = false;
            }
        for (int i=0;i<7;i++) {
            bool faaling = false;
            for (int j=0;j<30;j++) {
                if ((faaling)&&(board[i][j]>-1)&&(board[i][j]%10000000<7)) {
                    board[i][j]+=BLOCKFALL;
                }
                if ((!faaling)&&((board[i][j]/BLOCKFALL)%10==1))
                    board[i][j]-=BLOCKFALL;
                if (!((board[i][j]>-1)&&(board[i][j]%10000000<7)))
                    faaling=true;
                if (((board[i][j]/1000000)%10==1)||((board[i][j]/1000000)%10==2)||((board[i][j]/BLOCKHANG)%10==1)||((board[i][j]/BLOCKWAIT)%10==1))
                    faaling = false;
            }
        }


        for (int j=0;j<7;j++) {
            previus = -1;
            combo=0;

            for (int i=1;i<30;i++) {
                if ((board[j][i]>-1)&&(board[j][i]%10000000<7)) {
                    if (board[j][i]%10000000 == previus) {
                        combo++;
                    }
                    else {
                        if (combo>2)
                            for (int k = i-combo; k<i;k++) {
                                toBeCleared[j][k] = true;
                            }
                        combo=1;
                        previus = board[j][i]%10000000;
                    }
                } //if board
                else {
                    if (combo>2)
                        for (int k = i-combo; k<i;k++) {
                            toBeCleared[j][k] = true;
                        }
                    combo = 0;
                    previus = -1;
                }

            } //for i
        } //for j


        chain = 0;
        for (int i=0; i<6;i++)
            for (int j=0; j<30;j++) {
                //Clears blocks marked for clearing
                Sint32 temp=board[i][j];
                if (1==((temp/BLOCKWAIT)%10))
                    if (((temp/10)%100)==0) {
                        if (chainSize[chain]<chainSize[board[i][j]/10000000])
                            chain = board[i][j]/10000000;

                        theBallManeger.addBall(topx+40+i*bsize, topy+bsize*12-j*bsize, true, board[i][j]%10);
                        theBallManeger.addBall(topx+i*bsize, topy+bsize*12-j*bsize, false, board[i][j]%10);
                        theExplosionManeger.addExplosion(topx-10+i*bsize, topy+bsize*12-10-j*bsize);
                        board[i][j]=-2;
                    }
            }
        for (int i=0; i<7;i++) {
            bool setChain=false;
            for (int j=0; j<30;j++) {
                if (board[i][j]==-1)
                    setChain=false;
                if (board[i][j]==-2) {
                    board[i][j]=-1;
                    setChain=true;
                    if (SoundEnabled)Mix_PlayChannel(0, boing, 0);
                }
                if (board[i][j]!=-1)
                    if ((setChain)&&((board[i][j]/GARBAGE)%10!=1)&&((board[i][j]/GARBAGE)%10!=2)) {
                        board[i][j]=((board[i][j]%CHAINPLACE)+CHAINPLACE*chain);
                        //somethingsGottaFall = true;
                    }

            }
        }
        int startvalue;
        if (pixels == 0)
            startvalue=1;
        else
            startvalue=0;
        for (int i=startvalue;i<30;i++) {
            previus=-1;
            combo=0;
            for (int j=0;j<7;j++) {
                if (((board[j][i]>-1)&&(board[j][i]%10000000<7))) {
                    if (board[j][i]%10000000 == previus) {
                        combo++;
                    }
                    else {
                        if (combo>2)
                            for (int k = j-combo; k<j;k++) {
                                toBeCleared[k][i] = true;
                            }
                        combo=1;
                        previus = board[j][i]%10000000;
                    }
                } //if board
                else {
                    if (combo>2)
                        for (int k = j-combo; k<j;k++) {
                            toBeCleared[k][i] = true;
                        }
                    combo = 0;
                    previus = -1;
                }

            } //for j
        } //for i
        bool blockIsFalling[6][30]; //See that is falling
        for (int i=0;i<30;i++)
            for (int j=0;j<6;j++)
                blockIsFalling[j][i] = false;



        combo = 0;
        chain = 0;
        int grey = 0;
        for (int i=0;i<30;i++)
            for (int j=0;j<6;j++)
                if (toBeCleared[j][i]) {
                    //see if any garbage is around:
                    FirstGarbageMarker(j-1, i);
                    FirstGarbageMarker(j+1, i);
                    FirstGarbageMarker(j, i-1);
                    FirstGarbageMarker(j, i+1);
                    //that is checked now :-)
                    if (board[j][i]%10000000==6)
                        grey++;
                    if ((vsMode) && (grey>2) && (board[j][i]%10000000==6))
                        garbageTarget->CreateGreyGarbage();
                    if ((board[j][i]>-1)&&(board[j][i]%10000000<7))
                        board[j][i]+=BLOCKWAIT+10*FALLTIME;

                    if (chainSize[board[j][i]/10000000]>chainSize[chain])
                        chain=board[j][i]/10000000;
                    combo++;
                    stop+=140*combo;
                    score +=10;
                    if (combo>3)
                        score+=3*combo; //More points if more cleared simontanously
                }
        score+=chainSize[chain]*100;
        if (chain==0) {
            chain=firstUnusedChain();
            chainSize[chain]=0;
            chainUsed[chain]=true;
        }
        chainSize[chain]++;
        for (int i=0;i<30;i++)
            for (int j=0;j<6;j++) {
                //if(board[j][i]/10==(BLOCKWAIT+10*FALLTIME)/10)
                if (toBeCleared[j][i]) {
                    board[j][i]=(board[j][i]%10000000)+chain*10000000;
                }
            }

        { //This is here we add text to screen!
            bool dead = false;
            for (int i=29;i>=0;i--)
                for (int j=0;j<6;j++)
                    if (toBeCleared[j][i]) {
                        if (!dead) {
                            dead=true;
                            string tempS = itoa(chainSize[chain]);
                            if (chainSize[chain]>1)
                                theTextManeger.addText(topx-10+j*bsize, topy+12*bsize-i*bsize, tempS, 1000);
                        }
                    }
        } //This was there text was added

        if (vsMode)
            switch (combo) {
            case 0:
            case 1:
            case 2:
            case 3:
                break;
            case 4:
                garbageTarget->CreateGarbage(3, 1);
                break;
            case 5:
                garbageTarget->CreateGarbage(4, 1);
                break;
            case 6:
                garbageTarget->CreateGarbage(5, 1);
                break;
            case 7:
                garbageTarget->CreateGarbage(6, 1);
                break;
            case 8:
                garbageTarget->CreateGarbage(4, 1);
                garbageTarget->CreateGarbage(4, 1);
                break;
            case 9:
                garbageTarget->CreateGarbage(5, 1);
                garbageTarget->CreateGarbage(4, 1);
                break;
            case 10:
                garbageTarget->CreateGarbage(5, 1);
                garbageTarget->CreateGarbage(5, 1);
                break;
            case 11:
                garbageTarget->CreateGarbage(6, 1);
                garbageTarget->CreateGarbage(5, 1);
                break;
            case 12:
                garbageTarget->CreateGarbage(6, 1);
                garbageTarget->CreateGarbage(6, 1);
                break;
            case 13:
                garbageTarget->CreateGarbage(5, 1);
                garbageTarget->CreateGarbage(5, 1);
                garbageTarget->CreateGarbage(4, 1);
                break;
            default:
                garbageTarget->CreateGarbage(5, 1);
                garbageTarget->CreateGarbage(5, 1);
                garbageTarget->CreateGarbage(4, 1);
                break;
            }
        for (int i=0;i<30;i++)
            for (int j=0;j<6;j++)
                if (garbageToBeCleared[j][i]) {
                    GarbageClearer(j, i, board[j][i]%1000000, true, chain); //Clears the blocks and all blocks connected to it.
                }

        chain=0;

        //Break chains (if a block is stable it is resetted to (chain == 0)):
        for (int i=0;i<7;i++) {
            bool faaling = false;  //In the beginning we are NOT falling
            for (int j=0;j<30;j++) {
                if ((faaling)&&(board[i][j]>-1)&&(board[i][j]<7)) {
                    board[i][j]+=BLOCKFALL;
                }
                if ((!faaling)&&((board[i][j]/BLOCKFALL)%10==1))
                    board[i][j]-=BLOCKFALL;
                if ((!faaling)&&(board[i][j]>0)&&(board[i][j]/10000000!=0)&&((board[i][j]/BLOCKWAIT)%10!=1)&&((board[i][j]/BLOCKHANG)%10!=1)) {
                    if (chainSize[board[i][j]/10000000]>chainSize[chain])
                        chain=board[i][j]/10000000;
                    board[i][j]=board[i][j]%10000000;
                }
                if (!((board[i][j]>-1)&&(board[i][j]<7)))
                    faaling=true;
                if (((board[i][j]/1000000)%10==1)||((board[i][j]/BLOCKHANG)%10==1)||((board[i][j]/BLOCKWAIT)%10==1))
                    faaling = false;
            }
        }

        //Create garbage as a result
        //if((vsMode)&&(chainSize[chain]>1)) garbageTarget->CreateGarbage(6,chainSize[chain]-1);

        //Calculate chain
        chain=0;
        for (int i=0; i<6;i++)
            for (int j=0; j<30;j++) {
                if (chainSize[board[i][j]/10000000]>chain)
                    chain=chainSize[board[i][j]/10000000];
            }

        //Make space in table for more things
        if (chain==0)
            for (int i=0;i<NUMBEROFCHAINS;i++)
                if (chainUsed[i]==true) {
                    if ((vsMode)&&(chainSize[i]>1)) garbageTarget->CreateGarbage(6, chainSize[i]-1);
                    if ((SoundEnabled)&&(chainSize[i]>4))Mix_PlayChannel(1, applause, 0);
                    if(chainSize[i]>1 && !puzzleMode && !AI_Enabled)
                        Stats::getInstance()->addOne((string)"chainX"+itoa(chainSize[i]));
                    chainUsed[i]=false;
                }
    } //ClearBlocks

    //prints "Game Over" and ends game
    void SetGameOver() {
        if (!bGameOver) {
        	gameEndedAfter = SDL_GetTicks()-gameStartedAt; //We game ends now!
		if(!AI_Enabled && !bReplaying)
		{
			TimeHandler::addTime("playTime",TimeHandler::ms2ct(gameEndedAfter));
		}
	}
        if(bReplaying)
        {
            bReplaying = false;
        }
        else
        {
            strncpy(theReplay.name,name,30);
        }
        theReplay.setFinalFrame(getPackage(), 0);
        bGameOver = true;
        showGame = false;
        if(stageClear)
            stageButtonStatus = SBstageClear;
    }

    //Moves all peaces a spot down if possible
    int FallBlock(int x, int y, int number) {
        if (y == 0) return -1;
        if (x>0)
            if (board[x-1][y] == number)
                return -1;
        int i=x;
        bool canFall = true;
        //checks a line of a garbage block and see if something is under it
        while ((board[i][y] == number)&&(canFall)&&(i<6)) {
            if (board[i][y-1] != -1) canFall = false;
            i++;
        }
        if (canFall) {
            //cout << "Now falling" << endl;
            for (int j = x;j<i;j++) {
                board[j][y-1] = board[j][y];
                board[j][y] = -1;
            }
        }
        return 0;
    }	//FallBlock


    //Makes all Garbage fall one spot
    void GarbageFall() {
        for (int i=0;i<30;i++)
            for (int j=0;j<7; j++) {
                if ((((board[j][i]/1000000)%10) == 1)||(((board[j][i]/1000000)%10) == 2))
                    FallBlock(j, i, board[j][i]);
            }
    }

    //Makes the blocks fall (it doesn't test time, this must be done before hand)
    void FallDown() {
        bool falling =false;        //nothing is moving unless proven otherwise
        for (int i=0;i<29;i++)
            for (int j=0;j<6;j++) {
                if ((board[j][i]==-1) && (board[j][i+1]!=-1) && (board[j][i+1]%BLOCKFALL<7)) {
                    board[j][i] = board[j][i+1];
                    board[j][i+1] = -1;
                    falling = true;              //something is moving!
                }
                if ((board[j][i]/BLOCKWAIT)%10==1)
                    falling=true;
            }
        if (!falling) //If nothing is falling
        {
            if ((puzzleMode)&&(!bGameOver)&&(MovesLeft==0)&&(!(BoardEmpty()))) {
                //Puzzle not won
                SetGameOver();
                stageButtonStatus = SBpuzzleMode;
            }
        }
        GarbageFall();		//Makes the garbage fall
        nrFellDown++;      //Sets number of this fall, so we know then the next will occur
    }

    //Moves the cursor, receaves N,S,E or W as a char an moves as desired
    void MoveCursor(char way) {
        if (!bGameOver)       //If game over nothing happends
        {
            if ((way == 'N') && ((cursory<10)||(TowerHeight>12) ||(((pixels==bsize)||(pixels==0)) && (cursory<11))))
                cursory++;
            if ((way == 'S') && (cursory>0))
                cursory--;
            if ((way == 'W') && (cursorx>0))
                cursorx--;
            if ((way == 'E') && (cursorx<4))
                cursorx++;
        }
    }

    //switches the two blocks at the cursor position, unless game over
    void SwitchAtCursor() {
        if ((board[cursorx][cursory+1]<7) && (board[cursorx+1][cursory+1]<7) && (!bGameOver) && ((!puzzleMode)||(MovesLeft>0)) && (gameStartedAt<SDL_GetTicks())) {
            int temp = board[cursorx][cursory+1];
            board[cursorx][cursory+1] = board[cursorx+1][cursory+1];
            board[cursorx+1][cursory+1] = temp;
        }
        if ((puzzleMode)&&(gameStartedAt<SDL_GetTicks())&&(MovesLeft>0)) MovesLeft--;
    }

    //Generates a new line and moves the field one block up (restart puzzle mode)
    void PushLine() {
        //If not game over, not high tower and not puzzle mode
        if ((!bGameOver) && TowerHeight<13 && (!puzzleMode) && (gameStartedAt<SDL_GetTicks())&&(chain==0)) {
            for (int i=19;i>0;i--)
                for (int j=0;j<6;j++) {
                    board[j][i] = board[j][i-1];
                }
            for (int j=0;j<6;j++) {
                board[j][0] = rand2() % 4;
                if (j > 0) {
                    if (board[j][0] == board[j-1][0])
                        board[j][0] = rand2() % 6;
                }
                if (board[j][0] == board[j][1])
                    board[j][0] = rand2() % 6;
                if (board[j][0] == board[j][1])
                    board[j][0] = rand2() % 6;
                while ((j>0)&&(board[j][0]==board[j-1][0]))
                    board[j][0] = rand2() % 6;
            }
            score+=1;
            MoveCursor('N');
            if (vsMode) {
                if (rand2()%6==1)
                    board[rand2()%6][0]=6;
            }
            pixels = 0;
            stop=0;
            pushedPixelAt = SDL_GetTicks();
            linesCleared++;
            AI_LineOffset++;
            nrPushedPixel=(int)((double)(pushedPixelAt-gameStartedAt)/(1000.0*speed));
            
            if(!AI_Enabled && !bReplaying)
            {
                Stats::getInstance()->addOne("linesPushed");
            }
        } //if !bGameOver

        //Restart Puzzle mode
        if (puzzleMode && !bGameOver) {
            //Reloads level
            MovesLeft = nrOfMovesAllowed[Level];
            for (int i=0;i<6;i++)
                for (int j=0;j<12;j++) {
                    board[i][j+1] = puzzleLevels[Level][i][j];
                }
            score=0;
            bGameOver=false;
            showGame = true;
        }

        if ((TowerHeight>12) && (!puzzleMode)&&(!bGameOver)&&(chain==0)) {
            if ((!vsMode)&&(theTopScoresEndless.isHighScore(score))&&(!AI_Enabled)) {
                if (SoundEnabled)Mix_PlayChannel(1, applause, 0);
                theTopScoresEndless.addScore(name, score);
                cout << "New high score!" << endl;
            }
            SetGameOver();
        }
        
        
    }//PushLine

    //Pushes a single pixel, so it appears to scrool
    void PushPixels() {
        nrPushedPixel++;
        if ((pixels < bsize) && TowerHeight<13) {
            pixels++;
        }
        else
            PushLine();
        if (pixels>bsize)
            pixels=0;
    }

    //See how high the tower is, saved in integer TowerHeight
    /*void FindTowerHeight()
     * {
     * /*
     * This function needs to be corrected, if an empty line appears towerheight become to low!
     * /
     * bool found = false;
     * bool notNew = false;
     * TowerHeight = 0;
     * for(int i=0; i<19;i++)
     * //while(!found)
     * {
     * found = true;
     * for(int j=0;j<6;j++)
     * if(board[j][i] != -1)
     * found = false;
     * if((!found) && (notNew))
     * notNew =false;
     * if((found)&&(!notNew))
     * {
     * TowerHeight=i;
     * notNew = true;
     * }
     * }
     * TowerHeight--;
     * }*/

    //See how high the tower is, saved in integer TowerHeight
    void FindTowerHeight() {
        /*
         * Old implementation, used until I find the bug in the other.
         * This function has a bug in stage clear! if an empty line appears.
         */
        prevTowerHeight = TowerHeight;
        bool found = false;
        TowerHeight = 0;
        while (!found) {
            found = true;
            for (int j=0;j<6;j++)
                if (board[j][TowerHeight] != -1)
                    found = false;
            TowerHeight++;
        }
        TowerHeight--;
    }

///////////////////////////////////////////////////////////////////////////
/////////////////////////// AI starts here! ///////////////////////////////
///////////////////////////////////////////////////////////////////////////
    //First the helpet functions:
    int nrOfType(int line, int type) {
        // cout << "Start_ nrOfType" << endl;
        int counter = 0;
        for (int i=0; i<6; i++)
            if (board[i][line]==type)counter++;
        return counter;
    }

    int AIcolorToClear;
    int AIlineToClear;

    //See if a combo can be made in this line
    int horiInLine(int line) {
        //cout << "Start_ hori in line" << endl;
        int nrOfType[7] = {0, 0, 0, 0, 0, 0, 0};
        int iTemp;
        int max = 0;
        for (int i=0; i<6; i++) {
            iTemp = board[i][line];
            if ((iTemp>-1)&&(iTemp<7))
                nrOfType[iTemp]++;
        }
        for (int j=0; j<7; j++) {
            if (nrOfType[j]>max) {
                max = nrOfType[j];
                AIcolorToClear = j;
            }
        }
        return max;
    }

    bool horiClearPossible() {
        //cout << "Start_ horiclear possible" << endl;
        int i=13;
        bool solutionFound = false;
        do{
            if (horiInLine(i)>2) {
                AI_LineOffset = 0;
                AIlineToClear = i;
                solutionFound = true;
            }
            i--;
        }while ((!solutionFound)&&(i>0));
        return solutionFound;
    }

    //the Line Has Unmoveable Objects witch might stall the AI
    bool lineHasGarbage(int line) {
        for (int i=0;i<6;i++)
            if (board[i][line]>1000000)
                return true;
        return false;
    }

    //Types 0..6 in line
    inline int nrOfRealTypes(int line) {
        //cout << "Start_ nrOfReal" << endl;
        int counter = 0;
        for (int i=0; i<6; i++)
            if ((board[i][line]>-1)&&(board[i][line]<7))counter++;
        return counter;
    }

    //See if there is a tower
    bool ThereIsATower() {
        //cout << "Start_ there is a tower" << endl;
        bool bThereIsATower = false; //Unless proven otherwise!
        bool topReached = false; //If we have reached the top
        int lineNumber = 0;
        bool emptySpacesFound = false;
        do {
            if ((emptySpacesFound) && (nrOfRealTypes(lineNumber)>0)&&(nrOfType(lineNumber, -1)>0)) {
                AIlineToClear = lineNumber;
                if (lineHasGarbage(lineNumber))
                    return false;
                else
                    bThereIsATower = true;
            }
            else
                emptySpacesFound=false;
            if ((!emptySpacesFound)&&(nrOfType(lineNumber, -1)>0))
                emptySpacesFound = true;
            if (lineNumber<12)
                lineNumber++;
            else
                topReached = true;
        }while ((!bThereIsATower)&&(!topReached));
        //if(bThereIsATower)
        //cout << "There is actually a tower" << endl;
        return bThereIsATower;
    }

    double firstInLine1(int line) {
        for (int i=0;i<6;i++)
            if ((board[i][line]>-1)&&(board[i][line]<7))
                return (double)i;
        return 3.0;
    }

    //returns the first coordinate of the block of type
    double firstInLine(int line, int type) {
        for (int i=0;i<6;i++)
            if (board[i][line]==type)
                return (double)i;
        return 3.0;
    }

    //There in the line shall we move
    int closestTo(int line, int place) {
        if ((int)firstInLine1(line)>place)
            return (int)firstInLine1(line)-1;
        for (int i=place;i>=0;i--) {
            if ((board[i][line]>-1)&&(board[i][line]<7))
                return i;
        }
        AIstatus=0;
        return place;
    }

    //The AI will remove a tower
    inline void AI_ClearTower() {
        //   cout << "AI: ClearTower, line: " << AIlineToClear << endl;
        int place = (int)firstInLine(AIlineToClear-1, -1); //Find an empty field to frop a brick into
        int xplace = closestTo(AIlineToClear, place); //Find the brick to drop in it
        if (cursory+1<AIlineToClear)
            MoveCursor('N');
        else
            if (cursory+1>AIlineToClear)
                MoveCursor('S');
            else
                if (cursorx<xplace)
                    MoveCursor('E');
                else
                    if (cursorx>xplace)
                        MoveCursor('W');
                    else
                        SwitchAtCursor();
        if (!ThereIsATower())
            AIstatus = 0;
    }

    //The AI will try to clear block horisontally
    void AI_ClearHori() {
        //   cout << "AI: ClearHori";
        int lowestLine = AIlineToClear;
        //AIcolorToClear
        bool found =true;
        /*for(int i; (i<12)&&(!found);i++)
         * {
         * if(horiInLine(i)>2)
         * {
         * int lowestLine = i;
         * found = true;
         * }
         * }*/
        for (int i=0;i<7;i++) {
            if (nrOfType(lowestLine, i)>2)
                AIcolorToClear = i;
        }
        if (found) {
            if (cursory>lowestLine-1)
                MoveCursor('S');
            else if (cursory<lowestLine-1)
                MoveCursor('N');
            else if (nrOfType(lowestLine, AIcolorToClear)>2) {
                int left=0, right=0;
                if (board[0][lowestLine]==AIcolorToClear) left++;
                if (board[1][lowestLine]==AIcolorToClear) left++;
                if (board[2][lowestLine]==AIcolorToClear) left++;
                if (board[3][lowestLine]==AIcolorToClear) right++;
                if (board[4][lowestLine]==AIcolorToClear) right++;
                if (board[5][lowestLine]==AIcolorToClear) right++;
                int xplace = 0;
                if (left<right) {
                    //   cout << ", right>left";
                    int count=0;
                    for (int i=0;(i<4)&&(count<1);i++)
                        if ((board[i][lowestLine]==AIcolorToClear)&&((i==0)||(board[i+1][lowestLine]!=AIcolorToClear))) {
                            count++;
                            xplace = i;
                        }
                }
                else {
                    //   cout << ", left>=right";
                    int count=0;
                    for (int i=3;(i<=5)&&(count<1);i++)
                        if ((board[i][lowestLine]==AIcolorToClear)&&(board[i-1][lowestLine]!=AIcolorToClear)) {
                            count++;
                            xplace = --i;
                        }
                }
                //cout << ", xplace: " << xplace;
                if (cursorx<xplace)
                    MoveCursor('E');
                else
                    if (cursorx>xplace)
                        MoveCursor('W');
                    else
                        if (cursorx==xplace)
                            SwitchAtCursor();
                        else
                            AIstatus = 0;
            }
            else
                AIstatus = 0;
        }
        else
            AIstatus = 0;
        //cout << endl; //for debugging
    }

    //Test if vertical clear is possible
    inline bool veriClearPossible() {
        bool found=false;
        int colors[7] = {0, 0, 0, 0, 0, 0, 0};
        for (int i=12;(i>0)&&(!found);i--) {
            for (int j=0;j<7;j++) {
                if (nrOfType(i, j)==0)
                    colors[j]=0;
                else
                    if (++colors[j]>2) {
                        AIcolorToClear = j;
                        AIlineToClear = i;
                        found=true;
                    }

            }
        }
        return found;
    }



    //There in the line shall we move
    int closestTo(int line, int type, int place) {
        if ((int)firstInLine(line, type)>place)
            return (int)firstInLine(line, type)-1;
        for (int i=place;i>=0;i--) {
            if (board[i][line]==type)
                return i;
        }
        AIstatus=0;
        return place;
    }

    //The AI will try to clear blocks vertically
    inline void AI_ClearVertical() {
        // cout << "AI: ClearVeri";
        //First we find the place there we will align the bricks
        int placeToCenter = (int)(firstInLine(AIlineToClear, AIcolorToClear)/3.0+firstInLine(AIlineToClear+1, AIcolorToClear)/3.0+firstInLine(AIlineToClear+2, AIcolorToClear)/3.0);
        int unlimitedLoop=0;
        while (((board[placeToCenter][AIlineToClear]>1000000)||(board[placeToCenter][AIlineToClear+1]>1000000)||(board[placeToCenter][AIlineToClear+2]>1000000))&&(unlimitedLoop<10)) {
            unlimitedLoop++;
            placeToCenter++;
            if (placeToCenter>5)
                placeToCenter=0;
        }
        if(unlimitedLoop>9) {
            AIstatus = 0;
        }
        //cout << ", ptc: " << placeToCenter << ", line: " << AIlineToClear << ", cy: " << cursory;
        if (cursory+1>AIlineToClear+2) {
            // cout << ", cursory>line+2";
            MoveCursor('S');
        }
        if (cursory+1<AIlineToClear)
            MoveCursor('N');
        bool toAlign[3]={true, true, true};
        if (board[placeToCenter][AIlineToClear+0]==AIcolorToClear)
            toAlign[0]=false;
        if (board[placeToCenter][AIlineToClear+1]==AIcolorToClear)
            toAlign[1]=false;
        if (board[placeToCenter][AIlineToClear+2]==AIcolorToClear)
            toAlign[2]=false;
        //cout << "status: " << toAlign[0] << " " << toAlign[1] << " " << toAlign[2];
        if (cursory+1==AIlineToClear) {
            if (toAlign[0]==false)
                MoveCursor('N');
            else {
                if (cursorx>closestTo(AIlineToClear, AIcolorToClear, placeToCenter))
                    MoveCursor('W');
                else
                    if (cursorx<closestTo(AIlineToClear, AIcolorToClear, placeToCenter))
                        MoveCursor('E');
                    else
                        SwitchAtCursor();
            }
        }
        else
            if (cursory+1==AIlineToClear+1) {
                if (toAlign[1]==false) {
                    if (toAlign[2])
                        MoveCursor('N');
                    else
                        MoveCursor('S');
                }
                else {
                    if (cursorx>closestTo(AIlineToClear+1, AIcolorToClear, placeToCenter))
                        MoveCursor('W');
                    else
                        if (cursorx<closestTo(AIlineToClear+1, AIcolorToClear, placeToCenter))
                            MoveCursor('E');
                        else
                            SwitchAtCursor();
                }
            }
            else
                if (cursory+1==AIlineToClear+2) {
                    if (toAlign[2]==false)
                        MoveCursor('S');
                    else {
                        if (cursorx>closestTo(AIlineToClear+2, AIcolorToClear, placeToCenter))
                            MoveCursor('W');
                        else
                            if (cursorx<closestTo(AIlineToClear+2, AIcolorToClear, placeToCenter))
                                MoveCursor('E');
                            else
                                SwitchAtCursor();
                    }
                }

        if ((!toAlign[0])&&(!toAlign[1])&&(!toAlign[2]))
            AIstatus = 0;
        if ((nrOfType(AIlineToClear, AIcolorToClear)==0)||(nrOfType(AIlineToClear+1, AIcolorToClear)==0)||(nrOfType(AIlineToClear+2, AIcolorToClear)==0))
            AIstatus = 0;
        //cout << endl;
    }



    short AIstatus;   //Status flags:
    //0: nothing, 2: clear tower, 3: clear horisontal, 4: clear vertical
    //1: make more lines, 5: make 2 lines, 6: make 1 line
    bool firstLineCreated;

    void AI_Move() {
        switch (AIstatus) {
        case 1:
            if (TowerHeight<8)PushLine();
            else AIstatus = 0;
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
            if (!firstLineCreated){
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
            if (TowerHeight<6) AIstatus = 1;
            else
                if (horiClearPossible()) AIstatus = 3;
                else
                    if (veriClearPossible()) AIstatus = 4;
                    else
                        if (ThereIsATower()) AIstatus = 2;
                        else
                            AIstatus = 5;
            break;
        }
    }

//////////////////////////////////////////////////////////////////////////
///////////////////////////// AI ends here! //////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if WITH_SDL
    //Draws all the bricks to the board (including garbage)
    void PaintBricks() {
        for (int i=0;((i<13)&&(i<30));i++)
            for (int j=0;j<6;j++) {
                if ((board[j][i]%10 != -1) && (board[j][i]%10 < 7) && ((board[j][i]/1000000)%10==0)) {
                    DrawIMG(bricks[board[j][i]%10], sBoard, j*bsize, bsize*12-i*bsize-pixels);
                    if ((board[j][i]/BLOCKWAIT)%10==1)
                        DrawIMG(bomb[(SDL_GetTicks()/BOMBTIME)%2], sBoard, j*bsize, bsize*12-i*bsize-pixels);
                    if ((board[j][i]/BLOCKHANG)%10==1)
                        DrawIMG(ready[(SDL_GetTicks()/READYTIME)%2], sBoard, j*bsize, bsize*12-i*bsize-pixels);

                }
                if ((board[j][i]/1000000)%10==1) {
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
                if ((board[j][i]/1000000)%10==2) {
                    if (j==0)
                        DrawIMG(garbageGML, sBoard, j*bsize, bsize*12-i*bsize-pixels);
                    else
                        if (j==5)
                            DrawIMG(garbageGMR, sBoard, j*bsize, bsize*12-i*bsize-pixels);
                        else
                            DrawIMG(garbageGM, sBoard, j*bsize, bsize*12-i*bsize-pixels);
                }
            }
        const int j = 0;

        int garbageSize=0;
        for (int i=0;i<20;i++) {
            if ((board[j][i]/1000000)%10==1) {
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
                if (((left != number)&&(right == number)&&(over != number)&&(under == number))&&(garbageSize>0)) {
                    DrawIMG(smiley[board[j][i]%4], sBoard, 2*bsize, 12*bsize-i*bsize-pixels+(bsize/2)*garbageSize);
                }
                if (!((left != number)&&(right == number)&&(over == number)&&(under == number))) //not in garbage
                {
                    garbageSize=0;
                }
                else {
                    //cout << "In garbage" << endl;
                    garbageSize++;
                }

            }
        }
        for (int i=0;i<6;i++)
            if (board[i][0]!=-1)
                DrawIMG(transCover, sBoard, i*bsize, 12*bsize-pixels); //Make the appering blocks transperant

    }

    //Paints the bricks gotten from a replay/net package
    void SimplePaintBricks() {
        /*
         * We will need to mark the blocks that must have a bomb, we will here need to see, what is falling
         */
        bool bbomb[6][13]; //has a bomb on it!
        bool falling[6][13]; //this is falling
        bool getReady[6][13]; //getReady
        for (int i=0;i<6;i++)
            for (int j=0;j<13;j++) {
                bbomb[i][j]=false; //All false by default
                falling[i][j]=false;
                if (board[i][j]>29)
                    getReady[i][j]=true;
                else
                    getReady[i][j]=false;
            }
        //See that is falling
        for (int i=0;i<6;i++) {
            bool rowFalling = false;
            for (int j=0;j<13;j++) {
                if (rowFalling)
                    falling[i][j]=true;
                if ((!rowFalling)&&(board[i][j]%30==-1))
                    rowFalling = true;
                if ((rowFalling)&&(board[i][j]%30>6))
                    rowFalling = false;
                if (getReady[i][j]) {
                    falling[i][j]=true; //getReady is the same as falling
                    rowFalling = false;
                }
            }
        }
        //Now looking at rows:
        for (int i=0;i<6;i++) {
            int count = 0;
            int color = -1;
            for (int j=1;j<13;j++) {
                if ((board[i][j]%30==color)&&(!falling[i][j]))
                    count++;
                else
                    if (falling[i][j]) {
                        count = 0;
                    }
                    else {
                        color=board[i][j]%30;
                        count=1;
                    }
                if ((count>2)&&(color>-1)&&(color)<7) {
                    bbomb[i][j]=true;
                    bbomb[i][j-1]=true;
                    bbomb[i][j-2]=true;
                }
            }
        }
        //now looking for lines
        for (int i=1;i<13;i++) {
            int count = 0;
            int color = -1;
            for (int j=0;j<6;j++) {
                if ((board[j][i]%30==color)&&(!falling[j][i]))
                    count++;
                else
                    if (falling[j][i]) {
                        count = 0;
                    }
                    else {
                        color=board[j][i]%30;
                        count=1;
                    }
                if ((count>2)&&(color>-1)&&(color<7)) {
                    bbomb[j][i]=true;
                    bbomb[j-1][i]=true;
                    bbomb[j-2][i]=true;
                }
            }
        }
        for (int i=0;((i<13)&&(i<30));i++)
            for (int j=0;j<6;j++) {
                if ((board[j][i]%10 != -1) && (board[j][i]%30 < 7)) {
                    DrawIMG(bricks[board[j][i]%10], sBoard, j*bsize, 12*bsize-i*bsize-pixels);
                    if (bbomb[j][i])
                        DrawIMG(bomb[(SDL_GetTicks()/BOMBTIME)%2], sBoard, j*bsize, 12*bsize-i*bsize-pixels);
                    if (getReady[j][i])
                        DrawIMG(ready[(SDL_GetTicks()/READYTIME)%2], sBoard, j*bsize, 12*bsize-i*bsize-pixels);
                }
                if (board[j][i]%30>6) {
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
                        switch(j) {
                            case 0:
                                DrawIMG(garbageGML, sBoard,j*bsize, 12*bsize-i*bsize-pixels);
                                break;
                            case 5:
                                DrawIMG(garbageGMR, sBoard,j*bsize, 12*bsize-i*bsize-pixels);
                                break;
                            default:
                                DrawIMG(garbageGM, sBoard,j*bsize, 12*bsize-i*bsize-pixels);
                        }
                    //cout << "IS: " << board[j][i] << endl;
                }


            }

        int garbageSize=0;
        for (int i=0;i<20;i++) {
            if ((board[0][i]%30==12)&&(garbageSize>0)) {
                DrawIMG(smiley[0], sBoard, 2*bsize, bsize-i*bsize-pixels+(bsize/2)*garbageSize);
            }
            if (board[0][i]%30!=19) //not in garbage
            {
                garbageSize=0;
            }
            else {
                //cout << "In garbage" << endl;
                garbageSize++;
            }

        }
    }
    
    int lastCounter;

    //Draws everything
    void DoPaintJob() {
        DrawIMG(backBoard, sBoard, 0, 0);
        #if NETWORK
        if ((!bReplaying)&&(!bNetworkPlayer))
        #else
        if (!bReplaying)
        #endif
            PaintBricks();
        else
            SimplePaintBricks();
        if (stageClear) DrawIMG(blackLine, sBoard, 0, bsize*(12+2)+bsize*(stageClearLimit-linesCleared)-pixels-1);
        if (puzzleMode&&(!bGameOver)) {
            //We need to write nr. of moves left!
            strHolder = "Moves left: " + itoa(MovesLeft);
            SFont_Write(sBoard, fBlueFont, 5, 5, strHolder.c_str());
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
                SFont_Write(sBoard, fBlueFont, 5, 5, strHolder.c_str());
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
                SFont_Write(sBoard, fBlueFont, 5, 5, strHolder.c_str());
            }
        }
        
#if DEBUG
        if (AI_Enabled&&(!bGameOver)) {
            strHolder = "AI_status: " + itoa(AIstatus)+ ", "+ itoa(AIlineToClear);
            SFont_Write(sBoard, fBlueFont, 5, 5, strHolder.c_str());
        }
#endif
        if (!bGameOver)DrawIMG(cursor[(SDL_GetTicks()/600)%2],sBoard,cursorx*bsize-4,11*bsize-cursory*bsize-pixels-4);
        if (SDL_GetTicks()<gameStartedAt)
        {
            int currentCounter = abs((int)SDL_GetTicks()-(int)gameStartedAt)/1000;
            if( (currentCounter!=lastCounter) && (SoundEnabled)&&(!NoSound))
                Mix_PlayChannel(1,counterChunk,0);
            lastCounter = currentCounter;
            switch (currentCounter) {
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
            if(SoundEnabled&&(!NoSound)&&(timetrial)&&(SDL_GetTicks()>gameStartedAt+10000)&&(!bGameOver))
            {
                int currentCounter = (SDL_GetTicks()-(int)gameStartedAt)/1000;
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
            if (hasWonTheGame)DrawIMG(iWinner, sBoard, 0, 5*bsize);
            else if (bDraw) DrawIMG(iDraw, sBoard, 0, 5*bsize);
            else
                DrawIMG(iGameOver, sBoard, 0, 5*bsize);
    }

    #endif

    //Updates evrything, if not called nothing happends
    void Update() {
        Uint32 tempUInt32;
        Uint32 nowTime = SDL_GetTicks(); //We remember the time, so it doesn't change during this call
        if (bReplaying) {
            setBoard(theReplay.getFrameSec((Uint32)(nowTime-gameStartedAt)));
            strncpy(name,theReplay.name,30);
            if (theReplay.isFinnished((Uint32)(nowTime-gameStartedAt)))
                switch (theReplay.getFinalStatus()) {
                case 1: //Winner
                    bGameOver = true;
                    hasWonTheGame = true;
                    break;
                case 2: //Looser
                case 4: //GameOver
                    bGameOver = true;
                    break;
                case 3: //draw
                    bGameOver =true;
                    bDraw = true;
                    break;
                default:
                    bGameOver = true;
                    //Nothing
                    break;
                };
        }
        #if NETWORK
        if ((!bReplaying)&&(!bNetworkPlayer)) {
        #else
            if (!bReplaying) {
        #endif
            FindTowerHeight();
            if ((linesCleared-TowerHeight>stageClearLimit) && (stageClear) && (!bGameOver)) {
                stageCleared[Level] = true;
                if(stageScores[Level]<score)
                {
                    gameEndedAfter = nowTime-gameStartedAt;
                    stageScores[Level] = score;
                    stageTimes[Level] = gameEndedAfter;
                }

                ofstream outfile;
                outfile.open(stageClearSavePath.c_str(), ios::binary |ios::trunc);
                if (!outfile) {
                    cout << "Error writing to file: " << stageClearSavePath << endl;
                }
                else {
                    for (int i=0;i<nrOfStageLevels;i++) {
                        bool tempBool = stageCleared[i];
                        outfile.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
                    }
                    for (int i=0;i<nrOfStageLevels;i++) {
                        tempUInt32 = stageScores[i];
                        outfile.write(reinterpret_cast<char*>(&tempUInt32), sizeof(Uint32));
                    }
                    for (int i=0;i<nrOfStageLevels;i++) {
                        tempUInt32 = stageTimes[i];
                        outfile.write(reinterpret_cast<char*>(&tempUInt32), sizeof(Uint32));
                    }
                    outfile.close();
                }
                setPlayerWon();
                stageButtonStatus = SBstageClear;
            }
            if ((TowerHeight>12)&&(prevTowerHeight<13)&&(!puzzleMode)) {
                //if (SoundEnabled) Mix_PlayChannel(1, heartBeat, 0);
                stop+=1000;
            }

            if ((TowerHeight>12)&&(!puzzleMode)&&(!bGameOver)) {
                bNearDeath = true;
            }
            
            while (nowTime>nrStops*40+gameStartedAt) //Increase stops, till we reach nowTime
            {
                if (stop>0) {
                    stop = stop-20;
                    if (stop<=0) nrPushedPixel=(int)((nowTime-gameStartedAt)/(1000.0*speed));
                }
                if (stop<0)
                    stop = 0;
                nrStops++;
            }
            //If we have static content, we don't raise at all!
            if (hasStaticContent())
                stop++;
            if ((puzzleMode)&&(!bGameOver)&&BoardEmpty()) {
                if (!singlePuzzle) {
                    if(puzzleCleared[Level]==false)
                        Stats::getInstance()->addOne("puzzlesSolved");
                    puzzleCleared[Level] = true;
                    ofstream outfile;
                    stageButtonStatus = SBpuzzleMode;
                    outfile.open(puzzleSavePath.c_str(), ios::binary |ios::trunc);
                    if (!outfile) {
                        cout << "Error writing to file: " << puzzleSavePath << endl;
                    }
                    else {
                        for (int i=0;i<nrOfPuzzles;i++) {
                            bool tempBool = puzzleCleared[i];
                            outfile.write(reinterpret_cast<char*>(&tempBool), sizeof(bool));
                        }
                        outfile.close();
                    }
                }
                setPlayerWon();
            }
            
            //increse speed:
            if ((nowTime>gameStartedAt+20000*speedLevel)&&(speedLevel <99)&&(!bGameOver))
            {
                speed = (baseSpeed*0.9)/((double)speedLevel*0.5);
                speedLevel++;
                nrPushedPixel=(int)((double)(nowTime-gameStartedAt)/(1000.0*speed));
            }
            
            
            //To prevent the stack from raising a lot then we stop a chain (doesn't work anymore)
            if (chain>0)
                stop+=1;
            //Raises the stack
            if ((nowTime>gameStartedAt+nrPushedPixel*1000*speed) && (!bGameOver)&&(!stop))
                while ((nowTime>gameStartedAt+nrPushedPixel*1000*speed)&&(!(puzzleMode)))
                    PushPixels();
            if (!bGameOver)ClearBlocks();
            
            /*************************************************************
            Ai stuff
             **************************************************************/
            if (bGameOver) {
                AIstatus = 0;       //Enusres that AI is resetted
            }
            else
                if (AI_Enabled)
                    if (lastAImove+AI_MoveSpeed<SDL_GetTicks()) {
                        AI_Move();
                        lastAImove=SDL_GetTicks();
                    }

            /*************************************************************
            Ai stuff ended
             **************************************************************/
            if ((nowTime>gameStartedAt+nrFellDown*140) && (!bGameOver)) FallDown();
            if ((nowTime<gameStartedAt)&&(puzzleMode)) {
                FallDown();
                nrFellDown--;
            }
            ReduceStuff();
            if ((timetrial) && (!bGameOver) && (nowTime>gameStartedAt+2*60*1000)) {
                SetGameOver();
                if(!NoSound && SoundEnabled)Mix_PlayChannel(1,counterFinalChunk,0);
                if ((theTopScoresTimeTrial.isHighScore(score))&&(!AI_Enabled)) {
                    theTopScoresTimeTrial.addScore(name, score);
                    //new highscore
                    //Also check if it is better than the best result so far.
                    string checkFilename = getPathToSaveFiles() + "/bestTTresult";
                    ifstream inFile(checkFilename.c_str());
                    Uint32 bestResult = 3000; //Never accept a best result under 3000
                    if(inFile)
                    {
                        inFile >> bestResult;
                        inFile.close();
                    }
                    if(score>bestResult)
                    {
                        string bestFile = getPathToSaveFiles() + "/bestTT";
                        theReplay.saveReplay(bestFile);
                        ofstream outFile(checkFilename.c_str(),ios::trunc);
                        if(outFile)
                            outFile << score;
                    }
                }
            }
        }
        if ((!bGameOver)&&(!bReplaying)&&(nowTime>gameStartedAt))
        {
            //cout << nowTime << " bigger than " << gameStartedAt << endl;
            theReplay.setFrameSecTo(nowTime-gameStartedAt, getPackage());
        }
        DoPaintJob();
    }

}; //class BlockGame
////////////////////////////////////////////////////////////////////////////////
///////////////////////// BlockAttack class end ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
