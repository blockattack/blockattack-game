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

#ifndef SAGOSPRITEHOLDER_HPP
#define	SAGOSPRITEHOLDER_HPP

#include "SagoDataHolder.hpp"
#include "SagoSprite.hpp"
#include <vector>

namespace sago {

class SagoSpriteHolder final {
public:
	explicit SagoSpriteHolder(const SagoDataHolder &texHolder);
	~SagoSpriteHolder();
	/**
	 * Reads all the sprites from the "sprites" directory in alphabetical order
	 **/
	void ReadSprites();
	/**
	 * Reads an additional sprites. Used to append mod data.
	 **/
	void ReadSprites(const std::vector<std::string>& extra_sprites);
	const sago::SagoSprite& GetSprite(const std::string &spritename) const;
	const SagoDataHolder& GetDataHolder() const;
private:
	SagoSpriteHolder(const SagoSpriteHolder& base) = delete;
	SagoSpriteHolder& operator=(const SagoSpriteHolder& base) = delete;
	void ReadSpriteFile(const std::string &filename);
	struct SagoSpriteHolderData;
	SagoSpriteHolderData *data;
};

}

#endif	/* SAGOSPRITEHOLDER_HPP */

