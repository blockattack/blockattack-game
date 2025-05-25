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
#include "SagoMiscSdl2.hpp"
#include "SagoMisc.hpp"

#if PHYSFS_VER_MAJOR < 3
#define PHYSFS_readBytes(X,Y,Z) PHYSFS_read(X,Y,1,Z)
#define PHYSFS_writeBytes(X,Y,Z) PHYSFS_write(X,Y,1,Z)
#endif

namespace sago {

struct SagoDataHolder::SagoDataHolderData {
	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, std::map<int, TTF_Font*> > fonts;  //font, ptsize
	std::map<std::string, Mix_Music*> music;
	std::map<std::string, Mix_Chunk*> sounds;
	std::vector<SDL_RWops*> rwOpsToFree;
	std::vector<std::unique_ptr<char[]>> dataToFree;
	bool verbose = false;
	Uint64 version = 1;
	SDL_Renderer* renderer = nullptr;
};

static void printFileWeLoad(const std::string& value) {
	std::cout << "Loading " << value << "\n";
}

SagoDataHolder::SagoDataHolder() {
	data = new SagoDataHolderData();
}

SagoDataHolder::SagoDataHolder(SDL_Renderer* renderer) {
	data = new SagoDataHolderData();
	data->renderer = renderer;
}

void SagoDataHolder::invalidateAll(SDL_Renderer* renderer) {
	invalidateAll();
	data->renderer = renderer;
}

void SagoDataHolder::invalidateAll() {
	data->version++;
	for (auto& item : data->textures) {
		SDL_DestroyTexture(item.second);
	}
	data->textures.clear();
	for (auto& item : data->music) {
		Mix_FreeMusic(item.second);
	}
	data->music.clear();
	for (auto& item : data->sounds) {
		Mix_FreeChunk(item.second);
	}
	data->sounds.clear();
	for (auto& item : data->fonts) {
		for (auto& item2 : item.second) {
			TTF_CloseFont(item2.second);
		}
	}
	data->fonts.clear();
	for (auto& item : data->rwOpsToFree) {
		SDL_FreeRW(item);
	}
	data->rwOpsToFree.clear();
}

SagoDataHolder::~SagoDataHolder() {
	invalidateAll();
	delete data;
}

SDL_Texture* SagoDataHolder::getTexturePtr(const std::string& textureName) const {
	if (!data->renderer) {
		throw std::runtime_error("SagoDataHolder used before setting the renderer");
	}
	SDL_Texture* ret = data->textures[textureName];
	if (ret) {
		return ret;
	}
	std::string path = "textures/"+textureName+".png";
	if (data->verbose) {
		printFileWeLoad(path);
	}
	if (!PHYSFS_exists(path.c_str())) {
		// We did not find the png file. Try to see if there are a jpg file.
		std::string jpg_path = "textures/"+textureName+".jpg";
		if (!PHYSFS_exists(jpg_path.c_str())) {
			sago::SagoFatalErrorF("getTextureFailed - Texture does not exist: %s", path.c_str());
		}
		path = jpg_path;
	}
	unsigned int m_size = 0;
	std::unique_ptr<char[]> m_data;
	ReadBytesFromFile(path.c_str(), m_data, m_size);
	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);
	//The above might fail an return null.
	if (!rw) {
		std::cerr << "Error. Corrupt data file!\n";
		return NULL;
	}
	SDL_Surface* surface = IMG_Load_RW(rw,true);

	ret = SDL_CreateTextureFromSurface(data->renderer, surface);

	if (!ret) {
		std::cerr << "getTextureFailed to load " << path << "\n";
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
		std::cerr << "getFontPtr - Font does not exists: " << path << "\n";
		return ret;
	}
	unsigned int m_size = 0;
	std::unique_ptr<char[]> m_data;
	ReadBytesFromFile(path.c_str(), m_data, m_size);

	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);

	//The above might fail an return null.
	if (!rw) {
		std::cerr << "Error: Corrupt data file!\n";
		return ret;
	}

	ret = TTF_OpenFontRW(rw, SDL_FALSE, ptsize);
	if (!ret) {
		std::cerr << "Error openening font: " << fontName << " because: " << TTF_GetError() << "\n";
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
		std::cerr << "getMusicPtr - Music file does not exists: " << path << "\n";
		return ret;
	}
	unsigned int m_size = 0;
	std::unique_ptr<char[]> m_data;
	ReadBytesFromFile(path.c_str(), m_data, m_size);
	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);

	//The above might fail an return null.
	if (!rw) {
		std::cerr << "Error. Corrupt data file!\n";
		return NULL;
	}

	ret = Mix_LoadMUS_RW(rw, SDL_TRUE);  //SDL_TRUE causes rw to be freed

	if (!ret) {
		std::cerr << "getMusicPtr to load " << path << " because: " << Mix_GetError() << "\n";
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
		std::cerr << "getSoundPtr - Sound file does not exists: " << path << "\n";
		return ret;
	}
	unsigned int m_size = 0;
	std::unique_ptr<char[]> m_data;
	ReadBytesFromFile(path.c_str(), m_data, m_size);
	SDL_RWops* rw = SDL_RWFromMem (m_data.get(), m_size);

	//The above might fail an return null.
	if (!rw) {
		std::cerr << "Error. Corrupt data file!\n";
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

Uint64 SagoDataHolder::getVersion() const {
	return data->version;
}

TextureHandler::TextureHandler(const SagoDataHolder* holder, const std::string& textureName) {
	this->holder = holder;
	this->version = 0;
	this->textureName = textureName;
	this->data = nullptr;
}

SDL_Texture* TextureHandler::get() const {
	if (version != holder->getVersion()) {
		//The holder has been invalidated
		this->data = this->holder->getTexturePtr(textureName);
	}
	return data;
}


MusicHandler::MusicHandler(const SagoDataHolder* holder, const std::string& musicName) {
	this->holder = holder;
	this->version = 0;
	this->musicName = musicName;
	this->data = nullptr;
}

Mix_Music* MusicHandler::get() {
	if (version != holder->getVersion()) {
		//The holder has been invalidated
		this->data = this->holder->getMusicPtr(musicName);
	}
	return data;
}

SoundHandler::SoundHandler(const SagoDataHolder* holder, const std::string& soundName) {
	this->holder = holder;
	this->version = 0;
	this->soundName = soundName;
	this->data = nullptr;
}

Mix_Chunk* SoundHandler::get() {
	if (version != holder->getVersion()) {
		//The holder has been invalidated
		this->data = this->holder->getSoundPtr(soundName);
	}
	return data;
}


TextureHandler SagoDataHolder::getTextureHandler(const std::string& textureName) const {
	return TextureHandler(this, textureName);
}

MusicHandler SagoDataHolder::getMusicHandler(const std::string& musicName) const {
	return MusicHandler(this, musicName);
}

SoundHandler SagoDataHolder::getSoundHandler(const std::string& soundName) const {
	return SoundHandler(this, soundName);
}

} //name space sago
