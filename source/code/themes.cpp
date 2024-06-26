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
#include "sago/SagoMisc.hpp"
#include <vector>
#include <sstream>
#include <map>
#include "nlohmann/json.hpp"
#include <iostream>
#include "global.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <fmt/core.h>

using json = nlohmann::json;


const int NUMBER_OF_CUSTOM_SLOTS = 2;


void to_json(json& j, const ImgScale& p) {
	switch (p) {
	case ImgScale::Stretch:
		j = "stretch";
		break;
	case ImgScale::Tile:
		j = "tile";
		break;
	case ImgScale::Resize:
		j = "resize";
		break;
	case ImgScale::Cut:
		j = "cut";
		break;
	default:
		j = "stretch";
		break;
	}
}

void from_json(const json& j, ImgScale& p) {
	std::string s = j;
	if (s == "stretch") {
		p = ImgScale::Stretch;
	}
	else if (s == "tile") {
		p = ImgScale::Tile;
	}
	else if (s == "resize") {
		p = ImgScale::Resize;
	}
	else if (s == "cut") {
		p = ImgScale::Cut;
	}
	else {
		p = ImgScale::Stretch;
	}
}

void to_json(json& j, const BackGroundData& p) {
	j = json{ {"background_name", p.name}, {"background_sprite", p.background_sprite}, {"background_sprite_16x9", p.background_sprite_16x9}, {"background_scale", p.background_scale}, {"tileMoveSpeedX", p.tileMoveSpeedX}, {"tileMoveSpeedY", p.tileMoveSpeedY} };
}

void to_json(json& j, const Theme& p) {
	j = json{ {"theme_name", p.theme_name}, {"back_board", p.back_board}, {"background_name", p.background.name}, {"decoration_name", p.decoration.name} };
}

void to_json(json& j, const DecorationData& p) {
	j = json{ {"name", p.name}, {"decoration_sprites", p.decoration_sprites} };
}

void to_json(json& j, const ThemeFileData& p) {
	j = json{ {"background_data", p.background_data}, {"decoration_data", p.decoration_data}, {"themes", p.themes} };
}

void from_json(const json& j, BackGroundData& p) {
	j.at("background_name").get_to(p.name);
	j.at("background_sprite").get_to(p.background_sprite);
	j.at("background_sprite_16x9").get_to(p.background_sprite_16x9);
	j.at("background_scale").get_to(p.background_scale);
	j.at("tileMoveSpeedX").get_to(p.tileMoveSpeedX);
	j.at("tileMoveSpeedY").get_to(p.tileMoveSpeedY);
}

void from_json(const json& j, Theme& p) {
	j.at("theme_name").get_to(p.theme_name);
	j.at("back_board").get_to(p.back_board);
	j.at("background_name").get_to(p.background.name);
	j.at("decoration_name").get_to(p.decoration.name);
}

void from_json(const json& j, DecorationData& p) {
	j.at("name").get_to(p.name);
	j.at("decoration_sprites").get_to(p.decoration_sprites);
}

void from_json(const json& j, ThemeFileData& p) {
	if (j.contains("background_data")) {
		j.at("background_data").get_to(p.background_data);
	}
	if (j.contains("decoration_data")) {
		j.at("decoration_data").get_to(p.decoration_data);
	}
	if (j.contains("themes")) {
		j.at("themes").get_to(p.themes);
	}
}



static std::vector<Theme> themes(1);
static std::map<std::string, BackGroundData> background_data;
static std::map<std::string, DecorationData> decoration_data;
static std::map<std::string, ThemeBorderData> border_data;
static bool initialized = false;
static size_t current_theme = 0;


void ThemesFillMissingFields(Theme& theme) {
	if (theme.background.name.empty() || !ThemesBackgroundExists(theme.background.name) ){
		//If the theme does not define a background then use the standard.
		theme.background.name = "standard";
	}
	theme.background = background_data[theme.background.name];
}

