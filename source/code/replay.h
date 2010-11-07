/*
replay.h
Copyright (C) 2005 Poul Sander

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

//headerfile for replay.h
/*
replay is used to save a replay, there is saved 10 moves per second, should be
able to give a realistic replay
*/

#ifndef REPLAY_H
#define REPLAY_H 1

//constants - 3000 is 5 minutes
#define FRAMESPERSEC 10

#include "SDL.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

//board_package, stores a board can be used for network play and replay
struct boardPackage //92 bytes
{
    Uint32 time; //game time
    Uint8 brick[6][13];
    Uint8 pixels; //pixels pushed
    Uint8 cursorX; //Cursor coordinate
    Uint8 cursorY; // -||-
    Uint32 score;
    Uint8 speed;
    Uint8 chain;
    Uint8 result; //0=none,1=gameOver,2=winner,4=draw
};

class Replay{
private:
//Our replay is stored in an array of TOTALFRAMES length
    //boardPackage bps[TOTALFRAMES];
    vector<boardPackage> bps;
//The final package is not set to any specific time
    boardPackage finalPack;
//We store number of frames, so we know how long to read the array
    Uint32 nrOfFrames;
//An enumerator, so we know how it ends! (should be removed then boardPackage is the 92 byte version)
    enum { gameOver=0, winner, looser, draw } theResult;
//If we are loaded from a file, then we are read only
    bool isLoaded;


public:
    
    //Store player name
    char name[30];

    Replay(); //Constructor
    Replay(const Replay& r); //Copy constructor
    Uint32 getNumberOfFrames(); //Returns number of frames
    void setFrameSecTo(Uint32,boardPackage); //Sets frame at a given time to the package
    void setFinalFrame(boardPackage,int); //Sets the final package
    boardPackage getFrameSec(Uint32);  //Gets a frame to a time
    boardPackage getFinalFrame(); //Gets the last frame, that must remain
    int getFinalStatus();	//Return the result: winner, looser, draw or gameOver
    bool isFinnished(Uint32); //Returns true if we are done

//Ok, I'll ignore this for some time, and just save to file, if however, we ever use a dynamic saving structure, we are fucked
    bool saveReplay(string);	//Saves a replay
    bool saveReplay(string,Replay p2); //saves a replay, plus another replay given as a parameter
    bool loadReplay(string); //laods a replay
    bool loadReplay2(string); //loads the second part of the replay file, if it exists, returns false otherwise


};

#endif
