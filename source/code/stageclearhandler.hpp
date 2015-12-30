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
http://blockattack.net
===========================================================================
*/


/* 
 * File:   stageclearhandler.hpp
 * Author: poul
 *
 * Created on 29. december 2015, 12:56
 */

#ifndef STAGECLEARHANDLER_HPP
#define STAGECLEARHANDLER_HPP

#include <string>

extern std::string stageClearSavePath;

void StageClearSetClear(int level, int score, int time);
void LoadStageClearStages();
int GetTotalScore();
int GetTotalTime();
int GetNrOfLevels();
bool IsStageCleared(int level);
int GetStageScores(int level);
int GetStageTime(int level);

const int nrOfStageLevels = 50;		//number of stages in stage Clear

#endif /* STAGECLEARHANDLER_HPP */

