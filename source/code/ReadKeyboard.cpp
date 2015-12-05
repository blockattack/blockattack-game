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
#include "utf8.h"

using namespace std;

ReadKeyboard::ReadKeyboard(void) {
	maxLength = 16;
	position = text_string.begin();
}

ReadKeyboard::~ReadKeyboard(void) {
}

Uint8 ReadKeyboard::CharsBeforeCursor() {
	return std::distance(text_string.begin(), position);
}

ReadKeyboard::ReadKeyboard(const char* oldName) {
	maxLength = 16;
	text_string = oldName;
	position = text_string.end();
}


void ReadKeyboard::putchar(char thing) {
	if (text_string.length() < maxLength) {
		text_string.insert(position, thing);
		utf8::advance(position, 1, text_string.end());
	}
}


void ReadKeyboard::removeChar() {
	if (position < text_string.end()) {
		string::iterator endChar= position;
		utf8::advance(endChar, 1, text_string.end());
		text_string.erase(position, endChar);
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
		if ((text_string.length()>0)&& (position<text_string.end())) {
			ReadKeyboard::removeChar();
		}
		return true;
	}
	if (keyPressed == SDLK_BACKSPACE) {
		if (position>text_string.begin()) {
			utf8::prior(position, text_string.begin());
			ReadKeyboard::removeChar();
			return true;
		}
		return false;
	}
	if (keyPressed == SDLK_HOME) {
		position = text_string.begin();
		return true;
	}
	if (keyPressed == SDLK_END) {
		position=text_string.end();
		return true;
	}
	if ((keyPressed == SDLK_LEFT) && (position>text_string.begin())) {
		utf8::prior(position, text_string.begin());
		return true;
	}
	if ((keyPressed == SDLK_RIGHT) && (position<text_string.end())) {
		utf8::next(position, text_string.end());
		return true;
	}
	return true;
}

const char* ReadKeyboard::GetString() {
	return text_string.c_str();
}