static void ThemesReadDataFromFile(const std::string& filename) {
	if (globalData.verboseLevel) {
		std::cout << "Reading theme data from " << filename << "\n";
	}
	std::string s = sago::GetFileContent(filename);
	if (s.empty()) {
		std::cout << "File \"" << filename << "\" does not exit!\n";
		return;
	}
	json j = json::parse(s);
	ThemeFileData tfd = j;
	for (const auto& bg : tfd.background_data) {
		background_data[bg.name] = bg;
	}
	for (const auto& dec : tfd.decoration_data) {
		decoration_data[dec.name] = dec;
	}
	for (const Theme& theme : tfd.themes) {
		if (globalData.verboseLevel) {
			std::cout << "Adding theme " << theme.theme_name << "\n";
		}
		themes.push_back(theme);
		ThemesFillMissingFields(themes.back());
	}
}


static void ThemesInitBackGroundData() {
	BackGroundData standard;
	standard.name = "standard";
	standard.background_sprite = "background";
	standard.background_sprite_16x9 = "background_sixteen_nine";
	standard.background_scale = ImgScale::Stretch;
	background_data[standard.name] = standard;
	DecorationData smileys;
	decoration_data[smileys.name] = smileys;
}

static void ThemesInitBorderData() {
	ThemeBorderData standard;
	standard.name = "standard";
	standard.border_sprite = "board_back_back";
	standard.border_sprite_offset = {-60, -68};
	border_data[standard.name] = standard;
	ThemeBorderData mirror = standard;
	mirror.name = "mirror";
	mirror.border_sprite = "board_back_back_mirror";
	mirror.border_sprite_offset = {-116, -68};
	mirror.score_label_offset = {-100, 80};
	mirror.time_label_offset = {-100,129};
	mirror.chain_label_offset = {-100, 178};
	mirror.speed_label_offset = {-100, 227};
	border_data[mirror.name] = mirror;
}

static bool str_startswith(const std::string& s, const char* prefix) {
	return (s.rfind(prefix, 0) == 0);
}

static void ThemesDumpData() {
	ThemeFileData tfd;
	tfd.themes = themes;
	for (auto& pair : background_data) {
		tfd.background_data.push_back(pair.second);
	}
	for (auto& pair : decoration_data) {
		tfd.decoration_data.push_back(pair.second);
	}
	json j = tfd;
	std::string s = j.dump(4);
	sago::WriteFileContent("themes_dump.json", s);
}

void ThemesSaveCustomSlots() {
	ThemeFileData tfd;
	for (const Theme& theme : themes) {
		if ( str_startswith(theme.theme_name,"custom_slot_")) {
			tfd.themes.push_back(theme);
		}
	}
	json j = tfd;
	std::string s = j.dump(4);
	sago::WriteFileContent("custom_themes.json", s);
}

static void ThemesAddCustomSlot(std::vector<Theme>& themes, int slot_number) {
	std::string slot_name = fmt::format("custom_slot_{}", slot_number);
	for (const Theme& theme : themes) {
		if (theme.theme_name == slot_name) {
			//Custom slot already exists
			return;
		}
	}
	Theme new_theme;
	new_theme.theme_name = slot_name;
	themes.push_back(new_theme);
	ThemesFillMissingFields(themes.back());
}

void ThemesInit() {
	if (initialized) {
		return;
	}
	initialized = true;
	ThemesInitBackGroundData();
	ThemesInitCustomBackgrounds();
	ThemesInitBorderData();
	themes.resize(1);  //Add the default theme
	ThemesFillMissingFields(themes[0]);
	const std::vector<std::string>& theme_files = sago::GetFileList("themes");
	for (const std::string& filename : theme_files) {
		if (boost::algorithm::ends_with(filename,".json")) {
			ThemesReadDataFromFile("themes/"+filename);
		}
	}
	ThemesReadDataFromFile("custom_themes.json");
	for (int i=1; i <= NUMBER_OF_CUSTOM_SLOTS; ++i) {
		ThemesAddCustomSlot(themes, i);
	}
	ThemesDumpData();
}

