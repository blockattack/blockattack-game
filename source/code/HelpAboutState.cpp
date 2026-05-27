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
#include "fmt/core.h"


// SDL3 encodes versions as (major*1000000 + minor*1000 + patch)
static std::string sdl_version_int_as_string(int version) {
	int major = version / 1000000;
	int minor = (version / 1000) % 1000;
	int patch = version % 1000;
	return fmt::format("{}.{}.{}", major, minor, patch);
}

template <class T>
std::string sdl_verison_as_string(const T& version) {
	return fmt::format("{}.{}.{}", (int)version.major, (int)version.minor, (int)version.patch);
}


HelpAboutState::HelpAboutState() {
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
	infoStream << _("SDL render:") << " " << SDL_GetRendererName(globalData.screen) << "\n";
	infoStream << _("SDL audio driver:") << " " << audio_driver_name << "\n";
	infoStream << _("SDL compiled version:") << " " << sdl_version_int_as_string(SDL_VERSION) << "\n";
	infoStream << _("SDL linked version:") << " " << sdl_version_int_as_string(SDL_GetVersion()) << "\n";
	infoStream << _("SDL_image compiled version:") << " " << sdl_version_int_as_string(SDL_IMAGE_VERSION) << "\n";
	infoStream << _("SDL_image linked version:") << " " << sdl_version_int_as_string(IMG_Version()) << "\n";
	infoStream << _("SDL_mixer compiled version:") << " " << sdl_version_int_as_string(SDL_MIXER_VERSION) << "\n";
	infoStream << _("SDL_mixer linked version:") << " " << sdl_version_int_as_string(MIX_Version()) << "\n";
	infoStream << _("SDL_ttf compiled version:") << " " << sdl_version_int_as_string(SDL_TTF_VERSION) << "\n";
	infoStream << _("SDL_ttf linked version:") << " " << sdl_version_int_as_string(TTF_Version()) << "\n";
	PHYSFS_Version physfs_compiled_version;
	PHYSFS_Version physfs_linked_version;
	PHYSFS_VERSION(&physfs_compiled_version);
	PHYSFS_getLinkedVersion(&physfs_linked_version);
	infoStream << _("PhysFS compiled version:") << " " << sdl_verison_as_string(physfs_compiled_version) << "\n";
	infoStream << _("PhysFS linked version:") << " " << sdl_verison_as_string(physfs_linked_version) << "\n";
	infoStream << _("Save folder:") << " " << PHYSFS_getWriteDir() << "\n";
	infoStream << _("Locale:") << " " << setlocale( LC_CTYPE, nullptr ) << "\n";
	infoStream << "SHAREDIR: " << SHAREDIR << "\n";
	setHelp30FontThinOutline(&globalData.spriteHolder->GetDataHolder(), titleField, _("About"));
	setHelpBoxFont(&globalData.spriteHolder->GetDataHolder(), infoBox, infoStream.str().c_str());
	sago::WriteFileContent("about.txt", infoStream.str());
}

HelpAboutState::~HelpAboutState() {
}
