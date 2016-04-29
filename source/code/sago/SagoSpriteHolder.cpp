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
#ifndef _WIN32
#include <jsoncpp/json/json.h>
#else
#include <json/json.h>
#endif
#include <iostream>
#include <string.h>
#include <boost/algorithm/string/predicate.hpp>


//I truely hate the C way of checking for equal. Usually read: "if not X compares to Y then they must be equal"
#define strequal(X,Y) strcmp(X,Y)==0

using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;

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

void SagoSpriteHolder::ReadSpriteFile(const std::string& filename) {
	string fullfile = "sprites/"+filename;
	string content = sago::GetFileContent(fullfile.c_str());
	Json::Value root;   // will contains the root value after parsing
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( content, root );
	if ( !parsingSuccessful ) {
		cerr << "Failed to parse: " << fullfile << endl
		     << reader.getFormattedErrorMessages() << endl;
		return;
	}
	for (Json::Value::iterator it = root.begin(); it != root.end() ; ++it) {
		string spriteName = it.key().asString();
		Json::Value value = (*it);
		string textureName = value.get("texture","fallback").asString();
		int topx = value.get("topx",0).asInt();
		int topy = value.get("topy",0).asInt();
		int height = value.get("height",0).asInt();
		int width = value.get("width",0).asInt();
		int number_of_frames = value.get("number_of_frames",1).asInt();
		int frame_time = value.get("frame_time",1).asInt();
		int originx = value.get("originx",0).asInt();
		int originy = value.get("originy",0).asInt();
		if (number_of_frames < 1) {
			number_of_frames = 1;
		}
		if (frame_time < 1) {
			frame_time = 1;
		}
		std::shared_ptr<sago::SagoSprite> ptr(new SagoSprite(*(data->tex),textureName, {topx,topy,width,height},number_of_frames,frame_time));
		ptr->SetOrigin({originx,originy, 0, 0});
		this->data->sprites[std::string(spriteName)] = ptr;
	}
}

void SagoSpriteHolder::ReadSprites() {
	std::vector<std::string> spritefiles = GetFileList("sprites");
	for (std::string& item : spritefiles  ) {
		if (boost::algorithm::ends_with(item,".sprite")) {
			if (data->verbose) {
				cout << "Found " << item << endl;
			}
			ReadSpriteFile(item);
		}
		else {
			if (data->verbose) {
				cout << "Ignoreing " << item << endl;
			}
		}
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
