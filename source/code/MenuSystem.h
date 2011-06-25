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

// 
// File:   MenuSystem.h
// Author: poul
//
// Created on 28. september 2008, 17:06
//

#ifndef _MENUSYSTEM_H
#define	_MENUSYSTEM_H

#include <string>
#include "SDL.h"
#include <vector>
#include "Libs/NFont.h"
#include "CppSdl/CppSdlImageHolder.hpp"

using namespace std;

//The ButtonGfx object hold common media for all buttons, so we can reskin them by only changeing one pointer
class ButtonGfx {
    public:
        //Holds the graphic for a button that is selected
        static CppSdl::CppSdlImageHolder _marked;
        //Holds the graphic for a button that is not selected
        static CppSdl::CppSdlImageHolder _unmarked;
        //The size of the buttons, so we don't have to ask w and h from the SDL Surfaces each time
        static int xsize;
        static int ysize;
        //A TTFont used for writing the label on the buttons
        static NFont thefont;
        static void setSurfaces(CppSdl::CppSdlImageHolder marked,CppSdl::CppSdlImageHolder unmarked);
};

//A button
class Button {
    private:
        //The label. This is written on the button
        string label;
        //Pointer to a callback function.
        void (*action)(Button *b);
        
    public:
        //Is the button marked?
        bool marked;
        //Where is the button on the screen
        int x;
        int y;
    
        Button();
        Button(const Button& b);
        ~Button();
        
        
        //Set the text to write on the button
        void setLabel(string text);
        //Set the action to run
        void setAction(void (*action2run)(Button*));
        
        bool isClicked(int x,int y); //Returns true if (x,y) is within the borders of the button
        virtual void doAction(); //Run the callback function
        void drawTo(SDL_Surface **surface); //Draws to screen
};


class Menu {
    private:
        vector<Button*> buttons; //Vector holder the buttons
        Button exit; //The exit button is special since it does not have a callback function
        bool isSubmenu; //True if the menu is a submenu
        int marked; //The index of the marked button (for keyboard up/down)
        bool running; //The menu is running. The menu will terminate then this is false
        SDL_Surface *screen; //Pointer to the screen to draw to
//        SDL_Surface *background; //Pointer to the background image
        
        void drawSelf();        //Private function to draw the screen
        void performClick(int x, int y); //Private function to call then a click is detected.
        void placeButtons(); //Rearanges the buttons to the correct place.
    public:
        //numberOfItems is the expected numberOfItems for vector initialization
        //SubMenu is true by default
        Menu(SDL_Surface **screen,bool isSubmenu);
        Menu(SDL_Surface **screen);
        
        //Add a button to the menu
        void addButton(Button *b);
        
        //Run the menu
        void run();
};

#endif	/* _MENUSYSTEM_H */

