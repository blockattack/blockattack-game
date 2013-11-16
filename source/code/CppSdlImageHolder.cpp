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

namespace CppSdl
{

CppSdlImageHolder::CppSdlImageHolder()
{
	data = NULL;
	counter = new int;
	(*counter)=1;
}

CppSdlImageHolder::CppSdlImageHolder(std::string filename)
{
	data = IMG_Load(filename.c_str());
	if(!data)
	{
		//Here we should throw an exception
		CppSdlException e(IMAGE,CPPSDL_ERROR_MISSINGFILE,"Could not read file \""+filename+"\"");
		throw e;
	}
	SDL_GetClipRect(data,&area);
	OptimizeForBlit();
	counter = new int;
	*counter=1;
}

CppSdlImageHolder::CppSdlImageHolder(const CppSdlImageHolder& orig)
{
	//Just take the data from the original. This is technically wrong but adds a little performance.
	data = orig.data;
	area = orig.area;
	if(orig.counter)
	{
		counter = orig.counter;
		(*counter)++;
	}
}

CppSdlImageHolder& CppSdlImageHolder::operator =(const CppSdlImageHolder& rhs)
{
	// Check for self-assignment!
	if (this == &rhs)      // Same object?
		return *this;        // Yes, so skip assignment, and just return *this.
	MakeNull();
	data = rhs.data;
	area = rhs.area;
	if(rhs.counter)
	{
		counter = rhs.counter;
		(*counter)++;
	}
	return *this;
}

CppSdlImageHolder::CppSdlImageHolder(char* rawdata, int datasize)
{
	SDL_RWops *rw = SDL_RWFromMem (rawdata, datasize);

	//The above might fail and return null.
	if(!rw)
	{
		CppSdlException e(IMAGE,CPPSDL_ERROR_NULLPOINTER, "Could not read raw data");
		throw e;
	}

	data = IMG_Load_RW(rw,true); //the second argument tells the function to free RWops

	if(!data)
	{
		CppSdlException e(IMAGE,CPPSDL_ERROR_DATA,"Could not convert raw data to image");
		throw e;
	}

	SDL_GetClipRect(data,&area);
	OptimizeForBlit();
	counter = new int;
	*counter = 1;
}

CppSdlImageHolder::~CppSdlImageHolder()
{
	if(!counter)
		return; //There are no counter, so already freed
	MakeNull();
	if(*counter == 0)
	{
		delete counter;
		counter = NULL;
	}
}

SDL_Surface* CppSdlImageHolder::GetRawDataInsecure()
{
	Initialized();
	return data;
}

Uint32 CppSdlImageHolder::GetWidth()
{
	if(IsNull())
		return 0;
	return area.w;
}

Uint32 CppSdlImageHolder::GetHeight()
{
	if(IsNull())
		return 0;
	return area.h;
}

void CppSdlImageHolder::Cut(Uint32 x, Uint32 y, Sint32 w = -1, Sint32 h = -1)
{
	Initialized();
	if(w<0)
	{
		w = GetWidth() - x;
	}
	if(h<0)
	{
		h = GetHeight() - y;
	}
	if(x+w>GetWidth())
	{
		//throw exception
	}
	if(y+h>GetHeight())
	{
		//throw exception
	}
	if(x<0 || x>GetWidth())
	{
		//throw exception
	}
	if(y<0 || y>GetHeight())
	{
		//throw exception
	}
	area.x += x;
	area.y += y;
	area.w = w;
	area.h = h;
}

void CppSdlImageHolder::PaintTo(SDL_Surface* target, int x, int y)
{
	static SDL_Rect dest; //static for reuse
	if(IsNull())
		return;
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(data,&area, target,&dest);
}

void CppSdlImageHolder::OptimizeForBlit(bool allowAlpha)
{
	static SDL_Surface *tmp;
	Initialized();
	if(allowAlpha)
		tmp = SDL_DisplayFormatAlpha(data);
	else
		tmp = SDL_DisplayFormat(data);
	SDL_FreeSurface(data);
	data = tmp;
}

void CppSdlImageHolder::Initialized()
{
	if(data == NULL)
		throw(CppSdlException(IMAGE,CPPSDL_ERROR_NULLPOINTER,"ImageHolder used uninitialized!"));
}

bool CppSdlImageHolder::IsNull()
{
	if(data == NULL || counter == NULL)
		return true;
	else
		return false;
}

void CppSdlImageHolder::MakeNull()
{
	if(IsNull())
		return;
	(*counter)--;
	if(*counter == 0 && data != NULL)
	{
		SDL_FreeSurface(data);
		data = NULL;
	}
}

}
