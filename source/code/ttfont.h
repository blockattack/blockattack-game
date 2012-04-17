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

//
// File:   ttfont.hpp.h
// Author: poul
//
// Created on 25. december 2007, 13:52
//

#ifndef _TTFONT_H
#define	_TTFONT_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include "SDL.h"
//#include "SDL_ttf.h"        //To use True Type Fonts in SDL

using namespace std;

class TTFont
{
private:
	static int count; //The total number of instances
	TTF_Font *font;
	bool actualInstance;

public:
	TTFont();
	TTFont(TTF_Font *f);
	TTFont(const TTFont &t);
	~TTFont();

	int getTextWidth(string text);
	int getTextHeight();
	void writeText(string text, SDL_Surface *target, int x, int y);

};

#endif	/* _TTFONT_H */

