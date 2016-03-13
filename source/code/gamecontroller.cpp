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

#include "gamecontroller.h"
#include "SDL_gamecontroller.h"
#include <iostream>

void InitGameControllers() {
	std::cout << "Number of Game controllers: " << SDL_NumJoysticks() << std::endl;
	SDL_GameController* controller = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			std::cout << "Supported game controller detected: " << SDL_GameControllerName(controller) << std::endl;
		}
	}
}

bool isPlayerDownEvent(int playerNumber, const SDL_Event& event) {
	if (playerNumber != 1) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN ) {
			return true;
		}
	}
	return false;
}

bool isPlayerUpEvent(int playerNumber, const SDL_Event& event) {
	if (playerNumber != 1) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP ) {
			return true;
		}
	}
	return false;
}

bool isPlayerLeftEvent(int playerNumber, const SDL_Event& event) {
	if (playerNumber != 1) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ) {
			return true;
		}
	}
	return false;
}

bool isPlayerRightEvent(int playerNumber, const SDL_Event& event) {
	if (playerNumber != 1) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ) {
			return true;
		}
	}
	return false;
}

bool isPlayerSwitchEvent(int playerNumber, const SDL_Event& event) {
	if (playerNumber != 1) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A || event.cbutton.button == SDL_CONTROLLER_BUTTON_B ) {
			return true;
		}
	}
	return false;
}

bool isPlayerPushEvent(int playerNumber, const SDL_Event& event) {
	if (playerNumber != 1) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER || event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER ) {
			return true;
		}
	}
	return false;
}