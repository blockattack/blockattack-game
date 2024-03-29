/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2018 Poul Sander
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
#include "ShowFileState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"
#include <fmt/core.h>

ShowFileState::ShowFileState() {
	setHelp30FontThinOutline(&globalData.spriteHolder->GetDataHolder(), titleField, "");
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), infoBox, "");
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), filenameField, "");
}

ShowFileState::~ShowFileState() {
}

void ShowFileState::SetData(const std::string& filename, const std::string& header) {
	titleField.SetText(header);
	infoBox.SetText(sago::GetFileContent(filename));
	filenameField.SetText(fmt::format(_("Showing content of: {}"), filename));
}
