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

#ifndef DIALOGBOX_HPP
#define DIALOGBOX_HPP

#include "sago/GameStateInterface.hpp"
#include <string>
#include "ReadKeyboard.h"
#include <memory>
#include "scopeHelpers.hpp"
#include "global.hpp"

struct VirtualKeyboard {
	const std::string leftChar = "‹";
	const std::string rightChar = "›";
	const std::string backspace = "«";
	std::string alphabet = std::string(_("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	                       "abcdefghijklmnopqrstuvwxyz"
	                       ".,:!?+_^@#%&=*")) + leftChar + rightChar + backspace;
	std::vector<std::string> gamePadChars;
	std::vector<sago::SagoTextField> gamePadCharFields;
	int selectedChar = 0;
};

class DialogBox : public sago::GameStateInterface {
public:
	DialogBox(int x, int y, const std::string& name, const std::string& header);
	DialogBox(const DialogBox& orig) = delete;
	DialogBox& operator=(const DialogBox& orig) = delete;
	virtual ~DialogBox();

	bool IsActive() override;
	void Draw(SDL_Renderer* target) override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void SetName(const std::string& name);
	std::string GetName() const;
	bool IsUpdated() const;
private:
	void virtualKeyboardWriteSelectedChar(ReadKeyboard *rk, const std::string& insertChar) const;
	bool isActive = true;
	bool bMouseUp = false;
	std::shared_ptr<ReadKeyboard> rk;
	int x;
	int y;
	std::string name;
	std::string header;
	bool updated = false;
	SDL_TextInput textInputScope;
	sago::SagoTextField headerLabel;
	sago::SagoTextField enterLabel;
	sago::SagoTextField cancelLabel;
	sago::SagoTextField textField;
	sago::SagoTextField cursorLabel;
	VirtualKeyboard virtualKeyboard;
	int oldmousex = 0;
	int oldmousey = 0;
};

void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width);

#endif /* DIALOGBOX_HPP */

