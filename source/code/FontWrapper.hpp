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


#ifndef FONTWRAPPER_HPP
#define FONTWRAPPER_HPP

#include "Libs/NFont.h"
#include "sago/SagoDataHolder.hpp"

class FontWrapper {
	NFont theFont;
	
public:
	void draw(SDL_Renderer* target, int x, int y, const std::string& text) {
		theFont.draw(target, x, y, "%s", text.c_str());
	}
	
	void drawCenter2(SDL_Renderer* target, int x, int y, const std::string& text) {
		theFont.draw(target, x, y, NFont::CENTER, "%s", text.c_str());
	}
	
	void drawCenterAlsoHeight(SDL_Renderer* target, int x, int y, const std::string& text) {
		theFont.draw(target, x, y-theFont.getHeight()/2, NFont::CENTER, "%s", text.c_str());
	}
	
	void drawRight(SDL_Renderer* target, int x, int y, const std::string& text) {
		theFont.draw(target, x, y, NFont::RIGHT, "%s", text.c_str());
	}
	
	bool load(SDL_Renderer* target, TTF_Font* font, const NFont::Color& color) {
		return theFont.load(target, font, color);
	}
};

#endif /* FONTWRAPPER_HPP */

