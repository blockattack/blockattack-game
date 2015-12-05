/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2013 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
http://blockattack.sf.net
===========================================================================
*/

#include "CppSdlImageHolder.hpp"
#include "SDL_image.h"
#include <stdexcept>

namespace CppSdl {

CppSdlImageHolder::CppSdlImageHolder() {
	data = nullptr;
}

CppSdlImageHolder::CppSdlImageHolder(std::string filename) {
	data = IMG_Load(filename.c_str());
	if (!data) {
		//Here we should throw an exception
		throw std::runtime_error(std::string("Could not read file \""+filename+"\""));
	}
	SDL_GetClipRect(data,&area);
	OptimizeForBlit();
}

CppSdlImageHolder::CppSdlImageHolder(char* rawdata, int datasize) {
	SDL_RWops* rw = SDL_RWFromMem (rawdata, datasize);

	//The above might fail and return null.
	if (!rw) {
		throw std::runtime_error(std::string("Could not read raw data"));
	}

	data = IMG_Load_RW(rw,true); //the second argument tells the function to free RWops

	if (!data) {
		throw std::runtime_error("Could not convert raw data to image");
	}

	SDL_GetClipRect(data,&area);
	OptimizeForBlit();
}

CppSdlImageHolder::~CppSdlImageHolder() {
	MakeNull();
}

SDL_Surface* CppSdlImageHolder::GetRawDataInsecure() {
	Initialized();
	return data;
}

Uint32 CppSdlImageHolder::GetWidth() {
	if (IsNull()) {
		return 0;
	}
	return area.w;
}

Uint32 CppSdlImageHolder::GetHeight() {
	if (IsNull()) {
		return 0;
	}
	return area.h;
}

void CppSdlImageHolder::PaintTo(SDL_Surface* target, int x, int y) {
	static SDL_Rect dest; //static for reuse
	if (IsNull()) {
		return;
	}
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(data, &area, target,&dest);
}

void CppSdlImageHolder::OptimizeForBlit(bool allowAlpha) {
	static SDL_Surface* tmp;
	Initialized();
	if (allowAlpha) {
		tmp = SDL_DisplayFormatAlpha(data);
	}
	else {
		tmp = SDL_DisplayFormat(data);
	}
	SDL_FreeSurface(data);
	data = tmp;
}

void CppSdlImageHolder::Initialized() {
	if (data == nullptr) {
		throw std::runtime_error("ImageHolder used uninitialized!");
	}
}

bool CppSdlImageHolder::IsNull() {
	if (data == nullptr ) {
		return true;
	}
	return false;
}

void CppSdlImageHolder::MakeNull() {
	if (IsNull()) {
		return;
	}
	SDL_FreeSurface(data);
	data = nullptr;
}

}
