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

#include "SagoDataHolder.hpp"
#include <iostream>
#include <mutex>
#include <map>
#include <vector>
#include <physfs.h>
#include <memory>
#include <SDL_mixer.h>

namespace sago {

struct SagoDataHolder::SagoDataHolderData {
	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, std::map<int, TTF_Font*> > fonts;  //font, ptsize
	std::map<std::string, Mix_Music*> music;
	std::map<std::string, Mix_Chunk*> sounds;
	std::vector<SDL_RWops*> rwOpsToFree;
	std::vector<std::unique_ptr<char[]>> dataToFree;
	bool verbose = false;
	SDL_Renderer* renderer = nullptr;
};

static void printFileWeLoad(const std::string& value) {
	std::cout << "Loading " << value << std::endl;
}

SagoDataHolder::SagoDataHolder(SDL_Renderer* renderer) {
	data = new SagoDataHolderData();
	data->renderer = renderer;
}

SagoDataHolder::~SagoDataHolder() {
	for (auto& item : data->textures) {
		SDL_DestroyTexture(item.second);
	}
	for (auto& item : data->music) {
		Mix_FreeMusic(item.second);
	}
	for (auto& item : data->sounds) {
		Mix_FreeChunk(item.second);
	}
	for (auto& item : data->fonts) {
		for (auto& item2 : item.second) {
			TTF_CloseFont(item2.second);
		}
	}
	for (auto& item : data->rwOpsToFree) {
		SDL_FreeRW(item);
	}
	delete data;
}

SDL_Texture* SagoDataHolder::getTexturePtr(const std::string& textureName) const {
	SDL_Texture* ret = data->textures[textureName];
	if (ret) {
		return ret;
	}
	std::string path = "textures/"+textureName+".png";
	if (data->verbose) { 
		printFileWeLoad(path);
	}
	if (!PHYSFS_exists(path.c_str())) {
		std::cerr << "getTextureFailed - Texture does not exists: " << path << std::endl;
		exit(1);
		return ret;
	}
	PHYSFS_file* myfile = PHYSFS_openRead(path.c_str());
	unsigned int m_size = PHYSFS_fileLength(myfile);
	//char* m_data = new char[m_size];
	std::unique_ptr<char[]> m_data(new char[m_size]);
	int length_read = PHYSFS_read (myfile, m_data.get(), 1, m_size);
	if (length_read != (int)m_size) {
		PHYSFS_close(myfile);
		std::cerr << "Error: Curropt data file: " << path << std::endl;
		return ret;
	}
	PHYSFS_close(myfile);
	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);
	//The above might fail an return null.
	if (!rw) {
		PHYSFS_close(myfile);
		std::cerr << "Error. Curropt data file!" << std::endl;
		return NULL;
	}
	SDL_Surface* surface = IMG_Load_RW(rw,true);

	ret = SDL_CreateTextureFromSurface(data->renderer, surface);

	if (!ret) {
		std::cerr << "getTextureFailed to load " << path << std::endl;
	}
	SDL_FreeSurface(surface);
	data->textures[textureName] = ret;
	return ret;
}

