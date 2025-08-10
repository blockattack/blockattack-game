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

#include "editor_loader.hpp"
#include "../sago/SagoMisc.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <iostream>


void to_json(nlohmann::json& j, const SagoSprite& p) {
	j = nlohmann::json{{"texture", p.texture}, {"topx", p.topx}, {"topy", p.topy}, {"height", p.height}, {"width", p.width}, {"number_of_frames", p.number_of_frames}, {"frame_time", p.frame_time}, {"originx", p.originx}, {"originy", p.originy}};
}

void from_json(const nlohmann::json& j, SagoSprite& p) {
	j.at("texture").get_to(p.texture);
	j.at("topx").get_to(p.topx);
	j.at("topy").get_to(p.topy);
	j.at("height").get_to(p.height);
	j.at("width").get_to(p.width);
	if (j.contains("number_of_frames")) {
		j.at("number_of_frames").get_to(p.number_of_frames);
	}
	if (j.contains("frame_time")) {
		j.at("frame_time").get_to(p.frame_time);
	}
	if(j.contains("originx")) {
		j.at("originx").get_to(p.originx);
	}
	if(j.contains("originy")) {
		j.at("originy").get_to(p.originy);
	}
}

void from_json(const nlohmann::json& j, SagoSpriteFile& p) {
	std::cout << "from_json\n";
	for (auto& [key, value] : j.items()) {
		SagoSprite s;
		if (!value.is_object()) {
			continue;
		}
		from_json(value, s);
		p.sprites[key] = s;
	}
}

void AppendFromFile(const std::string& json_content, const std::string& filename, std::map<std::string, SagoSprite>& sprites) {
	nlohmann::json j = nlohmann::json::parse(json_content);
	//std::map<std::string, SagoSprite> file;
	for (auto& sprite : j.items()) {
		std::cout << sprite.key() << "\n";
		if (!sprite.value().is_object()) {
			std::cout << "Not an object\n";
			continue;
		}
		std::string name = sprite.key();
		SagoSprite s = sprite.value();
		s.meta_from_file = filename;
		s.meta_name = name;
		sprites[name] = s;
	}
}

std::map<std::string, SagoSprite> LoadSprites() {
	std::map<std::string, SagoSprite> sprites;
	std::vector<std::string> spritefiles = sago::GetFileList("sprites");
	for (std::string& item : spritefiles  ) {
		if (boost::algorithm::ends_with(item,".sprite")) {
			std::string filename = "sprites/"+item;
			std::string content = sago::GetFileContent(filename.c_str());
			if (content.empty()) {
				std::cout << "File \"" << filename << "\" does not exit!\n";
				return sprites;
			}
			std::cout << "Reading " << filename << "\n";
			AppendFromFile(content, item, sprites);
		}
	}
	return sprites;
}