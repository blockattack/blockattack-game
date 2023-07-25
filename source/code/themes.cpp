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

static std::vector<Theme> themes(1);
static bool initialized = false;
static size_t current_theme = 0;

void InitThemes() {
	if (initialized) {
		return;
	}
	themes.resize(1);  //Add the default theme
	Theme snow;
	snow.theme_name = "snow";
	snow.back_board = "back_board_sample_snow";
	themes.push_back(snow);
}

Theme getNextTheme() {
	InitThemes();
	current_theme++;
	current_theme = current_theme % themes.size();
	return themes.at(current_theme);
}
