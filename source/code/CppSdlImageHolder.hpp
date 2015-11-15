/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2012 Poul Sander

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

#ifndef _CPPSDLIMAGEHOLDER_HPP
#define	_CPPSDLIMAGEHOLDER_HPP

#include "SDL.h"
#include <string>

namespace CppSdl
{

class CppSdlImageHolder
{
public:
	/**
	 * Creates a nulled ImageHolder (without data)
	 */
	CppSdlImageHolder();
	/**
	 * Creates an image from a file
	 * @param filename to be loaded
	 */
	CppSdlImageHolder(std::string filename);
	/**
	 * Creates a copy. The new copy shares raw data with the original but is otherwise independent.
	 * @param orig
	 */
	CppSdlImageHolder(const CppSdlImageHolder& orig) = delete;
	CppSdlImageHolder(char *rawdata, int datasize);
	~CppSdlImageHolder();
	/**
	 * This gives access direct access to the internal data.
	 * Careful, the data might be shared between multiple ImageHolders,
	 * chaing one will change them all.
	 * @return A pointer
	 */
	SDL_Surface *GetRawDataInsecure();
	CppSdlImageHolder & operator=(const CppSdlImageHolder &rhs) = delete;
	/**
	 * The width of the image
	 * 0 if the image is nulled
	 * @return The width of the image in pizels
	 */
	Uint32 GetWidth();
	/**
	 * The height of the image
	 * 0 if the image is nulled
	 * @return  The height of the image
	 */
	Uint32 GetHeight();
	/**
	 * Draws an image to surface at a given position.
	 * If the image is nulled nothing is drawn
	 * @param target Destination surface
	 * @param x horizontal placement on destination surface
	 * @param y vertical placement on destination surface
	 */
	void PaintTo(SDL_Surface *target, int x, int y);
	void OptimizeForBlit(bool allowAlpha = true);
	/**
	 * Tests if there are data in the object
	 * @return true if where are data in the object
	 */
	bool IsNull();
	/**
	 * Removes the data in the object.
	 * Once all ImageHolders pointing to the same data has been freed the internal data will be freed too
	 */
	void MakeNull();
private:
	void Initialized(); //throws an exception if *data is null
	SDL_Rect area;
	SDL_Surface *data;
};

}
#endif	/* _CPPSDLIMAGEHOLDER_HPP */

