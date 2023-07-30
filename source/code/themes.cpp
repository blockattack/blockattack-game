/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2023 Poul Sander

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
https://www.blockattack.net
===========================================================================
*/


#include "themes.hpp"

#include <vector>
#include <unordered_map>

static std::vector<Theme> themes(1);
static std::unordered_map<std::string, BackGroundData> background_data;
static bool initialized = false;
static size_t current_theme = 0;

static void InitBackGroundData() {
	BackGroundData standard;
	standard.background_name = "standard";
	standard.background_sprite = "background";
	standard.background_sprite_16x9 = "background_sixteen_nine";
	standard.background_scale = ImgScale::Stretch;
	background_data["standard"] = standard;
	BackGroundData alt_background;
	alt_background.background_name = "alt_background";
	alt_background.background_sprite = "background_sample";
	alt_background.background_sprite_16x9 = "";
	alt_background.background_scale = ImgScale::Tile;
	alt_background.tileMoveSpeedX = 50;
	alt_background.tileMoveSpeedY = 100;
	background_data["alt_background"] = alt_background;
}

static void FillMissingFields(Theme &theme) {
	if (theme.background.background_name.empty()) {
		//If the theme does not define a background then use the standard.
		theme.background = background_data["standard"];
	}
}

void InitThemes() {
	if (initialized) {
		return;
	}
	InitBackGroundData();
	themes.resize(1);  //Add the default theme
	FillMissingFields(themes[0]);
	Theme snow;
	snow.theme_name = "snow";
	snow.back_board = "back_board_sample_snow";
	snow.background = background_data["alt_background"];
	FillMissingFields(snow);
	themes.push_back(snow);
}

Theme getNextTheme() {
	InitThemes();
	current_theme++;
	current_theme = current_theme % themes.size();
	return themes.at(current_theme);
}

Theme getTheme(size_t theme_number) {
	InitThemes();
	return themes.at(theme_number % themes.size());
}
