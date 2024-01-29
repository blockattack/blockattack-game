/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2013 Poul Sander

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
http://blockattack.net
===========================================================================
*/
#include "global.hpp"
#include "menudef_themes.hpp"
#include "MenuSystem.h"
#include "BlockGameSdl.hpp"
#include <fmt/core.h>

static void switchTheme() {
	globalData.theme = ThemesGetNext();
}


static void testMusic() {
	static bool highbeatNext = false;
	int musicVolume = Config::getInstance()->getInt("volume_music");
	std::string music_name = "bgmusic";
	if (highbeatNext) {
		music_name = "highbeat";
	}
	Mix_PlayMusic(globalData.spriteHolder->GetDataHolder().getMusicHandler(music_name.c_str()).get(), 1);
	Mix_VolumeMusic(musicVolume);
	highbeatNext = !highbeatNext; //Toggle between standard and highbeat
}



class ThemesMenu : public Menu {
public:
	std::shared_ptr<BlockGameSdl> game;
	sago::SagoTextField themeTitle;

	ThemesMenu(SDL_Renderer* screen, const std::string& title, bool submenu) : Menu(screen, title, submenu) {
		game = std::make_shared<BlockGameSdl>(globalData.xsize-450,100,&globalData.spriteHolder->GetDataHolder());
		game->putSampleBlocks();
		sagoTextSetBlueFont(themeTitle);
	}

	void placeButtons() override {
		int nextY = 100;
		int X = 10;
		for (Button* it : buttons) {
			it->x = X;
			it->y = nextY;
			nextY += it->standardButton.ysize+10;
		}
		exit.x = X;
		exit.y = nextY;
	}

	void Draw(SDL_Renderer* target) override {
		Menu::Draw(target);
		game->DoPaintJob();
		themeTitle.SetText(fmt::format(_("Theme: {}"), globalData.theme.theme_name));
		themeTitle.Draw(target, 10, globalData.ysize-50,sago::SagoTextField::Alignment::left);
	}
};

static Theme themeBackup;
static Theme themeToEdit;

static void themesEditSwitchBackground() {
	themeToEdit.background = ThemesGetNextBackground(themeToEdit.background.name);
	globalData.theme = themeToEdit;
}

static void themesEditSwitchBoardBackground() {
	themeToEdit.back_board = ThemesGetNextBoardBackground(themeToEdit.back_board);
	globalData.theme = themeToEdit;
}

static void themesEditSlot1() {
	ThemesMenu tem(globalData.screen, _("Edit custom theme 1"), true);
	size_t theme_index = ThemeGetNumber("custom_slot_1");
	if (theme_index == 0) {
		// Theme not found
		return;
	}
	Button bSwitchBackground;
	bSwitchBackground.setLabel(_("Switch background"));
	bSwitchBackground.setAction(&themesEditSwitchBackground);
	tem.addButton(&bSwitchBackground);
	Button bSwitchBoardBackground;
	bSwitchBoardBackground.setLabel(_("Switch board background"));
	bSwitchBoardBackground.setAction(&themesEditSwitchBoardBackground);
	tem.addButton(&bSwitchBoardBackground);
	themeBackup = globalData.theme;
	themeToEdit = ThemesGet(theme_index);
	globalData.theme = themeToEdit;
	RunGameState(tem);
	ThemesAddOrReplace(themeToEdit);
	globalData.theme = themeBackup;
}


void OpenThemesMenu() {
	ThemesMenu tm(globalData.screen, _("Themes"), true);
	Button bSwitchTheme;
	bSwitchTheme.setLabel(_("Switch theme"));
	bSwitchTheme.setAction(&switchTheme);
	tm.addButton(&bSwitchTheme);
	Button bEditTheme1;
	bEditTheme1.setLabel(_("Edit custom theme 1"));
	bEditTheme1.setAction(&themesEditSlot1);
	tm.addButton(&bEditTheme1);
	if (!globalData.NoSound) {
		Button bTestMusic;
		bTestMusic.setLabel(_("Test music"));
		bTestMusic.setAction(&testMusic);
		tm.addButton(&bTestMusic);
	}
	RunGameState(tm);
}
