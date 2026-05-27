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
#include <SDL3/SDL.h>
#include "platform_folders.h"
#include "common.h"
#include <iostream>
#include <map>
// SDL_joystick is now part of SDL3/SDL.h

static bool verbose = false;

struct ControllerStatus {
	std::map<int, bool> AxisInDeadZone;
	int player = 1;
};

static std::map<SDL_JoystickID, ControllerStatus> controllerStatusMap;
static std::map<std::string, int> gamecontrollers_assigned;
static std::vector<std::string> supportedControllers;
static std::vector<SDL_Gamepad*> controllersOpened;


void GameControllerSetVerbose(bool value) {
	verbose = value;
}

static const char* GameControllerGetName(SDL_Gamepad* gamecontroller) {
	const char* result = SDL_GetGamepadName(gamecontroller);
	if (!result) {
		result = "Unnamed";
	}
	return result;
}

static std::string GetGuidAsHex(const SDL_GUID& guid) {
	std::string ret;
	char buffer[3];
	for (size_t j = 0; j < sizeof(guid.data); j++) {
		snprintf(buffer, sizeof(buffer), "%02X", guid.data[j]);
		ret += buffer;
	}
	return ret;
}


static int GetNextPlayerByGui(const SDL_GUID& guid) {
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
	for (SDL_Gamepad* t : controllersOpened) {
		SDL_CloseGamepad(t);
	}
	controllersOpened.clear();
	controllerStatusMap.clear();
	gamecontrollers_assigned.clear();
	supportedControllers.clear();
}

void InitGameControllers() {
	std::string configFile = sago::getConfigHome()+"/blockattack/gamecontrollerdb.txt";
	int errorCode = SDL_AddGamepadMappingsFromFile(configFile.c_str());
	if (errorCode == -1 && verbose) {
		std::cerr << "Could not load mapping file: " << configFile << "\n";
	}
	int joystickCount = 0;
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&joystickCount);
	if (verbose) {
		std::cout << "Number of joysticks: " << joystickCount << "\n";
	}
	for (int i = 0; i < joystickCount; ++i) {
		SDL_JoystickID instanceId = joysticks[i];
		if (SDL_IsGamepad(instanceId)) {
			SDL_Gamepad* controller = SDL_OpenGamepad(instanceId);
			SDL_Joystick* j = SDL_GetGamepadJoystick(controller);
			SDL_GUID guid = SDL_GetJoystickGUID(j);
			int assingToPlayer = GetNextPlayerByGui(guid);
			controllerStatusMap[instanceId].player = assingToPlayer;
			supportedControllers.push_back(GameControllerGetName(controller));
			controllersOpened.push_back(controller);
			if (verbose) {
				char* mapping = SDL_GetGamepadMapping(controller);
				std::cout << "Supported game controller detected: " << GameControllerGetName(controller) << ", mapping: " << (mapping ? mapping : "") <<  "\n";
				std::cout << "Assigned to player: " << controllerStatusMap[instanceId].player << "\n";
				SDL_free(mapping);
			}
		}
	}
	SDL_free(joysticks);
}

const std::vector<std::string>& GetSupportedControllerNames() {
	return supportedControllers;
}

void GameControllerCheckDeadZone(const SDL_Event& event) {
	if (event.type != SDL_EVENT_GAMEPAD_AXIS_MOTION) {
		return;  //assert?
	}
	int value = event.gaxis.value;
	if (value > -deadZoneLimit && value < deadZoneLimit) {
		controllerStatusMap[event.gaxis.which].AxisInDeadZone[event.gaxis.axis] = true;
	}
}

bool GameControllerIsInDeadZone(SDL_JoystickID id, int axis) {
	return controllerStatusMap[id].AxisInDeadZone[axis];
}

void GameControllerSetIsInDeadZone(SDL_JoystickID id, int axis, bool value) {
	controllerStatusMap[id].AxisInDeadZone[axis] = value;
}

static bool GameControllerExtSkipThisPlayer(int playerNumber, const SDL_Event& event) {

	if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
		ControllerStatus& cs = controllerStatusMap[event.gbutton.which];
		if (cs.player == playerNumber) {
			return false;
		}
	}
	if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION ) {
		ControllerStatus& cs = controllerStatusMap[event.gaxis.which];
		if (cs.player == playerNumber) {
			return false;
		}
	}
	return true;
}


bool GameControllerIsConnectionEvent(const SDL_Event& event) {
	if ( event.type == SDL_EVENT_GAMEPAD_ADDED
	        || event.type == SDL_EVENT_GAMEPAD_REMOVED
	        || event.type == SDL_EVENT_GAMEPAD_REMAPPED ) {
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

bool GameControllerIsControllerDirectionEvent(const SDL_Event& event, SDL_GamepadButton dpad_direction, SDL_GamepadAxis axis, float axis_mod = 1.0f) {
	if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
		if (event.gbutton.button == dpad_direction ) {
			return true;
		}
	}
	if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION  && event.gaxis.axis == axis ) {
		const SDL_GamepadAxisEvent& a = event.gaxis;
		GameControllerCheckDeadZone(event);
		if (GameControllerIsInDeadZone(a.which, a.axis)) {
			if (event.gaxis.value * axis_mod > deadZoneLimit) {
				GameControllerSetIsInDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}

bool GameControllerIsDownEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_GAMEPAD_BUTTON_DPAD_DOWN, SDL_GAMEPAD_AXIS_LEFTY);
}

bool GameControllerIsUpEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_GAMEPAD_BUTTON_DPAD_UP, SDL_GAMEPAD_AXIS_LEFTY, -1.0f);
}

bool GameControllerIsLeftEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_GAMEPAD_BUTTON_DPAD_LEFT, SDL_GAMEPAD_AXIS_LEFTX, -1.0f);
}

bool GameControllerIsRightEvent(const SDL_Event& event) {
	return GameControllerIsControllerDirectionEvent(event, SDL_GAMEPAD_BUTTON_DPAD_RIGHT, SDL_GAMEPAD_AXIS_LEFTX);
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
	if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
		if (event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH || event.gbutton.button == SDL_GAMEPAD_BUTTON_EAST ) {
			return true;
		}
	}
	return false;
}

bool GameControllerExtIsPlayerPushEvent(int playerNumber, const SDL_Event& event) {
	if (GameControllerExtSkipThisPlayer(playerNumber, event)) {
		return false;
	}
	if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
		if (event.gbutton.button == SDL_GAMEPAD_BUTTON_LEFT_SHOULDER || event.gbutton.button == SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER ) {
			return true;
		}
	}
	if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION  && (event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER || event.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER ) ) {
		GameControllerCheckDeadZone(event);
		const SDL_GamepadAxisEvent& a = event.gaxis;
		if (GameControllerIsInDeadZone(a.which, a.axis)) {
			if (event.gaxis.value > deadZoneLimit) {
				GameControllerSetIsInDeadZone(a.which,a.axis,false);
				return true;
			}
		}
	}
	return false;
}
