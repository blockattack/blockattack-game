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


#ifndef _HELP_COMMON_HPP
#define _HELP_COMMON_HPP

#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextBox.hpp"
#include "sago/SagoTextField.hpp"


void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text);
void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text);
void setHelp30FontThinOutline(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text);

void setHelpBoxFont(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text);

class HelpCommonState : public sago::GameStateInterface {
public:
	HelpCommonState() = default;
	HelpCommonState(const HelpCommonState& orig) = delete;
	virtual ~HelpCommonState() = default;

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;
	const int buttonOffset = 160;

private:
	bool isActive = true;
	bool bMouseUp = true;
};

class HelpTextBoxState : public HelpCommonState {
public:
	HelpTextBoxState() = default;
	HelpTextBoxState(const HelpTextBoxState& orig) = delete;
	virtual ~HelpTextBoxState() = default;
	void Draw(SDL_Renderer* target) override;
protected:
	sago::SagoTextField titleField;
	sago::SagoTextBox infoBox;
	sago::SagoTextBox filenameField;
};

#endif  //_HELP_COMMON_HPP
