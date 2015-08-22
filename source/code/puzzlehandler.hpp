/* 
 * File:   puzzlehandler.hpp
 * Author: poul
 *
 * Created on 22. august 2015, 20:09
 */

#ifndef PUZZLEHANDLER_HPP
#define	PUZZLEHANDLER_HPP


#include <string>

int PuzzleNumberOfMovesAllowed(int level);
int PuzzleGetBrick(int level, int x, int y);
bool PuzzleIsCleared(int level);
int LoadPuzzleStages();
int PuzzleGetNumberOfPuzzles();
void PuzzleSetClear(int level);
const std::string& PuzzleGetName();
void PuzzleSetName(const std::string& name);
void PuzzleSetSavePath(const std::string& filepath);

#endif	/* PUZZLEHANDLER_HPP */

