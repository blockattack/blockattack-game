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

#include "HelpCommon.hpp"



template<typename T> void setHelp30FontTemplate(const sago::SagoDataHolder* holder, T& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(30);
	field.SetOutline(2, {0,0,0,255});
	field.SetText(text);
}

void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	setHelp30FontTemplate(holder, field, text);
}

void setHelp30Font(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text) {
	setHelp30FontTemplate(holder, field, text);
}

void setHelpBoxFont(const sago::SagoDataHolder* holder, sago::SagoTextBox& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetColor({0,0,0,255});
	field.SetFontSize(20);
	field.SetOutline(0, {0,0,0,255});
	field.SetText(text);
}
