/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2020 Poul Sander

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

#include "ModInfo.hpp"
#include "sago/SagoMisc.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

void ModInfo::InitModList(const std::vector<std::string>& modlist) {
	this->mod_list = modlist;
	sprite_filename_list.clear();
	for (const std::string& s : mod_list) {
		std::string filename_info = std::string("modinfo/") + s + ".json";
		if (sago::FileExists(filename_info.c_str())) {
			std::string content = sago::GetFileContent(filename_info.c_str());
			rapidjson::Document document;
			document.Parse(content.c_str());
			if ( !document.IsObject() ) {
				std::cerr << "Failed to parse: " << filename_info << "\n";
				continue;
			}
			const auto& t = document.GetObject().FindMember("sprites");
			if (t != document.MemberEnd() && t->value.IsString()) {
				std::string sprites = t->value.GetString();
				std::vector<std::string> sprites_vector;
				boost::split(sprites_vector, sprites, boost::is_any_of(","));
				sprite_filename_list.insert(sprite_filename_list.end(), sprites_vector.begin(), sprites_vector.end());
			}
		}
	}
}

void ModInfo::ParseModFile(const std::string& content) {
	std::istringstream ss(content);
	std::string line;
	while (std::getline(ss, line)) {
		std::vector<std::string> line_vector;
		boost::split(line_vector, line, boost::is_any_of(","));
		if (line_vector.size() < 2) {
			std::cerr << "skipping " << line << "\n";
			continue;
		}
		if (line_vector[1] == "1") {
			this->mod_list.push_back(line_vector[0]);
		}
	}
	InitModList(this->mod_list);
}

const std::vector<std::string>& ModInfo::getModList() {
	return this->mod_list;
}

const std::vector<std::string>& ModInfo::getModSpriteFiles() {
	return this->sprite_filename_list;
}
