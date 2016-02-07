/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GameStateInterface.hpp
 * Author: poul
 *
 * Created on 7. februar 2016, 17:26
 */

#ifndef GAMESTATEINTERFACE_HPP
#define GAMESTATEINTERFACE_HPP

#include "SDL.h"

namespace sago {

class GameStateInterface {
public:
	/**
	 * Is the state active. If this returns false then the State-manager will pop the state object
	 * @return true if active
	 */
	virtual bool IsActive() = 0;
	
	/**
	 * Tells the state to draw itself to target
	 * @param target The RenderWindow to draw to
	 */
	virtual void Draw(SDL_Renderer* target) = 0;
	
	virtual void ProcessInput(const SDL_Event& event, bool &processed) = 0;
	
	virtual void Update() {}
};

}  //sago

#endif /* GAMESTATEINTERFACE_HPP */

