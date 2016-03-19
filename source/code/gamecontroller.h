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

#include "SDL.h"


const int deadZoneLimit = 20000;

void InitGameControllers();

bool isPlayerDownEvent(int playerNumber, const SDL_Event& event);
bool isPlayerUpEvent(int playerNumber, const SDL_Event& event);
bool isPlayerLeftEvent(int playerNumber, const SDL_Event& event);
bool isPlayerRightEvent(int playerNumber, const SDL_Event& event);
bool isPlayerSwitchEvent(int playerNumber, const SDL_Event& event);
bool isPlayerPushEvent(int playerNumber, const SDL_Event& event);
void GameControllerSetVerbose(bool value);

/**
 * Checks that the given event is in the dead zone.
 * If it is in the dead zone. Then the dead zone variable for that axis will be set to true;
 * Otherwise nothing is done
 * @param event An SDL 
 */
void checkDeadZone(const SDL_Event& event);

/**
 * Checks that the given axis on a given gamepad was in a dead zone last time checked.
 * @param id The gamepad
 * @param axis The axis on the gamepad
 * @return true if the axis has been in the dead zone
 */
bool getDeadZone(SDL_JoystickID id, int axis);

/**
 * Sets dead zone status on a given axis on a given gamepad 
 * @param id The gamepad
 * @param axis The axis on the gamepad
 * @param value Value to set. Should normally be false as true will be set by checkDeadZone
 */
void setDeadZone(SDL_JoystickID id, int axis, bool value);