TTF_Font* SagoDataHolder::getFontPtr(const std::string& fontName, int ptsize) const {
	TTF_Font* ret = data->fonts[fontName][ptsize];
	if (ret) {
		return ret;
	}
	std::string path = "fonts/"+fontName+".ttf";
	if (data->verbose) { 
		printFileWeLoad(path);
	}
	if (!PHYSFS_exists(path.c_str())) {
		std::cerr << "getFontPtr - Font does not exists: " << path << std::endl;
		return ret;
	}
	PHYSFS_file* myfile = PHYSFS_openRead(path.c_str());
	unsigned int m_size = PHYSFS_fileLength(myfile);
	std::unique_ptr<char[]> m_data(new char[m_size]);
	int length_read = PHYSFS_read (myfile, m_data.get(), 1, m_size);
	if (length_read != (int)m_size) {
		PHYSFS_close(myfile);
		std::cerr << "Error: Curropt data file: " << path << std::endl;
		return ret;
	}
	PHYSFS_close(myfile);

	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);

	//The above might fail an return null.
	if (!rw) {
		PHYSFS_close(myfile);
		std::cerr << "Error: Curropt data file!" << std::endl;
		return ret;
	}

	ret = TTF_OpenFontRW(rw, SDL_FALSE, ptsize);
	//delete [] m_data;
	if (!ret) {
		std::cerr << "Error openening font: " << fontName << " because: " << TTF_GetError() << std::endl;
	}
	data->fonts[fontName][ptsize] = ret;
	data->dataToFree.push_back(std::move(m_data));
	data->rwOpsToFree.push_back(rw);
	return ret;
}

Mix_Music* SagoDataHolder::getMusicPtr(const std::string& musicName) const {
	Mix_Music* ret = data->music[musicName];
	if (ret) {
		return ret;
	}
	std::string path = "music/"+musicName+".ogg";
	if (data->verbose) { 
		printFileWeLoad(path);
	}
	if (!PHYSFS_exists(path.c_str())) {
		std::cerr << "getMusicPtr - Music file does not exists: " << path << std::endl;
		return ret;
	}
	PHYSFS_file* myfile = PHYSFS_openRead(path.c_str());
	unsigned int m_size = PHYSFS_fileLength(myfile);
	std::unique_ptr<char[]> m_data(new char[m_size]);
	int length_read = PHYSFS_read (myfile, m_data.get(), 1, m_size);
	if (length_read != (int)m_size) {
		PHYSFS_close(myfile);
		std::cerr << "Error: Curropt data file: " << path << std::endl;
		return ret;
	}
	PHYSFS_close(myfile);
	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);

	//The above might fail an return null.
	if (!rw) {
		PHYSFS_close(myfile);
		std::cerr << "Error. Curropt data file!" << std::endl;
		return NULL;
	}

	ret = Mix_LoadMUS_RW(rw, SDL_TRUE);  //SDL_TRUE causes rw to be freed

	if (!ret) {
		std::cerr << "getMusicPtr to load " << path << " because: " << Mix_GetError() << std::endl;
	}
	data->music[musicName] = ret;
	data->dataToFree.push_back(std::move(m_data));
	return ret;
}


Mix_Chunk* SagoDataHolder::getSoundPtr(const std::string& soundName) const {
	Mix_Chunk* ret = data->sounds[soundName];
	if (ret) {
		return ret;
	}
	std::string path = "sounds/"+soundName+".ogg";
	if (data->verbose) { 
		printFileWeLoad(path);
	}
	if (!PHYSFS_exists(path.c_str())) {
		std::cerr << "getSoundPtr - Sound file does not exists: " << path << std::endl;
		return ret;
	}
	PHYSFS_file* myfile = PHYSFS_openRead(path.c_str());
	unsigned int m_size = PHYSFS_fileLength(myfile);
	std::unique_ptr<char[]> m_data(new char[m_size]);
	int length_read = PHYSFS_read (myfile, m_data.get(), 1, m_size);
	if (length_read != (int)m_size) {
		PHYSFS_close(myfile);
		std::cerr << "Error: Curropt data file: " << path << std::endl;
		return ret;
	}
	PHYSFS_close(myfile);
	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);

	//The above might fail an return null.
	if (!rw) {
		PHYSFS_close(myfile);
		std::cerr << "Error. Curropt data file!" << std::endl;
		return NULL;
	}

	ret = Mix_LoadWAV_RW(rw, SDL_TRUE);
	data->sounds[soundName] = ret;
	data->dataToFree.push_back(std::move(m_data));
	return ret;
}

void SagoDataHolder::setVerbose(bool value) {
	data->verbose = value;
}

} //name space sago
