/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2015 Poul Sander

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
http://www.blockattack.net
===========================================================================
*/

#ifndef PUZZLEHANDLER_HPP
#define PUZZLEHANDLER_HPP


#include <string>

int PuzzleNumberOfMovesAllowed(size_t level);
int PuzzleGetBrick(size_t level, int x, int y);
bool PuzzleIsCleared(size_t level);
int LoadPuzzleStages();
int SavePuzzleStages();
int PuzzleGetNumberOfPuzzles();
void PuzzleSetClear(size_t level);
const std::string& PuzzleGetName();
void PuzzleSetName(const std::string& name);

#endif  /* PUZZLEHANDLER_HPP */

