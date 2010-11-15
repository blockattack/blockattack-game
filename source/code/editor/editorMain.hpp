/*
editorMain.hpp
Copyright (C) 2007 Poul Sander

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
    R�veh�jvej 36, V. 1111                                                    
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com       
*/

#include "SDL.h" 
#include "EditorInterface.hpp"
#include <vector>

//Do to stability reasons we load everything in the main and just refer to it here:
extern SDL_Surface *bCreateFile;
extern SDL_Surface *bDeletePuzzle;
extern SDL_Surface *bLoadFile;
extern SDL_Surface *bMoveBack;
extern SDL_Surface *bMoveDown;
extern SDL_Surface *bMoveForward;
extern SDL_Surface *bMoveLeft;
extern SDL_Surface *bMoveRight;
extern SDL_Surface *bMoveUp;
extern SDL_Surface *bNewPuzzle;
extern SDL_Surface *bSaveFileAs;
extern SDL_Surface *bSavePuzzle;
extern SDL_Surface *bSaveToFile;
extern SDL_Surface *bTestPuzzle;

//The screen we draw on:
extern SDL_Surface *screen;

extern EditorInterface ei;

class aButton
{
    public:
        int x,y,width,height;
        SDL_Surface *label;
    
    aButton()
    {
    }
    
    aButton(int x,int y,SDL_Surface *ss)
    {
        this->x = x;
        this->y = y;
        this->label = ss;
        width = this->label->w;
        height = this->label->h;
    }
    
    void draw();
    
    //Function to check if the button is clicked.
    bool clicked(int x, int y)
    {
        if((x>=this->x)&&(x<=this->x+width)&&(y>=this->y)&&(y<=this->y+height))
            return true;
        else
            return false;
    }
    
    void click(int x, int y);
};

const int numberOfButtons = 14;

//The editor itself...
class theEditor
{
      private:
      vector<aButton> buttons;	
      
      
      public:
      
      theEditor();
             
      virtual void drawButtons();
      
      virtual void click(int x, int y);
      
};
