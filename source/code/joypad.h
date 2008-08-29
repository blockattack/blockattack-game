/*
joypad.h - Enables joypad support
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
    Rævehøjvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
*/

//headerfile joypad.h

#include <SDL.h>
#include <stdlib.h>
#include <iostream>

#define NRofPADS 4
#define NRofBUTTONS 12

using namespace std;

struct Joypad_status{
    bool padLeft[NRofPADS];
    bool padRight[NRofPADS];
    bool padUp[NRofPADS];
    bool padDown[NRofPADS];
    bool button[NRofBUTTONS];
};

//Contains the init code
bool Joypad_init();

//How many joypads are availble?
#define Joypad_number SDL_NumJoysticks()

//Open a joystick
#define Joypad_open(X) SDL_JoystickOpen(X)

//Returns the status of the joypad
Joypad_status Joypad_getStatus(SDL_Joystick);

class Joypad{
private:
    SDL_Joystick *joystick;
    static int Joy_count;
public:
    bool up,down,left,right,but1,but2;
    bool upREL,downREL,leftREL,rightREL,but1REL,but2REL;
    bool working;

    Joypad();
    ~Joypad();

    void update();
};
