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

/*
TODO: The way we test for

*/

#include "EditorInterface.hpp"

EditorInterface::EditorInterface()
{
	boardActive = -1;
	saved = true;
	fileSaved = true;
	bh.isNull = true;
}

void EditorInterface::getModel(int nr)
{
	tb = bh.getModel(nr);
	saved = true;
	boardActive = nr;
}

bool EditorInterface::exists()
{
	if(bh.isNull)
		return false;
	else
		return true;
}

void EditorInterface::selectColor(int color)
{
	colorSelected = color;
}

int EditorInterface::getSelectedColor()
{
	return colorSelected;
}

void EditorInterface::drawOnModel(int x, int y)
{
	if(boardActive!=-1)
	{
		if(colorSelected<7)  //Ordinary thing
			tb.setBrick(x,y,colorSelected);
		if(colorSelected==7)
			tb.moveUp(x,y);
		if(colorSelected==8)
			tb.moveDown(x,y);
		saved = false;
	}
}

void EditorInterface::moveLeft()
{
	if(boardActive!=-1)
		tb.moveLeft();
	saved = false;
}

void EditorInterface::moveRight()
{
	if(boardActive!=-1)
		tb.moveRight();
	saved = false;
}

void EditorInterface::deleteBoard()
{
	if(boardActive!=-1)
	{
		bh.removeBoard(boardActive);
		boardActive = -1;
		saved = true;
		fileSaved = false;
	}
}

void EditorInterface::moveBoardBack()
{
	if(boardActive!=-1)
	{
		if(bh.moveBoardBack(boardActive))
			boardActive--;
	}
}

void EditorInterface::moveBoardForward()
{
	if(boardActive!=-1)
	{
		if(bh.moveBoardForward(boardActive))
			boardActive++;
	}
}

void EditorInterface::saveBoard()
{
	if(boardActive!=-1)
	{
		bh.setModel(boardActive,tb);
		saved = true;
		fileSaved = false;
	}
}

bool EditorInterface::saveFile(string filename)
{
	if((!bh.isNull)&&(filename!=""))
	{
		fileName = filename;
		bh.saveBoards(filename);
		fileSaved = true;
		return true;
	}
	return false;
}

bool EditorInterface::saveFile()
{
	if((!bh.isNull)&&(fileName!=""))
	{
		bh.saveBoards(fileName);
		fileSaved = true;
		return true;
	}
	return false;
}

void EditorInterface::openFile(string filename)
{
	fileName = filename;
	BoardHolder bh2(fileName);
	bh = bh2;
	saved = true;
	fileSaved = true;
}

void EditorInterface::newFile()
{
	fileName = "";
	bh = BoardHolder();
	TheBoard tb2;
	tb.isNull = true; // = null; What here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	boardActive = -1;
	saved = true;
	fileSaved = false;
}

int EditorInterface::getColor(int x, int y)
{
	if(boardActive==-1)
		return -1;
	return tb.getBrick(x,y);
}

int EditorInterface::getActiveBoardNr()
{
	return boardActive;
}

int EditorInterface::getNumberOfMoves()
{
	if(!tb.isNull)
	{
		return tb.getNumberOfMoves();
	}
	return 0;
}

bool EditorInterface::newBoard()
{
	if(!bh.isNull)
	{
		fileSaved = false;
		return bh.addBoard();
	}
	else
		return false;
}

int EditorInterface::getNumberOfBoards()
{
	if(!bh.isNull)
		return bh.getNumberOfBoards();
	else
		return 0;
}

void EditorInterface::setNumberOfMoves(int moves)
{
	if(!tb.isNull)
	{
		saved = false;
		tb.setNumberOfMoves(moves);
	}
}

bool EditorInterface::isSaved()
{
	return saved;
}

bool EditorInterface::fileIsSaved()
{
	return fileSaved;
}
