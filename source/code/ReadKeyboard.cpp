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

#include "ReadKeyboard.h"


using namespace std;

ReadKeyboard::ReadKeyboard(void) {
	length = 0;
	maxLength = 16;
	position = 0;
	strcpy(textstring,"                             ");
}

ReadKeyboard::~ReadKeyboard(void) {
}

Uint8 ReadKeyboard::CharsBeforeCursor() {
	return position;
}

ReadKeyboard::ReadKeyboard(const char* oldName) {
	length = 0;
	maxLength = 16;
	position = 0;
	strcpy(textstring,"                             ");
	strncpy(textstring,oldName,strlen(oldName));
	char charecter = textstring[maxLength+1];
	int i = maxLength+1;
	while ((charecter == ' ') && (i>0)) {
		i--;
		charecter = textstring[i];
	}
	if (i>0) {
		length = i+1;
	}
	else if (charecter == ' ') {
		length = 0;
	}
	else {
		length = 1;
	}
	position = length;

}


void ReadKeyboard::putchar(char thing) {
	if (length < maxLength) {
		for (int i = 28; i>position; i--) {
			textstring[i]=textstring[i-1];
		}
		textstring[position] = thing;
		length++;
		position++;
	}
}


void ReadKeyboard::removeChar() {
	for (int i = position; i<28; i++) {
		textstring[i]=textstring[i+1];
	}
	textstring[28]=' ';
	if (length>0) {
		length--;
	}
}

bool ReadKeyboard::ReadKey(const SDL_keysym& key) {
	if ( 
			(key.unicode >= 'a' && key.unicode <= 'z') ||
			(key.unicode >= '0' && key.unicode <= '9') || 
			(key.unicode >= 'A' && key.unicode <= 'Z')) {
		ReadKeyboard::putchar(key.unicode);
		return true;
	}
	if (key.unicode == '.' || key.unicode == ',') {
		ReadKeyboard::putchar('.');
	}
	return ReadKey(key.sym);
}

bool ReadKeyboard::ReadKey(SDLKey keyPressed) {
	if (keyPressed == SDLK_SPACE) {
		ReadKeyboard::putchar(' ');
		return true;
	}
	if (keyPressed == SDLK_DELETE) {
		if ((length>0)&& (position<length)) {
			ReadKeyboard::removeChar();
		}
		return true;
	}
	if (keyPressed == SDLK_BACKSPACE) {
		if (position>0) {
			position--;
			ReadKeyboard::removeChar();
			return true;
		}
		return false;
	}
	if (keyPressed == SDLK_HOME) {
		position=0;
		return true;
	}
	if (keyPressed == SDLK_END) {
		position=length;
		return true;
	}
	if ((keyPressed == SDLK_LEFT) && (position>0)) {
		position--;
		return true;
	}
	if ((keyPressed == SDLK_RIGHT) && (position<length)) {
		position++;
		return true;
	}
	return true;
}

const char* ReadKeyboard::GetString() {
	textstring[29]='\0';
	return &textstring[0];
}
