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

#include "TheBoard.hpp"
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>

#ifndef BOARDHOLDERDEFINED
#define BOARDHOLDERDEFINED
class BoardHolder
{
      
      private: 
      vector<TheBoard> theBoards;
      
      public:
      bool isNull;
      BoardHolder();
      BoardHolder(string filename);
      bool saveBoards(string filename);
      TheBoard getModel(int);
      bool setModel(int nr,TheBoard model);
      int getNumberOfBoards();
      bool addBoard();
      bool removeBoard(int);
      bool moveBoardBack(int);
      bool moveBoardForward(int);
};
#endif
