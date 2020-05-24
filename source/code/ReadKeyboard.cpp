/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2018 Poul Sander

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
https://blockattack.net
===========================================================================
*/

#include "ReadKeyboard.h"
#include "utf8.h"
#include <iostream>

ReadKeyboard::ReadKeyboard(void) {
	maxLength = 16;
	position = text_string.begin();
}

ReadKeyboard::~ReadKeyboard(void) {
}

int ReadKeyboard::CharsBeforeCursor() {
	return std::distance(text_string.begin(), position);
}

ReadKeyboard::ReadKeyboard(const char* oldName) : maxLength(16), text_string(oldName) {
	position = text_string.end();
}


void ReadKeyboard::putchar(const std::string& thing) {
	if ( (int)text_string.length() < maxLength) {
		int oldPostition = utf8::distance(text_string.begin(), position);
		int lengthOfInsertString = utf8::distance(thing.begin(), thing.end());
		text_string.insert(position, thing.begin(), thing.end());
		position = text_string.begin();  //Inserting may destroy our old iterator
		utf8::advance(position, oldPostition + lengthOfInsertString, text_string.end());
	}
}


void ReadKeyboard::removeChar() {
	if (position < text_string.end()) {
		std::string::iterator endChar= position;
		utf8::advance(endChar, 1, text_string.end());
		text_string.erase(position, endChar);
	}
}

bool ReadKeyboard::ReadKey(const SDL_Event& key) {
	if (key.type == SDL_TEXTINPUT) {
		putchar(key.text.text);
		if (key.text.text[0] != 0) {
			return true;
		}
	}
	return ReadKey(key.key.keysym.sym);
}

bool ReadKeyboard::cursorLeft() {
	if (position>text_string.begin()) {
		utf8::prior(position, text_string.begin());
		return true;
	}
	return false;
}
bool ReadKeyboard::cursorRight() {
	if (position<text_string.end()) {
		utf8::next(position, text_string.end());
		return true;
	}
	return false;
}

bool ReadKeyboard::emulateBackspace() {
	if (position>text_string.begin()) {
		utf8::prior(position, text_string.begin());
		ReadKeyboard::removeChar();
		return true;
	}
	return false;
}

bool ReadKeyboard::ReadKey(SDL_Keycode keyPressed) {
	if (keyPressed == SDLK_DELETE) {
		if ((text_string.length()>0)&& (position<text_string.end())) {
			ReadKeyboard::removeChar();
		}
		return true;
	}
	if (keyPressed == SDLK_BACKSPACE) {
		return emulateBackspace();
	}
	if (keyPressed == SDLK_HOME) {
		position = text_string.begin();
		return true;
	}
	if (keyPressed == SDLK_END) {
		position=text_string.end();
		return true;
	}
	if (keyPressed == SDLK_LEFT) {
		cursorLeft();
		return true;
	}
	if ((keyPressed == SDLK_RIGHT)) {
		cursorRight();
		return true;
	}
	return true;
}

const std::string& ReadKeyboard::GetString() const {
	return text_string;
}
