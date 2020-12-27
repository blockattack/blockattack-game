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

#include "ModConfigMenuState.hpp"
#include "MenuSystem.h"
#include "global.hpp"
#include "MenuSystem.h"
#include "sago/SagoMisc.hpp"
#include "os.hpp"
#include <boost/algorithm/string/predicate.hpp>

const int buttonOffset = 160;
extern sago::SagoSprite bExit;

bool sort_mods_enabled_order (const Mod& i,const Mod& j) {
	if (i.enabled && !j.enabled) {
		//Enabled mods always goes before disabled ones
		return true;
	}
	if (i.enabled) {
		return i.order < j.order;
	}
	return i.name < j.name;
}

static void initMods(std::vector<Mod>& mod_list) {
	for (size_t i=0; i < globalData.modList.size(); ++i) {
		for (Mod& m : mod_list) {
			if (m.name == globalData.modList[i]) {
				m.order = i;
				m.enabled = true;
			}
		}
	}
	std::sort(mod_list.begin(), mod_list.end(), sort_mods_enabled_order);
}


ModConfigMenuState::ModConfigMenuState() {
	mods_available.clear();
	std::string baseMods = std::string(PHYSFS_getBaseDir())+ "/mods";
	std::vector<std::string> baseModFiles = OsGetDirFileList(baseMods);
	for (const std::string& mod : baseModFiles) {
		if (!boost::ends_with(mod, ".data")) {
			continue;
		}
		Mod m;
		m.name = mod.substr(0, mod.length()-5);
		m.filename = baseMods + "/" + mod;
		mods_available.push_back(m);
	}
	std::string userMods = getPathToSaveFiles()+"/mods";
	initMods(mods_available);
}

ModConfigMenuState::~ModConfigMenuState() {}

bool ModConfigMenuState::IsActive() {
	return isActive;
}

void ModConfigMenuState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	standardButton.getLabel(_("Mod config"))->Draw(target, 50, 50);
	for (size_t i = 0; i < mods_available.size(); ++i) {
		standardButton.getLabel(mods_available[i].name)->Draw(target, 60, 160+30*i);
		standardButton.getLabel(mods_available[i].enabled ? _("Yes") : _("No"))->Draw(target, 600, 160+30*i);
		if (mods_available[i].enabled) {
			standardButton.getLabel(std::to_string(mods_available[i].order))->Draw(target, 700, 160+30*i);
		}
	}
	bExit.Draw(globalData.screen, SDL_GetTicks(), globalData.xsize-buttonOffset, globalData.ysize-buttonOffset);
}

void ModConfigMenuState::ProcessInput(const SDL_Event& event, bool& processed) {
	if (isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
}

void ModConfigMenuState::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;

		//The Score button:
		if ((globalData.mousex>globalData.xsize-buttonOffset) && (globalData.mousex<globalData.xsize-buttonOffset+bExit.GetWidth())
		        && (globalData.mousey>globalData.ysize-buttonOffset) && (globalData.mousey<globalData.ysize-buttonOffset+bExit.GetHeight())) {
			isActive = false;
		}

	}
}
