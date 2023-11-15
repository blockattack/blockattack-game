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

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "SDL.h"
#include <vector>
#include <string>

const int deadZoneLimit = 20000;

void InitGameControllers();
void UnInitGameControllers();

bool GameControllerIsDownEvent(const SDL_Event& event);
bool GameControllerIsUpEvent(const SDL_Event& event);
bool GameControllerIsLeftEvent(const SDL_Event& event);
bool GameControllerIsRightEvent(const SDL_Event& event);

bool GameControllerExtIsPlayerDownEvent(int playerNumber, const SDL_Event& event);
bool GameControllerExtIsPlayerUpEvent(int playerNumber, const SDL_Event& event);
bool GameControllerExtIsPlayerLeftEvent(int playerNumber, const SDL_Event& event);
bool GameControllerExtIsPlayerRightEvent(int playerNumber, const SDL_Event& event);
bool GameControllerExtIsPlayerSwitchEvent(int playerNumber, const SDL_Event& event);
bool GameControllerExtIsPlayerPushEvent(int playerNumber, const SDL_Event& event);
bool GameControllerIsConnectionEvent(const SDL_Event& event);
void GameControllerSetVerbose(bool value);
const std::vector<std::string>& GetSupportedControllerNames();

/**
 * Checks that the given event is in the dead zone.
 * If it is in the dead zone. Then the dead zone variable for that axis will be set to true;
 * Otherwise nothing is done
 * @param event An SDL
 */
void GameControllerCheckDeadZone(const SDL_Event& event);

/**
 * Checks that the given axis on a given gamepad was in a dead zone last time checked.
 * @param id The gamepad
 * @param axis The axis on the gamepad
 * @return true if the axis has been in the dead zone
 */
bool GameControllerIsInDeadZone(SDL_JoystickID id, int axis);

/**
 * Sets dead zone status on a given axis on a given gamepad
 * @param id The gamepad
 * @param axis The axis on the gamepad
 * @param value Value to set. Should normally be false as true will be set by checkDeadZone
 */
void GameControllerSetIsInDeadZone(SDL_JoystickID id, int axis, bool value);

#endif  //GAMECONTROLLER
