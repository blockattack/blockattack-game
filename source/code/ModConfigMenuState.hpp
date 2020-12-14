/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2020 Poul Sander

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
https://blockattack.net
===========================================================================
*/

#ifndef MODCONFIG_HPP
#define MODCONFIG_HPP

#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextField.hpp"
#include "sago/SagoTextBox.hpp"

class ModConfigMenuState : public sago::GameStateInterface {
public:
	ModConfigMenuState();
	ModConfigMenuState(const ModConfigMenuState& orig) = delete;
	virtual ~ModConfigMenuState();

	bool IsActive() override;
	void Draw(SDL_Renderer* target) override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Update() override;

private:
	bool isActive = true;
	bool bMouseUp = true;
};

#endif /* MODCONFIG_HPP */
