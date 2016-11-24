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

class SagoDataHolder {
public:
	SagoDataHolder(SDL_Renderer* renderer);
	/**
	 * Return a pointer to the given texture. The pointer is valid for the lifetime of SagoDataHolder object it was taken from.
	 * @param textureName Name of the texture
	 * @return  Pointer to the loaded texture
	 */
	SDL_Texture* getTexturePtr(const std::string &textureName) const;
	TTF_Font* getFontPtr(const std::string &fontName, int ptsize) const;
	Mix_Music* getMusicPtr(const std::string &musicName) const;
	Mix_Chunk* getSoundPtr(const std::string &soundName) const;
	void setVerbose(bool value);
	virtual ~SagoDataHolder();
private:
	SagoDataHolder(const SagoDataHolder& base) = delete;
	SagoDataHolder& operator=(const SagoDataHolder& base) = delete;
	struct SagoDataHolderData;
	mutable SagoDataHolderData *data;
};

} //namespace sago

#endif	/* TEXTUREHOLDER_HPP */

