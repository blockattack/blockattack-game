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
#include "platform_folders.h"
#include "common.h"
#include <iostream>
#include <map>
#include <SDL2/SDL_joystick.h>

static bool verbose = false;

struct ControllerStatus {
	std::map<int, bool> AxisInDeadZone;
	int player = 1;
};

static std::map<SDL_JoystickID, ControllerStatus> controllerStatusMap;
static std::map<std::string, int> gamecontrollers_assigned;
static std::vector<std::string> supportedControllers;
static std::vector<SDL_GameController*> controllersOpened;


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

static std::string GetGuidAsHex(const SDL_JoystickGUID& guid) {
	std::string ret;
	char buffer[3];
	for (size_t j = 0; j < sizeof(guid.data); j++) {
		snprintf(buffer, sizeof(buffer), "%02X", guid.data[j]);
		ret += buffer;
	}
	return ret;
}


static int GetNextPlayerByGui(const SDL_JoystickGUID& guid) {
	Config::getInstance()->setDefault("gc_AllToOnePlayer", "0");
	int fixedPlayer = Config::getInstance()->getInt("gc_AllToOnePlayer");
	if (fixedPlayer > 0 && fixedPlayer <= 2) {
		return fixedPlayer;
	}
	std::string guidAsHex= GetGuidAsHex(guid);
	std::string configName = "gc_assign_"+guidAsHex;
	Config::getInstance()->setDefault(configName, "1");
	int player = Config::getInstance()->getInt(configName) + gamecontrollers_assigned[guidAsHex];
	gamecontrollers_assigned[guidAsHex]++; //Next controller with same guid should be assigned to different player.

	if (player%2==0) {
		return 2;  //Even number means player 2
	}
	return 1;
}

void UnInitGameControllers() {
	if (controllersOpened.empty()) {
		return;
	}
	for (SDL_GameController* t : controllersOpened) {
		SDL_GameControllerClose(t);
	}
	controllersOpened.clear();
	controllerStatusMap.clear();
	gamecontrollers_assigned.clear();
	supportedControllers.clear();
}

void InitGameControllers() {
	std::string configFile = sago::getConfigHome()+"/blockattack/gamecontrollerdb.txt";
	int errorCode = SDL_GameControllerAddMappingsFromFile(configFile.c_str());
	if (errorCode == -1 && verbose) {
		std::cerr << "Could not load mapping file: " << configFile << "\n";
	}
	if (verbose) {
		std::cout << "Number of Game controllers: " << SDL_NumJoysticks() << "\n";
	}
	SDL_GameController* controller = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			SDL_Joystick* j = SDL_GameControllerGetJoystick(controller);
			SDL_JoystickID instanceId = SDL_JoystickInstanceID(j);
			SDL_JoystickGUID guid = SDL_JoystickGetGUID(j);
			int assingToPlayer = GetNextPlayerByGui(guid);
			controllerStatusMap[instanceId].player = assingToPlayer;
			supportedControllers.push_back(GameControllerGetName(controller));
			controllersOpened.push_back(controller);
			if (verbose) {
				std::cout << "Supported game controller detected: " << GameControllerGetName(controller) << ", mapping: " << SDL_GameControllerMapping(controller) <<  "\n";
				std::cout << "Assigned to player: " << controllerStatusMap[instanceId].player << "\n";
			}
		}
	}
}

const std::vector<std::string>& GetSupportedControllerNames() {
	return supportedControllers;
}

void GameControllerCheckDeadZone(const SDL_Event& event) {
	if (event.type != SDL_CONTROLLERAXISMOTION) {
		return;  //assert?
	}
	int value = event.caxis.value;
	if (value > -deadZoneLimit && value < deadZoneLimit) {
		controllerStatusMap[event.caxis.which].AxisInDeadZone[event.caxis.axis] = true;
	}
}

bool GameControllerIsInDeadZone(SDL_JoystickID id, int axis) {
	return controllerStatusMap[id].AxisInDeadZone[axis];
}

void GameControllerSetIsInDeadZone(SDL_JoystickID id, int axis, bool value) {
	controllerStatusMap[id].AxisInDeadZone[axis] = value;
}

static bool GameControllerExtSkipThisPlayer(int playerNumber, const SDL_Event& event) {

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


bool GameControllerIsConnectionEvent(const SDL_Event& event) {
	if ( event.type == SDL_CONTROLLERDEVICEADDED
	        || event.type == SDL_CONTROLLERDEVICEREMOVED
	        || event.type == SDL_CONTROLLERDEVICEREMAPPED ) {
		return true;
	}
	return false;
}



bool GameControllerExtIsPlayerDownEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	return GameControllerIsDownEvent(event);
}

bool GameControllerIsControllerDirectionEvent(const SDL_Event& event, SDL_GameControllerButton dpad_direction, SDL_GameControllerAxis axis, float axis_mod = 1.0f) {
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == dpad_direction ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && event.caxis.axis == axis ) {
		const SDL_ControllerAxisEvent& a = event.caxis;
		GameControllerCheckDeadZone(event);
		if (GameControllerIsInDeadZone(a.which, a.axis)) {
			if (event.caxis.value * axis_mod > deadZoneLimit) {
				GameControllerSetIsInDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool GameControllerIsDownEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_CONTROLLER_AXIS_LEFTY);
}

bool GameControllerIsUpEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_AXIS_LEFTY, -1.0f);
}

bool GameControllerIsLeftEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_AXIS_LEFTX, -1.0f);
}

bool GameControllerIsRightEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_AXIS_LEFTX);
}

bool GameControllerExtIsPlayerUpEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	return GameControllerIsUpEvent(event);
}

bool GameControllerExtIsPlayerLeftEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	return GameControllerIsLeftEvent(event);
}

bool GameControllerExtIsPlayerRightEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	return GameControllerIsRightEvent(event);
}

bool GameControllerExtIsPlayerSwitchEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A || event.cbutton.button == SDL_CONTROLLER_BUTTON_B ) {
			return true;
		}
	}
	return false;
}

bool GameControllerExtIsPlayerPushEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER || event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER ) {
			return true;
		}
	}
	if (event.type == SDL_CONTROLLERAXISMOTION  && (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT ) ) {
		GameControllerCheckDeadZone(event);
		const SDL_ControllerAxisEvent& a = event.caxis;
		if (GameControllerIsInDeadZone(a.which, a.axis)) {
			if (event.caxis.value > deadZoneLimit) {
				GameControllerSetIsInDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}
