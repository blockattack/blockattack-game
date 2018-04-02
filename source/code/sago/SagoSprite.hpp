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

#ifndef SAGOSPRITE_HPP
#define	SAGOSPRITE_HPP

#include "SagoDataHolder.hpp"

namespace sago {

class SagoSprite final {
public:
	SagoSprite();
	SagoSprite(const SagoDataHolder &texHolder, const std::string &texture,const SDL_Rect& initImage,const int animationFrames, const int animationFrameLength);
	/**
	 * Draws the sprite to a given render window
	 * @param target The render window to draw on
	 * @param frameTime The time in milliseonds since gamestart. Used to determen the place in the animation
	 * @param x Place to draw the sprite
	 * @param y Place to draw the sprite
	 */
	void Draw(SDL_Renderer* target, Sint32 frameTime, int x, int y) const;
	/**
	 * Draws part of the sprite to a given render window
	 * @param target The render window to draw on
	 * @param frameTime The time in milliseonds since gamestart. Used to determen the place in the animation
	 * @param x Place to draw the sprite
	 * @param y Place to draw the sprite
	 * @param part the part of the sprite that should be drawn. 
	 */
	void Draw(SDL_Renderer* target, Sint32 frameTime, int x, int y, const SDL_Rect& part) const;
	/**
	 * Draws the wprite to the given renderer but makes sure to not draw outside th bounds given
	 * @param target The render window to draw on
	 * @param frameTime The time in milliseonds since gamestart. Used to determen the place in the animation
	 * @param x Place to draw the sprite
	 * @param y Place to draw the sprite
	 * @param bounds A recagular area that we must not draw outside.
	 */
	void DrawBounded(SDL_Renderer* target, Sint32 frameTime, int x, int y, const SDL_Rect& bounds) const;/**
	 * Draws the sprite to a given render window
	 * @param target The render window to draw on
	 * @param progress A float with value from 0.0f to 1.0f. Tells how far in the animation that we got
	 * @param x Place to draw the sprite
	 * @param y Place to draw the sprite
	 */
	void DrawProgressive(SDL_Renderer* target, float progress, int x, int y) const;
	void DrawScaled(SDL_Renderer* target, Sint32 frameTime, int x, int y, int w, int h) const;
	/**
	 * Set a different origin. Normally it is the top left cornor. But in some cases you might want to center the origin or tranform it for other reasons
	 * @param newOrigin the coordinates that should be the new origin. Call with {0,0} to reset to default 
	 */
	void SetOrigin(const SDL_Rect& newOrigin);
	SagoSprite(const SagoSprite& base);
	SagoSprite& operator=(const SagoSprite& base);
	int GetWidth() const;
	int GetHeight() const;
	~SagoSprite();
private:
	struct SagoSpriteData;
	SagoSpriteData *data;
};

}

#endif	/* SAGOSPRITE_HPP */

