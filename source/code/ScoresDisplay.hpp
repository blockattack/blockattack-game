/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ScoresDisplay.hpp
 * Author: poul
 *
 * Created on 7. februar 2016, 17:34
 */

#ifndef SCORESDISPLAY_HPP
#define SCORESDISPLAY_HPP

#include "sago/GameStateInterface.hpp"

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
	int backX = 20;
	int backY = 0;
	int nextX = 0;
	int nextY = 0;
	int xsize = 0;
	int ysize = 0;
	int buttonXsize = 0;
	int buttonYsize = 0;
private:
	void DrawHighscores(int x, int y, bool endless);
	void DrawStats();
	bool isActive = true;
	bool bMouseUp = false;
};

#endif /* SCORESDISPLAY_HPP */

