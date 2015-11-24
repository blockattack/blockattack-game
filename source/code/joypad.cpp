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

#include "joypad.h"

bool Joypad_init() {
	if (0==SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
		return true;
	}
	else {
		return false;
	}
}

Joypad_status Joypad_getStatus(SDL_Joystick* joystick) {
	Joypad_status status;
	for (int i=0; i<NRofPADS; i++) {
		//cout << SDL_JoystickNumAxes(joystick) << endl;
		if (i*2>=SDL_JoystickNumAxes(joystick)) {
			status.padDown[i]=false;
			status.padUp[i] = false;
			status.padLeft[i] = false;
			status.padRight[i] = false;
		}
		else {
			//cout << SDL_JoystickGetAxis(joystick,i*2+1)<< endl;
			if (SDL_JoystickGetAxis(joystick,i*2)<(-8000)) {
				status.padLeft[i]=1;
			}
			else {
				status.padLeft[i]=0;
			}
			if (SDL_JoystickGetAxis(joystick,i*2)>(8000)) {
				status.padRight[i]=1;
			}
			else {
				status.padRight[i]=0;
			}
			if (SDL_JoystickGetAxis(joystick,i*2+1)<(-8000)) {
				status.padUp[i]=1;
			}
			else {
				status.padUp[i]=0;
			}
			if (SDL_JoystickGetAxis(joystick,i*2+1)>(8000)) {
				status.padDown[i]=1;
			}
			else {
				status.padDown[i]=0;
			}
		}
	}//NRofPADS
	for (int i=0; i<NRofBUTTONS; i++) {
		if (i>=SDL_JoystickNumButtons(joystick)) {
			status.button[i]=false;
		}
		else if (1==SDL_JoystickGetButton(joystick,i)) {
			status.button[i] = true;
		}
		else {
			status.button[i] = false;
		}
	}
	return status;
}

Joypad::Joypad() {
	up=false;
	down=false;
	left=false;
	right=false;
	but1=false;
	but2=false;
	upREL=true;
	downREL=true;
	leftREL=true;
	rightREL=true;
	but1REL=true;
	but2REL=true;
	int joynum = 0;
	while ((SDL_JoystickOpened(joynum))&&(joynum<Joypad_number)) {
		joynum++;
	}
	if (joynum>=Joypad_number) {
		working = false;
	}
	else {
		joystick=SDL_JoystickOpen(joynum);
		if (joystick==nullptr) {
			working =false;
		}
		else {
			working=true;
		}
	}
}

Joypad::~Joypad() {
	if (working) {
		SDL_JoystickClose(joystick);
	}
}

void Joypad::update() {
	SDL_JoystickUpdate();
	Joypad_status status = Joypad_getStatus(joystick);
	if ((upREL)&&((status.padUp[0])||(status.padUp[1])||(status.padUp[2])||(status.padUp[3]))) {
		up=true;
		upREL=false;
	}
	else {
		up=false;
	}
	if ((downREL)&&((status.padDown[0])||(status.padDown[1])||(status.padDown[2])||(status.padDown[3]))) {
		down=true;
		downREL=false;
	}
	else {
		down=false;
	}
	if ((leftREL)&&((status.padLeft[0])||(status.padLeft[1])||(status.padLeft[2])||(status.padLeft[3]))) {
		left=true;
		leftREL=false;
	}
	else {
		left=false;
	}
	if ((rightREL)&&((status.padRight[0])||(status.padRight[1])||(status.padRight[2])||(status.padRight[3]))) {
		right=true;
		rightREL=false;
	}
	else {
		right=false;
	}
	if ((but1REL)&&((status.button[0])||(status.button[2])||(status.button[4])||(status.button[6]))) {
		but1=true;
		but1REL=false;
	}
	else {
		but1=false;
	}
	if ((but2REL)&&((status.button[1])||(status.button[3])||(status.button[5])||(status.button[7]))) {
		but2=true;
		but2REL=false;
	}
	else {
		but2=false;
	}
	//Now testing for up
	if (!((status.padUp[0])||(status.padUp[1])||(status.padUp[2])||(status.padUp[3]))) {
		upREL = true;
	}
	if (!((status.padDown[0])||(status.padDown[1])||(status.padDown[2])||(status.padDown[3]))) {
		downREL = true;
	}
	if (!((status.padLeft[0])||(status.padLeft[1])||(status.padLeft[2])||(status.padLeft[3]))) {
		leftREL = true;
	}
	if (!((status.padRight[0])||(status.padRight[1])||(status.padRight[2])||(status.padRight[3]))) {
		rightREL= true;
	}
	if (!((status.button[0])||(status.button[2])||(status.button[4])||(status.button[6]))) {
		but1REL = true;
	}
	if (!((status.button[1])||(status.button[3])||(status.button[5])||(status.button[7]))) {
		but2REL = true;
	}
}
