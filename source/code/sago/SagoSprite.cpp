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

#include "SagoSprite.hpp"

namespace sago {

struct SagoSprite::SagoSpriteData {
	SDL_Texture* tex;
	SDL_Rect imgCord;
	SDL_Rect origin = {};
	int aniFrames = 0;
	int aniFrameTime = 0;
};

SagoSprite::SagoSprite(const SagoDataHolder& texHolder, const std::string& texture,const SDL_Rect& initImage,const int animationFrames, const int animationFrameLength) {
	data = new SagoSpriteData();
	data->tex = texHolder.getTexturePtr(texture);
	data->imgCord = initImage;
	data->aniFrames = animationFrames;
	data->aniFrameTime = animationFrameLength;
}



SagoSprite::~SagoSprite() {
	delete data;
}

void SagoSprite::Draw(SDL_Renderer* target, Sint32 frameTime, float x, float y) const {
	SDL_Rect rect = data->imgCord;
	rect.x+=rect.w*((frameTime/data->aniFrameTime)%data->aniFrames);
	SDL_Rect pos = rect;
	pos.x = x;
	pos.y = y;
	SDL_RenderCopy(target, data->tex, &rect, &pos);
}

void SagoSprite::Draw(SDL_Renderer* target, Sint32 frameTime, float x, float y, const SDL_Rect& part) const {
	SDL_Rect rect = data->imgCord;
	rect.x+=rect.w*((frameTime/data->aniFrameTime)%data->aniFrames);
	rect.x += part.x;
	rect.y += part.y;
	rect.w = part.w;
	rect.h = part.h;
	SDL_Rect pos = rect;
	pos.x = x;
	pos.y = y;
	SDL_RenderCopy(target, data->tex, &rect, &pos);
}

void SagoSprite::SetOrigin(const SDL_Rect& newOrigin) {
	data->origin = newOrigin;
}

}  //namespace sago