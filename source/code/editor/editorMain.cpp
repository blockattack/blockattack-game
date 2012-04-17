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

#include "editorMain.hpp"

EditorInterface ei;

//How to write!
inline void DrawIMG(SDL_Surface *img, SDL_Surface *target, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(img, NULL, target, &dest);
}

class createFileButton : public aButton
{

public:
	createFileButton()
	{
		aButton::aButton(100,100,bCreateFile);
	}

	void click(int x,int y)
	{
		if(clicked(x,y))
		{
			ei.newBoard();
		}
	}
};

class deletePuzzleButton : public aButton
{

public:
	deletePuzzleButton()
	{
		aButton::aButton(240,100,bDeletePuzzle);
	}

	void click(int x,int y)
	{
		if(clicked(x,y))
		{
			ei.deleteBoard();
		}
	}
};

class loadFileButton : public aButton
{

public:
	loadFileButton()
	{
		aButton::aButton(240,100,bLoadFile);
	}

	void click(int x,int y)
	{
		if(clicked(x,y))
		{
			//Thing to load file!
		}
	}
};

class moveBackButton : public aButton
{

public:
	moveBackButton()
	{
		aButton::aButton(240,100,bMoveBack);
	}

	void click(int x,int y)
	{
		if(clicked(x,y))
		{
			ei.moveBoardBack();
		}
	}
};

class moveDownButton : public aButton
{

public:
	moveDownButton()
	{
		aButton::aButton(240,100,bMoveDown);
	}

	void click(int x,int y)
	{
		if(clicked(x,y))
		{
			if(ei.getNumberOfMoves()>0)
				ei.setNumberOfMoves(ei.getNumberOfMoves()-1);
		}
	}
};

theEditor::theEditor()
{
	//This is the constructor
	createFileButton cb;
	buttons.push_back(cb);
	deletePuzzleButton dpb;
	buttons.push_back(dpb);
}

void theEditor::drawButtons()
{
	for(int i = 0; i<buttons.size(); i++)
	{
		(&(buttons[i]))->draw();
	}
}

void aButton::click(int x, int y)
{
	cout << "Error: aButtons's click command was executed!" << endl;
}

void aButton::draw()
{
	DrawIMG(label,screen,x,y);
}

void theEditor::click(int x, int y)
{
	for(int i = 0; i<buttons.size(); i++)
	{
		(&(buttons[i]))->click(x,y);
	}
}

int main()
{
	theEditor te;
	te.drawButtons();
}