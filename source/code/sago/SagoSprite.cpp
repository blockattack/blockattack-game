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


SagoSprite::SagoSprite() {
}

SagoSprite::SagoSprite(const SagoDataHolder& texHolder, const std::string& texture,const SDL_Rect& initImage,const int animationFrames, const int animationFrameLength) {
	tex = texHolder.getTextureHandler(texture);
	imgCord = initImage;
	aniFrames = animationFrames;
	aniFrameTime = animationFrameLength;
}

void SagoSprite::Draw(SDL_Renderer* target, Sint32 frameTime, int x, int y, SagoLogicalResize* resize) const {
	DrawScaled(target, frameTime, x, y, imgCord.w, imgCord.h, resize);
}

void SagoSprite::DrawRotated(SDL_Renderer* target, Sint32 frameTime, int x, int y, const double angleRadian, SagoLogicalResize* resize) const {
	SDL_Point center = {this->origin.x, this->origin.y};
	DrawScaledAndRotated(target, frameTime, x, y, imgCord.w, imgCord.h, angleRadian, &center, SDL_FLIP_NONE, resize);
}

void SagoSprite::DrawScaled(SDL_Renderer* target, Sint32 frameTime, int x, int y, int w, int h, SagoLogicalResize* resize) const {
	DrawScaledAndRotated(target, frameTime, x, y, w, h, 0.0, nullptr, SDL_FLIP_NONE, resize);
}

void SagoSprite::DrawScaledAndRotated(SDL_Renderer* target, Sint32 frameTime, int x, int y, int w, int h, const double angleRadian, const SDL_Point* center, const SDL_RendererFlip flip, SagoLogicalResize* resize) const {
	if (!tex.get()) {
		std::cerr << "Texture is null!\n";
	}
	SDL_Rect rect = imgCord;
	rect.x+=rect.w*((frameTime/aniFrameTime)%aniFrames);
	SDL_Rect pos = rect;
	pos.x = x - this->origin.x;
	pos.y = y - this->origin.y;
	if (w > 0) {
		pos.w = w;
	}
	if (h > 0) {
		pos.h = h;
	}
	double angleDegress = angleRadian/M_PI*180.0;
	if (resize) {
		resize->LogicalToPhysical(pos);
	}
	SDL_RenderCopyEx(target, tex.get(), &rect, &pos, angleDegress, center, flip);
}

void SagoSprite::Draw(SDL_Renderer* target, Sint32 frameTime, int x, int y, const SDL_Rect& part, SagoLogicalResize* resize) const {
	SDL_Rect rect = imgCord;
	rect.x+=rect.w*((frameTime/aniFrameTime)%aniFrames);
	rect.x += part.x;
	rect.y += part.y;
	rect.w = part.w;
	rect.h = part.h;
	SDL_Rect pos = rect;
	pos.x = x - this->origin.x;
	pos.y = y - this->origin.y;
	if (resize) {
		resize->LogicalToPhysical(pos);
	}
	SDL_RenderCopy(target, tex.get(), &rect, &pos);
}

void SagoSprite::DrawProgressive(SDL_Renderer* target, float progress, int x, int y, SagoLogicalResize* resize) const {
	Sint32 frameNumber = progress*aniFrames;
	Sint32 frameTime = frameNumber*aniFrameTime;
	Draw(target, frameTime, x, y, resize);
}

void SagoSprite::DrawBounded(SDL_Renderer* target, Sint32 frameTime, int x, int y, const SDL_Rect& bounds, SagoLogicalResize* resize) const {
	SDL_Rect rect = imgCord;
	rect.x+=rect.w*((frameTime/aniFrameTime)%aniFrames);
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

	if (resize) {
		resize->LogicalToPhysical(pos);
	}
	SDL_RenderCopy(target, tex.get(), &rect, &pos);
}

void SagoSprite::SetOrigin(const SDL_Rect& newOrigin) {
	origin = newOrigin;
}

int SagoSprite::GetWidth() const {
	return imgCord.w;
}
int SagoSprite::GetHeight() const {
	return imgCord.h;
}

}  //namespace sago
