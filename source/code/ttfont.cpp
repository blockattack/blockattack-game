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

#include "ttfont.h"

//extern SDL_Surface *tmp;

//#define CONVERTA(n) tmp = SDL_DisplayFormatAlpha(n); SDL_FreeSurface(n); n = tmp

TTFont::TTFont()
{
	font = nullptr;
	actualInstance = false;
}

int TTFont::count = 0;

TTFont::TTFont(TTF_Font *f)
{
	if(!(TTF_WasInit()))
	{
		//Init TTF for the first time
		TTF_Init();
	}


	if(f == nullptr)
		cout << "Font was null!" << endl;

	actualInstance = false; //We have not yet copied to final location
	font = f;

}

TTFont::~TTFont()
{
	if(!actualInstance)
		return;

	cout << "Closing a font" << endl;

	TTF_CloseFont(font);
	font = nullptr;

	count--;
	if(count==0)
		TTF_Quit();
}

//Copy constructor, you cannot copy an actual instance
TTFont::TTFont(const TTFont &t)
{
	if(t.font == nullptr || t.actualInstance)
	{
		font = nullptr;
		actualInstance = false;
	}
	else
	{
		font = t.font;
		actualInstance = true;
		count++;
	}
}

int TTFont::getTextHeight()
{
	return TTF_FontHeight(font);
}

int TTFont::getTextWidth(string text)
{
	int width = 0;
	if(TTF_SizeText(font,text.c_str(),&width,nullptr)!=0)
		cout << "Failed to get text width!" << endl;
	return width;
}

void TTFont::writeText(string text, SDL_Surface *target, int x, int y)
{
	SDL_Surface *text_surface;
	SDL_Color color= {255,255,255};
	if(!(text_surface=TTF_RenderText_Solid(font,text.c_str(), color)))
	{
		cout << "Error writing text: " << TTF_GetError() << endl;
	}
	else
	{
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		SDL_BlitSurface(text_surface,nullptr,target,&dest);
		SDL_FreeSurface(text_surface);
	}
}