Theme ThemesGetNext() {
	ThemesInit();
	current_theme++;
	current_theme = current_theme % themes.size();
	return themes.at(current_theme);
}

Theme& ThemesGet(size_t theme_number) {
	ThemesInit();
	return themes.at(theme_number % themes.size());
}

size_t ThemesGetNumber(const std::string& name) {
	size_t ret = 0;  // Default to 0
	ThemesInit();
	for (size_t i = 0; i < themes.size(); ++i) {
		if (themes[i].theme_name == name) {
			ret = i;
		}
	}
	return ret;
}

void ThemesAddOrReplace(const Theme& theme) {
	ThemesInit();
	for (size_t i = 0; i < themes.size(); ++i) {
		if (themes[i].theme_name == theme.theme_name) {
			themes[i] = theme;
			return;
		}
	}
	themes.push_back(theme);
}

bool ThemesBackgroundExists(const std::string& name) {
	ThemesInit();
	return background_data.find(name) != background_data.end();
}

BackGroundData ThemesGetNextBackground(const std::string& current) {
	ThemesInit();
	BackGroundData ret = background_data["standard"];
	for (auto& pair : background_data) {
		if (pair.first == current) {
			auto it = background_data.find(current);
			++it;
			if (it == background_data.end()) {
				it = background_data.begin();
			}
			ret = it->second;
			break;
		}
	}
	return ret;
}

std::string ThemesGetNextBoardBackground(const std::string& current) {
	ThemesInit();
	std::vector<std::string> back_boards = {"back_board", "back_board_sample_snow", "trans_cover"};
	std::string ret = "back_board";
	for (size_t i = 0; i < back_boards.size(); ++i) {
		if (back_boards[i] == current) {
			ret = back_boards[(i + 1) % back_boards.size()];
			break;
		}
	}
	return ret;
}

ThemeBorderData ThemesGetBorder(const std::string& name) {
	ThemesInit();
	auto it = border_data.find(name);
	if (it == border_data.end()) {
		return border_data["standard"];
	}
	return it->second;
}

ThemeBorderData ThemesGetNextBorder(const std::string& current) {
	ThemesInit();
	ThemeBorderData ret = border_data["standard"];
	for (auto& pair : border_data) {
		if (pair.first == current) {
			auto it = border_data.find(current);
			++it;
			if (it == border_data.end()) {
				it = border_data.begin();
			}
			ret = it->second;
			break;
		}
	}
	return ret;
}

void ThemesInitCustomBackgrounds() {
	std::vector<std::string> custom_backgrounds = sago::GetFileList("textures/backgrounds");
	std::stringstream sprite_stream;
	sprite_stream << "{\n";
	bool first = true;
	for (const std::string& filename : custom_backgrounds) {
		std::cout << "Found custom background " << filename << "\n";
		if (boost::algorithm::ends_with(filename, ".png") || boost::algorithm::ends_with(filename, ".jpg")) {
			BackGroundData bg;
			bg.name = filename;
			std::string texture_name = fmt::format("custom_background_{}",filename);;
			bg.background_sprite = texture_name;
			bg.background_sprite_16x9 = texture_name;
			bg.background_scale = ImgScale::Resize;
			background_data[bg.name] = bg;
			if (!first) {
				sprite_stream << ",\n";
			}
			sprite_stream << "\"" << texture_name << "\": {\n";
			sprite_stream << "  \"texture\": \"" << "backgrounds/" << filename.substr(0, filename.size()-4) << "\",\n";
			sprite_stream << "  \"topx\": 0,\n";
			sprite_stream << "  \"topy\": 0,\n";
			sprite_stream << "  \"height\": 7680,\n";
			sprite_stream << "  \"width\": 10240,\n";
			sprite_stream << "  \"number_of_frames\": 1,\n";
			sprite_stream << "  \"frame_time\": 1\n";
			sprite_stream << "}\n";
			first = false;

		}
	}
	sprite_stream << "}\n";
	sago::WriteFileContent("sprites/custom_backgrounds.sprite", sprite_stream.str());
}