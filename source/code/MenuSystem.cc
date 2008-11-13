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

#include "MenuSystem.h"
#include "common.h"

extern SDL_Surface *mouse;
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

SDL_Surface* ButtonGfx::marked;
SDL_Surface* ButtonGfx::unmarked;
int ButtonGfx::xsize;
int ButtonGfx::ysize;
TTFont* ButtonGfx::ttf;

void ButtonGfx::setSurfaces(SDL_Surface **marked,SDL_Surface **unmarked)
{
    ButtonGfx::marked = *marked;
    ButtonGfx::unmarked = *unmarked;
    xsize=(*marked)->w;
    ysize=(*marked)->h;
}

Button::Button()
{
    label = "";
    marked = false;
    surfaceMarked = SDL_ConvertSurface(ButtonGfx::marked, ButtonGfx::marked->format, SDL_SWSURFACE);
    surfaceUnmarked = SDL_ConvertSurface(ButtonGfx::unmarked, ButtonGfx::unmarked->format, SDL_SWSURFACE);
}

Button::~Button()
{
    SDL_FreeSurface(surfaceMarked);
    SDL_FreeSurface(surfaceUnmarked);
}

Button::Button(const Button& b)
{
    label = b.label;
    marked = b.marked;
    surfaceMarked = SDL_ConvertSurface(ButtonGfx::marked, ButtonGfx::marked->format, SDL_SWSURFACE);
    surfaceUnmarked = SDL_ConvertSurface(ButtonGfx::unmarked, ButtonGfx::unmarked->format, SDL_SWSURFACE);
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
    action();
}

void Button::drawTo(SDL_Surface *surface)
{
    #if DEBUG
    cout << "Painting button: " << label << endl;
    #endif
    if (marked)
        DrawIMG(surfaceMarked,surface,x,y);
    else
        DrawIMG(surfaceUnmarked,surface,x,y);
    //int stringx = x + (ButtonGfx::xsize)/2 - ButtonGfx::ttf->getTextWidth(label)/2; 
    //int stringy = y + (ButtonGfx::ysize)/2 - ButtonGfx::ttf->getTextHeight()/2;
    //ButtonGfx::ttf->writeText(label,surface,stringx,stringy);
}

void Menu::drawSelf()
{
    DrawIMG(backgroundImage,screen,0,0);
    vector<Button>::iterator it;
    for(it = buttons.begin();it < buttons.end(); it++)
        (*it).drawTo(screen);
    exit.drawTo(screen);
    DrawIMG(mouse,screen,mousex,mousey);
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
        nextY += 50;
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
            }
            
            SDL_GetMouseState(&mousex,&mousey);

        }
        
        drawSelf();
        SDL_Flip(screen);
    }
}
