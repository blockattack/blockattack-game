/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2017 Poul Sander

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

#ifndef TEXTMANAGER_HPP
#define TEXTMANAGER_HPP


class TextMessage {
private:
	int x = 0;
	int y = 0;
	string textt;
	unsigned long int time = 0;
	unsigned long int placeTime = 0; //Then the text was placed
public:

	TextMessage() {
	}

	//constructor:
	TextMessage(int X, int Y,const char* Text,unsigned int Time) {
		placeTime = currentTime;
		x = X;
		y = Y;
		textt = Text;
		time = Time;
	}  //constructor

	//true if the text has expired
	bool removeMe() {
		return currentTime-placeTime>time;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	const char* getText() {
		return textt.c_str();
	}
};  //text popup

class TextManager {
	static const int maxNumberOfTexts = 6*12*2*2;
public:
	TextMessage textArray[maxNumberOfTexts];
	bool textUsed[maxNumberOfTexts];

	TextManager() {
		for (int i=0; i<maxNumberOfTexts; i++) {
			textUsed[i] = false;
		}
	}

	int addText(int x, int y,string Text,unsigned int Time) {
		int textNumber = 0;
		while (textNumber<maxNumberOfTexts && textUsed[textNumber]) {
			textNumber++;
		}
		if (textNumber==maxNumberOfTexts) {
			return -1;
		}
		currentTime = SDL_GetTicks();
		textArray[textNumber] = TextMessage(x,y,Text.c_str(),Time);
		textUsed[textNumber] = true;
		return 1;
	}  //addText

	void update() {
		currentTime = SDL_GetTicks();
		for (int i = 0; i<maxNumberOfTexts; i++) {

			if (textUsed[i]) {
				if (textArray[i].removeMe()) {
					textUsed[i] = false;
				}
			}
		}
	} //update


}; //textManager

#endif /* TEXTMANAGER_HPP */

