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

#include "SDL.h"
#include <SDL_mixer.h>      //Used for sound & music
#include <SDL_image.h>      //To load PNG images!
#include <SDL_ttf.h>
#include <physfs.h>         //Abstract file system. To use containers
#include <string>

#ifndef TEXTUREHOLDER_HPP
#define	TEXTUREHOLDER_HPP

namespace sago {

class SagoDataHolder;

class TextureHandler {
public:
	TextureHandler() {};
	TextureHandler(const SagoDataHolder* holder, const std::string &textureName);
	SDL_Texture* get();
private:
	std::string textureName;
	const SagoDataHolder* holder = nullptr;
	SDL_Texture* data = nullptr;
	Uint64 version = 0;
};


class MusicHandler final {
public:
	MusicHandler() {};
	MusicHandler(const SagoDataHolder* holder, const std::string &musicName);
	Mix_Music* get();
private:
	std::string musicName;
	const SagoDataHolder* holder = nullptr;
	Mix_Music* data = nullptr;
	Uint64 version = 0;
};


class SoundHandler final {
public:
	SoundHandler() {};
	SoundHandler(const SagoDataHolder* holder, const std::string &soundName);
	Mix_Chunk* get();
private:
	std::string soundName;
	const SagoDataHolder* holder = nullptr;
	Mix_Chunk* data = nullptr;
	Uint64 version = 0;
};

class SagoDataHolder final {
public:
	/**
	 * The renderer must be set before requesting a texture.
	 * If the constructor without elements is used then invalidateAll(SDL_Renderer*) must be called before getTexturePtr
	 */
	SagoDataHolder();
	explicit SagoDataHolder(SDL_Renderer* renderer);

	/**
	 * Return a pointer to the given texture. The pointer is valid for the lifetime of SagoDataHolder object it was taken from or until invalidateAll is called.
	 * @param textureName Name of the texture
	 * @return  Pointer to the loaded texture
	 */
	SDL_Texture* getTexturePtr(const std::string &textureName) const;

	TextureHandler getTextureHandler(const std::string &textureName) const;
	TTF_Font* getFontPtr(const std::string &fontName, int ptsize) const;
	Mix_Music* getMusicPtr(const std::string &musicName) const;
	MusicHandler getMusicHandler(const std::string &musicName) const;
	Mix_Chunk* getSoundPtr(const std::string &soundName) const;
	SoundHandler getSoundHandler(const std::string &soundName) const;
	void setVerbose(bool value);

	/**
	 * Invalidates all pointers returned by any of the get variables
	 */
	void invalidateAll();

	/**
	 * Invalidates all pointers returned by any of the get variables.
	 * Also sets a new renderer.
	 *
	 * Setting a new renderer might cause all old textures to no longer match the renderer format.
	 */
	void invalidateAll(SDL_Renderer* renderer);

	/**
	 * The version number. Changes everytime the pointers are invalidated.
	 * Can be used to determen if it is neccecary to get a new pointer.
	 * @return A globally unique number.
	 */
	Uint64 getVersion() const;
	
	~SagoDataHolder();
private:
	SagoDataHolder(const SagoDataHolder& base) = delete;
	SagoDataHolder& operator=(const SagoDataHolder& base) = delete;
	struct SagoDataHolderData;
	mutable SagoDataHolderData *data;
};

} //namespace sago

#endif	/* TEXTUREHOLDER_HPP */

