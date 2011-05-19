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

#include <SDL/SDL_events.h>

#include "MenuSystem.h"
#include "common.h"
#include "CppSdl/CppSdlImageHolder.hpp"

extern CppSdl::CppSdlImageHolder mouse;
extern SDL_Surface *backgroundImage;
extern bool highPriority;
int mousex;
int mousey;

/*Draws a image from on a given Surface. Takes source image, destination surface and coordinates*/
inline void DrawIMG(SDL_Surface *img, SDL_Surface *target, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_BlitSurface(img, NULL, target, &dest);
}

CppSdl::CppSdlImageHolder ButtonGfx::_marked;
CppSdl::CppSdlImageHolder ButtonGfx::_unmarked;
int ButtonGfx::xsize;
int ButtonGfx::ysize;
NFont ButtonGfx::thefont;

void ButtonGfx::setSurfaces(CppSdl::CppSdlImageHolder marked,CppSdl::CppSdlImageHolder unmarked)
{
    ButtonGfx::_marked = marked;
    ButtonGfx::_unmarked = unmarked;
    xsize=(marked).GetWidth();
    ysize=(marked).GetHeight();
    cout << "Surfaces set, size: " <<xsize << " , " << ysize << endl;
}

Button::Button()
{
    label = "";
    marked = false;
    action = NULL;
}

Button::~Button()
{
}

Button::Button(const Button& b)
{
    label = b.label;
    marked = b.marked;
    action = b.action;
}

void Button::setLabel(string text)
{
    label = text;
}

void Button::setAction(void (*action2run)())
{
    action = action2run;
}

bool Button::isClicked(int x,int y)
{
    if ( x >= this->x && y >= this->y && x<= this->x+ButtonGfx::xsize && y <= this->y + ButtonGfx::ysize)
        return true;
    else
        return false;
}

void Button::doAction()
{
    if(action)
        action();
    else
        cout << "Warning: button \"" << label << "\" has no action assigned!";
}

void Button::drawTo(SDL_Surface **surface)
{
    #if DEBUG
    //cout << "Painting button: " << label << " at: " << x << "," << y << endl;
    #endif
    if (marked)
        ButtonGfx::_marked.PaintTo(*surface,x,y);
    else
        ButtonGfx::_unmarked.PaintTo(*surface,x,y);
    //int stringx = x + (ButtonGfx::xsize)/2 - ButtonGfx::ttf->getTextWidth(label)/2; 
    //int stringy = y + (ButtonGfx::ysize)/2 - ButtonGfx::ttf->getTextHeight()/2;
    //ButtonGfx::ttf->writeText(label,surface,stringx,stringy);
    ButtonGfx::thefont.setDest(*surface);
    ButtonGfx::thefont.drawCenter(x+ButtonGfx::xsize/2,y+ButtonGfx::ysize/2-ButtonGfx::thefont.getHeight(label.c_str())/2,label.c_str());
}

void Menu::drawSelf()
{
    DrawIMG(backgroundImage,screen,0,0);
    vector<Button>::iterator it;
    for(it = buttons.begin();it < buttons.end(); it++)
        (*it).drawTo(&screen);
    exit.drawTo(&screen);
    mouse.PaintTo(screen,mousex,mousey);
}

void Menu::performClick(int x,int y)
{
    vector<Button>::iterator it;
    for(it = buttons.begin();it < buttons.end(); it++)
    {
        Button b = (*it);
        if(b.isClicked(x,y))
            b.doAction();
    }
    if(exit.isClicked(x,y))
        running = false;
}

void Menu::placeButtons()
{
    int nextY = 50;
    const int X = 50;
    vector<Button>::iterator it;
    for(it = buttons.begin();it < buttons.end(); it++)
    {
        (*it).x = X;
        (*it).y = nextY;
        nextY += ButtonGfx::ysize+10;
    }
    exit.x = X;
    exit.y = nextY;
}

void Menu::addButton(Button b)
{
    buttons.push_back(b);
    b.marked = false;
    placeButtons();
}

Menu::Menu(SDL_Surface **screen)
{
    this->screen = *screen; 
    buttons = vector<Button>(10);
    isSubmenu = true;
    exit.setLabel("Back");
}

Menu::Menu(SDL_Surface **screen,bool submenu)
{
    this->screen = *screen;
    buttons = vector<Button>(0);
    isSubmenu = submenu;
    if(isSubmenu)
        exit.setLabel("Back");
    else
        exit.setLabel("Exit");
}

void Menu::run()
{
    running = true;
    while(running)
    {
        if (!(highPriority)) SDL_Delay(10);

       
        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )  {
                running = false;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE )
                {
                    running = false;
                }

                if (event.key.keysym.sym == SDLK_UP)
                {
                    marked--;
                    if(marked<0)
                        marked = buttons.size(); //not -1, since exit is after the last element in the list
                }

                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    marked++;
                    if(marked>buttons.size())
                        marked = 0; 
                }

                if(event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER ) {
                    if(marked < buttons.size())
                        buttons.at(marked).doAction();
                    if(marked == buttons.size())
                        running = false;
                }
            }
            

        }

        for(int i=0;i<buttons.size();i++) {
            buttons.at(i).marked = (i == marked);
        }
        exit.marked = (marked == buttons.size());
            SDL_GetMouseState(&mousex,&mousey);
        
        drawSelf();
        SDL_Flip(screen);
    }
}
