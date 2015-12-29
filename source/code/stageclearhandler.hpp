/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

