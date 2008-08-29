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

TheBoard::TheBoard()
{
    numberOfMoves = 0;
    for(int i = 0; i < BOARDWIDTH; i++)
    for(int j = 0; j < BOARDHEIGHT;j++)
        TheBoard::board[i][j] = -1;
    isNull = false;
}

TheBoard::TheBoard(const TheBoard &tb)
{
    TheBoard::numberOfMoves = tb.numberOfMoves;
    for(int i = 0; i < BOARDWIDTH; i++)
    for(int j = 0; j < BOARDHEIGHT;j++)
        TheBoard::board[i][j] = tb.board[i][j];
    isNull = false;
}

void TheBoard::copyFrom(TheBoard *tb)
{
    TheBoard::numberOfMoves = tb->numberOfMoves;
    for(int i = 0; i < BOARDWIDTH; i++)
    for(int j = 0; j < BOARDHEIGHT;j++)
        TheBoard::board[i][j] = tb->board[i][j];
}

bool TheBoard::setBrick(int x, int y, int color)
{
    if((x<0)||(x>=BOARDWIDTH)||(y<0)||(y>=12))
			return false;
		board[x][y]=color;
		return true;
}

int TheBoard::getBrick(int x, int y)
{
    if((x<0)||(x>=BOARDWIDTH)||(y<0)||(y>=BOARDHEIGHT))
			return -999;
		return board[x][y];
}

void TheBoard::setNumberOfMoves(int moves)
{
    TheBoard::numberOfMoves = moves;
}

int TheBoard::getNumberOfMoves()
{
    return TheBoard::numberOfMoves;
}

bool TheBoard::moveUp(int x, int y)
{
		if((x<0)||(x>=BOARDWIDTH)||(y<0)||(y>=BOARDHEIGHT))
			return false;
		for(int i=1; i<=y; i++)
		{
			board[x][i-1]=board[x][i];
		}
		board[x][y]=-1;
		return true;
}  //moveUp

bool TheBoard::moveDown(int x, int y)
{
		if((x<0)||(x>=BOARDWIDTH)||(y<0)||(y>=BOARDHEIGHT))
			return false;
		for(int i=y-1; i>=0; i--)
		{
			board[x][i+1]=board[x][i];
		}
		board[x][0]=-1;
		return true;
}

void TheBoard::moveRight()
{
		for(int i=0; i<BOARDHEIGHT;i++)
		{
			for(int j=BOARDWIDTH-2; j>=0; j--)
			{
				board[j+1][i]=board[j][i];
			}
			board[0][i] = -1;
		}
} //moveRight


//moves all pieces one to the right
void TheBoard::moveLeft()
{
		for(int i=0; i<BOARDHEIGHT;i++)
		{
			for(int j=1; j<=BOARDWIDTH-1; j++)
			{
				board[j-1][i]=board[j][i];
			}
			board[BOARDWIDTH-1][i] = -1;
		}
} //moveRight

bool TheBoard::equals(TheBoard &tb2)
{
		if(tb2.getNumberOfMoves()!=getNumberOfMoves())
			return false;
		for(int i=0;i<BOARDWIDTH;i++)
		for(int j=0;j<BOARDHEIGHT;j++)
		{
			if(tb2.board[i][j]!=board[i][j])
				return false;
		}
		return true;
}
