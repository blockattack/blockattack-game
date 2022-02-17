/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2022 Poul Sander

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

#include "DialogBox.hpp"
#include "HelpModInfoState.hpp"
#include "global.hpp"
#include "common.h"
#include "sago/SagoMisc.hpp"
#include "os.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

struct Mod {
	std::string name;
	std::string filename;
	bool enabled = false;
	int order = 0;
};

static bool sort_mods_enabled_order (const Mod& i,const Mod& j) {
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
	std::vector<Mod> mod_list_no_duplicates;
	std::string last_mod = "";
	for (const Mod& m : mod_list) {
		if (m.name != last_mod) {
			mod_list_no_duplicates.push_back(m);
		}
		last_mod = m.name;
	}
	std::swap(mod_list, mod_list_no_duplicates);
}

static void appendMods(const std::vector<std::string>& mod_files, const std::string& dir, std::vector<Mod>& mods_available) {
	for (const std::string& mod : mod_files) {
		if (!boost::ends_with(mod, ".data")) {
			continue;
		}
		Mod m;
		m.name = mod.substr(0, mod.length()-5);
		m.filename = dir + "/" + mod;
		mods_available.push_back(m);
	}
}

static std::vector<Mod> get_mods() {
	std::vector<Mod> mods_available;
	std::string baseMods = std::string(PHYSFS_getBaseDir())+ "/mods";
	std::vector<std::string> baseModFiles = OsGetDirFileList(baseMods);
	appendMods(baseModFiles, baseMods, mods_available);
	std::string sharedMods = std::string(SHAREDIR)+ "/mods";
	std::vector<std::string> sharedModFiles = OsGetDirFileList(sharedMods);
	appendMods(sharedModFiles, sharedMods, mods_available);
	std::string userMods = getPathToSaveFiles()+"/mods";
	std::vector<std::string> userModFiles = OsGetDirFileList(userMods);
	appendMods(userModFiles, userMods, mods_available);
	initMods(mods_available);
	return mods_available;
}

HelpModInfoState::HelpModInfoState() {
	std::vector<Mod> mods_available = get_mods();
	std::stringstream infoStream;
	infoStream << _("Load order:") << "\n";
	for (size_t i=0; i < mods_available.size(); ++i) {
		const Mod& mod = mods_available[i];
		if (mod.enabled) {
			infoStream << mod.order << " : " << mod.name << " ";
		}
		else {
			infoStream << "- : " << mod.name << " " << _("(Disabled)");
		}
		infoStream << "\n";
	}
	setHelp30FontThinOutline(&globalData.spriteHolder->GetDataHolder(), titleField, _("Mod info"));
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), infoBox, infoStream.str().c_str());
}

HelpModInfoState::~HelpModInfoState() {
}