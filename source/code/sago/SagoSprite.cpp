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
#include <iostream>

#ifndef M_PI
// M_PI is a custom extension that most C++ toolchains provide. Out Windows compiler does not provide it.
# define M_PI 3.14159265358979323846
#endif

namespace sago {

struct SagoSprite::SagoSpriteData {
	TextureHandler tex;
	SDL_Rect imgCord = {};
	SDL_Rect origin = {};
	int aniFrames = 0;
	int aniFrameTime = 0;
};

SagoSprite::SagoSprite() {
	data = new SagoSpriteData();
}

SagoSprite::SagoSprite(const SagoDataHolder& texHolder, const std::string& texture,const SDL_Rect& initImage,const int animationFrames, const int animationFrameLength) {
	data = new SagoSpriteData();
	data->tex = texHolder.getTextureHandler(texture);
	data->imgCord = initImage;
	data->aniFrames = animationFrames;
	data->aniFrameTime = animationFrameLength;
}

SagoSprite::SagoSprite(const SagoSprite& base) : data(new SagoSpriteData(*base.data)) {

}

SagoSprite& SagoSprite::operator=(const SagoSprite& base) {
	*data = *base.data;
	return *this;
}

SagoSprite::~SagoSprite() {
	delete data;
}

void SagoSprite::Draw(SDL_Renderer* target, Sint32 frameTime, int x, int y) const {
	DrawScaled(target, frameTime, x, y, data->imgCord.w, data->imgCord.h);
}

void SagoSprite::DrawRotated(SDL_Renderer* target, Sint32 frameTime, int x, int y, const double angleRadian) const {
	SDL_Point center = {this->data->origin.x, this->data->origin.y};
	DrawScaledAndRotated(target, frameTime, x, y, data->imgCord.w, data->imgCord.h, angleRadian, &center, SDL_FLIP_NONE);
}

void SagoSprite::DrawScaled(SDL_Renderer* target, Sint32 frameTime, int x, int y, int w, int h) const {
	DrawScaledAndRotated(target, frameTime, x, y, w, h, 0.0, nullptr, SDL_FLIP_NONE);
}

void SagoSprite::DrawScaledAndRotated(SDL_Renderer* target, Sint32 frameTime, int x, int y, int w, int h, const double angleRadian, const SDL_Point* center, const SDL_RendererFlip flip) const {
	if (!data->tex.get()) {
		std::cerr << "Texture is null!\n";
	}
	SDL_Rect rect = data->imgCord;
	rect.x+=rect.w*((frameTime/data->aniFrameTime)%data->aniFrames);
	SDL_Rect pos = rect;
	pos.x = x - this->data->origin.x;
	pos.y = y - this->data->origin.y;
	if (w > 0) {
		pos.w = w;
	}
	if (h > 0) {
		pos.h = h;
	}
	double angleDegress = angleRadian/M_PI*180.0;
	SDL_RenderCopyEx(target, data->tex.get(), &rect, &pos, angleDegress, center, flip);
}

void SagoSprite::Draw(SDL_Renderer* target, Sint32 frameTime, int x, int y, const SDL_Rect& part) const {
	SDL_Rect rect = data->imgCord;
	rect.x+=rect.w*((frameTime/data->aniFrameTime)%data->aniFrames);
	rect.x += part.x;
	rect.y += part.y;
	rect.w = part.w;
	rect.h = part.h;
	SDL_Rect pos = rect;
	pos.x = x - this->data->origin.x;
	pos.y = y - this->data->origin.y;
	SDL_RenderCopy(target, data->tex.get(), &rect, &pos);
}

void SagoSprite::DrawProgressive(SDL_Renderer* target, float progress, int x, int y) const {
	Sint32 frameNumber = progress*data->aniFrames;
	Sint32 frameTime = frameNumber*data->aniFrameTime;
	Draw(target, frameTime, x, y);
}

void SagoSprite::DrawBounded(SDL_Renderer* target, Sint32 frameTime, int x, int y, const SDL_Rect& bounds) const {
	SDL_Rect rect = data->imgCord;
	rect.x+=rect.w*((frameTime/data->aniFrameTime)%data->aniFrames);
	SDL_Rect pos = rect;
	pos.x = x;
	pos.y = y;
	if (pos.x > bounds.x+bounds.w) {
		return;
	}
	if (pos.y > bounds.y+bounds.h) {
		return;
	}
	if (pos.x+pos.w < bounds.x) {
		return;
	}
	if (pos.y+pos.h < bounds.y) {
		return;
	}
	if (pos.x < bounds.x) {
		Sint16 absDiff = bounds.x-pos.x;
		pos.x+=absDiff;
		rect.x+=absDiff;
		pos.w-=absDiff;
		rect.w-=absDiff;
	}
	if (pos.y < bounds.y) {
		Sint16 absDiff = bounds.y-pos.y;
		pos.y+=absDiff;
		rect.y+=absDiff;
		pos.h-=absDiff;
		rect.h-=absDiff;
	}
	if (pos.x+pos.w > bounds.x+bounds.w) {
		Sint16 absDiff = pos.x+pos.w-(bounds.x+bounds.w);
		pos.w -= absDiff;
		rect.w -= absDiff;
	}
	if (pos.y+pos.h > bounds.y+bounds.h) {
		Sint16 absDiff = pos.y+pos.h-(bounds.y+bounds.h);
		pos.h -= absDiff;
		rect.h -= absDiff;
	}

	SDL_RenderCopy(target, data->tex.get(), &rect, &pos);
}

void SagoSprite::SetOrigin(const SDL_Rect& newOrigin) {
	data->origin = newOrigin;
}

int SagoSprite::GetWidth() const {
	return data->imgCord.w;
}
int SagoSprite::GetHeight() const {
	return data->imgCord.h;
}

}  //namespace sago
