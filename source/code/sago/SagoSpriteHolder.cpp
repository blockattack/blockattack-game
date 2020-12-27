/*
Copyright (c) 2015 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SagoSpriteHolder.hpp"
#include "SagoMisc.hpp"
#include <memory>
#include <unordered_map>
#include "rapidjson/document.h"
#include <iostream>
#include <string.h>
#include <boost/algorithm/string/predicate.hpp>


namespace sago {

struct SagoSpriteHolder::SagoSpriteHolderData {
	const sago::SagoDataHolder* tex;
	std::unordered_map<std::string,std::shared_ptr<sago::SagoSprite>> sprites;
	const sago::SagoSprite* defaultSprite;
	bool verbose = false;
};

SagoSpriteHolder::SagoSpriteHolder(const SagoDataHolder& texHolder) {
	data = new SagoSpriteHolderData();
	try {
		data->tex = &texHolder;
		ReadSprites();
		data->defaultSprite = new sago::SagoSprite(texHolder,"fallback", {0,0,64,64},1,100);
	}
	catch (...) {
		delete data;
	}
}

SagoSpriteHolder::~SagoSpriteHolder() {
	delete data;
}

static int getDefaultValue(const rapidjson::Value& value, const char* name, int defaultValue) {
	assert(value.IsObject());
	const auto& t = value.GetObject().FindMember(name);
	if (t != value.MemberEnd() && t->value.IsInt()) {
		return t->value.GetInt();
	}
	return defaultValue;
}

static std::string getDefaultValue(const rapidjson::Value& value, const char* name, std::string defaultValue) {
	assert(value.IsObject());
	const auto& t = value.GetObject().FindMember(name);
	if (t != value.MemberEnd() && t->value.IsString()) {
		defaultValue = t->value.GetString();
	}
	return defaultValue;
}

void SagoSpriteHolder::ReadSpriteFile(const std::string& filename) {
	std::string fullfile = "sprites/"+filename;
	std::string content = sago::GetFileContent(fullfile.c_str());
	rapidjson::Document document;
	document.Parse(content.c_str());
	if ( !document.IsObject() ) {
		std::cerr << "Failed to parse: " << fullfile << "\n";
		return;
	}
	for (auto& m : document.GetObject()) {
		const std::string& spriteName = m.name.GetString();
		if (!m.value.IsObject()) {
			if (spriteName[0] != '_') {
				std::cerr << "Invalid sprite: " << spriteName << "\n";
			}
			continue;
		}
		std::string textureName = getDefaultValue(m.value, "texture", "fallback");
		int topx = getDefaultValue(m.value, "topx", 0);
		int topy = getDefaultValue(m.value, "topy",0);
		int height = getDefaultValue(m.value, "height",0);
		int width = getDefaultValue(m.value, "width",0);
		int number_of_frames = getDefaultValue(m.value, "number_of_frames",1);
		int frame_time = getDefaultValue(m.value, "frame_time",1);
		SDL_Rect origin = {};
		origin.x = getDefaultValue(m.value, "originx",0);
		origin.y = getDefaultValue(m.value, "originy",0);
		if (number_of_frames < 1) {
			number_of_frames = 1;
		}
		if (frame_time < 1) {
			frame_time = 1;
		}
		std::shared_ptr<sago::SagoSprite> ptr(new SagoSprite(*(data->tex),textureName, {topx,topy,width,height},number_of_frames,frame_time));
		ptr->SetOrigin(origin);
		this->data->sprites[std::string(spriteName)] = ptr;
	}
}

void SagoSpriteHolder::ReadSprites() {
	std::vector<std::string> spritefiles = GetFileList("sprites");
	for (std::string& item : spritefiles  ) {
		if (boost::algorithm::ends_with(item,".sprite")) {
			if (data->verbose) {
				std::cout << "Found " << item << "\n";
			}
			ReadSpriteFile(item);
		}
		else {
			if (data->verbose) {
				std::cout << "Ignoreing " << item << "\n";
			}
		}
	}
}

void SagoSpriteHolder::ReadSprites(const std::vector<std::string>& extra_sprites) {
	for (std::string item : extra_sprites) {
		item+=".sprite";
		ReadSpriteFile(item);
	}
}

const sago::SagoSprite& SagoSpriteHolder::GetSprite(const std::string& spritename) const {
	std::unordered_map<std::string,std::shared_ptr<sago::SagoSprite>>::const_iterator got = data->sprites.find (spritename);
	if ( got == data->sprites.end() ) {
		return *data->defaultSprite;
	}
	else {
		return *(got->second);
	}
}

const SagoDataHolder& SagoSpriteHolder::GetDataHolder() const {
	return *data->tex;
}

}
