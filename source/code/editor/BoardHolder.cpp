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
    Raevehjvej 36, V. 1111                                                    
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com 
    http://blockattack.sf.net     
*/

#include "BoardHolder.hpp"

BoardHolder::BoardHolder(string filename)
{
    fstream inFile(filename.c_str(),ios::in);
    if(inFile)
    {
        int nrOfBoards;
        inFile >> nrOfBoards;
        for(int i=0;i<nrOfBoards;i++)
			{
				TheBoard ourBoard;
				int temp;
				inFile >> temp;
				ourBoard.setNumberOfMoves(temp);
				for(int j=0;j<12;j++)
				{
					for(int k=0;k<6;k++)
					{
                        inFile >> temp;
						ourBoard.setBrick(k,j,temp);						
					}
				}
				theBoards[i] = ourBoard;
			}
		isNull = false;
    }
}

BoardHolder::BoardHolder()
{
    isNull = false;
}

bool BoardHolder::saveBoards(string filename)
{
     fstream outFile(filename.c_str(),ios::out);
     if(outFile)
     {
         outFile << getNumberOfBoards() << endl;
         for(int i=0;i<getNumberOfBoards();i++)
			{
				outFile << theBoards[i].getNumberOfMoves() << endl;
				for(int j=0;j<12;j++)
				{
					for(int k=0;k<6;k++)
					{
						outFile << theBoards[i].getBrick(k,j);
						if(k<5)
							outFile << " ";
						else
							outFile << endl;
					}
				}
			}
			outFile.close();   
         return true;
     }
     else
     return false;
}

TheBoard BoardHolder::getModel(int nr)
{
    if(theBoards.size()<nr)
    {
        //Problem!
        
    }
    else
    {
        return theBoards[nr];
    }
}

bool BoardHolder::setModel(int nr, TheBoard model)
{
     if((nr>49)||(nr<0))
         return false;
     theBoards[nr] = model;
     return true;
}

int BoardHolder::getNumberOfBoards()
{
    return theBoards.size();
}

bool BoardHolder::addBoard()
{
    if(!(theBoards.size()<49))
			return false;
    TheBoard tb;
		theBoards[theBoards.size()] = tb;
		return true;
}

bool BoardHolder::removeBoard(int nr)
{
    vector<TheBoard>::iterator itr;
    itr = theBoards.begin();
    itr+=nr;
    theBoards.erase(itr);
}

bool BoardHolder::moveBoardBack(int nr)
{
    if(nr<1) //We can't move back
			return false;
		if(nr>getNumberOfBoards())
			return false;
		TheBoard temp = theBoards[nr];
		theBoards[nr] = theBoards[nr-1];
		theBoards[nr-1] = temp;
		return true;
}

bool BoardHolder::moveBoardForward(int nr)
{
    if(nr>getNumberOfBoards()-2) //We can't move forward
			return false;
		if(nr<0)
			return false;
		TheBoard temp = theBoards[nr];
		theBoards[nr] = theBoards[nr+1];
		theBoards[nr+1] = temp;
		return true;
}
