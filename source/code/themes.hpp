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

#pragma once

#include <string>
#include <vector>

enum class ImgScale {
	Stretch,
	Tile,
	Resize,
	Cut
};

struct BackGroundData {
	std::string name = "standard";
	std::string background_sprite = "";
	std::string background_sprite_16x9 = "";
	int tileMoveSpeedX = 0; //Move the tiled background every X milliseconds. 0 disables it.
	int tileMoveSpeedY = 0; //Move the tiled background every X milliseconds. 0 disables it.
	ImgScale background_scale = ImgScale::Stretch;
};


struct ThemeBorderData {
	std::string name = "standard";
	std::string border_sprite = "board_back_back";
	std::pair<int, int> border_sprite_offset = {-60, -68};
	std::pair<int, int> score_label_offset = {310, 80};
	std::pair<int, int> time_label_offset = {310,129};
	std::pair<int, int> chain_label_offset = {310, 178};
	std::pair<int, int> speed_label_offset = {310, 227};
};


struct DecorationData {
	std::string name = "smilies";
	std::vector<std::string> decoration_sprites = {"smileys0", "smileys1", "smileys2", "smileys3"};
};

struct Theme {
	std::string theme_name = "standard";
	std::string back_board = "back_board";  // Can also be "back_board_sample_snow" or "trans_cover"
	BackGroundData background;  //Serialized as background.background_name
	DecorationData decoration;
	ThemeBorderData border;
};

struct ThemeFileData {
	std::vector<BackGroundData> background_data;
	std::vector<DecorationData> decoration_data;
	std::vector<Theme> themes;
};

void ThemesFillMissingFields(Theme& theme);

void ThemesAddOrReplace(const Theme& theme);


/**
 * @brief returns a theme from a list
 * @return A copy of a theme
 */
Theme ThemesGetNext();

/**
 * @brief getTheme returns a specific theme
 * @param theme_number
 * @return A reference to the theme. The referance is valid until the next call to a "Themes*" function
 */
Theme& ThemesGet(size_t theme_number);

/**
 * @brief Saves the themes starting with "custom_slot_" to "custom_themes.json"
 *
 */
void ThemesSaveCustomSlots();

/**
 * Returns the index of a given file.
 * Returns 0 if it does not exist or is the standard theme.
*/
size_t ThemesGetNumber(const std::string& name);

bool ThemesBackgroundExists(const std::string& name);

BackGroundData ThemesGetNextBackground(const std::string& current);

std::string ThemesGetNextBoardBackground(const std::string& current);

ThemeBorderData ThemesGetBorder(const std::string& name);

ThemeBorderData ThemesGetNextBorder(const std::string& current);

void ThemesInitCustomBackgrounds();