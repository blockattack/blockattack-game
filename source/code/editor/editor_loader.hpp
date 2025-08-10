/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2024 Poul Sander

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
https://github.com/sago007/saland
===========================================================================
*/

#include <string>
#include "nlohmann/json.hpp"
#include <vector>
#include <map>

struct SagoSprite {
	std::string texture;
	int topx = 0;
	int topy = 0;
	int height = 1;
	int width = 1;
	int number_of_frames = 1;
	int frame_time = 1;
	int originx = 0;
	int originy = 0;

	std::string meta_name = "";
	std::string meta_from_file = "<memory>";
	std::string meta_modified = "false";
};

struct SagoSpriteFile {
	std::map<std::string, SagoSprite> sprites;
};

void to_json(nlohmann::json& j, const SagoSprite& p);

void from_json(const nlohmann::json& j, SagoSprite& p);

void from_json(const nlohmann::json& j, SagoSpriteFile& p);

void AppendFromJson(const std::string& json_content, const std::string& filename, std::map<std::string, SagoSprite>& sprites);

std::map<std::string, SagoSprite> LoadSprites();