/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2016 Poul Sander

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

#ifndef SCORESDISPLAY_HPP
#define SCORESDISPLAY_HPP

#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextField.hpp"
#include <map>
#include <memory>
#include "highscore.h"

class ScoresDisplay : public sago::GameStateInterface {
public:
	ScoresDisplay();
	ScoresDisplay(const ScoresDisplay& orig) = delete;
	virtual ~ScoresDisplay();
	
	bool IsActive() override;
	void Draw(SDL_Renderer* target) override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Update() override;
	
	int page = 0;
	//button coodinates:
	int scoreX = 0;
	int scoreY = 0;
	int buttonXsize = 0;
	int buttonYsize = 0;
	Highscore theTopScoresEndless0 = Highscore("endless", 0.5);      //Stores highscores for endless
	Highscore theTopScoresEndless1 = Highscore("endless", 0.1);      //Stores highscores for endless
	Highscore theTopScoresEndless2 = Highscore("endless", 0.07);      //Stores highscores for endless
	Highscore theTopScoresEndless3 = Highscore("endless", 0.04);      //Stores highscores for endless
	Highscore theTopScoresEndless4 = Highscore("endless", 0.015);      //Stores highscores for endless
	Highscore theTopScoresTimeTrial = Highscore("timetrial", 0.5);    //Stores highscores for timetrial
private:
	void DrawHighscores(int x, int y, bool endless, int speedLevel);
	void DrawStats();
	void DrawBackgroundAndCalcPlacements();
	void Write(SDL_Renderer* target, int x, int y, const char* text);
	sago::SagoTextField* getCachedText(const std::string& text);
	std::map<std::string, std::shared_ptr<sago::SagoTextField> > fieldCache;
	bool isActive = true;
	bool bMouseUp = false;
	int backX = 20;
	int backY = 0;
	int nextX = 0;
	int nextY = 0;
};

#endif /* SCORESDISPLAY_HPP */

