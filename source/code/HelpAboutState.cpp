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

#include "DialogBox.hpp"
#include "HelpCommon.hpp"
#include "HelpAboutState.hpp"
#include "global.hpp"
#include "common.h"
#include "MenuSystem.h"
#include "sstream"
#include "version.h"
#include "sago/SagoMisc.hpp"


HelpAboutState::HelpAboutState() {
	SDL_RendererInfo renderInfo;
	SDL_version compiled;
	SDL_version linked;
	SDL_GetRendererInfo(globalData.screen, &renderInfo);
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	const char* audio_driver_name = SDL_GetCurrentAudioDriver();
	if (!audio_driver_name) {
		audio_driver_name = _("No audio driver");
	}
	std::stringstream infoStream;
	infoStream << _("Name:") << " " << _("Block Attack - Rise of the Blocks") << "\n";
	infoStream << _("Original name:") << " Block Attack - Rise of the Blocks" << "\n";
	infoStream << _("Version:") << " " << VERSION_NUMBER << "\n";
	infoStream << _("Homepage:") << " " << "https://blockattack.net\n";
	infoStream << _("Github page:") << " " << "https://github.com/blockattack/blockattack-game\n";
	infoStream << _("SDL render:") << " " << renderInfo.name << "\n";
	infoStream << _("SDL audio driver:") << " " << audio_driver_name << "\n";
	infoStream << _("SDL compiled version:") << " " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << "\n";
	infoStream << _("SDL linked version:") << " " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << "\n";
	infoStream << _("Save folder:") << " " << PHYSFS_getWriteDir() << "\n";
	infoStream << _("Locale:") << " " << setlocale( LC_CTYPE, nullptr ) << "\n";
	setHelp30FontThinOutline(&globalData.spriteHolder->GetDataHolder(), titleField, _("About"));
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), infoBox, infoStream.str().c_str());
	sago::WriteFileContent("about.txt", infoStream.str());
}

HelpAboutState::~HelpAboutState() {
}


void HelpAboutState::Draw(SDL_Renderer* target) {
	DrawBackground(target);
	titleField.Draw(target, 50, 50);
	DrawRectYellow(target, 40, 90, 600, 900);
	infoBox.SetMaxWidth(850);
	infoBox.Draw(target, 50, 100);
	HelpCommonState::Draw(target);
}
