/*
Block Attack - Rise of the Blocks, SDL game, besed on Nintendo's Tetris Attack
Copyright (C) 2008 Poul Sander

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
    R�vehjvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
    http://blockattack.sf.net
*/

#include "ttfont.h"

TTFont::TTFont()
{
    font = NULL;
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

    
    if(f == NULL)
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
    font = NULL;
    
    count--;
    if(count==0)
        TTF_Quit();
}

//Copy constructor, you cannot copy an actual instance
TTFont::TTFont(const TTFont &t)
{
    if(t.font == NULL || t.actualInstance)
    {
        font = NULL;
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
    if(TTF_SizeText(font,text.c_str(),&width,NULL)!=0)
        cout << "Failed to get text width!" << endl;
    return width;
}

void TTFont::writeText(string text, SDL_Surface *target, int x, int y)
{
    SDL_Surface *text_surface;
    SDL_Color color={255,255,255};
    if(!(text_surface=TTF_RenderText_Solid(font,text.c_str(), color))) 
    {
        cout << "Error writing text: " << TTF_GetError() << endl;
    }
    else
    {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        SDL_BlitSurface(text_surface,NULL,target,&dest);
        SDL_FreeSurface(text_surface);
    }
}

