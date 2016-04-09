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
#include "sago/platform_folders.h"
#include <iostream>
#include <map>

static bool verbose = false;

struct ControllerStatus {
	std::map<int, bool> AxisInDeadZone;
	int player = 1;
};

static std::map<SDL_JoystickID, ControllerStatus> controllerStatusMap;


void GameControllerSetVerbose(bool value) {
	verbose = value;
}

static const char* GameControllerGetName(SDL_GameController* gamecontroller) {
	const char* result = SDL_GameControllerName(gamecontroller);
	if (!result) {
		result = "Unnamed";
	}
	return result;
}

void InitGameControllers() {
	std::string configFile = sago::getConfigHome()+"/blockattack/gamecontrollerdb.txt";
	int errorCode = SDL_GameControllerAddMappingsFromFile(configFile.c_str());
	if (errorCode == -1 && verbose) {
		std::cerr << "Could not load mapping file: " << configFile << std::endl;
	}
	if (verbose) {
		std::cout << "Number of Game controllers: " << SDL_NumJoysticks() << std::endl;
	}
	SDL_GameController* controller = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			SDL_Joystick *j = SDL_GameControllerGetJoystick(controller);
			SDL_JoystickID instanceId = SDL_JoystickInstanceID(j);
			controllerStatusMap[instanceId].player = 1;
			if (verbose) {
				std::cout << "Supported game controller detected: " << GameControllerGetName(controller) << ", mapping: " << SDL_GameControllerMapping(controller) <<  std::endl;
				std::cout << "Assigned to player: " << controllerStatusMap[instanceId].player << std::endl;
			}
		}
	}
}

void checkDeadZone(const SDL_Event& event) {
	if (event.type != SDL_CONTROLLERAXISMOTION) {
		return;  //assert?
	}
	int value = event.caxis.value;
	if (value > -deadZoneLimit && value < deadZoneLimit) {
		controllerStatusMap[event.caxis.which].AxisInDeadZone[event.caxis.axis] = true;
	}
}

bool getDeadZone(SDL_JoystickID id, int axis) {
	return controllerStatusMap[id].AxisInDeadZone[axis];
}

void setDeadZone(SDL_JoystickID id, int axis, bool value) {
	controllerStatusMap[id].AxisInDeadZone[axis] = value;
}

static bool skipThisPlayer(int playerNumber, const SDL_Event& event) {

	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		ControllerStatus& cs = controllerStatusMap[event.cbutton.which];
		if (cs.player == playerNumber) {
			return false;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION ) {
		ControllerStatus& cs = controllerStatusMap[event.caxis.which];
		if (cs.player == playerNumber) {
			return false;
		}
	}
	return true;
}

bool isPlayerDownEvent(int playerNumber, const SDL_Event& event) {
	if (skipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ) {
		const SDL_ControllerAxisEvent& a = event.caxis;
		checkDeadZone(event);
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value > deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isPlayerUpEvent(int playerNumber, const SDL_Event& event) {
	if (skipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value < -deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isPlayerLeftEvent(int playerNumber, const SDL_Event& event) {
	if (skipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value < -deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isPlayerRightEvent(int playerNumber, const SDL_Event& event) {
	if (skipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX ) {
		checkDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (getDeadZone(a.which, a.axis)) {
			if (event.caxis.value > deadZoneLimit) {
				setDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool isPlayerSwitchEvent(int playerNumber, const SDL_Event& event) {
	if (skipThisPlayer(playerNumber, event)) {
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
	if (skipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER || event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER ) {
			return true;
		}
	}
	return false;
}