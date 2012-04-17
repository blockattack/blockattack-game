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

using namespace std;

#define BOARDWIDTH 6
#define BOARDHEIGHT 12

#ifndef THEBOARDDEFINED
#define THEBOARDDEFINED
class TheBoard
{
private:
	int board[BOARDWIDTH][BOARDHEIGHT];
	int numberOfMoves;

public:
	bool isNull;
	TheBoard();
	TheBoard(const TheBoard &tb);
	void copyFrom(TheBoard *tb);
	bool setBrick(int x, int y, int color);
	int getBrick(int x, int y);
	void setNumberOfMoves(int);
	int getNumberOfMoves();
	bool moveUp(int x, int y);
	bool moveDown(int x, int y);
	void moveRight();
	void moveLeft();
	bool equals(TheBoard &tb);
};
#endif
