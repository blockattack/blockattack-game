/*
Block Attack - Rise of the Blocks, SDL game, besed on Nintendo's Tetris Attack
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
    Rævehjvej 36, V. 1111                                                    
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com 
    http://blockattack.sf.net     
*/

#include "BoardHolder.hpp"

#ifndef EDITORINTERFACEDEFINED
#define EDITORINTERFACEDEFINED
class EditorInterface
{
      private: 
      int colorSelected; //The color the user have selected
      int boardActive;   //Number of the active board, -1 if not active
      TheBoard tb;       //The active board
      BoardHolder bh;    //The Vector that holds the boards
      bool saved;        //The current board is saved
      bool fileSaved;    //The whole file is saved
      string fileName;   //The filename that is open, "" if no filename is given
      
      public:
      EditorInterface();
      void getModel(int);
      bool exists();
      void selectColor(int color);
      int getSelectedColor();
      void drawOnModel(int x, int y);
      void moveLeft();
      void moveRight();
      void deleteBoard();
      void moveBoardBack();
      void moveBoardForward();
      void saveBoard();
      bool saveFile(string);
      bool saveFile();
      void openFile(string);
      void newFile();
      int getColor(int x, int y);
      int getActiveBoardNr();
      int getNumberOfMoves();
      bool newBoard();
      int getNumberOfBoards();
      void setNumberOfMoves(int);
      bool isSaved();
      bool fileIsSaved();
};
#endif
